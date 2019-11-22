#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdint.h>

#include <cstdio>
#include <map>
#include <string>

// Forward declarations.
class BlockDevice;
class DevFileSystem;
class Directory;

/**
    Possible file system types, including some more generic definitions. The
    disk or partition driver stores one of these as a hint for determining the
    file system when it is mounted.
 */
enum class FileSystemType {
    /** Unknown, to be later determined. */
    unknown,
    /** Unknown, determination failed. */
    failed,
    /** None, eg. for a disk rather than a partition. */
    none,
    /** Generic ext, version to be determined. */
    ext,
    /** Second extended file system. */
    ext2
};

/**
    Abstract class for a file system. Has pure virtual methods for common
    operations.
 */
class FileSystem {
public:
    /**
        Constructor. Just sets the underlying disk driver and the /dev name
        of the device.

        @param n /dev name of the device.
        @param ro Whether the file system is read only.
     */
    FileSystem(const klib::string& n, bool ro = false) :
        drv_name{n}, read_only{ro}
    {}

    /**
        Destructor doesn't need to do anything here, since the DeviceDriver
        should not be freed, but here to make it virtual.
     */
    virtual ~FileSystem() {}

    /**
        Opens a directory, returning a pointer to the directory handle. nullptr
        is returned on errors, such as the directory not existing, or the name
        referring to a file.

        @param name Full path name from the root directory of the file system.
        @return Directory pointer. nullptr on error.
     */
    virtual Directory* diropen(const klib::string& name) = 0;

    /**
        Opens a file, returning a pointer to the file handle. nullptr is
        returned on errors, such as the file not existing when reading is
        requested, asking for write when the file system is read only, or if the
        file is a directory.

        @param name Full path name from the root directory of the file system.
        @param mode C-style mode in which to open the file.
        @return File pointer. nullptr on error.
     */
    virtual klib::FILE* fopen(const klib::string& name, const char* mode) = 0;

    /**
        Reads some characters from the underlying device into the buffer.

        @param offset Position on the disk to start reading from.
        @param buf Character buffer to read into.
        @param n Number of characters to read.
     */
    virtual void read(uint64_t offset, char* buf, size_t n);

    /**
        Rename the given file to the new given name.

        @param f File to rename.
        @param n New name for the file.
     */
    virtual void rename(const klib::string& f, const klib::string& n) = 0;

    /**
        Write some characters from the buffer into the file.

        @param offset Position on the disk to start writing to.
        @param buf Character buffer to get the data from.
        @param n Number of characters to write.
     */
    virtual void write(uint64_t offset, const char* buf, size_t n = 0);

    /**
        Get the /dev name of the underlying device. This might be blank for a
        virtual file system.

        @return The underlying device name, or blank if it does not exist.
     */
    virtual const klib::string& get_drv_name() const { return drv_name; }

    /**
        Whether the file system is read only.

        @return True if the file system is read-only, false otherwise.
     */
    virtual bool ro() const { return read_only; }

    /**
        Get the block size of the file system.

        @return Block size.
     */
    virtual size_t block_size() const = 0;

protected:
    // /dev name of the underlying device.
    klib::string drv_name;
    // Whether the file system is read only.
    bool read_only;
};

/**
    Helper function to create a file system. Reads the device driver to try to
    narrow down the file system type, then attempts to determine what the file
    system is and create a driver for it. Returns a nullptr if unsuccessful.

    @param drv Device to create a file system for.
    @param n /dev name of the device.
    @return File system driver for the device, or nullptr for failure.
 */
FileSystem* create_fs(const klib::string& n);

/**
    This is the kernel's overall VFS, not the more general concept of a file
    system existing virtually. Contains mappings from directory names to mounted
    devices and special file systems like /dev.
 */
class VirtualFileSystem : public FileSystem {
public:
    /**
        Default constructor. Creates an empty set of mappings.
     */
    VirtualFileSystem() : FileSystem{""}, mtab {}
    {}

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
        Get the block size of the file system. Since this is a virtual FS, it
        doesn't have blocks and the size is therefore 1 byte.

        @return Block size.
     */
    virtual size_t block_size() const { return 1; }

    /**
        Given a file name, searches for the deepest matching mapping and then
        returns the file system the file exists on. The original file name is
        adjusted to be relative to the root of the device.

        @param f Absolute path name to look up. Gets modified to the file name
               relative to the root of the device.
        @return File system the requested file resides on.
     */
    FileSystem* lookup(klib::string& f) const;
    FileSystem* lookup(klib::string&& f) const;

    /**
        Gets the /dev file system driver. Often needed for getting device
        drivers.

        @return Pointer to /dev file system, or nullptr if it doesn't exist.
     */
    DevFileSystem* get_dev() const;

    /**
        Add a mapping between a directory name and a device. Basically mounts a
        drive. Looks up the drive in the /dev file system. Fails if the device
        does not exist or if the mapping already exists. Attempts to read the
        file system for the device.

        @param mount_point Mount point for the drive.
        @param dev_name /dev name of device to map to.
        @return Whether the mount succeeded.
     */
    bool mount(const klib::string& mount_point, const klib::string& dev_name);

    /**
        Mount a virtual file system, ie one for which there is no underlying
        DiskDriver.

        @param m Mount point for the drive.
        @param fs FileSystem to mount.
        @return Whether the mount succeeded.
     */
    bool mount_virtual(const klib::string& m, FileSystem* fs);

    /**
        Rename the given file to the new given name.

        @param f File to rename.
        @param n New name for the file.
     */
    virtual void rename(const klib::string& f, const klib::string& n) override;

    /**
        Remove a mapping between a directory name and a device. Does nothing if
        the mapping does not exist. Either the directory name or the device name
        may be provided.

        @param n Name of mount point or device to unmap.
     */
    void umount(const klib::string& n);

private:
    // List of mappings between mount points and file systems. Device drivers
    // are accessed through the /dev/ file system.
    klib::map<klib::string, FileSystem*> mtab;
};

/**
    A file system that exists only in memory, like the Linux tmpfs.
 */
class MemoryFileSystem : public FileSystem {
public:
    /**
        Default constructor. Creates an empty set of mappings.
     */
    MemoryFileSystem() : FileSystem{""}, files {}
    {}

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
        Get the block size of the file system. Since this is a virtual FS, it
        doesn't have blocks and the size is therefore 1 byte.

        @return Block size.
     */
    virtual size_t block_size() const { return 1; }

    /**
        Rename the given file to the new given name.

        @param f File to rename.
        @param n New name for the file.
     */
    virtual void rename(const klib::string& f, const klib::string& n) override;

    /**
        Create a new memory file mapping. Does not allocate memory.

        @param name Name of the file, from the root of the file system.
        @param addr Start address of the file in memory.
        @param sz Size of the file, in bytes.
     */
    void create_mapping(const klib::string& name, void* addr, size_t sz);

    /**
        Create a new memory file mapping. Allocates memory.

        @param name Name of the file, from the root of the file system.
        @param sz Size of the file, in bytes.
        @return Start address, or nullptr on failure.
     */
    void* create_file(const klib::string& name, size_t sz);

    /**
        Removes an exisiting mapping. Does not delete memory.

        @param name Name of the file, from the root of the file system.
     */
    void delete_mapping(const klib::string& name);

    /**
        Removes an exisiting mapping. Deletes memory.

        @param name Name of the file, from the root of the file system.
     */
    void delete_file(const klib::string& name);

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
    // List of mappings bgetween names and memory files. The files are stored as
    // pairs of memory addresses and offsets. The names are from the root of the
    // file system.
    klib::map<klib::string, klib::pair<void*, size_t>> files;
};

#endif /* FILE_SYSTEM_H */
