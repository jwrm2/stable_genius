#include "Vbe.h"

#include <cstring>
#include <ostream>

#include "Kernel.h"
#include "MultiBoot.h"
#include "PageDescriptorTable.h"
#include "util.h"

/******************************************************************************
 ******************************************************************************/

VbeCntrl::VbeCntrl(const uint32_t* start, PageDescriptorTable& pdt)
{
    // Temporary stores for segment:offset pointers.
    uint32_t off;
    uint32_t seg;
    const char* phys_addr;

    for (size_t i = 0; i < sig_size; ++i)
        signature[i] = *(reinterpret_cast<const char*>(start) + i);
    signature[sig_size] = '\0';
    ver = static_cast<uint16_t>(*(start + 1));

    off = *(start + 1) >> 16;
    seg = *(start + 2) & 0xFFFF;
    phys_addr = reinterpret_cast<char*>((seg << 4) + off);
    oem_str = getstring(phys_addr, pdt);

    cap = (*(start + 2) >> 16) + (*(start + 3) & 0xFFFF);

    off = *(start + 3) >> 16;
    seg = *(start + 4) & 0xFFFF;
    phys_addr = reinterpret_cast<char*>((seg << 4) + off);
    // We don't know how long the list will be, so we'll just have to guess and
    // increase if necessary. The list is terminated by 0xFFFF.
    size_t alloc_size = PageDescriptorTable::page_size;
    const uint16_t* virt_addr = static_cast<uint16_t*>(
        pdt.map(phys_addr, alloc_size)
    );
    if (virt_addr == nullptr)
        global_kernel->panic("No virtual memory for multiboot allocation");
    size_t size = 0;
    const uint16_t* current = virt_addr;
    while (*current != 0xFFFF)
    {
        ++size;
        if (size/2 >= alloc_size)
        {
            pdt.unmap(virt_addr, alloc_size);
            alloc_size += PageDescriptorTable::page_size;
            virt_addr = static_cast<uint16_t*>(pdt.map(phys_addr, alloc_size));
            if (virt_addr == nullptr)
                global_kernel->panic(
                    "No virtual memory for multiboot allocation");
            current = virt_addr + size - 1;
        }
        video_modes_list.push_back(*current);
        ++current;
    }
    pdt.unmap(virt_addr, alloc_size);

    video_mem = static_cast<uint16_t>(*(start + 4) >> 16);
    soft_rev = static_cast<uint16_t>(*(start + 5));

    off = *(start + 5) >> 16;
    seg = *(start + 6) & 0xFFFF;
    phys_addr = reinterpret_cast<char*>((seg << 4) + off);
    vendor_str = getstring(phys_addr, pdt);

    off = *(start + 6) >> 16;
    seg = *(start + 7) & 0xFFFF;
    phys_addr = reinterpret_cast<char*>((seg << 4) + off);
    product_name_str = getstring(phys_addr, pdt);

    off = *(start + 7) >> 16;
    seg = *(start + 8) & 0xFFFF;
    phys_addr = reinterpret_cast<char*>((seg << 4) + off);
    product_rev_str = getstring(phys_addr, pdt);

    for (size_t i = 0; i < rev_size; ++i)
        reserved[i] = *(reinterpret_cast<const char*>(start) + 34 + i);
    for (size_t i = 0; i < oem_data_size; ++i)
        oem_data[i] = start[i + 64];
}

/******************************************************************************/

bool VbeCntrl::valid() const
{
    return (klib::get<0>(signature) == 'V' && klib::get<1>(signature) == 'E' &&
        klib::get<2>(signature) == 'S' && klib::get<3>(signature) == 'A');
}

/******************************************************************************/

void VbeCntrl::dump(klib::ostream& dest) const
{
    dest << "Dumping VBE Control Information:\n";
    dest << "  Signature: " << signature.data() << '\n';
    dest << "  Version: " << klib::hex << ver << '\n';
    dest << "  OEM identifier: " << oem_str << '\n';;
    dest << "  Capabilites: " << cap << '\n';
    dest << "  Supported Video Modes:";

    dest.fill('0');
    dest << klib::internal;
    for (size_t i = 0; i < video_modes_list.size(); ++i)
    {
        if (i%16 == 0)
            dest << "\n    ";

        dest.width(4);
        dest << video_modes_list[i] << (i%16 == 15 ? "" : " ");
    }
    dest << '\n';
    dest.fill(' ');
    dest <<  klib::left;

    dest << "  Size of video memory: " << format_bytes(video_memory()) << '\n';
    dest << "  Software revision: " << soft_rev << '\n';
    dest << "  Vendor identifier: " << vendor_str << '\n';
    dest << "  Product identifier: " << product_name_str << '\n';
    dest << "  Product revision identifier: " << product_rev_str << '\n';
    dest << klib::dec;

    dest.flush();
}

/******************************************************************************
 ******************************************************************************/

VbeMode::VbeMode(const uint32_t* start)
{
    attributes = static_cast<uint16_t>(*start);
    window_a = static_cast<uint8_t>(*start >> 16);
    window_b = static_cast<uint8_t>(*start >> 24);
    granularity = static_cast<uint16_t>(*(start + 1));
    window_size = static_cast<uint16_t>(*(start + 1) >> 16);
    segment_a = static_cast<uint16_t>(*(start + 2));
    segment_b = static_cast<uint16_t>(*(start + 2) >> 16);
    win_func_ptr = *(start + 3);
    pitch = static_cast<uint16_t>(*(start + 4));
    width = static_cast<uint16_t>(*(start + 4) >> 16);
    height = static_cast<uint16_t>(*(start + 5));
    w_char = static_cast<uint8_t>(*(start + 5) >> 16);
    y_char = static_cast<uint8_t>(*(start + 5) >> 24);
    planes = static_cast<uint8_t>(*(start + 6));
    bpp = static_cast<uint8_t>(*(start + 6) >> 8);
    banks = static_cast<uint8_t>(*(start + 6) >> 16);
    memory_model = static_cast<uint8_t>(*(start + 6) >> 24);
    bank_size = static_cast<uint8_t>(*(start + 7));
    image_pages = static_cast<uint8_t>(*(start + 7) >> 8);
    reserved0 = static_cast<uint8_t>(*(start + 7) >> 16);

    red_mask = static_cast<uint8_t>(*(start + 7) >> 24);
    red_position = static_cast<uint8_t>(*(start + 8));
    green_mask = static_cast<uint8_t>(*(start + 8) >> 8);
    green_position = static_cast<uint8_t>(*(start + 8) >> 16);
    blue_mask = static_cast<uint8_t>(*(start + 8) >> 24);
    blue_position = static_cast<uint8_t>(*(start + 9));
    reserved_mask = static_cast<uint8_t>(*(start + 9) >> 8);
    reserved_position = static_cast<uint8_t>(*(start + 8) >> 16);
    direct_color_attributes = static_cast<uint8_t>(*(start + 8) >> 24);

    framebuffer = *(start + 10);
    off_screen_mem_off = *(start + 11);
    off_screen_mem_size = static_cast<uint16_t>(*(start + 12));

    for (size_t i = 0; i < rev_size; ++i)
        reserved1[i] = *(reinterpret_cast<const char*>(start) + 50 + i);
}

/******************************************************************************/

void VbeMode::dump(klib::ostream& dest) const
{
    dest << "Dumping VBE Mode Information:\n";
    dest << "  Attributes: " << klib::hex << attributes << '\n';
    dest << "    Graphics Mode: " << (attributes & (1 << 4) ? "Y\n" : "N\n");
    dest << "    VGA Compatible: " << (attributes & (1 << 5) ? "N\n" : "Y\n");
    dest << "    LFB available: " << (attributes & (1 << 7) ? "Y\n" : "N\n");
    dest << "  Width: " << width << '\n';
    dest << "  Height: " << height << '\n';

    dest << "Framebuffer location: ";
    if (framebuffer != 0)
        dest << framebuffer;
    else
        dest << "unsupported";
    dest << '\n' << klib::dec;

    dest.flush();
}

/******************************************************************************
 ******************************************************************************/
