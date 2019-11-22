#include "PageFrameAllocator.h"

#include <cstdio>
#include <ostream>
#include <string>
#include <vector>

#include "MultiBoot.h"
#include "PageDescriptorTable.h"
#include "util.h"

/******************************************************************************
 ******************************************************************************/

void pfa_initialise(const void* kps, const void* kpe)
{
    // Skip if already initialised
    if (PageFrameAllocator::initialised)
        return;

    // Set the addresses.
    PageFrameAllocator::kernel_physical_start = reinterpret_cast<uint32_t>(kps);
    PageFrameAllocator::kernel_physical_end = reinterpret_cast<uint32_t>(kpe);

    // Clear the used array, up to the current memory size.
    for (size_t i = 0;
        i < PageFrameAllocator::mem_size / (PageFrameAllocator::page_size * 32);
        ++i)
        PageFrameAllocator::used_pages[i] = 0;

    // Set everything used by the kernel to used. We'll leave the first 1MB as
    // defintely used, as that's for various hardware stuff.
    for (size_t i = 0; i < 8; ++i)
        PageFrameAllocator::used_pages[i] = 0xFFFFFFFF;

    // Round the kernel physical start down to a page boundary and the end up to
    // a page boundary.
    size_t boundary_low = PageFrameAllocator::kernel_physical_start >> 12;
    size_t boundary_high = (PageFrameAllocator::kernel_physical_end >> 12) + 1;

    // Any initial pages if the low boundary is not divisible by 32.
    for (size_t i = boundary_low; i < (boundary_low / 32 + 1) * 32; ++i)
        PageFrameAllocator::used_pages[i/32] |= 1 << (i%32);
    // Simple intermediate sets of 32 used pages.
    for (size_t i = boundary_low / 32 + 1; i < boundary_high/32; ++i)
        PageFrameAllocator::used_pages[i] = 0xFFFFFFFF;
    // And any trailing pages if the high boundary is not divisible by 32.
    for (size_t i = (boundary_high / 32) * 32; i < boundary_high; ++i)
        PageFrameAllocator::used_pages[i/32] |= 1 << (i%32);

    // To start with, we assume only 64MB, and hope we don't encounter a smaller
    // computer. Set everything above this to used.
    for (size_t i = PageFrameAllocator::mem_size /
        (PageFrameAllocator::page_size * 32);
        i < PageFrameAllocator::number_of_pages/32; ++i)
        PageFrameAllocator::used_pages[i] = 0xFFFFFFFF;

    // Disable reinitialisation
    PageFrameAllocator::initialised = true;
}

/******************************************************************************
 ******************************************************************************/

void pfa_read_map(const klib::vector<MultiBootMap>& map)
{
    // You'd have though we could just cycle through the list and set to used or
    // unused depending on type. However, it looks like the list can have
    // overlaps, for example an entry listing all the memory as available and
    // further entries listing unavailable sections. I think the safest way is
    // to set everything to used, find all entries that list available and set
    // those regions to free, then find all entries that list unavailable and
    // set those to used. We also have to set as used anything in the first
    // 128MB that's already used, as that's the kernel, heap etc.

    // Skip if already mapped.
    if (PageFrameAllocator::mapped)
        return;

    // Set up our temporary list. We'll avoid changing the real array until the
    // end to try to avoid race conditions.
    klib::vector<uint32_t> temp_used
        (PageFrameAllocator::number_of_pages / 32, 0xFFFFFFFF);
    uint32_t temp_mem_size = PageFrameAllocator::mem_size;

    // Find available sections in the map and set to unused.
    for (const MultiBootMap& m : map)
    {
        if (m.type() != MultiBootMap::available)
            continue;

        // To be on the safe side, we'll round up the start and round down the
        // end.
        size_t start_addr = reinterpret_cast<uintptr_t>(m.base_addr());
        size_t boundary_low =
            (start_addr % PageFrameAllocator::page_size == 0 ?
            start_addr / PageFrameAllocator::page_size :
            start_addr / PageFrameAllocator::page_size + 1);
        size_t end_addr = reinterpret_cast<uintptr_t>(m.base_addr()) +
            m.length();
        size_t boundary_high = end_addr / PageFrameAllocator::page_size;

        // Check for overflow. Since the maximum amount of memory is 4GB, the
        // possible overflowed value us zero.
        if (boundary_high == 0 && boundary_low != 0)
            boundary_high = (1 << 20);

        // Check for the highest address.
        if (end_addr > temp_mem_size)
            temp_mem_size = end_addr;

        // Any initial pages if the low boundary is not divisible by 32.
        temp_used[boundary_low / 32] = 0x0;
        for (size_t i = (boundary_low / 32) * 32; i < boundary_low; ++i)
            temp_used[i/32] |= 1 << (i%32);
        // Simple intermediate sets of 32 unused pages.
        for (size_t i = boundary_low/32 + 1; i < boundary_high / 32; ++i)
            temp_used[i] = 0x0;
        // And any trailing pages if the high boundary is not divisible by
        // 32.
        temp_used[boundary_high / 32] = 0x0;
        for (size_t i = boundary_high; i < (boundary_high / 32 + 1) * 32; ++i)
            temp_used[i/32] |= 1 << (i%32);
    }

    // Find unavailable sections in the map and set to used.
    for (const MultiBootMap& m : map)
    {
        // Invalid means the map entry is invalid, not that the memory is
        // invalid.
        if (m.type() == MultiBootMap::available ||
            m.type() == MultiBootMap::invalid)
            continue;

        // To be on the safe side, we'll round down the start and round up the
        // end.
        size_t start_addr = reinterpret_cast<uintptr_t>(m.base_addr());
        size_t boundary_low = start_addr / PageFrameAllocator::page_size;
        size_t end_addr = reinterpret_cast<uintptr_t>(m.base_addr()) +
            m.length();
        size_t boundary_high =
            (end_addr % PageFrameAllocator::page_size == 0 ?
            end_addr / PageFrameAllocator::page_size :
            end_addr / PageFrameAllocator::page_size + 1);

        // Check for overflow. Since the maximum amount of memory is 4GB, the
        // possible overflowed value us zero.
        if (boundary_high == 0 && boundary_low != 0)
            boundary_high = (1 << 20);

        // Check for the highest address.
        if (end_addr > temp_mem_size)
            temp_mem_size = end_addr;

        // Any initial pages if the low boundary is not divisible by 32.
        for (size_t i = boundary_low; i < (boundary_low / 32 + 1) * 32; ++i)
            temp_used[i/32] |= 1 << (i%32);
        // Simple intermediate sets of 32 used pages.
        for (size_t i = boundary_low / 32 + 1; i < boundary_high/32; ++i)
            temp_used[i] = 0xFFFFFFFF;
        // And any trailing pages if the high boundary is not divisible by 32.
        for (size_t i = (boundary_high / 32) * 32; i < boundary_high; ++i)
            temp_used[i/32] |= 1 << (i%32);
    }

    // Set anything already used in the memory already available to used.
    for (size_t i = 0;
        i < PageFrameAllocator::mem_size / (PageFrameAllocator::page_size*32);
        ++i)
        temp_used[i] |= PageFrameAllocator::used_pages[i];

    // Copy the data into the real array.
    for (size_t i = 0; i < PageFrameAllocator::number_of_pages / 32; ++i)
        PageFrameAllocator::used_pages[i] = temp_used[i];
    PageFrameAllocator::mem_size = temp_mem_size;

    // Disable remapping.
    PageFrameAllocator::mapped = true;
}

/******************************************************************************
 ******************************************************************************/

void pfa_dump_status(klib::ostream& dest)
{
    dest << "Dumping memory information:\n";

    dest << "  Memory allocation ";
    dest << (PageFrameAllocator::initialised ? "is" : "is not");
    dest << " initialised\n";
    dest << "  Memory allocation ";
    dest << (PageFrameAllocator::mapped ? "is" : "is not");
    dest << " mapped\n";

    klib::string tmp;
    klib::helper::strprintf(tmp, "%X", PageFrameAllocator::mem_size);
    dest << "  Total memory is ";
    dest << format_bytes(PageFrameAllocator::mem_size) << '\n';

    size_t tot_used = 0;
    for (size_t i = 0;
        i < PageFrameAllocator::mem_size / (PageFrameAllocator::page_size * 32);
        ++i)
    {
        // Count the number of bits set at this index.
        size_t v = PageFrameAllocator::used_pages[i];
        for (; v; ++tot_used)
            v &= v - 1;
    }
    tot_used *= PageFrameAllocator::page_size;
    dest << "  Total used memory is " << format_bytes(tot_used) << '\n';
    size_t tot_av = PageFrameAllocator::mem_size - tot_used;
    dest << "  Total available memory is " << format_bytes(tot_av)  << '\n';

    dest.flush();
}

/******************************************************************************
 ******************************************************************************/

klib::array<uint32_t, PageFrameAllocator::number_of_pages / sizeof(uint32_t)>
    PageFrameAllocator::used_pages;
uint32_t PageFrameAllocator::mem_size = (1 << 26);
uint32_t PageFrameAllocator::kernel_physical_start = 0;
uint32_t PageFrameAllocator::kernel_physical_end = 0;
bool PageFrameAllocator::initialised = false;
bool PageFrameAllocator::mapped = false;

/******************************************************************************/

PageFrameAllocator::PageFrameAllocator()
    : last_alloc{1024} // Start allocating pages at 4MB, ensures no clashes
                       // with the kernel data/stack.
{}

/******************************************************************************/

void* PageFrameAllocator::allocate(bool large)
{
    size_t loc = 0;
    bool found = false;

    if (!large)
    {
        // Looking for a 4K page. Start looking at the last allocated one.
        for (size_t test = last_alloc+1; test < number_of_pages; ++test)
        {
            found = !check(reinterpret_cast<void*>(test * page_size));
            if (found)
            {
                loc = test;
                break;
            }
        }

        // Try again from the start
        for (size_t test = 0; test <= last_alloc && !found; ++test)
        {
            found = !check(reinterpret_cast<void*>(test * page_size));
            {
                loc = test;
                break;
            }
        }
    }
    else
    {
        // Round last_alloc up to a 4MB page bounday.
        size_t boundary = (last_alloc >> 10) + 1;
        // Looking for a 4MB page. Start looking at the last allocated one.
        for (size_t test = boundary; test < (number_of_pages >> 10); ++test)
        {
            found = !check(reinterpret_cast<void*>(test * large_page_size),
                           true);
            if (found)
            {
                loc = test;
                break;
            }
        }

        // Try again from the start
        for (size_t test = 0; test < boundary && !found; ++test)
        {
            found = !check(reinterpret_cast<void*>(test * large_page_size),
                           true);
            if (found)
            {
                loc = test;
                break;
            }
        }
    }

    if (!found)
        // No free pages
        return nullptr;

    // Convert page number to physical address and set to used.
    last_alloc = loc;
    if (!large)
    {
        used_pages[loc/32] |= (1 << (loc % 32));
        return reinterpret_cast<void*>(loc * page_size);
    }
    else
    {
        for (size_t i = loc; i < loc + large_page_size/page_size; i += 32)
            used_pages[i/32] = 0xFFFFFFFF;
        return reinterpret_cast<void*>(loc * large_page_size);
    }
}

/******************************************************************************/

void PageFrameAllocator::free(const void* phys_addr, bool large)
{
    if (large)
    {
        // Get the large page number.
        size_t addr = reinterpret_cast<size_t>(phys_addr) >> 22;
        // Convert back to the 4K page number at the start of the large page.
        addr <<= 10;
        // Deallocate all 4K pages in the large page
        for (size_t i = addr; i < addr + large_page_size/(32*page_size); ++i)
            used_pages[i] = 0x0;
    }
    else
    {
        // Get the page number.
        size_t addr = reinterpret_cast<size_t>(phys_addr) >> 12;
        used_pages[addr/32] &= ~(1 << (addr % 32));
    }
}

/******************************************************************************/

bool PageFrameAllocator::check(const void* addr, bool large) const
{
    // Convert address to page index
    size_t test = reinterpret_cast<size_t>(addr) >> 12;
    bool used = false;

    if (!large)
        // Check whether the single page is allocated
        used = (used_pages[test/32] & (1 << (test % 32)));
    else
    {
        // Check all pages within this large page.
        // Round down to the 4MB page boundary.
        size_t boundary = (test >> 10) << 10;
        for (size_t i = boundary; i < boundary + large_page_size/page_size; ++i)
        {
            used = (used_pages[i/32] & (1 << (i % 32)));
            if (used)
                break;
        }
    }

    return used;
}

/******************************************************************************
 ******************************************************************************/

