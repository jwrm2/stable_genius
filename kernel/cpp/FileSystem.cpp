#include "FileSystem.h"

#include <stdint.h>

#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <utility>

#include "DevFileSystem.h"
#include "Ext.h"
#include "Kernel.h"
#include "Logger.h"

/******************************************************************************
 ******************************************************************************/

size_t FileSystem::read(uint64_t offset, char* buf, size_t n)
{
    klib::ifstream in {drv_name};
    in.seekg(offset);
    in.read(buf, n);
    return in.gcount();
}

/******************************************************************************/

void FileSystem::write(uint64_t offset, const char* buf, size_t n)
{
    klib::string tmp;
    if (n == 0)
        tmp = klib::string {reinterpret_cast<const char*>(buf)};
    else
        tmp = klib::string{reinterpret_cast<const char*>(buf), n};

    klib::ofstream out {drv_name};
    out.seekp(offset);
    out << tmp;
}

/******************************************************************************
 ******************************************************************************/

FileSystem* create_fs(const klib::string& drv)
{
    // We need the device driver to see if the file system has been determined.
    DevFileSystem* dev = global_kernel->get_vfs()->get_dev();
    Device* d = dev->get_device_driver(drv);

    if (d == nullptr || d->get_type() != DeviceType::ata_disk)
        return nullptr;

    // Add a /dev/ to the start of the drive name, if it's not there already.
    klib::string name {drv};
    if (name.compare(0, 5, "/dev/") != 0)
        name = "/dev/" + name;

    // Inspect the block device.
    BlockDevice* bl = dynamic_cast<BlockDevice*>(d);
    switch (bl->get_fst())
    {
    case FileSystemType::none:
    case FileSystemType::failed:
        return nullptr;
    case FileSystemType::ext:
        return create_ext(name);
    case FileSystemType::ext2:
        return create_ext2(name);
    case FileSystemType::unknown:
        FileSystem* fs;
        if ((fs = create_ext(name)) != nullptr)
        {
            // Try ext first.
            bl->set_fst(FileSystemType::ext);
            return fs;
        }
        // Any other file systems will go here.
    }

    return nullptr;
}

/******************************************************************************
 ******************************************************************************/

Directory* VirtualFileSystem::diropen(const klib::string& name)
{
    klib::string tmp {name};
    FileSystem* fs = lookup(tmp);
    return fs->diropen(tmp);
}

/******************************************************************************/

klib::FILE* VirtualFileSystem::fopen(const klib::string& name, const char* mode)
{
    klib::string tmp {name};
    FileSystem* fs = lookup(tmp);
    klib::FILE* f = fs->fopen(tmp, mode);

    // The filesystem might return nullptr on error.
    if (f == nullptr)
        return nullptr;

    // If there was some reason the file could not be opened, it may be in a
    // valid, but closed, state. In that case we need to delete it and fail.
    if (f->closed())
    {
        global_kernel->syslog()->warn("VFS::fopen %s file is closed after open\n", name.c_str());
        delete f;
        return nullptr;
    }

    // Deal with mode specific actions.
    if (klib::strcmp(mode, "w") == 0|| klib::strcmp(mode, "w+") == 0)
        f->truncate();
    if (klib::strcmp(mode, "a") == 0 || klib::strcmp(mode, "a+") == 0)
        klib::fseek(f, 0, SEEK_END);

    return f;
}

/******************************************************************************/

int VirtualFileSystem::unlink(const klib::string& name)
{
    // Look up the file system.
    klib::string tmp {name};
    FileSystem* fs = lookup(tmp);

    // Pass the call onto the file system.
    return fs->unlink(tmp);
}

/******************************************************************************/

FileSystem* VirtualFileSystem::lookup(const klib::string& fname) const
{
    // Copy contructing a new string is not ideal, but is unlikely to be
    // overly burdensome.
    return lookup(klib::string {fname});
}

FileSystem* VirtualFileSystem::lookup(klib::string& fname) const
{
    // Search for the longest match.
    klib::map<klib::string, FileSystem*>::const_iterator best = mtab.end();
    size_t best_len = 0;
    for (auto it = mtab.begin(); it != mtab.end(); ++it)
    {
        size_t len = it->first.size();
        if (len > best_len && fname.compare(0, len, it->first) == 0)
        {
            best = it;
            best_len = len;
        }
    }

    // Edit the initial string.
    fname.erase(0, best_len);
    if (fname.empty() || fname[0] != '/')
        fname = '/' + fname;

    // Return the file system.
    if (best == mtab.end() && mtab.find("/") != mtab.end())
        return mtab.find("/")->second;
    else if (best == mtab.end())
        return nullptr;
    
    return best->second;
}

FileSystem* VirtualFileSystem::lookup(klib::string&& fname) const
{
    return lookup(fname);
}

/******************************************************************************/

DevFileSystem* VirtualFileSystem::get_dev() const
{
    auto it = mtab.find("/dev");
    if (it == mtab.end())
        return nullptr;

    return dynamic_cast<DevFileSystem*>(it->second);
}

/******************************************************************************/

bool VirtualFileSystem::mount(const klib::string& mount_point,
    const klib::string& dev_name)
{
    // Get the device driver.
    DevFileSystem* devfs = get_dev();
    Device* d = devfs->get_device_driver(dev_name);
    if (d == nullptr)
        return false;

    // Check that the device is not already mounted.
    for (auto p : mtab)
        if (p.second->get_drv_name() == dev_name)
            return false;

    // Attempt to create the file system.
    FileSystem* fs = create_fs(dev_name);
    if (fs == nullptr)
        return false;

    mtab[mount_point] = fs;
    return true;
}

/******************************************************************************/

bool VirtualFileSystem::mount_virtual(const klib::string& m, FileSystem* fs)
{
    if (fs == nullptr)
        return false;

    // Create a mapping.
    mtab[m] = fs;
    return true;
}

/******************************************************************************/

void VirtualFileSystem::rename(const klib::string& f, const klib::string& n)
{
    // Lookup the file system.
    klib::string tmp {f};
    FileSystem* fs = lookup(tmp);
    fs->rename(tmp, n);
}

/******************************************************************************/

void VirtualFileSystem::umount(const klib::string& n)
{
    // Test for a mount point named n first, since that's easier.
    klib::map<klib::string, FileSystem*>::iterator it = mtab.find(n);
    if (it != mtab.end())
    {
        delete it->second;
        mtab.erase(it);
        return;
    }

    // Now search for devices.
    for (it = mtab.begin(); it != mtab.end(); ++it)
    {
        if (it->second->get_drv_name() == n)
        {
            delete it->second;
            mtab.erase(it);
            return;
        }
    }
}

/******************************************************************************
 ******************************************************************************/
