#include "DevFileSystem.h"

#include <map>
#include <string>
#include <vector>

#include "Device.h"
#include "DiskPartition.h"
#include "File.h"
#include "FileSystem.h"
#include "Ide.h"
#include "Kernel.h"
#include "Logger.h"
#include "MultiBoot.h"
#include "Serial.h"
#include "Tty.h"
#include "VgaController.h"

/******************************************************************************
 ******************************************************************************/

DevFileSystem::DevFileSystem() :
    FileSystem {""},
    device_drivers {}
{}

/******************************************************************************/

DevFileSystem::~DevFileSystem()
{
    // Destruct drivers for all physical devices.
    for (auto p : device_drivers)
        delete p.second;
}

/******************************************************************************/

Directory* DevFileSystem::diropen(const klib::string& name)
{
    // dev does not have a true directory structure, but the names of devices
    // might contain /, indicating a hierarchy. Make a list of devices that have
    // the provided name as the prefix, but with anything after the next '/'
    // stripped.
    klib::vector<klib::string> files;

    // Get rid of leading slashes in name, since device names don't have them.
    klib::string test {name};
    while (test[0] == '/')
        test.erase(test.begin());

    // Create a list of device names.
    klib::vector<klib::string> all_devs;
    for (auto p : device_drivers)
        all_devs.push_back(p.first);

    for (klib::string& temp : all_devs)
    {
        // Test and remove the matching prefix.
        if (test != temp.substr(0, temp.size()))
            continue;
        temp.erase(0, temp.size());
        while (temp[0] == '/')
            temp.erase(temp.begin());

        // Remove anything after the next '/', so we don't get a recursive
        // listing.
        size_t pos = temp.find('/');
        if (pos != klib::string::npos)
            temp.erase(pos);

        // Add to the match list.
        files.push_back(temp);
    }

    // Create a directory to return.
    return new VirtualDirectory {files};
}

/******************************************************************************/

klib::FILE* DevFileSystem::fopen(const klib::string& name, const char* mode)
{
    // Check arguments.
    if (name.empty() || mode == nullptr)
        return nullptr;

    // Strip leading '/' from the name.
    klib::string test {name};
    while (test[0] == '/')
        test.erase(test.begin());

    // Check the disk drivers.
    for (auto it = device_drivers.begin(); it != device_drivers.end(); ++it)
    {
        if (it->first == test)
        {
            switch(it->second->get_type())
            {
            case DeviceType::ata_disk:
                return new BlockFile
                    {*static_cast<BlockDevice*>(it->second), mode};
            case DeviceType::serial_port: case DeviceType::console:
                return new CharacterFile
                    {*static_cast<CharacterDevice*>(it->second), mode};
            default:
                return nullptr;
            }
        }
    }

    return nullptr;
}

/******************************************************************************/

Device* DevFileSystem::get_device_driver(const klib::string& name) const
{
    // Be ambivalent as to whether the name begins with /dev/.
    klib::string test {name};
    while (test[0] == '/')
        test.erase(test.begin());

    if (test.substr(0, 3) == "dev")
        test.erase(0, 3);

    while (test[0] == '/')
        test.erase(test.begin());

    auto it = device_drivers.find(test);
    if (it == device_drivers.end())
        return nullptr;

    return it->second;
}

/******************************************************************************/

void DevFileSystem::add_ide(klib::vector<IdeController>& ides)
{
    // Add a device for each raw disk.
    for (IdeController& ide : ides)
        ide.add_drivers(device_drivers);

    // Now we search the disks for partitions. We need to use raw disks only, so
    // make a list of disks to search before adding any partitions.
    klib::vector<klib::string> matches;

    // Cycle over the devices and find ones which are hard disks.
    for (auto it = device_drivers.begin(); it != device_drivers.end(); ++it)
    {
        // Test for ata disk (sdX) match.
        if (it->second->get_type() == DeviceType::ata_disk)
            matches.push_back(it->first);
    }

    // Read partitions for all the matches and add their drivers.
    for (const klib::string& m : matches)
        read_partition_table(m, device_drivers);

    // Dump the drive map.
    for (const klib::pair<klib::string, Device*> p : device_drivers)
    {
        if (p.second->get_type() == DeviceType::ata_disk)
        {
            global_kernel->syslog()->write("  " + p.first + ": ");
            static_cast<BlockDevice*>(p.second)->
                dump(*global_kernel->syslog()->stream());
        }
    }
}

/******************************************************************************/

void DevFileSystem::add_serial()
{
    // Create drivers for all the serial ports.
    // TODO so far, we only know about the first serial port.
    device_drivers["ttyS0"] = new Serial{SerialAddress::com1};
}

/******************************************************************************/

void DevFileSystem::add_tty()
{
    // Create drivers for all the TTY devices.
    // TODO so far, we only know about the first device.

    // Make the VGA device for the output.
    const MultiBootInfo& mbi {global_kernel->get_multiboot()};
    if (!mbi.framebuffer().valid())
    {
        global_kernel->syslog()->warn(
            "Multiboot had no valid framebuffer information.\n");
    }
    VgaController* vga = new VgaController {mbi.framebuffer().addr(),
        mbi.framebuffer().width(),
        mbi.framebuffer().height(),
        mbi.framebuffer().bpp(),
        *global_kernel->get_pdt()};
    global_kernel->syslog()->info(
        "Initialised VGA driver at %p. Framebuffer at %p.\n", vga,
        vga->framebuffer());
    const char* tmp = "----- Stable Genius OS initialising -----\n";
    for (const char* c = tmp; *c != '\0'; ++c)
        vga->write_char(*c);

    // Create the TTY device.
    Tty* tty = new Tty {vga};
    device_drivers["tty"] = tty;
}

/******************************************************************************/

klib::string get_new_device_name(DeviceType t,
    const klib::map<klib::string, Device*>& m)
{
    // Get the dev standard prefix.
    klib::string prefix {type_to_prefix(t)};

    klib::string i;
    // Devices indexed by a letter.
    if (alpha_index(t))
    {
        i = klib::string{'a'};
        while (m.find(prefix + i) != m.end())
        {
            klib::string::iterator it;
            for (it = --i.end(); *it == 'z' && it != i.begin(); --it) ;
            if (*it != 'z')
                ++(*it);
            else
                i = klib::string(i.size() + 1, 'a');
        }
    }
    // Devices indexed by a digit.
    else
    {
        size_t ii;
        for (ii = 0; m.find(prefix + klib::to_string(ii)) != m.end(); ++ii) ;
        i = klib::string{klib::to_string(ii)};
    }

    return prefix + i;
}

/******************************************************************************/

klib::string type_to_prefix(DeviceType t)
{
    static const klib::map<DeviceType, klib::string> m {
        klib::pair<DeviceType, klib::string> {DeviceType::ata_disk, "sd"},
        klib::pair<DeviceType, klib::string> {DeviceType::cdrom, "sr"},
        klib::pair<DeviceType, klib::string> {DeviceType::floppy_disk, "fd"},
        klib::pair<DeviceType, klib::string> {DeviceType::serial_port, "ttyS"},
        klib::pair<DeviceType, klib::string> {DeviceType::ata_disk, "sd"},
        klib::pair<DeviceType, klib::string> {DeviceType::console, "tty"}};

    return m.find(t)->second;
}

/******************************************************************************/

bool alpha_index(DeviceType t)
{
    switch(t)
    {
        case DeviceType::ata_disk: case DeviceType::floppy_disk:
            return true;
        default:
            return false;
    }
}

/******************************************************************************
 ******************************************************************************/
