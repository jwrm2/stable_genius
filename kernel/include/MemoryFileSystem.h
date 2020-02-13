#ifndef MEMORY_FILE_SYSTEM_H
#define MEMORY_FILE_SYSTEM_H

#include <stddef.h>

#include <cstdio>
#include <map>
#include <string>

#include "FileSystem.h"

// Forward declarations.
class Directory;

/**
    A file system that exists only in memory, like the Linux tmpfs.
 */
class MemoryFileSystem : public FileSystem {
protected:
    // A simplified inode-like record for each memory file.
    struct MemoryInode
    {
        // Memory address of the start of the data. Not owned by the inode.
        void* addr;
        // Size of the memory allocation.
        size_t sz;
        // Number of hard links to the file. The file will be deleted after this
        // reaches zero.
        size_t no_links;
    };

public:
    /**
        Default constructor. Creates an empty set of mappings.
     */
    MemoryFileSystem() : FileSystem{""}, files {}
    {}

    /**
        Desctructor needs to make sure the files are all deallocated. This
        immediately deletes all memory files, regardless of whether any
        processes have them open. Attempted accesses to memory files after the
        file system has been deleted will fail, as might be expected.
     */
    virtual ~MemoryFileSystem();

    /**
        Opens a directory, returning a pointer to the directory handle. nullptr
        is returned on errors, such as the directory not existing, or the name
        referring to a file.

        @param name Full path name from the root directory of the file system.
        @return Directory pointer. nullptr on error.
     */
    virtual Directory* diropen(const klib::string& name) override;

    /**
        Opens a file, returning a pointer to the file handle. nullptr is
        returned on errors, such as the file not existing when reading is
        requested, asking for write when the file system is read only, or if the
        file is a directory.

        @param name Full path name from the root directory of the file system.
        @param mode C-style mode in which to open the file.
        @return File pointer. nullptr on error.
     */
    virtual klib::FILE* fopen(const klib::string& name, const char* mode)
        override;

    /**
        Removes (unlinks) a file. If the number of links remaining is zero, the
        file is deleted. If the file has open file descriptors, the deletion is
        postponed until the file descriptors are closed.

        @param name Full path name from the root directory of the file system.
        @return 0 on success, -1 on failure.
     */
    virtual int unlink(const klib::string& name) override;

    /**
        Rename the given file to the new given name.

        @param f File to rename.
        @param n New name for the file.
     */
    virtual void rename(const klib::string& f, const klib::string& n) override;

    /**
        Get the block size of the file system. Since this is a virtual FS, it
        doesn't have blocks and the size is therefore 1 byte.

        @return Block size.
     */
    virtual size_t block_size() const { return 1; }

    /**
        Used to notify the file system that a file is being closed. Runs any
        clean up actions necessary, like deleting memory if the number of hard
        links has been reduced to zero.

        @param addr Memory address of the file being closed.
     */
    void close(void* addr);

    /**
        Create a new memory file mapping. Does not allocate memory. Checks
        whether the memory address is already owned by another inode, and simply
        increments the hard link count if it is.

        @param name Name of the file, from the root of the file system.
        @param mi Inode for the file.
        @param addr Address for the file.
        @param sz Size of the file.
     */
    void create_mapping(const klib::string& name, MemoryInode* mi);
    void create_mapping(const klib::string& name, void* addr, size_t sz);

    /**
        Create a new memory file mapping. Allocates memory.

        @param name Name of the file, from the root of the file system.
        @param sz Size of the file, in bytes.
     */
    void create_file(const klib::string& name, size_t sz);

    /**
        Removes an existing mapping. Does not delete memory.

        @param name Name of the file, from the root of the file system.
     */
    void delete_mapping(const klib::string& name);

    /**
        Removes an exisiting mapping. Deletes memory.

        @param name Name of the file, from the root of the file system.
        @param ft_check Whether to check the file table for open handles
               deleting. This should usually be true, but is bypassed when
               calling from closing a file when the hard link count is already
               zero, as that means the delete has already been postponed due to
               active file counts and now it's time to really delete. Defaults
               to true.
     */
    void delete_file(const klib::string& name, bool ft_check = true);

    /**
        Reallocates the file size of an existing mapping. If the new size is
        smaller the file will be truncated. The file will be completely copied.

        @param name Name of the file, from the root of the file system.
        @param addr Current starting address of the file.
        @param sz New size to allocate.
        @return New start address, or nullptr on failure or if the size is 0.
     */
    void* reallocate_file(const klib::string& name, size_t sz);
    void* reallocate_file(void* addr, size_t sz);

private:
    // Amount of memory to allocate to a newly created file.
    static constexpr size_t new_file_size = 1024;

    // List of mappings between names and memory files. The file records are 
    // stored as pointers to simple inode-like objects. We use pointers so we
    // can have a many-to-one mapping for links.
    klib::map<klib::string, MemoryInode*> files;
};

#endif /* MEMORY_FILE_SYSTEM_H */
