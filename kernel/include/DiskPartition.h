#ifndef DISK_PARTITION_H
#define DISK_PARTITION_H

#include <stddef.h>
#include <stdint.h>

#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "Device.h"

// Forward declarations.
enum class FileSystemType;

/**
    Like a block device, but for an individual partition. Just stores a block
    device and an offset. Applies the offset to all reads and writes to the
    underlying disk.
 */
class PartitionDriver : public BlockDevice {
public:
    /**
        Constructor. Gets some information from the underlying disk driver.
        Other information is specific to this partition.

        @param t Type of device.
        @param d Description of the device.
        @param f File system type, can be left as unknown.
        @param s Size of the disk or partition.
        @param dd Underlying device driver.
        @param o Offset from the start of the device that this partition starts
               at.
     */
    PartitionDriver(DeviceType t, const klib::string& d, FileSystemType f,
        uint64_t s, BlockDevice* dd, uint64_t o) :
        BlockDevice{t, d, dd->get_alignment(), f, s}, drv{dd}, offset{o}
    {}

    /**
        Override dump to also print the partition start.

        @param dest Stream to print information to.
     */
    virtual void dump(klib::ostream& dest) const override;

    /**
        Gives the offset of this partition from the start of the real device, in
        bytes.

        @return Partition offset.
     */
    virtual uint64_t get_offset() const { return offset; }

    /**
        Read bytes from an offset on the disk into a memory location. An error
        may be returned if not a convenient size or offset.

        @param off Offset from the start of the disk to read from.
        @param addr Address in memory to put the data.
        @return Success or error code of the operation.
     */
    virtual size_t read_block(uint64_t off, void* addr) override;

    /**
        Write bytes from a memory location onto the disk. An error may be
        returned if not a convenient size or offset.

        @param off Offset from the start of the disk to write to.
        @param addr Address in memory to get the data from.
        @return Success or error code of the operation.
     */
    virtual size_t write_block(uint64_t off, const void* addr) override;

    /**
        Waits until the pending write is complete. Calls the underlying device
        flush.

        @return 0 on success, otherwise EoF.
     */
    virtual int flush() override { return drv->flush(); }

    /**
        Performs any clean up operations required. Just calls flush() for now.

        @return 0 on success, otherwise EoF.
     */
    virtual int close() override { return flush(); }

    /**
        Determines whether a poll condition is currently satisfied. Forwards the
        check to the physical device driver.

        @param cond Bitmask of polling conditions to check.
        @return Mask representing the currently true events.
     */
    virtual PollType poll_check(PollType cond) const override
    {
        return drv->poll_check(cond);
    }

protected:
    // Driver for the underlying disk. We need this so that this driver could
    // be for any type of disk device.
    BlockDevice* drv;
    // Offset of the start of the partition from the start of the underlying
    // disk.
    uint64_t offset;
    // For DMA, indicates whether the device is currently busy with a
    // read/write.
    bool busy;
};

/**
    Represents a partition table entry in the Master Boot Record.
 */
class MbrEntry {
public:
    /**
        Constructor. Unpacks the provided data into this structure.

        @param d Data of the entry. Address of a 16 byte array.
     */
    explicit MbrEntry(const uint8_t* d);

    /**
        Whether the partition is active (bootable).

        @return Whether the partition is active.
     */
    bool active() const { return (val ? act : false); }

    /**
        Returns the LBA of the start of the partition. This needs to be
        multiplied by the sector size to get the byte offset of the partition
        start.

        @return LBA of the first sector of the partition.
     */
    uint32_t lba() const { return (val ? l : 0); }

    /**
        Returns the number of sectors in the partition.

        @return Number of sectors in the partition.
     */
    uint32_t sectors() const { return (val ? sects : 0); }

    /**
        Returns the system ID byte, which partially indicates the type of the
        partition, eg. the file system.

        @return System ID byte.
     */
    uint8_t system_id() const { return (val ? id : 0); }

    /**
        Whether the entry is valid. Not present counts as not valid.

        @return Whether the entry is present and valid.
     */
    bool valid() const { return val; }

private:
    // Whether the entry is valid.
    bool val;
    // Whether the partition is 'active' (bootable).
    bool act;
    // System ID byte.
    uint8_t id;
    // Index of the first sector of the partition.
    uint32_t l;
    // Number of sectors in the partition.
    uint32_t sects;
};

/**
    Represents an Master boot record.
 */
class MasterBootRecord {
public:
    /**
        Constructor. Reads in the data from a block device.

        @param dev_name Dev name of the device to read data in from. The first
               512 bytes will be read.
     */
    explicit MasterBootRecord(const klib::string& dev_name);

    /**
        Gives the address of the start of the data, to be interpreted as
        boot loader code.

        @return Address of the bootstrap code. Normally up to 436 bytes long.
     */
    void* bootstrap() { return (val ? data : nullptr); }

    /**
        Gives the 'unique disk ID' which could actually be anything.

        @return Disk ID. 10 byte array.
     */
    const uint8_t* disk_id() const { return (val ? &data[0x1b4] : nullptr); }

    /**
        Tests whether this MBR seems to be a protective MBR for a GPT.

        @return Whether this is really a GPT.
     */
    bool is_gpt() const;

    /**
        Gives a vector of the partition table entries, with up to 4 entries.

        @return Copy of the partition table entries.
     */
    klib::vector<MbrEntry> partition_table() const;

    /**
        Whether the MBR was recognised as valid. Other functions will return
        undefined values if it was not.

        @return Whether the MBR is valid.
     */
    bool valid() { return val; }

private:
    // Whether this is a valid MBR.
    bool val;
    // Size of an MBR.
    static constexpr size_t sz = 512;
    // Actual data in byte form.
    uint8_t data[sz];
};

/**
    Represents an entry in a GUID partition table.
 */
class GptEntry {
public:
    /**
        GUID file system type values.
     */
    static constexpr uint8_t linux_data[] = {0xAF, 0x3D, 0xC6, 0x0F, 0x83, 0x84,
        0x72, 0x47, 0x8E, 0x79, 0x3D, 0x69, 0xD8, 0x47, 0x7D, 0xE4};

    /**
        Constructor. Unpacks the provided data into this structure.

        @param d Address of the start of the data of the entry.
     */
    explicit GptEntry(const uint8_t* d);

    /**
        Return the type specific attributes of this partition.

        @return Type specific attributes of this partition.
     */
    uint16_t attributes() const { return (val ? attrib : 0); }

    /**
        Whether this partition has the legacy BIOS bootable attribute set.

        @return Whether the partition is BIOS bootable.
     */
    bool active() const { return (val ? act : false); }

    /**
        Whether this partition has the EFI ignore attribute set.

        @return Whether the partition should be ignored by EFI firmware.
     */
    bool efi_ignore() const { return (val ? efi_ign : false); }

    /**
        Returns the name of this partition. Only ASCII characters are currently
        supported.

        @return The name of this partition.
     */
    const klib::string& get_name() const { return name; }

    /**
        Returns the UUID of the partition.

        @return UUID of the partition.
     */
    const char* get_uuid() const { return (val ? uuid : nullptr); }

    /**
        Returns the LBA of the start of the partition. This needs to be
        multiplied by the sector size to get the byte offset of the partition
        start.

        @return LBA of the first sector of the partition.
     */
    uint64_t lba() const { return (val ? l : 0); }

    /**
        Whether this partition has the platform required attribute set.

        @return Whether the partition is platform required.
     */
    bool platform_required() const { return (val ? plat_req : false); }

    /**
        Returns the number of sectors in the partition.

        @return Number of sectors in the partition.
     */
    uint64_t sectors() const { return (val ? sects : 0); }

    /**
        Returns the GUID representing the type of this partition.

        @return GUID of the partition type.
     */
    const uint8_t* type_guid() const { return (val ? guid : nullptr); }

    /**
        Whether the GPT entry was recognised as valid. Other functions will
        return undefined values if it was not.

        @return Whether the GPT entry is valid.
     */
    bool valid() { return val; }

    /**
        Compares two entries for equality. Tests each individual value.

        @param other GptEntry to compare against.
        @return Whether this and other contain equivalent data.
     */
    bool operator==(const GptEntry& other) const;

    /**
        Compares two entries for non-equality. Tests each individual value.

        @param other GptEntry to compare against.
        @return Whether this and other contain equivalent data.
     */
    bool operator!=(const GptEntry& other) const { return !(*this == other); }

    /**
        Length of the UUID and type GUID.
     */
    static constexpr size_t uuid_length = 16;

private:
    // Whether the entry is valid.
    bool val;
    // Partition type GUID.
    uint8_t guid[uuid_length];
    // Partition UUID.
    char uuid[uuid_length];
    // LBA index of the first sector in the partition.
    uint64_t l;
    // Number of sectors in the partition.
    uint64_t sects;
    // Platform required.
    bool plat_req;
    // Should be ignored by EFI firmware.
    bool efi_ign;
    // Legacy BIOS bootable.
    bool act;
    // Type specific attributes.
    uint16_t attrib;
    // Name of the partition. Non-ASCII characters are not currently supported.
    static constexpr size_t name_length = 72;
    klib::string name;
};

/**
    Represents a GUID partition table.
 */
class Gpt {
public:
    /**
        Default constructor. Leaves everything blank and does no reading.
     */
    Gpt() {}

    /**
        Constructor. Reads in data from a block device.

        @param dev_name Name of the device to read from.
     */
    explicit Gpt(const klib::string& dev_name);

    /**
        Returns the LBA of the first sector usable for partitions.

        @return First partition LBA.
     */
    uint64_t first_lba() const { return (val ? f_lba : 0); }

    /**
        Returns the UUID of the disk.

        @return Disk UUID.
     */
    const char* get_uuid() const { return (val ? uuid : 0); }

    /**
        Returns the LBA of the primary copy of the header, usually 1.

        @return Primary header LBA.
     */
    uint64_t header_lba() const { return (val ? hdr_lba : 0); }

    /**
        Returns the LBA of the backup copy header, usually the last sector.

        @return Backup header LBA.
     */
    uint64_t header_lba_backup() const { return (val ? hdr_lba_other : 0); }

    /**
        Returns the size of the header in bytes, usually 92.

        @return Header size.
     */
    uint32_t header_size() const { return (val ? hdr_sz : 0); }

    /**
        Returns the LBA of the last sector usable for partitions.

        @return Last partition LBA.
     */
    uint64_t last_lba() const { return (val ? l_lba : 0); }

    /**
        Returns the LBA of the first partition table entry, usually 2.

        @return LBA of the first partition table entry.
     */
    uint64_t partition_lba() const { return (val ? pt_lba : 0); }

    /**
        Returns the number of entries in the partition table.

        @return size of a partition table entry.
     */
    uint32_t partition_length() const { return (val ? pt_len : 0); }

    /**
        Gets a reference to the partition table.

        @return The partition table.
     */
    const klib::vector<GptEntry>& partition_table() const
    {
        return pt_table;
    }

    /**
        Returns the size in bytes of each partition table entry, at least 128.

        @return size of a partition table entry.
     */
    uint32_t partition_size() const { return (val ? pt_sz : 0); }

    /**
        Returns the revision number. Usually 0x100000.

        @return Revision number.
     */
    bool revision() const { return (val ? rev_no : 0); }

    /**
        Checks whether another entry is set up correctly to act as the backup
        for this entry. Mostly that means they're the same as each other, but
        some of the LBAs will be different.

        @param other Gpt to compare against.
        @return Whether this and other are backups of each other.
     */
    bool test_backup(const Gpt& other) const;

    /**
        Whether the GPT was recognised as valid. Other functions will return
        undefined values if it was not.

        @return Whether the MBR is valid.
     */
    bool valid() { return val; }

private:
    // Whether this is a valid GPT.
    bool val;
    // Magic signature for a GPT header.
    static constexpr uint64_t signature = 0x5452415020494645ull;
    // Revision number.
    uint32_t rev_no;
    // Header size.
    uint32_t hdr_sz;
    // LBA of this header copy.
    uint64_t hdr_lba;
    // LBA of the other header copy.
    uint64_t hdr_lba_other;
    // LBA of the first sector usable for data.
    uint64_t f_lba;
    // LBA of the last sector usable for data.
    uint64_t l_lba;
    // UUID of the disk.
    static constexpr size_t uuid_length = 16;
    char uuid[uuid_length];
    // LBA of the first entry in the partition table.
    uint64_t pt_lba;
    // Number of entries in the partition table.
    uint32_t pt_len;
    // Size of each entry in the partition table, usually 128 bytes.
    uint32_t pt_sz;
    // List of partitions.
    klib::vector<GptEntry> pt_table;

    // read_gpt needs access to the private members.
    friend void read_gpt(Gpt& gpt, const klib::string& dev_name, uint64_t sect);
};

/**
    Read a GPT header and partition table.

    @param gpt GPT to put the data in.
    @param dev_name Dev name of the device to read from.
    @param sect Sector to look for the header in.
 */
void read_gpt(Gpt& gpt, const klib::string& dev_name, uint64_t sect);

/**
    Given a disk name, checks for partitions, creates the drivers and adds
    them to the list of kernel drivers.

    @param key /dev/ name of the disk.
    @param disk_drivers List of disk drivers to add to.
 */
void read_partition_table(const klib::string& key,
    klib::map<klib::string, Device*>& disk_drivers);

#endif /* DISK_PARTITION_H */
