#ifndef FILE_H
#define FILE_H

#include <stddef.h>
#include <stdint.h>

#include <cstdio>
#include <string>
#include <vector>

#include "Device.h"

// Forward declarations
class FileSystem;
class MemoryFileSystem;

/**
    Extension of FILE for character devices, such as serial ports. Contains a
    reference to the driver for the device, of type CharacterDevice, which
    implements a write_char function to send characters. The FILE is unbuffered,
    so seeks are not posible.
 */
class CharacterFile : public klib::FILE {
public:
    /**
        Constructor. Sets the connected device driver.

        @param Character device.
        @param mode C style mode string.
     */
    CharacterFile(CharacterDevice& d, const char* mode) :
        klib::FILE(mode), dev{d}
    {}

    /**
        Virtual destructor. Calls close.
     */
    virtual ~CharacterFile() { CharacterFile::close(); }

    /**
        Closes the file. Calls the devices close function to make sure the state
        is happy.

        @return 0 if successful, otherwise EoF.
     */
    virtual int close() override
    {
        dev.close();
        int ret_val = CharacterFile::flush();
        ret_val = (klib::FILE::close() == 0 ? ret_val : -1);
        return ret_val;
    }

    /**
        Writes to the file.

        @param buffer Location of the first object to be written.
        @param size Size of each object.
        @param count Number of objects to be written.
        @return Number of objects actually written.
     */
    virtual size_t write(const void* buffer, size_t size, size_t count)
        override;

    /**
        Reads from the file.

        @param buffer Location of the first object to be read into.
        @param size Size of each object.
        @param count Number of objects to be read.
        @return Number of objects actually read.
     */
    virtual size_t read(void* buffer, size_t size, size_t count) override;

    /**
        Writes any unwritten data. There is no buffer, so we just have to ask
        the character device to wait until the last character has been sent.

        @return 0 on success, otherwise EoF.
     */
    virtual int flush() override { return dev.flush(); }

    /**
        File position is meaningless for charcter devices. Always fails.

        @param pos Pointer to a position indicator. Set to -1, indicating
               failure.
        @return EOF, indicating failure.
     */
    virtual int getpos(klib::fpos_t* pos) override
    {
        *pos = klib::fpos_t {-1};
        return EOF;
    }

    /**
        File position is meaningless for character devices. Always fails.

        @param offset Number of characters to shift relative to the origin.
        @param origin Base position for the shift. May be the beginning, current
               or end position.
        @return EOF, indicating failure.
     */
    virtual int seek(long, int) override { return EOF; }

    /**
        Empties the contents of a file, without changing any of the metadata.
        Used, for example, when opening a file in "w" mode. This has no meaning
        for a character file, so does nothing successfully.

        @return 0 on success, -1 on failure.
     */
    virtual int truncate() override { return 0; }

protected:
    // Character device that handles the writes.
    CharacterDevice& dev;
};

/**
    Extension of FILE for block devices, such as hard disks. Contains a
    reference to the driver of a device, of type BlockDevice, which implements
    functions to read and write blocks. Has a buffer, which will be of the same
    size size as a block.
 */
class BlockFile : public klib::FILE {
public:
    /**
        Constructor. Sets the connected device driver. Changes to buffered mode
        and allocates a buffer. The size of the buffer is the secotr size as
        resported by the block device.

        @param Block device.
        @param mode C style open mode string.
     */
    BlockFile(BlockDevice& d, const char* mode);

    /**
        Virtual destructor. Calls close.
     */
    virtual ~BlockFile() { BlockFile::close(); }

    /**
        Closes the file. Calls the devices close function to make sure the state
        is happy.

        @return 0 if successful, otherwise EoF.
     */
    virtual int close() override
    {
        dev.close();
        int ret_val = BlockFile::flush();
        ret_val = (klib::FILE::close() == 0 ? ret_val : -1);
        return ret_val;
    }

    /**
        Writes to the file buffer. Sends to the device only if necessary.

        @param buf Location of the first object to be written.
        @param size Size of each object.
        @param count Number of objects to be written.
        @return Number of objects actually written.
     */
    virtual size_t write(const void* buf, size_t size, size_t count)
        override;

    /**
        Reads from the file buffer. Refills the buffer from the device only if
        necessary.

        @param buf Location of the first object to be read into.
        @param size Size of each object.
        @param count Number of objects to be read.
        @return Number of objects actually read.
     */
    virtual size_t read(void* buf, size_t size, size_t count) override;

    /**
        Writes any unwritten data from the output buffer to the underlying
        device. Implements fflush.

        @param stream File to flush.
        @return 0 on success, otherwise EoF.
     */
    virtual int flush() override;

    /**
        Gets the current file position indicator. Implements fgetpos.

        @param pos Pointer to a position indicator that will be set.
        @return 0 on success, nonzero otherwise.
     */
    virtual int getpos(klib::fpos_t* pos) override
    {
        *pos = position;
        return 0;
    }

    /**
        Changes the current file position indicator to that specified by the
        offset from the origin. Implements fseek.

        @param offset Number of characters to shift relative to the origin.
        @param origin Base position for the shift. May be the beginning, current
               or end position.
        @return 0 on success, nonzero otherwise.
     */
    virtual int seek(long offset, int origin) override;

    /**
        Empties the contents of a file, without changing any of the metadata.
        Used, for example, when opening a file in "w" mode. This has no meaning
        for a bloack file, so does nothing successfully.

        @return 0 on success, -1 on failure.
     */
    virtual int truncate() override { return 0; }

protected:
    // Block device that handles the reads and writes.
    BlockDevice& dev;
    // Whether data has been read into the buffer. The buffer must be up to date
    // for reads, and writes that don't cover a whole block. The buffer is not
    // filled on open or seeks, as the data may not actuallybe required.
    bool current;
};

/**
    Abstract class for files stored on disks or disk like devices. The
    implementation depends on the file system. Stores a reference to the file
    system and the file size, in addition to the klib base.
 */
class DiskFile : public klib::FILE {
public:
    /**
        Constructor. Calls the base to work out the mode. Sets the file system
        and size.

        @param m C-style file open mode.
        @param f File system the file is on.
        @param s Size of the file.
     */
    DiskFile(const char* m, FileSystem& f, klib::streamoff s);

    /**
        Virtual destructor. Calls close().
     */
    virtual ~DiskFile() { DiskFile::close(); }

    /**
        Flushes the buffer to disk. Doesn't actually do anything here, as it's
        specific to each disk type. However, it's called by klib::FILE::close(),
        which can be called from the constructor and it's pure virtual in the
        parent, so we need an implementation here.

        @return 0 on success, nonzero otherwise.
     */
    virtual int flush() override { return 0; }

    /**
        Closes the file. Calls flush() and the parent close().

        @return 0 on success, nonzero otherwise.
     */
    virtual int close() override
    {
        int ret_val = DiskFile::flush();

        ret_val = (klib::FILE::close() == 0 ? ret_val : -1);
        return ret_val;
    }

protected:
    // Reference to the file system. Used for reads and writes.
    FileSystem& fs;
    // File size.
    klib::streamoff sz;
};

/**
    Implementation of a file for a virtual file that exists in memory.
 */
class MemoryFile : public klib::FILE {
public:
    /**
        Constructor. Sets the address and size.

        @param a Address of the file start.
        @param sz Size of the file, in bytes.
        @param mode C style open mode string.
        @param fs File system the file belongs to. Necessary for size
               reallocation.
     */
    MemoryFile(void* a, size_t sz, const char* mode, MemoryFileSystem& fs);

    /**
        Virtual destructor. Calls close.
     */
    virtual ~MemoryFile() { MemoryFile::close(); }

    /**
        Closes the file. Deletes memory if there are no hard links left.

        @return 0 if successful, otherwise EoF.
     */
    virtual int close() override;

    /**
        Overwrites the file memory. Reallocates the file if too large.

        @param buf Location of the first object to be written.
        @param size Size of each object.
        @param count Number of objects to be written.
        @return Number of objects actually written.
     */
    virtual size_t write(const void* buf, size_t size, size_t count) override;

    /**
        Reads from the file memory.

        @param buf Location of the first object to be read into.
        @param size Size of each object.
        @param count Number of objects to be read.
        @return Number of objects actually read.
     */
    virtual size_t read(void* buf, size_t size, size_t count) override;

    /**
        Doesn't have to do anything. Implements fflush.

        @param stream File to flush.
        @return 0 on success, otherwise EoF.
     */
    virtual int flush() override { return 0; }

    /**
        Gets the current file position indicator. Implements fgetpos.

        @param pos Pointer to a position indicator that will be set.
        @return 0 on success, nonzero otherwise.
     */
    virtual int getpos(klib::fpos_t* pos) override
    {
        *pos = position;
        return 0;
    }

    /**
        Changes the current file position indicator to that specified by the
        offset from the origin. Implements fseek.

        @param offset Number of characters to shift relative to the origin.
        @param origin Base position for the shift. May be the beginning, current
               or end position.
        @return 0 on success, nonzero otherwise.
     */
    virtual int seek(long offset, int origin) override;

    /**
        Empties the contents of a file, without changing any of the metadata.
        Used, for example, when opening a file in "w" mode. This wipes the
        contents of the memory file, freeing the associated memory. 

        @return 0 on success, -1 on failure.
     */
    virtual int truncate() override;

protected:
    // Start address of the file.
    void* addr;
    // Size of the file.
    size_t file_size;
    // File system the file belongs to.
    MemoryFileSystem& mfs;
};

/**
    Abstract class for directories. Doesn't actually contain anything, but
    specifies a general interface for directories.
 */
class Directory {
public:
    /**
        Default constructor. The directory is not open for writing.
     */
    Directory() : writing {false} {}

    /**
        Constructor. Sets the value of writing.

        @param w Whether the diectory is open for writing.
     */
    explicit Directory(bool w) : writing {w} {}

    /**
        Virtual destructor. Calls close() to free resources.
     */
    virtual ~Directory() { Directory::close(); }

    /**
        If the directory data has been changed, write it back to the disk.

        @return 0 on success, -1 on failure.
     */
    virtual int flush() { return 0; }

    /**
        Closes the handle, freeing any resources used. Further accesses will
        fail.

        @return 0 on success, -1 on failure.
     */
    virtual int close() { return 0; }

    /**
        Returns a list of the contents of the directory.

        @return Contents of the directory.
     */
    virtual klib::vector<klib::string> ls() const = 0;

protected:
    // Whether the directory is open for writing (ie renaming/creating files).
    bool writing;
    // Whether the directory data has been edited. Used to determine whether a
    // flush is necessary.
    bool changed = false;
};

/**
    Implementation of directory for virtual file systems. A true directory
    structure doesn't exist, so just populate a vector of strings with the names
    of files that can be accessed.
 */
class VirtualDirectory : public Directory {
public:
    /**
        Constructor. Simply copies the list of files.

        @param f List of files in the directory.
     */
    VirtualDirectory(const klib::vector<klib::string>& f) : files{f} {}

    /**
        Returns a list of the contents of the directory.

        @return Contents of the directory.
     */
    virtual klib::vector<klib::string> ls() const { return files; }

protected:
    // Actual list of directory contents.
    klib::vector<klib::string> files;
};

#endif /* FILE_H */
