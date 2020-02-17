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
        Makes a new directory.

        @param pathname Absolute path for the new directory.
        @param mode Permissions for the new directory. TODO permissions don't
               exist yet.
        @return 0 on success, -1 on falure.
     */
    virtual int mkdir(const klib::string& name, int mode) = 0;

    /**
        Reads some characters from the underlying device into the buffer.

        @param offset Position on the disk to start reading from.
        @param buf Character buffer to read into.
        @param n Number of characters to read.
        @return Number of character actually read.
     */
    virtual size_t read(uint64_t offset, char* buf, size_t n);

    /**
        Rename the given file to the new given name.

        @param f File to rename.
        @param n New name for the file.
     */
    virtual void rename(const klib::string& f, const klib::string& n) = 0;

    /**
        Removes (unlinks) a directory. If the number of links remaining is zero,
        the directory is deleted. If the directory has open file descriptors,
        the deletion is postponed until the file descriptors are closed. Fails
        if the directory is not empty.

        @param name Full path name from the root directory of the file system.
        @return 0 on success, -1 on failure.
     */
    virtual int rmdir(const klib::string& name) = 0;

    /**
        Removes (unlinks) a file. If the number of links remaining is zero, the
        file is deleted. If the file has open file descriptors, the deletion is
        postponed until the file descriptors are closed.

        @param name Full path name from the root directory of the file system.
        @return 0 on success, -1 on failure.
     */
    virtual int unlink(const klib::string& name) = 0;

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
        Makes a new directory.

        @param name Absolute path for the new directory.
        @param mode Permissions for the new directory. TODO permissions don't
               exist yet.
        @return 0 on success, -1 on falure.
     */
    virtual int mkdir(const klib::string& name, int mode) override;

    /**
        Rename the given file to the new given name.

        @param f File to rename.
        @param n New name for the file.
     */
    virtual void rename(const klib::string& f, const klib::string& n) override;

    /**
        Removes (unlinks) a directory. If the number of links remaining is zero,
        the directory is deleted. If the directory has open file descriptors,
        the deletion is postponed until the file descriptors are closed. Fails
        if the directory is not empty.

        @param name Full path name from the root directory of the file system.
        @return 0 on success, -1 on failure.
     */
    virtual int rmdir(const klib::string& name) override;

    /**
        Removes (unlinks) a file. If the number of links remaining is zero, the
        file is deleted. If the file has open file descriptors, the deletion is
        postponed until the file descriptors are closed. Works out the file
        system on which the file resides, then passes the call onto that file
        system.

        @param name Full path name to remove.
        @return 0 on success, -1 on failure.
     */
    virtual int unlink(const klib::string& name) override;

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
               relative to the root of the device in the non-const version.
        @return File system the requested file resides on.
     */
    FileSystem* lookup(const klib::string& f) const;
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

#endif /* FILE_SYSTEM_H */
