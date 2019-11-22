#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stddef.h>
#include <stdint.h>

#include <array>
#include <ostream>
#include <string>
#include <vector>

#include "AOut.h"
#include "Elf.h"
#include "Vbe.h"

// Forward declarations.
class PageDescriptorTable;

/**
    Specifies the boot device used.
 */
class MultiBootDev {
public:
    /**
        Unpacks the boot loader generated information into this nice structure.

        @param start Pointer to the beginning of the information. The whole
                     thing should be 4 bytes long.
     */
    explicit MultiBootDev(const uint32_t* start);

    /**
        Default constructor, sets everything to zero and valid to false.
     */
    MultiBootDev() : val{false}, p3{0}, p2{0}, p1{0}, drv{0} {}

    /**
        Check whether the structure is valid. Not all possible values of the
        data are allowed, so this serves as a check on whether the boot loader
        is behaving sensibly.

        @return Whether the data structure is valid.
     */
    bool valid() const { return val; }

    /**
        Get the third partition specifier.

        @return Index of the second sub partition.
     */
    uint8_t part3() const { return p3; }

    /**
        Get the second partition specifier.

        @return Index of the first sub partition.
     */
    uint8_t part2() const { return p2; }

    /**
        Get the first partition specifier.

        @return Index of the main partition.
     */
    uint8_t part1() const { return p1; }

    /**
        Get the drive.

        @return Drive descriptor using BIOS int 0x13 labels.
     */
    uint8_t drive() const { return drv; }

private:
    // Whether the structure has valid values
    bool val;
    // Partition numbers
    uint8_t p3;
    uint8_t p2;
    uint8_t p1;
    // Drive number, according to BIOS scheme
    uint8_t drv;
};

/**
    Describes a boot module.
 */
class MultiBootModule {
public:
    /**
        Size in bytes of a module record.
     */
    static constexpr size_t size = 16;

    /**
        Unpacks the boot loader generated information into this nice structure
        and copies the module binary from it's bootloaded physical location to
        the heap.

        @param start Pointer to the beginning of the information. The whole
                     thing should be 16 bytes long, although the final four are
                     ignored.
        @param pdt Page Descriptor Table for setting up virtual mappings.
     */
    MultiBootModule(const uint32_t* start, PageDescriptorTable& pdt);

    /**
        Default constructor, sets everything to zero and valid to false.
     */
    MultiBootModule() : val{false}, str{""}, mstart{nullptr}, mend{nullptr} {}

    /**
        Copy constructor. We need to copy the module binary.

        @param other Object to copy from.
     */
    MultiBootModule(const MultiBootModule& other);

    /**
        Move constructor. We need can pirate the module binary.

        @param other Object to steal from.
     */
    MultiBootModule(MultiBootModule&& other);

    /**
        Destructor. We need to free the space we made on the heap for the
        module binary.
     */
    ~MultiBootModule() { delete[] static_cast<uint8_t*>(mstart); }

    /**
        Copy assignemnt. We need to copy the module binary.

        @param other Object to copy from.
     */
    MultiBootModule& operator=(const MultiBootModule& other);

    /**
        Move constructor. We need can pirate the module binary.

        @param other Object to steal from.
     */
    MultiBootModule& operator=(MultiBootModule&& other);

    /**
        Check whether the structure is valid. Not all possible values of the
        data are allowed, so this serves as a check on whether the boot loader
        is behaving sensibly.

        @return Whether the data structure is valid.
     */
    bool valid() const { return val; }

    /**
        Get the address of the start of the module.

        @return Physical address of the start of the module.
     */
    const void* mod_start() const { return mstart; }

    /**
        Get the address of the end of the module.

        @return Physical address of the end of the module.
     */
    const void* mod_end() const { return mend; }

    /**
        Get the size, in bytes, of the module.

        @return Size of the module binary.
     */
    size_t mod_size() const
    {
        return static_cast<uint8_t*>(mend) - static_cast<uint8_t*>(mstart);
    }

    /**
        Get the string associated with the module (eg a command line).

        @return String associated with the module.
     */
    const klib::string& mod_string() const { return str; }

    /**
        Interpret the start address as a function pointer.

        @param T Type of function to cast to.
        @return A function pointer of type T.
     */
    template<typename T>
    T get_func() const { return reinterpret_cast<T>(mstart); }

private:
    // Whether the structure has valid values
    bool val;
    // Arbitrary string, eg a command line
    klib::string str;
    // Start and end addresses
    void* mstart;
    void* mend;
};

/**
    An entry in the BIOS provided memory map.
 */
class MultiBootMap {
public:
    /**
        Unpacks the boot loader generated information into this nice structure.

        @param start Pointer to the beginning of the information. The whole
                     thing should be at least 20 bytes long.
     */
    explicit MultiBootMap(const uint32_t* start);

    /**
        Default constructor, sets everything to zero and valid to false.
     */
    MultiBootMap()
        : val{false},
          addr{nullptr},
          len{0},
          mtype{MemType::invalid}
    {}

    /**
        Check whether the structure is valid. Not all possible values of the
        data are allowed, so this serves as a check on whether the boot loader
        is behaving sensibly.

        @return Whether the data structure is valid.
     */
    bool valid() const { return val; }

    /**
        Get base address of the memory section this entry refers to.

        @return Address of the memory section.
     */
    void* base_addr() const { return addr; }

    /**
        Get the length of the memory section this entry refers to.

        @return Length of the memory section (in bytes).
     */
    size_t length() const { return len; }

    /**
        Possible types of memory the memory map can refer to.
     */
    enum MemType {
        /** Available RAM. */
        available = 1,
        /** Usable memory with ACPI information. */
        acpi = 3,
        /** Reserved memory to be preserved on hibernation. */
        hibernate = 4,
        /** Defective RAM */
        defective = 5,
        /** Reserved memory */
        reserved,
        /** Invalid */
        invalid = 0xBAD
    };

    /**
        Get the type of memory this entry refers to.

        @return The type of memory.
     */
    MemType type() const { return mtype; }

    /**
        Dump information on this entry to the provided stream.

        @param dest Stream to write to.
     */
    void dump(klib::ostream& dest) const;

    /**
        Field headers and widths for dumping.
     */
    static constexpr size_t field_number = 4;
    static klib::array<const char* const, field_number> field_headers;
    static klib::array<const size_t, field_number> field_widths;

private:
    // Whether the structure has valid values
    bool val;
    // Size of entry
    size_t sz;
    // Base address of memory section
    void* addr;
    // Length of memory section
    size_t len;
    // Type of memory
    MemType mtype;
};

/**
    An entry in the BIOS provided list of drives.
 */
class MultiBootDrive {
public:
    /**
        Unpacks the boot loader generated information into this nice structure.

        @param start Pointer to the beginning of the information. The whole
                     thing should be at least 10 bytes long.
     */
    explicit MultiBootDrive(const uint32_t* start);

    /**
        Default constructor, sets everything to zero and valid to false.
     */
    MultiBootDrive()
        : val{false},
          sz{0},
          no{0},
          mode{AccessMode::invalid},
          cyl{0},
          head{0},
          sect{0},
          ioports{} {}

    /**
        Check whether the structure is valid. Not all possible values of the
        data are allowed, so this serves as a check on whether the boot loader
        is behaving sensibly.

        @return Whether the data structure is valid.
     */
    bool valid() const { return val; }

    /**
        Size of this entry (at least 10 bytes).

        @return Size of the entry (in bytes).
     */
    size_t size() const { return sz; }

    /**
        The drive number this entry refers to.

        @return Drive number, as used by BIOS int 0x13.
     */
    uint8_t drive_number() const { return no; }

    /**
        Possible modes for accessing the drive.
     */
    enum class AccessMode {
        // Cylinder, Head, Sector (old school)
        chs,
        // Logical Block Addressing (modern)
        lba,
        // Invalid
        invalid
    };

    /**
        Get the access mode used by the bootloader for this drive.

        @return Access mode for the drive.
     */
    AccessMode drive_mode() const { return mode; }

    /**
        Get the number of cylinders in the drive, as detected by the BIOS.

        @return Number of cylinders.
     */
    uint16_t drive_cylinders() const { return cyl; }

    /**
        Get the number of heads in the drive, as detected by the BIOS.

        @return Number of heads.
     */
    uint8_t drive_heads() const { return head; }

    /**
        Get the number of sectors in the drive, as detected by the BIOS.

        @return Number of sectors.
     */
    uint8_t drive_sectors() const { return sect; }

    /**
        Get the IO port at index n for accessing the drive, or 0 if the index is
        out of range.

        @param n Index of port to get.
        @return IO port at index n, or 0 if out of range.
     */
    uint16_t drive_ports(size_t n) const
    {
        return (n < ioports.size() ? ioports[n] : n);
    }

private:
    // Whether the data is valid.
    bool val;
    // Size of entry
    size_t sz;
    // Drive number, in BIOS int 0x13 style
    uint8_t no;
    // Drive access mode
    AccessMode mode;
    // Number of cylinders, heads and sectors
    uint16_t cyl;
    uint8_t head;
    uint8_t sect;
    // The list of IO ports.
    klib::vector<uint16_t> ioports;
};

/**
    Data provided by the BIOS on it's own configuration. There's a lot of info
    here, but I'm not sure any of it is very relevant. Seems like GRUB doesn't
    provide this anyway. Just store the data and provide access.
 */
class MultiBootRom {
public:
    /**
        Constructor. Copy the data into the data vector.

        @param addr Address to copy the data from.
     */
    MultiBootRom(const uint32_t* addr);

    /**
        Default constructor. Set to invalid.
     */
    MultiBootRom() : val{false} {}

    /**
        Access to the data, indexed by byte. Unchecked access.

        @param n Index of the byte to access.
     */
    uint8_t operator[](size_t n) const
    {
        return static_cast<uint8_t>(data[n / sizeof(uint32_t)] >>
            ((n % sizeof(uint32_t) * 8)));
    }
private:
    // Whether the information is valid.
    bool val;
    // Bytewise store of the data. The first two are the make and model. Then
    // there's the BIOS version number, followed by 5 feature bytes and various
    // system dependent stuff.
    klib::vector<uint32_t> data;
};

/**
    An Advanced Power Managment information table.
 */
class MultiBootApm {
public:
    /**
        Size (in bytes) of the table, when packed in memory.
     */
    static constexpr size_t size = 20;

    /**
        Unpacks the boot loader generated information into this nice structure.

        @param start Pointer to the beginning of the information. The whole
                     thing should be 20 bytes long.
     */
    explicit MultiBootApm(const uint32_t* start);

    /**
        Default constructor, sets everything to zero and valid to false.
     */
    MultiBootApm()
        : val{false},
          ver{0},
          code_seg{0},
          off{nullptr},
          code_seg_16{0},
          data_seg{0},
          flag{0},
          code_seg_len{0},
          code_seg_16_len{0},
          data_seg_len{0}
    {}

    /**
        Check whether the structure is valid. Not all possible values of the
        data are allowed, so this serves as a check on whether the boot loader
        is behaving sensibly.

        @return Whether the data structure is valid.
     */
    bool valid() const { return val; }

    /**
        Get the APM version number.

        @return Version number.
     */
    uint16_t version() const { return ver; }

    /**
        Get the 32 bit protected mode code segment.

        @return 32 bit protected mode code segment.
     */
    uint16_t cseg() const { return code_seg; }

    /**
        Get the offset of the entry point.

        @return Entry point offset.
     */
    const void* offset() const { return off; }

    /**
        Get the 16 bit protected mode code segment.

        @return 16 bit protected mode code segment.
     */
    uint16_t cseg_16() const { return code_seg_16; }

    /**
        Get the 16 bit protected mode data segment.

        @return 16 bit protected mode data segment.
     */
    uint16_t dseg() const { return data_seg; }

    /**
        Get the APM flags.

        @return APM flags.
     */
    uint16_t flags() const { return flag; }

    /**
        Get the length of the 32 bit protected mode code segment.

        @return Length of the 32 bit protected mode code segment.
     */
    uint16_t cseg_len() const { return code_seg_len; }

    /**
        Get the length of the 16 bit protected mode code segment.

        @return Length of the 16 bit protected mode code segment.
     */
    uint16_t cseg_16_len() const { return code_seg_16_len; }

    /**
        Get the length of the 16 bit protected mode data segment.

        @return Length of the 16 bit protected mode data segment.
     */
    uint16_t dseg_len() const { return data_seg_len; }

    /**
        Prints the information to the designated stream.

        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

private:
    // Whether the data is valid.
    bool val;
    // Version number
    uint16_t ver;
    // 32 bit code segment
    uint16_t code_seg;
    // Entry point offset
    void* off;
    // 16 bit code segment
    uint16_t code_seg_16;
    // 16 bit data segment
    uint16_t data_seg;
    // APM flags
    uint16_t flag;
    // 32 bit code segment length
    uint16_t code_seg_len;
    // 16 bit code segment length
    uint16_t code_seg_16_len;
    // 16 bit data segment length
    uint16_t data_seg_len;
};

/**
    Information on the VESA BIOS extensions.
 */
class MultiBootVbe {
public:
    /**
        Unpacks the boot loader generated information into this nice structure.

        @param start Pointer to the beginning of the information. The whole
                     thing should be 16 bytes long.
        @param pdt Page Descriptor Table for making virtual mappings.
     */
    explicit MultiBootVbe(const uint32_t* start, PageDescriptorTable& pdt);

    /**
        Default constructor, sets everything to zero and valid to false.
     */
    MultiBootVbe()
        : val{false},
          cntrl{},
          minfo{},
          m{0},
          int_seg{0},
          int_off{0},
          int_len{0}
    {}

    /**
        Check whether the structure is valid. Not all possible values of the
        data are allowed, so this serves as a check on whether the boot loader
        is behaving sensibly.

        @return Whether the data structure is valid.
     */
    bool valid() const { return val; }

    /**
        Get the control info as generated by VBE function 0x00.

        @return Reference to the VBE control information.
     */
    const VbeCntrl& control_info() const { return cntrl; }

    /**
        Get the address of the mode info as generated by VBE function 0x01.

        @return Address of VBE mode information.
     */
    const VbeMode& mode_info() const { return minfo; }

    /**
        Get the mode in VBE 3.0 format.

        @return VBE mode.
     */
    uint16_t mode() const { return m; }

    /**
        Get the VBE 2.0 protected mode interface segment, or zero if not
        available.

        @return Interface segment.
     */
    uint16_t interface_seg() const { return int_seg; }

    /**
        Get the VBE 2.0 protected mode interface offset, or zero if not
        available.

        @return Interface offset.
     */
    uint16_t interface_off() const { return int_off; }

    /**
        Get the VBE 2.0 protected mode interface length, or zero if not
        available.

        @return Interface length.
     */
    uint16_t interface_len() const { return int_len; }

    /**
        Prints the information to the designated stream.

        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

private:
    // Whether the data is valid.
    bool val;
    // Control info table.
    VbeCntrl cntrl;
    // Mode info table.
    VbeMode minfo;
    // Current Mode.
    uint16_t m;
    // Interface segment.
    uint16_t int_seg;
    // Interface offset.
    uint16_t int_off;
    // Interface length.
    uint16_t int_len;
};

/**
    A colour member of the indexed colour palette.
 */
class MultiBootColor {
public:
    /**
        Size when packed, in bytes.
     */
    static constexpr size_t size = 3;

     /**
        Unpacks the boot loader generated information into this nice structure.

        @param start Pointer to the beginning of the information. The whole
                     thing should be 3 bytes long.
     */
    explicit MultiBootColor(const uint8_t* start)
    {
        data = start[0] + (start[1] << 8) + (start[2] << 16);
    }

    /**
        Default constructor, sets everything to zero.
     */
    MultiBootColor() : data{0} {}

    /**
        Get the red value.

        @return Red value.
     */
    uint8_t red_value() const { return static_cast<uint8_t>(data & 0xFF); }

    /**
        Get the green value.

        @return Green value.
     */
    uint8_t green_value() const
    {
        return static_cast<uint8_t>((data >> 8) & 0xFF);
    }

    /**
        Get the Blue value.

        @return Blue value.
     */
    uint8_t blue_value() const
    {
        return static_cast<uint8_t>((data >> 16) & 0xFF);
    }

    /**
        Print usefult information to the provides stream.

        @param dest Stream to send output to.
     */
    void dump(klib::ostream& dest) const;

private:
    uint32_t data;
};

/**
    Framebuffer indexed colour palette.
 */
class MultiBootIndex {
public:
    /**
        Unpacks the boot loader generated information into this nice structure.

        @param start Pointer to the beginning of the information. The whole
                     thing should be 6 bytes long.
        @param pdt Page Decsriptor Table for setting up virtual mappings.
     */
    MultiBootIndex(const uint8_t* start, PageDescriptorTable& pdt);

    /**
        Default constructor, sets everything to zero and valid to false.
     */
    MultiBootIndex() : val{false}, cols{}
    {}

    /**
        Check whether the structure is valid. Not all possible values of the
        data are allowed, so this serves as a check on whether the boot loader
        is behaving sensibly.

        @return Whether the data structure is valid.
     */
    bool valid() const { return val; }

    /**
        Colour in the palette at the specified index, or black (all zeros) if
        the index is out of range.

        @param n Index of colour to access.
        @return A colour.
     */
    const MultiBootColor& palette(size_t n) const
    {
        return (n >= cols.size() ? cols[cols.size() - 1] : cols[n]);
    }

    /**
        Get the number of colours in the palette.

        @return Number of colours.
     */
    uint16_t num_colors() const { return cols.size() - 1; }

    /**
        Print useful information to the provided stream.

        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

private:
    // Whether the structure has valid values
    bool val;
    // The list of colours in the palette, including invalid (black) at the end.
    klib::vector<MultiBootColor> cols;
};

/**
    RGB framebuffer colour information.
 */
class MultiBootRgb {
public:
    /**
        Size when packed, in bytes.
     */
    static constexpr size_t size = 6;

     /**
        Unpacks the boot loader generated information into this nice structure.

        @param start Pointer to the beginning of the information. The whole
                     thing should be 6 bytes long.
     */
    explicit MultiBootRgb(const uint8_t* start);

    /**
        Default constructor, sets everything to zero and valid to false.
     */
    MultiBootRgb() : val{false}, data{}
    {}

    /**
        Check whether the structure is valid. Not all possible values of the
        data are allowed, so this serves as a check on whether the boot loader
        is behaving sensibly.

        @return Whether the data structure is valid.
     */
    bool valid() const { return val; }

    /**
        Get the red field position.

        @return Red field position.
     */
    uint8_t red_field_position() const { return klib::get<0>(data); }

    /**
        Get the red mask size.

        @return Red mask size.
     */
    uint8_t red_mask_size() const { return klib::get<1>(data); }

    /**
        Get the green field position.

        @return Green field position.
     */
    uint8_t green_field_position() const { return klib::get<2>(data); }

    /**
        Get the green mask size.

        @return Green mask size.
     */
    uint8_t green_mask_size() const { return klib::get<3>(data); }

    /**
        Get the blue field position.

        @return Blue field position.
     */
    uint8_t blue_field_position() const { return klib::get<4>(data); }

    /**
        Get the blue mask size.

        @return Blue mask size.
     */
    uint8_t blue_mask_size() const { return klib::get<5>(data); }

    /**
        Print useful information to the provided stream.

        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

private:
    // Whether the structure has valid values
    bool val;
    // Colour information
    klib::array<uint8_t, size> data;
};

/**
    Information on the framebuffer.
 */
class MultiBootFrameBuffer {
public:
    /**
        Unpacks the boot loader generated information into this nice structure.

        @param start Pointer to the beginning of the information. The whole
                     thing should be 28 bytes long.
        @param pdt Page Decsriptor Table for setting up virtual mappings.
     */
    MultiBootFrameBuffer(const uint32_t* start, PageDescriptorTable& pdt);

    /**
        Default constructor, sets everything to zero and valid to false.
     */
    MultiBootFrameBuffer()
        : val{false},
          add{nullptr},
          p{0},
          w{0},
          h{0},
          b{0},
          ty{ColorType::invalid},
          index{},
          rgb{}
    {}

    /**
        Check whether the structure is valid. Not all possible values of the
        data are allowed, so this serves as a check on whether the boot loader
        is behaving sensibly.

        @return Whether the data structure is valid.
     */
    bool valid() const { return val; }

    /**
        Get the address of the framebuffer.

        @return Address of the framebuffer.
     */
    void* addr() const { return add; }

    /**
        Get the pitch of the framebuffer. If in text mode, this is number of
        bytes per line.

        @return Framebuffer pitch.
     */
    uint32_t pitch() const { return p; }

    /**
        Get the width of the framebuffer, in characters for text mode, otherwise
        in pixels.

        @return Width of the framebuffer.
     */
    uint32_t width() const { return w; }

    /**
        Get the height of the framebuffer, in characters for text mode,
        otherwise in pixels.

        @return Height of the framebuffer.
     */
    uint32_t height() const { return w; }

    /**
        Get the number of bits per pixel. This should be 16 in text mode.

        @return Bits per pixel.
     */
    uint8_t bpp() const { return b; }

    /**
        Represents the possible colour types the framebuffer can use.
     */
    enum class ColorType : uint8_t {
        // Indexed colour type
        indexed,
        // Red Green Blue colour type
        rgb,
        // Text mode
        text,
        // Invalid
        invalid
    };

    /**
        Get the colour type in use.

        @return Colour type.
     */
    ColorType type() const { return ty; }

    /**
        Get the index colour information. Could be invalid if the colour type is
        not indexed.       

        @return Index colour information.
     */
    const MultiBootIndex& color_info_index() const { return index; }

    /**
        Get the RGB colour information. Could be invalid if the colour type is
        not RGB.       

        @return RGB colour information.
     */
    const MultiBootRgb& color_info_rgb() const { return rgb; }

    /**
        Print useful information to the provided stream.

        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

private:
    // Whether the data is valid.
    bool val;
    // Physical address of the framebuffer
    void* add;
    // Pitch
    uint32_t p;
    // Width
    uint32_t w;
    // Height
    uint32_t h;
    // Bits per pixel
    uint8_t b;
    // Colour type
    ColorType ty;
    // Indexed colour information
    MultiBootIndex index;
    // RGB colour information
    MultiBootRgb rgb;
};

/**
    Multiboot information, generated by the boot loader.
 */
class MultiBootInfo {
public:
    /*
        Unpacks the boot loader generated information into this nice structure.

        @param phys_start Pointer to the beginning of the information. The whole
               thing should be 116 bytes long. This is a physical, not virtual,
               memory address.
     */
    explicit MultiBootInfo(const void* phys_start);

    /**
        Check whether the structure is valid. Not all possible values of the
        data are allowed, so this serves as a check on whether the boot loader
        is behaving sensibly.

        @return Whether the data structure is valid.
     */
    bool valid() const { return val; }

    /**
        Possible pieces of data that may or may not be present in the header.
     */
    enum class Flag {
        /** Lower and upper memory sizes */
        memory,
        /** Boot device */
        device,
        /** Kernel command line */
        cmdline,
        /** Bootloaded modules */
        modules,
        /** a.out format symbol table */
        aout,
        /** ELF format section header */
        elf,
        /** Memory map */
        map,
        /** Table of attached drives */
        drives,
        /** ROM configuration table */
        rom,
        /** Name of the boot loader */
        name,
        /** Advanced power management table */
        apm,
        /** VESA BIOS Extensions table */
        vbe,
        /** Framebuffer information */
        fb
    };
    /**
        Check whether a specific information type was included in the data.

        @return Whether the information was included.
     */
    bool flags(Flag f) const;

    /**
        Size of lower memory in kilobytes, which starts at 0x00. Maximum
        possible value is 640 kilobytes.

        @return Size of lower memory.
     */
    size_t mem_lower() const { return mlow; }

    /**
        Size of upper memory in kilobytes, which starts at 1MB. Maximum value
        is to just before the first memory hole, but it could be smaller.

        @return Size of upper memory.
     */
    size_t mem_upper() const { return mupper; }

    /**
        Get the disk and partition that the bootloader got the kernel from.

        @return Boot device.
     */
    const MultiBootDev& boot_device() const { return dev; }

    /**
        Get the kernel command line.

        @return Kernel command line.
     */
    const klib::string& cmdline() const { return cmd; }

    /**
        Get the number of modules loaded by the boot loader.

        @return Number of modules.
     */
    size_t mods_count() const { return mcount; }

    /**
        Get the module indicated by the index, or an invalid module if the index
        is out of range.

        @return n Index of the module to access.
        @return A module.
     */
    const MultiBootModule& mods(size_t n) const;

    /**
        Get the a.out symbol table. May be invalid if ELF is used instead.

        @return a.out symbol table information.
     */
    const AOutSymTab& syms_aout() const { return aout; }

    /**
        Get the ELF section header table. May be invalid if a.out is used
        instead.

        @return ELF section header information.
     */
    const ElfSectionTab& syms_elf() const { return elf; }

    /**
        Get the number of entries in the memory map.

        @return Size of the memory map.
     */
    size_t mmap_length() const { return map.size(); }

    /**
        Get the memory map entry indicated by the index, or an invalid entry if
        the index is out of range.

        @return A memory map entry.
     */
    const MultiBootMap& mmap(size_t n) const;

    /**
        Get the entry in the drives list at index n, or an invalid entry if the
        index is out of range.

        @param n Index of drive to fetch.
        @return Drive at the specified index, or an invalid drive if out of
                range.
     */
    const MultiBootDrive& drive(size_t n) const;

    /**
        Get the ROM configuration table.

        @return Reference to the config table.
     */
    const MultiBootRom& config_table() const { return rom; }

    /**
        Get the name of the bootloader.

        @return String containing the self-reported name of the bootloader.
     */
    klib::string boot_loader_name() const { return name; }

    /**
        Get the Advanced Power Management table.

        @return APM table.
     */
    const MultiBootApm& apm_table() const { return apm; }

    /**
        Get the VESA BIOS extensions information.

        @return VBE table.
     */
    const MultiBootVbe& vbe() const { return vbe_table; }

    /**
        Get the framebuffer information.

        @return Framebuffer information table.
     */
    const MultiBootFrameBuffer& framebuffer() const { return fb; }
        
private:
    // Size of the multiboot information in bytes.
    static constexpr size_t mb_size = 116;

    // Whether the structure has valid values
    bool val;
    // Flags to indicate which other parts of the information are present
    uint32_t fl;

    // Sizes of lower and upper memory
    size_t mlow;
    size_t mupper;

    // Boot device
    MultiBootDev dev;

    // Command line
    klib::string cmd;

    // Modules
    size_t mcount;
    // The list of modules.
    klib::vector<MultiBootModule> modules;

    // a.out symbol table information
    AOutSymTab aout;
    // ELF symbol table information
    ElfSectionTab elf;

    // Memory map
    klib::vector<MultiBootMap> map;

    // List of drives.
    klib::vector<MultiBootDrive> drvs;

    // Rom configuration table from the BIOS.
    MultiBootRom rom;

    // Name of the boot loader
    klib::string name;

    // APM table
    MultiBootApm apm;

    // VBE table
    MultiBootVbe vbe_table;

    // Framebuffer information
    MultiBootFrameBuffer fb;
};

/**
    Interpret the bytes beginning at the physical address as a null terminated
    string, then store as a C++ string.

    @param phys_addr Physical address to start interpreting at.
    @param pdt Page Descriptor Table for virtual mappings.
    @return String representation of the data at phys_addr.
 */
klib::string getstring(const char* phys_addr, PageDescriptorTable& pdt);

#endif
