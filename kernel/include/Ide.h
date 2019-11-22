#ifndef IDE_H
#define IDE_H

#include <stdint.h>

#include <array>
#include <map>
#include <ostream>
#include <string>

#include "Device.h"
#include "FileSystem.h"
#include "InterruptHandler.h"
#include "Pci.h"

// Channels on the IDE bus.
enum class channel : uint8_t {
    primary = 0x0,
    secondary = 0x1
};

// Master or slave device.
enum class rank : uint8_t {
    master = 0x0,
    slave = 0x1,
    unknown = 0x2
};

/**
    Class representing an IDE controller on the PCI bus.
 */
class IdeController : public PciDevice {
public:
    /**
        Constructor. Inherited PCI constructor sets the bus, device and function
        numbers, then checks for existence.

        @param b Bus number.
        @param d Device number.
        @param f Function number.
     */
    IdeController(uint32_t b, uint32_t d, uint32_t f);

    /**
        Constructor. Extracts the bus, device and function numbers, then checks
        for existence.

        @param p Pci device to copy.
     */
    explicit IdeController(const PciDevice& other);

    /**
        Prints information on the device configuration to the provided stream.

        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

    /**
        Read sectors from the specified disk.

        @param cha Channel of the device.
        @param ra Master or slave device.
        @param off Offset (in bytes) on the device to read.
        @param addr Memory location to copy the data to.
        @param sz Number of bytes to read. Must be an integer number of sectors.
        @return Error code indicating what happened.
     */
    DiskIoError ata_read(channel cha, rank ra, uint64_t off, void* addr,
        size_t sz);

    /**
        Write sectors to the specified disk.

        @param cha Channel of the device.
        @param ra Master or slave device.
        @param off Offset (in bytes) on the device to write.
        @param addr Memory location to get the data from.
        @param sz Number of bytes to write. Must be an integer number of
               sectors.
        @return Error code indicating what happened.
     */
    DiskIoError ata_write(channel cha, rank ra, uint64_t off, const void* addr,
        size_t sz);

    /**
        Creates disk drivers for the detected devices and adds them to the list.

        @param drvs Mapping between dev names and disk drivers. This will
               already contain known devices, and the new devices will be added
               with sequentially numbered dev names.
     */
    void add_drivers(klib::map<klib::string, Device*>& drvs);

private:
    // Default values of the addresses. These are used by older parallel IDE
    // controllers. If the PCI BAR is 0x0 or 0x1, these values are used instead.
    enum class default_addr : uint16_t {
        primary_addr = 0x01F0,
        primary_control_addr = 0x03F4,
        secondary_addr = 0x0170,
        secondary_control_addr = 0x0374
    };

    // Bits of the status obtained by reading the command/status port.
    enum class status_bits : uint8_t {
        bsy = 0x80, // busy
        drdy = 0x40, // drive ready
        df = 0x20, // drive write fault
        dsc = 0x10, // drive seek complete
        drq = 0x08, // data request ready
        corr = 0x04, // corrected data
        idx = 0x02, // index
        err = 0x01 // error
    };

    // Bits of the error code obtained by reading the features/error port.
    enum class error_bits : uint8_t {
        bbk = 0x80, // bad block
        unc = 0x40, // uncorrectable data
        mc = 0x20, // media changed
        idnf = 0x10, // ID mark not found
        mcr = 0x08, // media change request
        abrt = 0x04, // command aborted
        tk0nf = 0x02, // track 0 not found
        amnf = 0x01 // no address mark
    };

    // Bits of the hddevsel port.
    enum class hddevsel_bits : uint8_t {
        // The lower four bits are used for the CHS/LBA addressing.
        // Turn this bit on for the slave device.
        slave = 0x10,
        // This bit is required to be on.
        req1 = 0x20,
        // Set this bit for LBA.
        lba = 0x40,
        // Tis bit is required to be on.
        req2 = 0x80
    };

    // Bits of the control port.
    enum class control_bits : uint8_t {
        // Set to turn off interrupts.
        nien = 0x02,
        // Set to do a software reset.
        reset = 0x04,
        // Set to set or read back the high order LBA byte.
        hob = 0x80
    };

    // Bits of the the capabilities section of the identification space.
    enum class capabilities_bits : uint16_t {
        // LBA addressing support.
        lba = 0x0200,
        // DMA support.
        dma = 0x0100
    };

    // Bits of the command sets section of the identification space.
    enum class command_set_bits : uint32_t {
        // 48-bit address feature set supported.
        extended_address = 0x04000000
    };

    // Bits of the sector properties section of the identification space.
    enum class sector_properties_bits : uint16_t {
        // Whether the sector size is specified.
        size_specified = 0x1000
    };

    // Commands to write to the command/status port.
    enum class commands : uint8_t {
        read_pio = 0x20,
        read_pio_ext = 0x24,
        read_dma = 0xC8,
        read_dma_ext = 0x25,
        write_pio = 0x30,
        write_pio_ext = 0x34,
        write_dma = 0xCA,
        write_dma_ext = 0x35,
        cache_flush = 0xE7,
        cache_flush_ext = 0xEA,
        packet = 0xA0,
        identify_packet = 0xA1,
        identify = 0xEC,
        atapi_read = 0xA8,
        atapi_eject = 0x1B
    };

    // Offests of information in the identification space.
    enum class ident_offset : uint16_t {
        device_type = 0x0,
        cylinders = 0x2,
        heads = 0x6,
        sectors = 0xC,
        serial = 0x14,
        model = 0x36,
        capabilities = 0x62,
        fieldvalid = 0x68,
        max_lba = 0x78,
        commandsets = 0xA4,
        max_lba_ext = 0xC8,
        sector_properties = 0xD4,
        sector_size = 0xEA
    };

    // Length of the model name.
    static constexpr size_t model_length = 40;
    // Sector size if not indicated in identification space.
    static constexpr uint16_t default_sector_size = 512;
    static constexpr uint16_t default_atapi_sector_size = 2048;

    // ATA or ATAPI device.
    enum class interface_type : uint8_t {
        ata = 0x0,
        atapi = 0x1
    };

    // Register offsets.
    enum class register_offset : uint16_t {
        // Offset from primary or secondary address.
        data = 0x0,
        error = 0x1,
        features = 0x1,
        seccount0 = 0x2,
        lba0 = 0x3,
        lba1 = 0x4,
        lba2 = 0x5,
        hddevsel = 0x6,
        command = 0x7,
        status = 0x7,
        // Some of the values are reused. These will need to adjusted on
        // conversion to a port number (subtract 0x6).
        seccount1 = 0x8,
        lba3 = 0x9,
        lba4 = 0xA,
        lba5 = 0xB,
        // Offsets from primary or secondary control address (subtract 0xA).
        control = 0xC,
        altstatus = 0xC,
        devaddress = 0xD,
        // bmide start at 0xE and offset from the bmide start (subtract 0xE).
        bmide_top = 0x15
    };
    // Adjustments for the above values when converting to actual port numbers.
    static constexpr uint16_t low_byte_adjust = 0x0;
    static constexpr uint16_t high_byte_adjust = 0x6;
    static constexpr uint16_t control_adjust = 0xA;
    static constexpr uint16_t bmide_adjust = 0xE;

    // Responses to polling.
    enum class polling_response : uint8_t {
        no_error = 0,
        device_fault = 1,
        error = 2,
        not_ready = 3,
        write_protected = 4,
        data_not_ready = 5
    };

    // Addressing modes.
    enum class address_mode {
        // CHS
        chs,
        // LBA28
        lba28,
        // LBA48
        lba48
    };

    // Class to store the channel register addresses.
    struct ChannelRegisters {
        // Base address of the registers.
        uint16_t base;
        // Base address of the control registers.
        uint16_t ctrl;
        // Base address of the bus master registers.
        uint16_t bmide;
        // Whether interrupts are disabled.
        bool nien;
        // Which device is currently active.
        rank dev;
    };

    // Class to store information on the devices.
    struct IdeDevice {
        // Whether there is a device connected.
        bool exists;
        // Primary or secondary channel.
        channel cha;
        // Master or slave.
        rank ra;
        // Interface type.
        interface_type type;
        // Device signature.
        uint16_t signature;
        // Device features.
        uint16_t features;
        // Device command sets.
        uint32_t command_sets;
        // Size (number of sectors).
        uint64_t size;
        // Size of a sector.
        uint16_t sector_size;
        // Name of model.
        klib::string model;
        // Whether LBA addressing is supported.
        bool lba_support;
        // Whether DMA is supported.
        bool dma_support;
    };

    // First interrupt number.
    InterruptNumber int1;
    // Second interrupt number.
    InterruptNumber int2;
    // True for serial, false for parallel.
    bool serial;
    // Registers for the two channels.
    klib::array<ChannelRegisters, 2> channels;
    // Size of the identification space (in bytes).
    static constexpr size_t ident_size = 512;
    // Indicates whether an interrupt is expected and has been invoked.
    bool irq_expected;
    bool irq_invoked;
    // Descriptions of the four drives. In order: primary master, primary slave,
    // secondary master, secondary slave.
    static constexpr size_t max_drives = 4;
    klib::array<IdeDevice, max_drives> devs;
    // Maximum number of sectors to read in one go.
    static constexpr size_t max_sectors = 256;
    // Assume heads per cylinder is 16, only necessary for LBA to CHS
    // translation.
    static constexpr size_t heads_per_cylinder = 16;
    // Assumer sectors per track is 63, only necessary for LBA to CHS
    // translation.
    static constexpr size_t sectors_per_track = 63;

    // Perform general configuration activities. Called by the constructors.
    void configure();
    // Gets the addresses of the IO ports.
    void configure_bar();
    // Works out whether this is a parallel or serial controller, based on the
    // response to setting IRQs. Sets the serial IRQ.
    void configure_irq();
    // Detects attached devices.
    void configure_devices();
    // Generates device information by reading the ATA identify information.
    void read_identify(channel cha, rank ra, char* ident_buf);
    // Generates device information by reading the ATAPI identify information.
    void read_identify_packet(channel cha, rank ra, char* ident_buf);

    // Read the specified register in the specified channel.
    uint8_t ide_read(channel cha, register_offset reg) const;
    // Write data to the specified register in the specified channel.
    void ide_write(channel cha, register_offset reg, uint8_t data) const;
    // Polling, to be used after commands. If not advanced, returns no_error
    // after BSY is clear. If advanced, check for errors and readiness after
    // BSY is clear.
    polling_response polling(channel cha, bool advanced_check) const;
    // Does the tasks common between read and write to prepare for IO.
    DiskIoError prepare_io(channel cha, rank ra, uint64_t off, size_t sz,
        address_mode& mode);
    // Inspects the error register and prints info on the error.
    void print_error(size_t drive, polling_response err) const;
    // Switches the active device on the bus, if necessary.
    polling_response switch_device(channel cha, rank ra);
};

/**
    Implementation of the abstract disk driver for PATA drives.
 */
class PataDriver : public BlockDevice {
public:
    /**
        Constructor. Sets the IDE controller, device location and sector size.

        @param c IDE controller for operations.
        @param ch Primary or secondary channel.
        @param r Master or slave device.
        @param d Description string to associate with the device.
        @param f File system type, can be left as unknown.
        @param s Sector size.
        @param sz Disk or partition size.
     */
    PataDriver(IdeController& c, channel ch, rank r, const klib::string d,
        size_t s, FileSystemType f, uint64_t sz) :
        BlockDevice {DeviceType::ata_disk, d, s, f, sz},
        cont {c},
        cha {ch},
        ra {r},
        busy {false}
    {}

    /**
        Read bytes from an offset on the disk into a memory location. A single
        block is read. The offset must be aligned.

        @param off Offset from the start of the disk to read from.
        @param addr Address in memory to put the data.
        @return Number of bytes read.
     */
    virtual size_t read_block(uint64_t off, void* addr) override;

    /**
        Write bytes from a memory location onto the disk. A single block is 
        written. The offset must be aligned.

        @param off Offset from the start of the disk to write to.
        @param addr Address in memory to get the data from.
        @return Number of bytes written.
     */
    virtual size_t write_block(uint64_t off, const void* addr) override;

    /**
        Waits until the pending write is complete. Write doesn't return until
        the write is complete, so no action is required.

        @return 0 on success, otherwise EoF.
     */
    virtual int flush() override { return 0; }

    /**
        Performs any clean up operations required. Just calls flush() for now.

        @return 0 on success, otherwise EoF.
     */
    virtual int close() override { return flush(); }

    /**
        Determines whether a poll condition is currently satisfied.

        @param cond Bitmask of polling conditions to check.
        @return Mask representing the currently true events.
     */
    virtual PollType poll_check(PollType cond) const override;

private:
    // IDE driver for read and write operations.
    IdeController& cont;
    // Bus this device is on.
    channel cha;
    // Device on the bus.
    rank ra;
    // For DMA, indicates whether the device is currently busy with a rea/write.
    bool busy;
};

#endif /* IDE_H */
