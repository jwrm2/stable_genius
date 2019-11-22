#include "VgaController.h"

#include <cctype>
#include <string>

#include "Kernel.h"
#include "Logger.h"
#include "io.h"
#include "PageDescriptorTable.h"
#include "paging.h"
#include "SignalManager.h"

/******************************************************************************
 ******************************************************************************/

VgaController::VgaController(void* addr, size_t w, size_t h, size_t b,
    PageDescriptorTable& p) :
    buffer{nullptr},
    width{w},
    height{h},
    bpp{b},
    pdt{p},
    virt_map{false},
    foreground{default_fg},
    background{default_bg},
    write_mode{VgaWriteMode::overwrite},
    vga_cursor{}
{
    set_io_address();

    // We need to check the address. It starts as a physical address. If it's
    // before the kernel, we can assume it's mapped and just add the kernel
    // virtual base. Otherwise we need to create a custom mapping.
    uintptr_t p_addr = reinterpret_cast<uintptr_t>(addr);
    if (p_addr <
        reinterpret_cast<uintptr_t>(global_kernel->get_physical_start()))
    {
        p_addr += kernel_virtual_base;
        buffer = reinterpret_cast<uint16_t*>(p_addr);
    }
    else
    {
        size_t sz = width * height * bpp;
        // Let's put it near the end, so we don't clash with any process stuff
        // or the kernel_heap.
        void* v_addr = pdt.map(addr, sz,
            reinterpret_cast<void*>((1 << 31) - sz));
        if (v_addr == nullptr)
            global_kernel->panic(
                "Failed to get virtual memory for framebuffer mapping.\n");
        virt_map = true;
        buffer = static_cast<uint16_t*>(v_addr);
    }

    clear();
}

/******************************************************************************/

VgaController::~VgaController()
{
    // Free virtual mapping, if it was made.
    if (virt_map)
    {
        size_t sz = width * height * bpp;
        pdt.unmap(buffer, sz);
        virt_map = false;
    }
}

/******************************************************************************/

void VgaController::write_char(char c)
{
    putchar(VgaCharacter{c, foreground, background});
}

/******************************************************************************/

void VgaController::putchar(VgaCharacter cc)
{
    if (klib::isprint(cc.ch()))
    {
        if (write_mode == VgaWriteMode::insert)
        {
            size_t chars_after = 0;
            for (size_t i = vga_cursor.pos();
                 VgaCharacter{buffer[i]}.ch() && i <= VgaCursor::cursor_max;
                 ++i)
                ++chars_after;
            if (vga_cursor.pos() + chars_after + 1 >= VgaCursor::cursor_max)
                scroll();
            else if ((vga_cursor.pos() + chars_after + 1) % width == 0)
                new_line(vga_cursor.pos() + chars_after + 1);
            for (size_t i = vga_cursor.pos() + chars_after;
                 i > vga_cursor.pos(); --i)
                buffer[i] = buffer[i-1];
        }
        buffer[vga_cursor.pos()] = cc.val();
        if (vga_cursor.pos() == VgaCursor::cursor_max)
            scroll();
        vga_cursor.advance();
    }
    else if (cc.ch() == '\n')
    {
        new_line();
    }   
}

/******************************************************************************/

bool VgaController::putchar_at(char c, size_t row, size_t column)
{
    return putchar_at(VgaCharacter{c, foreground, background}, row, column);
}

bool VgaController::putchar_at(char c,
                               VgaColor fg,
                               VgaColor bg,
                               size_t row, 
                               size_t column)
{
    return putchar_at(VgaCharacter{c, fg, bg}, row, column);
}

bool VgaController::putchar_at(VgaCharacter cc, size_t row, size_t column)
{
    if (column >= width || row >= height)
        return false;

    size_t pos = row * width + column;
    if (klib::isprint(cc.ch()))
    {
        buffer[pos] = cc.val();
    }
    else if(cc.ch() == '\n')
        new_line(pos);

    return true;
}

/******************************************************************************/

void VgaController::initialise()
{
    set_io_address();
    foreground = default_fg;
    background = default_bg;
    clear();
    vga_cursor.set_pos(0);
}

/******************************************************************************/

void VgaController::clear()
{
    for (size_t i = 0; i <= VgaCursor::cursor_max; ++i)
        buffer[i] = VgaCharacter{0, foreground, background}.val();
}

/******************************************************************************/

void VgaController::set_foreground(VgaColor fg)
{
    foreground = fg;
}

/******************************************************************************/

void VgaController::set_background(VgaColor bg)
{
    background = bg;
}

/******************************************************************************/

bool VgaController::new_line()
{
    new_line(vga_cursor.pos());
    vga_cursor.set_pos(vga_cursor.pos() / width + 1, 0);
    return true;
}

bool VgaController::new_line(size_t pos)
{
    if (pos > VgaCursor::cursor_max)
        return false;

    bool last_line_occupied = false;
    for (size_t i = VgaCursor::cursor_max - width + 1;
         i <= VgaCursor::cursor_max;
         ++i)
    {
        if (VgaCharacter{buffer[i]}.ch())
        {
            last_line_occupied = true;
            break;
        }
    }

    size_t chars_after_new_line = 0;
    for (size_t i = pos;
         VgaCharacter{buffer[i]}.ch() && i <= VgaCursor::cursor_max;
         ++i)
        ++chars_after_new_line;

    bool later_lines_down = true;
    if (pos % width + chars_after_new_line > width &&
        chars_after_new_line < width)
        later_lines_down = false;

    if (later_lines_down && last_line_occupied)
    {
        if (pos >= width)
        {
            scroll();
            pos -= width;
        }
        else
        {
            for (size_t i = 0; i < chars_after_new_line; ++i)
                buffer[i] = buffer[i + pos];
            for (size_t i = chars_after_new_line;
                 i < pos + chars_after_new_line;
                 ++i)
                buffer[i] = VgaCharacter{0, foreground, background}.val();
            return true;
        }
    }

    if (later_lines_down)
    {
        size_t later_lines_start = 
            ((pos + chars_after_new_line) / width + 1) * width;
        for (size_t i = VgaCursor::cursor_max;
             i >= later_lines_start + width;
             --i)
            buffer[i] = buffer[i - width];
        for (size_t i = later_lines_start; i < later_lines_start + width; ++i)
            buffer[i] = VgaCharacter{0, foreground, background}.val();
    }

    size_t shift = width - (pos % width);
    for (size_t i = pos + chars_after_new_line + shift - 1;
         i >= pos + shift;
         --i)
        buffer[i] = buffer[i- shift];
        
    for (size_t i = pos; i < (pos / width + 1) * width; ++i)
        buffer[i] = VgaCharacter{0, foreground, background}.val();

    return true;
}

bool VgaController::new_line(size_t row, size_t column)
{
    if (column >= width || row >= height)
        return false;

    return new_line(row * width + column);
}

/******************************************************************************/

void VgaController::scroll()
{
    for (size_t i = 0; i <= VgaCursor::cursor_max - width; ++i)
        buffer[i] = buffer[i+width];

    for (size_t i = VgaCursor::cursor_max - width + 1;
         i <= VgaCursor::cursor_max;
         ++i)
        buffer[i] = VgaCharacter{0, foreground, background}.val();

    if (vga_cursor.pos() >= width)
        vga_cursor.set_pos(vga_cursor.pos() - width);
}

/******************************************************************************/

void VgaController::set_write_mode(VgaWriteMode mode)
{
    write_mode = mode;
    switch(mode)
    {
    case VgaWriteMode::insert:
        vga_cursor.set_shape(VgaCursor::Shape::underscore);
        break;
    case VgaWriteMode::overwrite:
        vga_cursor.set_shape(VgaCursor::Shape::block);
        break;
    }
}

/******************************************************************************/

void VgaController::set_io_address()
{
    uint8_t data = inb(static_cast<uint16_t>(VgaIoAddress::misc_out_read));
    // The lowest bit is the one that matters, and must be set.
    data |= 0x01;
    outb(data, static_cast<uint16_t>(VgaIoAddress::misc_out_write));
}

/******************************************************************************
 ******************************************************************************/
