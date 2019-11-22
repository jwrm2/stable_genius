#ifndef VGA_IO_H
#define VGA_IO_H
 
#include <stdint.h>
 
/**
    The available colours.
 */
enum class VgaColor : uint16_t {
    black = 0, blue = 1, green = 2, cyan = 3, red = 4, magenta = 5,
    brown = 6, light_grey = 7, dark_grey = 8, light_blue = 9,
    light_green = 10, light_cyan = 11, light_red = 12, light_magenta = 13,
    light_brown = 14, white = 15
};
 
/**
    Address for various registers used to control the framebuffer.
 */
enum class VgaIoAddress : uint16_t {
    /** Address for reading the Miscellaneous Output reigster. */
    misc_out_read = 0x03CC,
    /** Address for writing to the Miscellaneous Output register. */
    misc_out_write = 0x03C2,
    /** Address for indexing a CRTC register. */
    crtc_index = 0x03D4,
    /** Address to send data to a CRTC register. */
    crtc_data = 0x03D5
};
 
/**
    Register indices for the CRT controller.
 */
enum class VgaCrtc : uint8_t {
    /** Register for the maximum scan line. */
    max_scan_line = 0x09,
    /** Register for the cursor start line and cursor enablement. */
    cursor_start = 0x0A,
    /** Register for the cursor end line. */
    cursor_end = 0x0B,
    /** Register for the high byte of the cursor position. */
    cursor_pos_h = 0x0E,
    /** Register for the low byte of the cursor position. */
    cursor_pos_l = 0x0F
};

/**
    Modes for inserting or overwriting existing characters when typing.
 */
enum class VgaWriteMode {
    insert, overwrite
};
 
/**
    Type for a charcter to be displayed in the VGA framebuffer.
 */
class VgaCharacter {
public:
    /**
        Constructs a character for display.
 
        @param c ASCII character to display.
        @param fg Foreground colour.
        @param bg Background colour.
     */
    VgaCharacter(char c, VgaColor fg, VgaColor bg);
 
    /**
        Reinterprets a 4 byte value as a character and colours to display.
        No alterations or checks are done.
 
        @param v Value containing the character and colours.
     */
    explicit VgaCharacter(uint16_t v) : value{v} {}
 
    /**
        Gives the character and colours in the correct format for the VGA
        framebuffer.
 
        @return Character and colours value.
     */
    uint16_t val() const { return value; }
 
    /**
        Gives the character represented.
 
        @return The ASCII character.
     */
    char ch() const { return static_cast<char>(value & 0x00FF); }
 
private:
    // Internal value store.
    uint16_t value;
};
 
#endif
