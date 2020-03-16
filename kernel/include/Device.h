#ifndef DEVICE_H
#define DEVICE_H

#include <string>

// Forward declarations
enum class FileSystemType;
enum class PollType;

/**
    List of device types. See also string mappings in DevFileSystem.cpp.
 */
enum class DeviceType {
    /** ATA disk, /dev/sd* */
    ata_disk,
    /** CD-ROM, /dev/sr* */
    cdrom,
    /** Floppy disk, /dev/fd* */
    floppy_disk,
    /** Physical serial port, /dev/ttyS* */
    serial_port,
    /** Console, /dev/tty* */
    console
};

/**
    Abstract class for any device. Only stores the type.
 */
class Device {
public:
    /**
        Constructor. Sets the type.

        @param t Type of device.
     */
    Device(DeviceType t) : type {t} {}

    /**
        Virtual destructor. Does nothing.
     */
    virtual ~Device() {}

    /**
        Determines whether a poll condition is currently satisfied.

        @param cond Bitmask of polling conditions to check.
        @return Mask representing the currently true events.
     */
    virtual PollType poll_check(PollType cond) const = 0;

    /**
        Gets the type of the device.

        @return Type of device.
     */
    DeviceType get_type() const { return type; }

protected:
    DeviceType type;
};

/**
    Abstract class for character device drivers. Provides functions for an
    attached FILE object to call.
 */
class CharacterDevice : public Device {
public:
    /**
        Constructor. Just calls the parent to set the type.

        @param t Type of device.
     */
    CharacterDevice(DeviceType t) : Device {t} {}

    /**
        Virtual destructor. Base version does nothing. Derived class will
        probably want to override to call close().
     */
    virtual ~CharacterDevice() {}

    /**
        Send a character to the device port.

        @param c Character to send.
     */
    virtual void write_char(char c) = 0;

    /**
        Waits until the pending write is complete. Base version does nothing.

        @return 0 on success, otherwise EoF.
     */
    virtual int flush() = 0;

    /**
        Performs any clean up operations required. Base version does nothing.

        @return 0 on success, otherwise EoF.
     */
    virtual int close() = 0;

    /**
        Reads characters. Stops at the requested number of characters or when
        no more characters are availble.

        @param count Number of characters to read. 0 (the default) means read
               all availble characters.
        @return String containing all the characters read.
     */
    virtual klib::string read_chars(size_t count = 0) = 0;
};

/**
    Abstract class for block device drivers. Provides functions for an attached
    FILE object to call.
 */
class BlockDevice : public Device {
public:
    /**
        Constructor. Should not be called directly. Sets the description string,
        sector size, type and size.

        @param t Type of the device.
        @param d Description of the device.
        @param a Alignment/sector size
        @param f File system type, can be left as unknown.
        @param s Size of the disk or partition.
     */
    BlockDevice(DeviceType t, const klib::string& d, size_t a, FileSystemType f,
        uint64_t s) :
        Device{t}, desc{d}, s_sz{a}, fst{f}, size{s} {}

    /**
        Read bytes from an offset on the disk into a memory location. A single
        block is read. The offset must be aligned.

        @param off Offset from the start of the disk to read from.
        @param addr Address in memory to put the data.
        @return Number of bytes read.
     */
    virtual size_t read_block(uint64_t off, void* addr) = 0;

    /**
        Write bytes from a memory location onto the disk. A single block is 
        written. The offset must be aligned.

        @param off Offset from the start of the disk to write to.
        @param addr Address in memory to get the data from.
        @return Number of bytes written.
     */
    virtual size_t write_block(uint64_t off, const void* addr) = 0;

    /**
        Waits until the pending write is complete.

        @return 0 on success, otherwise EoF.
     */
    virtual int flush() = 0;

    /**
        Performs any clean up operations required.

        @return 0 on success, otherwise EoF.
     */
    virtual int close() = 0;

    /**
        Prints the description string, offset and size to the provided stream.

        @param dest Stream to print information to.
     */
    virtual void dump(klib::ostream& dest) const;

    /**
        Gives the necessary alignment of the offset and size, probably a sector
        size.

        @return Alignment size.
     */
    virtual size_t get_alignment() const { return s_sz; }

    /**
        Gives the description string associated with the device.

        @return Decription string for this device.
     */
    virtual const klib::string& get_description() const { return desc; }

    /**
        Sets the description string associated with the device.

        @param d Decription string for this device.
     */
    virtual void set_description(const klib::string& d) { desc = d; }

    /**
        Gets the file system type, as best the driver currently knows.

        @return File system type.
     */
    virtual FileSystemType get_fst() const { return fst; }

    /**
        Sets the file system type.

        @param f File system type to set.
     */
    virtual void set_fst(FileSystemType f) { fst = f; }

    /**
        Gives the size of this disk or partition, in bytes.

        @return Device size.
     */
    virtual uint64_t get_size() const { return size; }

    /**
        Gives the sector size.

        @return Sector size.
     */
    virtual size_t sector_size() const { return s_sz; }

protected:
    // String with a description of the device.
    klib::string desc;
    // Size of a sector.
    size_t s_sz;
    // File system type existing on the device.
    FileSystemType fst;
    // Size of the device, in bytes.
    uint64_t size;
};

/**
    Possible errors to return from read and write operations.
 */
enum class DiskIoError {
    /** Success. */
    success = 0,
    /** Bad offset alignment. */
    bad_alignment = 1,
    /** Bad size. */
    bad_size = 2,
    /** Writing to a read only disk. */
    read_only = 3,
    /** Hardware fault. */
    hardware_fault = 4,
    /** Device does not exist. */
    no_device = 5,
    /** Using the wrong driver for the device. */
    bad_driver = 6
};

/**
    Translates an error code into a printable string and sends it to the
    provided stream.

    @param dest Stream to send the error message to.
    @param err Error code.
 */
void print_DiskIoError(klib::ostream& dest, DiskIoError err);

#endif /* DEVICE_H */
