#include <stddef.h>

#include <map>
#include <string>

#include "File.h"
#include "Kernel.h"
#include "MemoryFileSystem.h"
#include "ProcTable.h"

/******************************************************************************
 ******************************************************************************/

MemoryFileSystem::~MemoryFileSystem()
{
    // Cycle over the files.
    for (auto it = files.begin(); it != files.end(); ++it)
    {
        // Decrement the hard link count for the inode.
        if (it->second->no_links != 0)
            --it->second->no_links;

        // If the link count is zero, it's time to deallocate the file and the
        // inode.
        if (it->second->no_links == 0)
        {
            delete[] static_cast<char*>(it->second->addr);
            delete it->second;
        }

        // Now we can erase the entry without losing pointers.
        it = files.erase(it);
    }
}

/******************************************************************************/

void MemoryFileSystem::close(void* addr)
{
    // Find the MemoryInode corresponding to the address.
    for (auto it = files.begin(); it != files.end(); ++it)
    {
        if (it->second->addr == addr && it->second->no_links <= 0)
        {
            // Found the file and it needs to be deleted. We can bypass the file
            // table check in delete here, as we'll only be closing a file with
            // zero hard links if we got here from the FileTable's file delete.
            delete_file(it->first, false);
        }
    }
}

/******************************************************************************/

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
    {
        if (klib::strcmp(mode, "r") == 0 || klib::strcmp(mode, "r+") == 0)
            // File does not exist and is to be opened in read mode. This is an
            // error.
            return nullptr;
        else
        {
            // Make a new file.
            create_file(name, new_file_size);
            it = files.find(name);
            if (it == files.end())
                return nullptr;
        }
    }

    return new MemoryFile {it->second->addr, it->second->sz, mode, *this};
}

/******************************************************************************/

int MemoryFileSystem::unlink(const klib::string& name)
{
    // Find the file.
    auto it = files.find(name);
    if (it == files.end())
        return -1;

    delete_file(name);
    return 0;
}

/******************************************************************************/

void MemoryFileSystem::rename(const klib::string& f, const klib::string& n)
{
    auto it = files.find(f);
    if (it == files.end())
        return;

    // Take a copy of the data pointer.
    MemoryInode* tmp = it->second;
    // Erase the old mapping.
    delete_mapping(f);
    // Create a new mapping.
    create_mapping(n, tmp);
}

/******************************************************************************/

void MemoryFileSystem::create_mapping(const klib::string& name,
    MemoryInode* mi)
{
    auto it = files.find(name);
    if (it == files.end())
    {
        // Check whether the memory address is already owned by another file.
        for (it = files.begin(); it != files.end(); ++it)
        {
            if (it->second == mi)
            {
                // Match found. Increment the hard link count and add a new
                // mapping.
                ++it->second->no_links;
                break;
            }
        }
        // Add a new mapping. We need to do this whether the mapping already
        // exists or not.
        files[name] = mi;
    }
}

void MemoryFileSystem::create_mapping(const klib::string& name, void* addr,
    size_t sz)
{
    create_mapping(name, new MemoryInode {addr, sz, 1});
}

/******************************************************************************/

void MemoryFileSystem::create_file(const klib::string& name, size_t sz)
{
    auto it = files.find(name);
    if (it == files.end())
    {
        void* addr = new char[sz];
        if (addr != nullptr)
        {
            files.emplace(name, new MemoryInode {addr, sz, 1});
        }
    }
}

/******************************************************************************/

void MemoryFileSystem::delete_mapping(const klib::string& name)
{
    // Check that the file exists.
    auto it = files.find(name);
    if (it == files.end())
        return;

    // Decrement the hard link count.
    if (it->second->no_links != 0)
        --it->second->no_links;

    // If the link count is zero, check whether there are any active file
    // handles for the file.
    if (it->second->no_links == 0)
    {
        FileTable* ft = global_kernel->get_file_table();
        if (ft == nullptr ||
            (ft != nullptr && ft->is_open(it->first) == 0))
        {
            delete it->second;
            files.erase(it);
        }
    }
}

/******************************************************************************/

void MemoryFileSystem::delete_file(const klib::string& name, bool ft_check)
{
    auto it = files.find(name);
    if (it == files.end())
        return;

    // Decrement the hard link count.
    if (it->second->no_links != 0)
        --it->second->no_links;

    // If the link count is zero, check whether there are any active file
    // handles for the file.
    if (it->second->no_links == 0)
    {
        FileTable* ft = global_kernel->get_file_table();
        // If there is no global file table, we haven't transferred to user mode
        // yet. In that case, we're using temporary files as a convenience to
        // access sequences of memory and it is safe to delete the file.
        if (!ft_check || ft == nullptr ||
            (ft != nullptr && ft->is_open(it->first) == 0))
        {
            delete[] static_cast<char*>(it->second->addr);
            delete[] it->second;
            files.erase(it);
        }
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
            delete[] static_cast<char*>(it->second->addr);
            it->second->addr = nullptr;
            it->second->sz = sz;
        }
        else
        {
            void* new_addr = new char[sz];
            size_t copy_size = klib::min(sz, it->second->sz);
            klib::memcpy(new_addr, it->second->addr, copy_size);
            delete[] static_cast<char*>(it->second->addr);
            files[name]->addr = new_addr;
            files[name]->sz = sz;
            return new_addr;
        }
    }

    return nullptr;
}

void* MemoryFileSystem::reallocate_file(void* addr, size_t sz)
{
    for (const auto& p : files)
        if (p.second->addr == addr)
            return reallocate_file(p.first, sz);

    return nullptr;
}

/******************************************************************************
 ******************************************************************************/
