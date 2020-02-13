#include "DiskPartition.h"

#include <stddef.h>
#include <stdint.h>

#include <cstring>
#include <fstream>
#include <ostream>
#include <vector>

#include "Device.h"
#include "DevFileSystem.h"
#include "File.h"
#include "FileSystem.h"
#include "Kernel.h"
#include "Logger.h"
#include "util.h"

/******************************************************************************
 ******************************************************************************/

void PartitionDriver::dump(klib::ostream& dest) const
{
    dest << desc;
    if (offset != 0)
        dest << " begins at " << format_bytes(offset);
    dest << " size is " << format_bytes(size) << '\n';

    dest.flush();
}

/******************************************************************************/

size_t PartitionDriver::read_block(uint64_t off, void* addr)
{
    size_t ret_val = drv->read_block(off + offset, addr);
    return ret_val;
}

/******************************************************************************/

size_t PartitionDriver::write_block(uint64_t off, const void* addr)
{
    return drv->write_block(off + offset, addr);
}

/******************************************************************************
 ******************************************************************************/

MbrEntry::MbrEntry(const uint8_t* d) :
    val{false}, act{false}, id{0}, l{0}, sects{0}
{
    // Start with the system ID, since that being zero is supposed to be the
    // definitve indicator of presence.
    id = d[4];
    if (id == 0)
    {
        val = false;
        return;
    }

    // Now do the bootable flag.
    if (d[0] & 0x80)
        act = true;

    // The start and end CHS values should be present, but for devices larger
    // than 8GB, they are meaningless, and for smaller ones the LBA should match
    // anyway, so we'll only use the LBA values.
    l = *reinterpret_cast<const uint32_t*>(d + 8);
    sects = *reinterpret_cast<const uint32_t*>(d + 12);

    // An LBA of 0, in either field, is not valid. The first, since the first
    // sector must be the partition table, and the second because a size of 0 is
    // meaningless.
    if (l == 0 || sects == 0)
    {
        val = false;
        return;
    }

    // If we have made it here, the entry is probably ok.
    val = true;
}

/******************************************************************************
 ******************************************************************************/

MasterBootRecord::MasterBootRecord(const klib::string& dev_name) : val{false}
{
    // Set up disk reader.
    klib::string name {dev_name};
    if (name.compare(0, 5, "/dev/") != 0)
        name = "/dev/" + name;
    klib::ifstream ifs {name};

    if (ifs)
    {
        // Read 512 bytes in.
        ifs.read(reinterpret_cast<klib::istream::char_type*>(data),
            sz / sizeof(klib::istream::char_type));

/*        klib::ostream& ofs = *global_kernel->syslog()->stream();
        ofs << klib::hex << klib::right;
        ofs.fill('0');
        for (size_t i = 0; i < sz / 16; ++i)
        {
            ofs.width(8);
            ofs << i*16 << ' ' << ' ';
            for (size_t j = 0; j < 16; ++j)
            {
                ofs.width(2);
                ofs << static_cast<size_t>(data[i*16 + j]) << ' ';
                if (j == 7)
                    ofs << ' ';
            }
            ofs << '\n';
        }
        ofs << klib::dec << klib::left;
        ofs.fill(' ');*/

        // Check the signature.
        if (data[0x1fe] != 0x55 || data[0x1ff] != 0xAA)
            val = false;
        else
            val = true;
    }
    else
        val = false;
}

/******************************************************************************/

bool MasterBootRecord::is_gpt() const
{
    if (!val)
        return false;

    // A GPT is indicated by having one partition with type 0xEE.
    klib::vector<MbrEntry> parts = partition_table();
    if (parts.size() == 1 && parts[0].system_id() == 0xEE)
        return true;
    else
        return false;
}

/******************************************************************************/

klib::vector<MbrEntry> MasterBootRecord::partition_table() const
{
    klib::vector<MbrEntry> ret_val;

    if (!val)
        return ret_val;

    for (size_t i = 0x1be; i <= 0x1ee; i += 16)
    {
        MbrEntry tmp {data + i};
        if (tmp.valid())
            ret_val.push_back(tmp);
    }

    return ret_val;
}

/******************************************************************************
 ******************************************************************************/

constexpr uint8_t GptEntry::linux_data[];

/******************************************************************************/

GptEntry::GptEntry(const uint8_t* d)
{
    klib::memcpy(guid, d, uuid_length);
    klib::memcpy(uuid, d + uuid_length, uuid_length);

    l = *reinterpret_cast<const uint64_t*>(d + 32);
    sects = *reinterpret_cast<const uint64_t*>(d + 40) - l + 1;

    if (l == 0 || sects == 0)
    {
        val = false;
        return;
    }

    plat_req = (d[48] & 0x1);
    efi_ign = (d[48] & 0x2);
    act = (d[48] & 0x4);

    attrib = *reinterpret_cast<const uint16_t*>(d + 54);

    // Name characters are 16 bit unicode. We'll just look at the lower byte
    // and interpret it as ASCII.
    for (size_t i = 0; i < name_length; i += 2)
    {
        char c = d[i + 56];
        if (c == '\0')
            break;
        name += c;
    }

    val = true;
}

/******************************************************************************/

bool GptEntry::operator==(const GptEntry& other) const
{
    bool ret_val = true;

    for (size_t i = 0; i < uuid_length; ++i)
    {
        if (guid[i] != other.guid[i] || uuid[i] != other.uuid[i])
        {
            ret_val = false;
            break;
        }
    }

    return (ret_val &&
        val == other.val &&
        l == other.l &&
        sects == other.sects &&
        plat_req == other.plat_req &&
        efi_ign == other.efi_ign &&
        act == other.act &&
        attrib == other.attrib &&
        name == other.name);
}

/******************************************************************************
 ******************************************************************************/

Gpt::Gpt(const klib::string& dev_name)
{
    // Check that the MBR is sensible.
    if (!(MasterBootRecord {dev_name}).is_gpt())
    {
        val = false;
        return;
    }

    // GPT header should be in sector 1 (whatever the sector size).
    read_gpt(*this, dev_name, 1);
    if (!val)
        return;

    // Read the backup header.
    Gpt backup;
    read_gpt(backup, dev_name, hdr_lba_other);

    if (!backup.val || !test_backup(backup))
    {
        val = false;
        return;
    }
}

/******************************************************************************/

bool Gpt::test_backup(const Gpt& other) const
{
    bool ret_val = true;
    for (size_t i = 0; i < uuid_length; ++i)
    {
        if (uuid[i] != other.uuid[i])
        {
            ret_val = false;
            break;
        }
    }   

    return (ret_val &&
        val == other.val &&
        rev_no == other.rev_no &&
        hdr_sz == other.hdr_sz &&
        hdr_lba == other.hdr_lba_other &&
        hdr_lba_other == other.hdr_lba &&
        f_lba == other.f_lba &&
        l_lba == other.l_lba &&
        pt_len == other.pt_len &&
        pt_sz == other.pt_sz &&
        pt_table == other.pt_table);
}

/******************************************************************************
 ******************************************************************************/

void read_gpt(Gpt& gpt, const klib::string& dev_name, uint64_t sect)
{
    // Get the disk driver (needed for the sector size).
    Device* dev =
        global_kernel->get_vfs()->get_dev()->get_device_driver(dev_name);
    if (dev->get_type() != DeviceType::ata_disk)
    {
        gpt.val = false;
        return;
    }
    BlockDevice* bl = reinterpret_cast<BlockDevice*>(dev);

    // Open the block device and seek to the sector.
    klib::string name {dev_name};
    if (name.compare(0, 5, "/dev/") != 0)
        name = "/dev/" + name;
    klib::ifstream ifs {name};
    ifs.seekg(sect * bl->sector_size());
    if (!ifs)
    {
        gpt.val = false;
        return;
    }

    // Check the signature.
    char buf[sizeof(Gpt::signature)];
    ifs.read(buf, sizeof(Gpt::signature));
    uint64_t sig = *reinterpret_cast<const uint64_t*>(buf);
    if (sig != Gpt::signature)
    {
        gpt.val = false;
        return;
    }

    // Revision number.
    ifs.read(buf, 4);
    gpt.rev_no = *reinterpret_cast<const uint32_t*>(buf);

    // Header size.
    ifs.read(buf, 4);
    gpt.hdr_sz = *reinterpret_cast<const uint32_t*>(buf);

    // Skip 8 bytes (CRC32 and reserved).
    ifs.ignore(8);

    // Header lba.
    ifs.read(buf, 8);
    gpt.hdr_lba = *reinterpret_cast<const uint64_t*>(buf);

    // Backup header lba.
    ifs.read(buf, 8);
    gpt.hdr_lba_other = *reinterpret_cast<const uint64_t*>(buf);

    // First usable partition lba.
    ifs.read(buf, 8);
    gpt.f_lba = *reinterpret_cast<const uint64_t*>(buf);

    // Last usable partition lba.
    ifs.read(buf, 8);
    gpt.l_lba = *reinterpret_cast<const uint64_t*>(buf);

    // Disk UUID.
    ifs.read(gpt.uuid, 16);

    // Starting LBA of partition table entries.
    ifs.read(buf, 8);
    gpt.pt_lba = *reinterpret_cast<const uint64_t*>(buf);

    // Number of partition table entries.
    ifs.read(buf, 4);
    gpt.pt_len = *reinterpret_cast<const uint32_t*>(buf);

    // Size of a partition table entry.
    ifs.read(buf, 4);
    gpt.pt_sz = *reinterpret_cast<const uint32_t*>(buf);

    ifs.close();

    // Read each partition entry.
    ifs = klib::ifstream {name};
    ifs.seekg(gpt.pt_lba * bl->sector_size());
    char* buf2 = new char[gpt.pt_sz];
    for (size_t i = 0; i < gpt.pt_len; ++i)
    {
        ifs.read(buf2, gpt.pt_sz);
        GptEntry temp_entry {reinterpret_cast<uint8_t*>(buf2)};
        if (temp_entry.valid())
            gpt.pt_table.push_back(temp_entry);
    }

    ifs.close();
    delete[] buf2;

    gpt.val = true;
}

/******************************************************************************
 ******************************************************************************/

void read_partition_table(const klib::string& key,
    klib::map<klib::string, Device*>& disk_drivers)
{
    // Get the disk driver.
    Device* dev = disk_drivers[key];
    if (dev->get_type() != DeviceType::ata_disk)
        return;
    BlockDevice* drv = reinterpret_cast<BlockDevice*>(dev);
//    drv->dump(*global_kernel->syslog()->stream());

    // Read the partition table.
    MasterBootRecord mbr {key};

    if (!mbr.valid())
    {
        drv->set_description(drv->get_description() +
            " - unrecognised partition table");
    }
    else if (mbr.is_gpt())
    {
        Gpt gpt {key};

        if (gpt.valid())
        {
            drv->set_description(drv->get_description() + " - GPT");
            klib::vector<GptEntry> parts = gpt.partition_table();
            for (size_t i = 0; i < parts.size(); ++i)
            {
                if (!parts[i].valid())
                    continue;

                // Test known GUIDs.
                FileSystemType fst;
                if (klib::memcmp(parts[i].type_guid(), GptEntry::linux_data,
                    GptEntry::uuid_length) == 0)
                    fst = FileSystemType::ext;
                else
                    fst = FileSystemType::unknown;

                // Create the partition driver.
                PartitionDriver* p_drv = new PartitionDriver{
                    drv->get_type(),
                    "Partition " + parts[i].get_name(),
                    fst,
                    parts[i].sectors() * drv->get_alignment(),
                    drv,
                    parts[i].lba() * drv->get_alignment()};
                disk_drivers[key + klib::to_string(i + 1)] = p_drv;
            }
        }
        else
        {
            drv->set_description(drv->get_description() + 
                " - unrecognised partition table");
        }
    }
    else
    {
        drv->set_description(drv->get_description() + " - MBR");
        klib::vector<MbrEntry> parts {mbr.partition_table()};
        for (size_t i = 0; i < parts.size(); ++i)
        {
            if (!parts[i].valid())
                continue;

            // Determine what we can about the file system.
            klib::string id;
            klib::helper::strprintf(id, "ID %X",
                static_cast<uint32_t>(parts[i].system_id()));
            FileSystemType fst;
            switch (parts[i].system_id())
            {
            case 0x83:
                fst = FileSystemType::ext;
                break;
            default:
                fst = FileSystemType::unknown;
            }

            // Create the partition driver.
            PartitionDriver* p_drv = new PartitionDriver{
                drv->get_type(),
                "Partition " + id,
                fst,
                parts[i].sectors() * drv->get_alignment(),
                drv,
                parts[i].lba() * drv->get_alignment()};
            disk_drivers[key + klib::to_string(i + 1)] = p_drv;
        }
    }
}

/******************************************************************************
 ******************************************************************************/
