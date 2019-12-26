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

void FileSystem::read(uint64_t offset, char* buf, size_t n)
{
    klib::ifstream in {drv_name};
    in.seekg(offset);
    in.read(buf, n);
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

    // Deal with mode specific actions.
    if (klib::strcmp(mode, "w") == 0|| klib::strcmp(mode, "w+") == 0)
        f->truncate();
    if (klib::strcmp(mode, "a") == 0 || klib::strcmp(mode, "a+") == 0)
        klib::fseek(f, 0, SEEK_END);

    return f;
}

/******************************************************************************/

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

    // Strictly unsafe, but I hope I haven't been foolish enough to mount
    // something else at /dev. Can't dynamic cast as the vmi_class_type_info
    // not exist.
    return static_cast<DevFileSystem*>(it->second);
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

Directory* MemoryFileSystem::diropen(const klib::string& name)
{
    // tmpfs does not have a true directory structure, but the names of files
    // might contain /, indicating a hierarchy. Make a list of file that have
    // the provided name as the prefix, but with anything after the next '/'
    // stripped.
    klib::vector<klib::string> match_files;

    // Get rid of leading slashes in name.
    klib::string test {name};
    while (test[0] == '/')
        test.erase(test.begin());

    // Create a list of file names.
    klib::vector<klib::string> all_files;
    for (auto p : files)
        all_files.push_back(p.first);

    for (klib::string& temp : all_files)
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
        match_files.push_back(temp);
    }

    // Create a directory to return.
    return new VirtualDirectory {match_files};
}

/******************************************************************************/

klib::FILE* MemoryFileSystem::fopen(const klib::string& name, const char* mode)
{
    auto it = files.find(name);
    if (it == files.end())
        return nullptr;

    return new MemoryFile {it->second.first, it->second.second, mode, *this};
}

/******************************************************************************/

void MemoryFileSystem::rename(const klib::string& f, const klib::string& n)
{
    auto it = files.find(f);
    if (it == files.end())
        return;

    // Take a copy of the data.
    klib::pair<void*, size_t> tmp = it->second;
    // Erase the old mapping.
    delete_mapping(f);
    // Create a new mapping.
    create_mapping(n, tmp.first, tmp.second);
}

/******************************************************************************/

void MemoryFileSystem::create_mapping(const klib::string& name, void* addr,
    size_t sz)
{
    auto it = files.find(name);
    if (it == files.end())
        files[name] = klib::pair<void*, size_t> {addr, sz}; 
}

/******************************************************************************/

void* MemoryFileSystem::create_file(const klib::string& name, size_t sz)
{
    auto it = files.find(name);
    if (it == files.end())
    {
        void* addr = new char[sz];
        if (addr != nullptr)
        {
            files[name] = klib::pair<void*, size_t> {addr, sz};
            return addr;
        }
    }

    return nullptr;
}

/******************************************************************************/

void MemoryFileSystem::delete_mapping(const klib::string& name)
{
    files.erase(name);
}

/******************************************************************************/

void MemoryFileSystem::delete_file(const klib::string& name)
{
    auto it = files.find(name);
    if (it != files.end())
    {
        delete[] static_cast<char*>(it->second.first);
        files.erase(it);
    }
}

/******************************************************************************/

void* MemoryFileSystem::reallocate_file(const klib::string& name, size_t sz)
{
    auto it = files.find(name);
    if (it != files.end())
    {
        if (sz == 0)
        {
            delete[] static_cast<char*>(it->second.first);
            files[name] = klib::pair<void*, size_t> {nullptr, 0u};
        }
        else
        {
            void* new_addr = new char[sz];
            size_t copy_size = klib::min(sz, it->second.second);
            klib::memcpy(new_addr, it->second.first, copy_size);
            delete[] static_cast<char*>(it->second.first);
            files[name] = klib::pair<void*, size_t> {new_addr, sz};
            return new_addr;
        }
    }

    return nullptr;
}

void* MemoryFileSystem::reallocate_file(void* addr, size_t sz)
{
    for (const auto& p : files)
        if (p.second.first == addr)
            return reallocate_file(p.first, sz);

    return nullptr;
}

/******************************************************************************
 ******************************************************************************/
