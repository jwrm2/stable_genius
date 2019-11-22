#ifndef VBE_H
#define VBE_H

#include <stddef.h>
#include <stdint.h>

#include <array>
#include <ostream>
#include <string>
#include <vector>

// Forward declarations
class PageDescriptorTable;

/**
    Stores the contents of a VBE control table, as output by VESA function 0x00.
 */
class VbeCntrl {
private:
    // Signature. Should contain "VESA" (not null terminated). 
    static constexpr size_t sig_size = 4;
    klib::array<char, sig_size + 1> signature;
    // High byte is major version, low byte is minor version.
    uint16_t ver;
    // OEM identifier string.
    klib::string oem_str;
    // Bitfield describing card capabilities.
    uint32_t cap;
    // List of video modes identifiers.
    klib::vector<uint16_t> video_modes_list;
    // Number of 64KB blocks of video memory.
    uint16_t video_mem;
    static constexpr size_t block_size = (1 << 16);
    // Software revision number.
    uint16_t soft_rev;
    // Vendor identifier string.
    klib::string vendor_str;
    // segment:offset pointer to card model name.
    klib::string product_name_str;
    // segment:offset pointer to product revision.
    klib::string product_rev_str;
    // Reserved for future expansion.
    static constexpr size_t rev_size = 222;
    klib::array<char, rev_size> reserved;
    // Space for OEM BIOSes to store strings.
    static constexpr size_t oem_data_size = 256 / sizeof(uint32_t);
    klib::array<uint32_t, oem_data_size> oem_data;

public:
    /**
        Size of the structure when packed.
     */
    static constexpr size_t size = 512;

    /**
        Constructor. Unpack data into this structure.
    
        @param start Pointer to the start of the data.
        @param pdt PageDescriptorTable for setting up virtual mappings.
     */
    VbeCntrl(const uint32_t* start, PageDescriptorTable& pdt);

    /**
        Default constructor. Fill the signature with something to indicate
        invalid.
     */
    VbeCntrl() :
        signature{'b', 'a', 'd', '\0'},
        ver{0},
        oem_str{""},
        cap{0},
        video_modes_list{},
        video_mem{0},
        soft_rev{0},
        vendor_str{""},
        product_name_str{""},
        product_rev_str{""},
        reserved{},
        oem_data{}
    {}

    /**
        Check whether the structure is valid, as indicated by the contents of
        the signature.

        @return Whether this structure is valid.
     */
    bool valid() const;

    /**
        Returns the version number.

        @return Version number. High byte is major version, low byte is minor
                version.
     */
    uint16_t version() const { return ver; }

    /**
        Returns the hardware provided OEM identifier string.

        @return OEM string.
     */
    const klib::string& oem() const { return oem_str; }

    /**
        Returns a bitfield representing the capabilities of the card.

        @return Capabilities of the card.
     */
    uint32_t capabilities() const { return cap; }

    /**
        Returns an entry in the list of supported video mode identifiers, or
        0xFFFF if out of range.

        @param n Index of the mode identifier to fetch.
        @return Video mode identifier, or 0xFFFF.
     */
    uint16_t video_mode(size_t n) const 
    {
        return (n < video_modes_list.size() ? video_modes_list[n] : 0xFFFF);
    }

    /**
        Returns the number of supported video modes.

        @return Number of supported video modes.
     */
    size_t video_mode_length() const { return video_modes_list.size(); }

    /**
        Returns amount of video memory, in bytes.

        @return Amount of video memory, in bytes.
     */
    uint32_t video_memory() const { return video_mem * block_size; }

    /**
        Returns the software revision number.

        @return Software revision number.
     */
    uint16_t software_rev() const { return soft_rev; }

    /**
        Returns the hardware provided vendor identifier string.

        @return Vendor string.
     */
    const klib::string& vendor() const { return vendor_str; }

    /**
        Returns the hardware provided product name identifier string.

        @return Product name string.
     */
    const klib::string& product_name() const { return product_name_str; }

    /**
        Returns the hardware provided product revision identifier string.

        @return Product revision string.
     */
    const klib::string& product_revision() const {return product_rev_str;}

    /**
        Prints contents to the provided stream.

        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;
};

/**
    Stores the contents of a VBE mode table, as output by VESA function 0x01.
 */
class VbeMode {
private:
    // Deprecated, except for bit 7 which indicates supprt for an LFB.
	uint16_t attributes;
    // Deprecated. Window A attributes.
	uint8_t window_a;
    // Deprecated. Window B attributes.
	uint8_t window_b;
    // Deprecated. Used for calculating bank numbers.
	uint16_t granularity;
    // Size of the window.
	uint16_t window_size;
    // Window A start segment.
	uint16_t segment_a;
    // Window B start segment.
	uint16_t segment_b;
    // Deprecated. Used to switch banks from protected mode.
	uint32_t win_func_ptr;
    // Number of bytes per scan line.
	uint16_t pitch;
    // Width in pixels (graphics mode)/characters (text mode).
	uint16_t width;
    // Height in pixels (graphics mode)/characters (text mode).
	uint16_t height;
    // Character cell width in pixels.
	uint8_t w_char;
    // Character cell height in pixels.
	uint8_t y_char;
    // Number of memory planes.
	uint8_t planes;
    // Bits per pixel.
	uint8_t bpp;
    // Deprecated. Number of banks.
	uint8_t banks;
    // Memory model type.
	uint8_t memory_model;
    // Deprecated. Size of memory bank in KB.
	uint8_t bank_size;
    // Number of images.
	uint8_t image_pages;
    // Reserved for page function.
	uint8_t reserved0;
 
    // Size of direct colour red mask in bits.
	uint8_t red_mask;
    // Bit position of lsb red mask.
	uint8_t red_position;
    // Size of direct colour green mask in bits.
	uint8_t green_mask;
    // Bit position of lsb green mask.
	uint8_t green_position;
    // Size of direct colour blue mask in bits.
	uint8_t blue_mask;
    // Bit position of lsb blue mask.
	uint8_t blue_position;
    // Size of direct colour reserved mask in bits.
	uint8_t reserved_mask;
    // Bit position of lsb reserved mask.
	uint8_t reserved_position;
    // Direct colour mode attributes.
	uint8_t direct_color_attributes;
 
    // Physical address of the linear frame buffer.
	uint32_t framebuffer;
    // Pointer to start of off screen memory.
	uint32_t off_screen_mem_off;
    // Size of off screen memory in KB.
	uint16_t off_screen_mem_size;
    // Reserved.
    static constexpr size_t rev_size = 206;
	klib::array<char, rev_size> reserved1;

public:
    /**
        Size of the structure when packed.
     */
    static constexpr size_t size = 256;

    /**
        Constructor. Unpack data into this structure.
    
        @param start Pointer to the start of the data.
     */
    explicit VbeMode(const uint32_t* start);

    /**
        Default constructor. Set everything to zero.
     */
    VbeMode() :
	    attributes{0},
	    window_a{0},
	    window_b{0},
	    granularity{0},
	    window_size{0},
	    segment_a{0},
	    segment_b{0},
	    win_func_ptr{0},
	    pitch{0},
	    width{0},
	    height{0},
	    w_char{0},
	    y_char{0},
	    planes{0},
	    bpp{0},
	    banks{0},
	    memory_model{0},
	    bank_size{0},
	    image_pages{0},
	    reserved0{0},
	    red_mask{0},
	    red_position{0},
	    green_mask{0},
	    green_position{0},
	    blue_mask{0},
	    blue_position{0},
	    reserved_mask{0},
	    reserved_position{0},
	    direct_color_attributes{0},
	    framebuffer{0},
	    off_screen_mem_off{0},
	    off_screen_mem_size{0},
	    reserved1{0}
    {}

    /**
        Print useful information to the provided stream.
 
        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;
};

#endif /* VBE_H */
