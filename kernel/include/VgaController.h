#ifndef VGACONTROLLER_H
#define VGACONTROLLER_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <string>

#include "VgaCursor.h"
#include "VgaIo.h"

// Forward declarations.
class PageDescriptorTable;

/**
    Driver for the VGA framebuffer. TODO I'm not sure how readline manages to
    move the cursor around.
 */
class VgaController {
public:
    /**
        Initialises the buffer as empty, sets the foreground colour to light
        grey, the background colour to black and the cursor position to the top
        left.

        @param addr Physical address of the start of the framebuffer for
               writing.
        @param w Framebuffer width.
        @param h Framebuffer height.
        @param b Framebuffer bits per pixel (or character).
        @param pdt Page Descriptor Table for creating virtual mappings.
     */
    VgaController(void* addr,
        size_t w,
        size_t h,
        size_t b,
        PageDescriptorTable& p);

    /**
        Copy constructor and assignment are deleted.
     */
    VgaController(const VgaController&) = delete;
    VgaController& operator=(const VgaController&) = delete;

    /**
        Destructor. Frees the virtual framebuffer mapping, if one was made.
     */
    ~VgaController();

    /**
        Write a single character. Uses the current colours.

        @param c Character to send.
     */
    void write_char(char c);

    /**
        Writes a single character at the current cursor position and advances
        the cursor. The colours are specified in the character. Will scroll if
        necessary.

        @param cc Character with the colours specified.
        @param c Character. Current colours are used.
     */
    void putchar(VgaCharacter cc);

    /**
        Writes a single character at the given location. The cursor remains
        unmoved. Always overwrites. If the colour is not specified, the
        currently set colours are used. If an invalid position is specified,
        nothing happens.

        @param c An ASCII character.
        @param row The row to use, beteen 0 and 79 inclusive.
        @param column The column to use, between 0 and 24 inclusive.
        @param cc Character with the colours specified.
        @return Success or failure.
     */
    bool putchar_at(char c, size_t row, size_t column);
    bool putchar_at(char c,
                    VgaColor fg,
                    VgaColor bg,
                    size_t row,
                    size_t column);
    bool putchar_at(VgaCharacter cc, size_t row, size_t column);

    /**
        Clear the buffer, set the foreground colour to light grey, the
        background colour to black and the cursor position to the top left.
     */
    void initialise();

    /**
        Clear the buffer and set the cursor position to the top left.
        Characters are all set to zero. The foreground and background colours
        are set for each position.
     */
    void clear();

    /**
        Set the background colour for subsequent writing.

        @param bg background colour.
     */
    void set_background(VgaColor bg);

    /**
        Set the foreground colour for subsequent writing.

        @param fg Foreground colour.
     */
    void set_foreground(VgaColor fg);

    /**
        If a position is specified, insert a new line at that position, without
        moving the cursor. Otherwise insert a new line at the cursor and move
        the cursor to the start of the new line. Characters after the new line
        will be shifted as necessary. Scrolling will be done as necessary.
        Nothing will be done if the position is invalid.

        @param pos Position (row * width + column) at which to insert the new 
                   line.
        @param row Row of the position.
        @param column Column of the position.
        @return Success or failure.
     */
    bool new_line();
    bool new_line(size_t pos);
    bool new_line(size_t row, size_t column);

    /**
        Scroll one line, by shifting every line up one in the buffer and
        forgetting the first line.
     */
    void scroll();

    /**
        Sets mode to either insert or overwrite.

        @param mode Whether to insert or overwrite characters when typing.
     */
    void set_write_mode(VgaWriteMode mode);

    /**
        Provides access to the cursor.

        @return A reference to the VGA cursor.
     */
    VgaCursor& cursor() { return vga_cursor; }

    /**
        Gets the virtual address of the framebuffer start.

        @return Pointer to the start of the framebuffer.
     */
    const void* framebuffer() const { return buffer; }

private:
    // Memory address of the start of the buffer
    uint16_t* buffer;
    // Number of columns
    size_t width;
    // Number of rows
    size_t height;
    // Number of bits per pixel.
    size_t bpp;
    // Framebuffer for virtual mapping.
    PageDescriptorTable& pdt;
    // Whether a virtual mapping was created for the framebuffer.
    bool virt_map;
    // Default foreground colour
    static const VgaColor default_fg = VgaColor::light_grey;
    // Default background colour
    static const VgaColor default_bg = VgaColor::black;
    // Foreground colour
    VgaColor foreground;
    // Background colour
    VgaColor background;
    // Write mode
    VgaWriteMode write_mode;
    // Access to the VGA cursor
    VgaCursor vga_cursor;

    /**
        Ensures that the CRTC registers are mapped at 0x03D4 and 0x03D5 (for
        colour mode) rather than 0x03B4 and 0x03B5 (for monochrome).
     */
    void set_io_address();
};

#endif
