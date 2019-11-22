#include "VgaCursor.h"

#include <stdint.h>

#include "io.h"
#include "VgaIo.h"

/*******************************************************************************
 ******************************************************************************/

VgaCursor::VgaCursor() : cursor_pos{0}
{
    set_pos(cursor_pos);
    set_shape(Shape::block);
}

/******************************************************************************/

bool VgaCursor::set_pos(size_t pos)
{
    if (pos > cursor_max)
        return false;

    outb(static_cast<uint8_t>(VgaCrtc::cursor_pos_h),
         static_cast<uint16_t>(VgaIoAddress::crtc_index));
    outb(static_cast<uint8_t>(pos >> 8 & 0xFF),
         static_cast<uint16_t>(VgaIoAddress::crtc_data));
    outb(static_cast<uint8_t>(VgaCrtc::cursor_pos_l),
         static_cast<uint16_t>(VgaIoAddress::crtc_index));
    outb(static_cast<uint8_t>(pos & 0xFF),
         static_cast<uint16_t>(VgaIoAddress::crtc_data));

    cursor_pos = pos;
    return true;
}

bool VgaCursor::set_pos(size_t row, size_t column)
{
    if (column >= width || row >= height)
        return false;

    set_pos(row * width + column);
    return true;
}

/******************************************************************************/

bool VgaCursor::advance(int n)
{
    int new_pos = cursor_pos + n;
    if (new_pos < 0)
    {
        set_pos(0);
        return false;
    }

    size_t np = static_cast<size_t>(new_pos);
    if (np > cursor_max)
    {
        set_pos(cursor_max);
        return false;
    }

    set_pos(np);
    return true;
}

/******************************************************************************/

void VgaCursor::set_enabled(bool state)
{
    // Get the current state of the register
    outb(static_cast<uint8_t>(VgaCrtc::cursor_start),
         static_cast<uint16_t>(VgaIoAddress::crtc_index));
    uint8_t data = inb(static_cast<uint16_t>(VgaIoAddress::crtc_data));
    // Cursor enablement is in the bit 5.
    if (state)
        data &= ~0x20;
    else
        data |= 0x20;
    // Write back the new value
    outb(data, static_cast<uint16_t>(VgaIoAddress::crtc_data));
}

/******************************************************************************/

void VgaCursor::set_shape(VgaCursor::Shape sh)
{
    switch (sh)
    {
    case Shape::underscore:
        set_top();
        set_bottom();
        break;
    case Shape::block:
        set_top(0);
        set_bottom();
        break;
    }
}

/******************************************************************************/

uint8_t VgaCursor::max_scan_line()
{
    // Get the current state of the register
    outb(static_cast<uint8_t>(VgaCrtc::max_scan_line),
         static_cast<uint16_t>(VgaIoAddress::crtc_index));
    uint8_t data = inb(static_cast<uint16_t>(VgaIoAddress::crtc_data));
    // Max scan line is in bits 0 to 4.
    return data & 0x1F;
}

/******************************************************************************/

bool VgaCursor::set_top()
{
    return set_top(max_scan_line() - 1);
}

bool VgaCursor::set_top(uint8_t value)
{
    if (value > max_scan_line())
        return false;

    // Get the current state of the register
    outb(static_cast<uint8_t>(VgaCrtc::cursor_start),
         static_cast<uint16_t>(VgaIoAddress::crtc_index));
    uint8_t data = inb(static_cast<uint16_t>(VgaIoAddress::crtc_data));
    // Cursor start is in bits 0 to 4.
    data &= 0xE0;
    data |= (value & 0x1F);
    // Write back the new value
    outb(data, static_cast<uint16_t>(VgaIoAddress::crtc_data));

    return true;
}

/******************************************************************************/

bool VgaCursor::set_bottom()
{
    return set_bottom(max_scan_line());
}

bool VgaCursor::set_bottom(uint8_t value)
{
    if (value > max_scan_line())
        return false;

    // Get the current state of the register
    outb(static_cast<uint8_t>(VgaCrtc::cursor_end),
         static_cast<uint16_t>(VgaIoAddress::crtc_index));
    uint8_t data = inb(static_cast<uint16_t>(VgaIoAddress::crtc_data));
    // Cursor end is in bits 0 to 4.
    data &= 0xE0;
    data |= (value & 0x1F);
    // Write back the new value
    outb(data, static_cast<uint16_t>(VgaIoAddress::crtc_data));

    return true;
}

/******************************************************************************
 ******************************************************************************/
