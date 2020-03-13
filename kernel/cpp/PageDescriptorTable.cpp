#include "PageDescriptorTable.h"

#include <cstring>
#include <new>
#include <ostream>
#include <string>

#include "Kernel.h"
#include "KernelHeap.h"
#include "Logger.h"
#include "paging.h"

/******************************************************************************
 ******************************************************************************/

PageTable::PageTable()
{
    uint32_t val = static_cast<uint32_t>(PdeSettings::writable);
    for (size_t i = 0; i < sz; ++i)
        entries[i] = val;
}

/******************************************************************************/

void PageTable::dump(klib::ostream& dest) const
{
    dest << "PT Entries:\n" << klib::hex;
    for (size_t i = 0; i < sz; i += 4)
    {
        dest << i << '-' << (i+3) << ": " << entries[i] << ' ' << entries[i+1];
        dest << ' ' << entries[i+2] << ' ' << entries[i+3] << '\n';
    }
    dest << klib::dec;

    dest.flush();
}

/******************************************************************************/

bool PageTable::empty() const
{
    for (size_t i = 0; i < sz; ++i)
    {
        if ((entries[i] & static_cast<uint32_t>(PdeSettings::present)) != 0)
        {
            return false;
        }
    }

    return true;
}

/******************************************************************************/

void PageTable::free(const void* virt_addr)
{
    // Get the index within this table.
    size_t index = (reinterpret_cast<size_t>(virt_addr) >> 12) & 0x000003FF;
    // Set the page to not present.
    entries[index] = static_cast<uint32_t>(PdeSettings::writable);
    // Invalidate the page in the TLB.
    size_t v_addr = (reinterpret_cast<size_t>(virt_addr) >> 12) << 12;
    invalidate_page(reinterpret_cast<void*>(v_addr));
}

/******************************************************************************/

bool PageTable::set(const void* page, size_t n, uint32_t conf, const void* virt)
{
    if (n >= sz)
        return false;

    uint32_t entry = reinterpret_cast<uint32_t>(page);
    if (entry & 0x00000FFF)
    {
        global_kernel->syslog()->error(
            "Physical page address is not 4K aligned\n"
        );
        return false;
    }

    if (conf & 0xFFFFF000)
        return false;

    entry |= conf;
    entries[n] = entry;

    // Invalidate the page in the TLB.
    invalidate_page(virt);

    return true;
}

/******************************************************************************/

uint32_t PageTable::operator[](size_t n) const
{
    // Returning anything with the last bit unset (not present) will show the
    // caller something has gone wrong.
    if (n >= sz)
        return 0;

    return entries[n];
}

/******************************************************************************
 ******************************************************************************/

PageDescriptorTable::PageDescriptorTable(const PageFrameAllocator& p)
    : pfa{p} 
{
    uint32_t val = static_cast<uint32_t>(PdeSettings::writable);
    for (size_t i = 0; i < sz; ++i)
    {
        entries[i] = val;
        virt_entries[i] = nullptr;
    }
}

/******************************************************************************/

PageDescriptorTable::PageDescriptorTable(const PageDescriptorTable& other) :
    pfa {other.pfa}
{
    for (size_t i = 0; i < sz; ++i)
        entries[i] = other.entries[i];
    for (size_t i = 0; i < sz; ++i)
        virt_entries[i] = other.virt_entries[i];
}

/******************************************************************************/

PageDescriptorTable& PageDescriptorTable::operator=(
    const PageDescriptorTable& other)
{
    pfa = other.pfa;
    for (size_t i = 0; i < sz; ++i)
        entries[i] = other.entries[i];
    for (size_t i = 0; i < sz; ++i)
        virt_entries[i] = other.virt_entries[i];

    return *this;
}

/******************************************************************************/

bool PageDescriptorTable::allocate(const void* virt_addr,
    uint32_t conf, const void* phys_addr, bool* recursive)
{
    // Fail if the configuration is for not present
    if (!(conf & static_cast<uint32_t>(PdeSettings::present)))
        return false;

    // Fail if the configuration has bits set beyond the 12th bit.
    if (conf & 0xFFFFF000)
        return false;

    // Fail if the virtual address is already in use.
    if (translate(virt_addr) != nullptr)
        return false;

    size_t v_addr = reinterpret_cast<size_t>(virt_addr);
    bool success = false;

    if (conf & static_cast<uint32_t>(PdeSettings::large))
    {
        uint32_t p_addr;
        if (phys_addr == nullptr)
        {
            // Get the physical address of an unused large page.
            p_addr = reinterpret_cast<uint32_t>(pfa.allocate(true));
        }
        else
        {
            // Round the provided physical address down to a large page
            // boundary.
            p_addr =
                reinterpret_cast<uint32_t>(phys_addr) & ~(large_page_size - 1);
        }

        // Fail if no memory was found.
        if (p_addr == 0)
            return false;

        // Calculate the PDT index.
        size_t pdt_index = v_addr >> 22;

        // Set the entry.
        entries[pdt_index] = reinterpret_cast<uint32_t>(p_addr) & conf;

        // Invalidate the pages in the TLB.
        v_addr = (v_addr >> 22) << 22;
        for (size_t i = v_addr; i < v_addr + large_page_size; i += page_size)
            invalidate_page(reinterpret_cast<void*>(i));

        success = true;
    }
    else
    {
        // Calculate the PDT index.
        size_t pdt_index = v_addr >> 22;

        void* p_addr;
        if (phys_addr == nullptr)
        {
            // Get the physical address of an unused page.
            p_addr = pfa.allocate();
        }
        else
        {
            // Round the provided physical address down to a page boundary.
            p_addr = reinterpret_cast<void*>(
                reinterpret_cast<uint32_t>(phys_addr) & ~(page_size - 1));
        }

        // Fail if no memory was found.
        if (p_addr == nullptr)
            return false;

        if (!(entries[pdt_index] & static_cast<uint32_t>(PdeSettings::present)))
        {
            // We need to create a new Page Table, but the heap is not
            // guaranteed to be available. Call a function to do some fiddling.
            // All PTs are kernel accessible only.
            new_page_table(virt_addr, conf);
        }

        // Get the Page Table.
        PageTable* pt = get(pdt_index);
        if (pt == nullptr)
            return false;

        // We can get a weird fail condition here: if we created a new Page
        // Table that contains it's own virtual address, it may occupy the space
        // we were asked to allocate. In that case, we need to not overwrite it
        // with the physical memory we just got, or we'll wipe the Page Table
        // data and get a page fault. Thus we just need to check that the page
        // is not present first.

        // Calculate the Page Table index.
        size_t pt_index = (v_addr >> 12) & 0x000003FF;

        // Set the value
        if ((pt->entries[pt_index] &
            static_cast<size_t>(PdeSettings::present)) == 0)
        {
            success = pt->set(p_addr, pt_index, conf, virt_addr);
        }
        else
        {
            // This is a weird situation: we've set up a mapping for the virtual
            // memory requested, but promptly filled it. This is only going to
            // be an issue for the kernel heap (as new Page Tables only go on
            // the kernel heap), so we'll trust the heap to sort it out. We do
            // want to free the physical memory we just grabbed, or it will be
            // leaked.
            pfa.free(p_addr);
            if (recursive != nullptr)
                *recursive = true;
            success = true;
        }
    }

    return success;
}

/******************************************************************************/

void PageDescriptorTable::clean_user_space(const void* end)
{
    const uintptr_t e = reinterpret_cast<uintptr_t>(end) / large_page_size;
    for (size_t i = 0; i < e; ++i)
        entries[i] = static_cast<uint32_t>(PdeSettings::writable);
    for (size_t i = 0; i < e; ++i)
        virt_entries[i] = nullptr;

    // Invalidate the TLB, but only if this is the kernel PDT.
    if (global_kernel->get_pdt() == this)
        load();
}

/******************************************************************************/

void PageDescriptorTable::dump(klib::ostream& dest) const
{
    dest << "Entries:\n" << klib::hex;
    for (size_t i = 0; i < sz; i += 4)
    {
        dest << i * large_page_size << '-' << (i+3) * large_page_size << ": ";
        dest << entries[i] << ' ' << entries[i+1] << ' ';
        dest << entries[i+2] << ' ' << entries[i+3] << '\n';
    }

    dest << "Virtual Entries:\n";
    for (size_t i = 0; i < sz; i += 4)
    {
        dest << i * large_page_size << '-' << (i+3) * large_page_size << ": ";
        dest << virt_entries[i] << ' ' << virt_entries[i+1] << ' ';
        dest << virt_entries[i+2] << ' ' << virt_entries[i+3] << '\n';
    }

    for (size_t i = 0; i < sz; ++i)
    {
        if (entries[i] & static_cast<uint32_t>(PdeSettings::present))
        {
            dest << "Dumping PT for address " << i * large_page_size << '\n';
            virt_entries[i]->dump(dest);
        }
    }

    dest << klib::dec;

    dest.flush();
}

/******************************************************************************/

void PageDescriptorTable::duplicate_user_space(const void* end)
{
    // The kernel PDT, for address translation of kernel space.
    PageDescriptorTable& k_pdt = *global_kernel->get_pdt();

    // Get the end address, rounded up to a page table boundary.
    uintptr_t e = reinterpret_cast<uintptr_t>(end);
    e = (e % large_page_size == 0 ?
        e / large_page_size : e / large_page_size + 1);

    // Reserve some space for the copying operation. Needs to be page aligned.
    void* copy_space = global_kernel->get_heap()->malloc(page_size, page_size);
    if (copy_space == nullptr)
        global_kernel->panic(
            "Failed to get heap space for user memory copying");
    if ((reinterpret_cast<size_t>(copy_space) & 0x00000FFF) != 0)
        global_kernel->panic("Memory allocated for user memory copying was not 4096 byte aligned");

    // Outside iteration over the page tables.
    for (size_t i = 0; i < e; ++i)
    {
        if (k_pdt.virt_entries[i] != nullptr)
        {
            // We have a page table to copy. First make a new page table. This
            // needs to be 4K aligned.
            PageTable* new_pt = static_cast<PageTable*>(
                global_kernel->get_heap()->malloc(sizeof(PageTable),
                page_size));
            if (new_pt == nullptr)
                global_kernel->panic(
                    "Failed to get heap space for new Page Table");
            if ((reinterpret_cast<size_t>(new_pt) & 0x00000FFF) != 0)
                global_kernel->panic("Memory allocated for new Page Table was not 4096 byte aligned");
            // Add it's physical and virtual addresses to the PDT.
            virt_entries[i] = new_pt;
            uint32_t conf = k_pdt.entries[i] & 0x00000FFF;
            entries[i] =
                reinterpret_cast<uint32_t>(k_pdt.translate(new_pt)) | conf;

            // Inside iteration over the pages within the table.
            for (size_t j = 0; j < PageTable::sz; ++j)
            {
                // Calculate the virtual address in user space.
                const void* virt_addr = reinterpret_cast<const void*>(
                    i * large_page_size + j * page_size);

                // Continue if it doesn't exist.
                if ((k_pdt.virt_entries[i]->entries[j] &
                    static_cast<uint32_t>(PdeSettings::present)) == 0)
                    continue;

                // Copy the data into the temporary copy space.
                klib::memcpy(copy_space, virt_addr, page_size);

                // Now we have a copy of the data in kernel space. Get the PT to
                // point at the data.
                conf = k_pdt.virt_entries[i]->entries[j] & 0x00000FFF;
                virt_entries[i]->set(k_pdt.translate(copy_space), j, conf,
                    virt_addr);

                // So now we have two virtual mappings to the same physical
                // address. Remap the kernel space one so it's ready for the
                // next page.
                conf = static_cast<uint32_t>(PdeSettings::present) |
                    static_cast<uint32_t>(PdeSettings::writable);
                k_pdt.free(copy_space, false);
                k_pdt.allocate(copy_space, conf);
            }
        }
    }

    // Free up the copy space.
    global_kernel->get_heap()->free(copy_space);
}

/******************************************************************************/

void PageDescriptorTable::free(const void* virt_addr, bool phys_free)
{
    // Do nothing if the virt_addr is currently unmapped.
    void* phys_addr = translate(virt_addr);
    if (phys_addr == nullptr)
    {
        return;
    }

    // Get the Page Descriptor Entry.
    size_t v_addr = reinterpret_cast<size_t>(virt_addr);
    size_t pdt_index = v_addr >> 22;

    // Check whether or not it's a 4MB page.
    bool large = entries[pdt_index] & static_cast<uint32_t>(PdeSettings::large);

    if (large)
    {
        if (phys_free)
        {
            // Free the physical memory.
            pfa.free(phys_addr, true);
        }

        // Set the PDE to not present. Hope this entry doesn't cover the kernel
        // %eip and stack...
        entries[pdt_index] = static_cast<uint32_t>(PdeSettings::writable);

        // Invalidate the pages in the TLB.
        v_addr = (v_addr >> 22) << 22;
        for (size_t i = v_addr; i < v_addr + large_page_size; i += page_size)
            invalidate_page(reinterpret_cast<void*>(i));
    }
    else
    {
        if (phys_free)
        {
            // Free the physical memory.
            pfa.free(phys_addr);
        }

        // Get the Page Table and free.
        PageTable* pt = get(pdt_index);
        pt->free(virt_addr);

        // Check whether the Page Table is now empty.
        if (pt->empty())
        {
            // Delete the Page Table from the heap and set the PDE entry to
            // not present.
            entries[pdt_index] = static_cast<uint32_t>(PdeSettings::writable);
            virt_entries[pdt_index] = nullptr;
            delete pt;
        }
    }
}

/******************************************************************************/

void PageDescriptorTable::free_user_space(const void* end, bool phys_free)
{
    uintptr_t e = reinterpret_cast<uintptr_t>(end) / large_page_size;
    for (size_t pdt_index = 0; pdt_index < e; ++pdt_index)
    {
        if (virt_entries[pdt_index] != nullptr)
        {
            for (size_t pt_index = 0; pt_index < page_size; ++pt_index)
            {
                const void* virt_addr = reinterpret_cast<const void*>(
                    pdt_index * page_size + pt_index * page_size);
                free(virt_addr, phys_free);
            }
        }
    }

    // Invalidate the TLB, but only if this is the kernel PDT.
    if (global_kernel->get_pdt() == this)
        load();
}

/******************************************************************************/

PageTable* PageDescriptorTable::get(size_t n)
{
    if (n >= sz)
        return nullptr;

    if (!(entries[n] & static_cast<uint32_t>(PdeSettings::present)))
        return nullptr;

    return virt_entries[n];
}

/******************************************************************************/

const PageTable* PageDescriptorTable::get(size_t n) const
{
    if (n >= sz)
        return nullptr;

    if (!(entries[n] & static_cast<uint32_t>(PdeSettings::present)))
        return nullptr;

    return virt_entries[n];
}

/******************************************************************************/

void PageDescriptorTable::load() const
{
    // Translate from virtual to physical address, using the kernel pdt.
    void* phys_addr = global_kernel->get_pdt()->translate(entries.data());

    if (reinterpret_cast<uint32_t>(phys_addr) & 0x00000FFF)
        global_kernel->syslog()->error("PDT is not 4K aligned\n");
    else if (phys_addr == nullptr)
        global_kernel->syslog()->error(
            "PDT virtual address cannot be translated to physical address\n");
    else
    {
        load_pdt(phys_addr);
    }
}

/******************************************************************************/

void* PageDescriptorTable::map(const void* phys_addr,
    size_t size,
    const void* hint)
{
    // We could look to see whether the requested address is already mapped, but
    // that would be slow, and since we can have multiple mappings to the same
    // physical address, well just create a new mapping.

    // Distance from the start of the physical page to the address of interest.
    size_t offset = reinterpret_cast<size_t>(phys_addr) & (page_size - 1);

    // Physical address rounded down to a page boundary.
    void* p_addr = reinterpret_cast<void*>(
        reinterpret_cast<size_t>(phys_addr) & ~(page_size - 1)
    );

    // Find how many pages we need.
    size_t pages = (size + offset) / page_size + 1;

    // Start searching at the hint.
    void* virt_addr = nullptr;
    for (uint32_t page_no = 
        reinterpret_cast<uint32_t>(hint) / page_size;
        page_no < PageFrameAllocator::number_of_pages;
        ++page_no)
    {
        bool found = true;
        for (size_t test_no = 0; test_no < pages; ++test_no)
        {
            if (translate(
                    reinterpret_cast<void*>((page_no + test_no) * page_size)) !=
                    nullptr)
            {
                found = false;
                page_no += test_no;
                break;
            }
        }

        if (found)
        {
            virt_addr = reinterpret_cast<void*>(page_no * page_size);
            break;
        }
    }

    // Carry on searching from the start if necessary.
    if (virt_addr == nullptr)
    {
        for (uint32_t page_no = 0;
            page_no < reinterpret_cast<uint32_t>(hint) / page_size;
            ++page_no)
        {
            bool found = true;
            for (size_t test_no = 0; test_no < pages; ++test_no)
            {
                if (translate(reinterpret_cast<void*>(page_no * page_size))
                    != nullptr)
                {
                    found = false;
                    page_no += test_no;
                    break;
                }
            }

            if (found)
            {
                virt_addr = reinterpret_cast<void*>(page_no * page_size);
            }
        }
    }

    // Give up if we're out of virtual memory.
    if (virt_addr == nullptr)
    {
        global_kernel->syslog()->error(
            "PageDescriptorTable::map out of virtual memory\n"
        );
        return nullptr;
    }

    //global_kernel->syslog()->info("Mapping %u pages of physical address %p to virtual address %p.\n", pages, p_addr, virt_addr);

    // Allocate the pages.
    void* ret_address = reinterpret_cast<void*>(
        reinterpret_cast<size_t>(virt_addr) + offset
    ); 
    uint32_t conf = static_cast<uint32_t>(PdeSettings::present) |
        static_cast<uint32_t>(PdeSettings::writable);
    for (size_t i = 0; i < pages; ++i)
    {
        if(!allocate(virt_addr, conf, p_addr))
            return nullptr;
        virt_addr = reinterpret_cast<void*>(
            reinterpret_cast<size_t>(virt_addr) + page_size
        );
        p_addr = reinterpret_cast<void*>(
            reinterpret_cast<size_t>(p_addr) + page_size
        );
    }

    // Return the virtual address that maps to the start of phys_addr.
    return ret_address;
}

/******************************************************************************/

void PageDescriptorTable::unmap(const void* virt_addr, size_t size)
{
    // Virtual address rounded down to a page boundary.
    void* v_addr = reinterpret_cast<void*>(
        reinterpret_cast<size_t>(virt_addr) & ~(page_size - 1)
    );

    // Distance from the start of the virtual page to the address of interest.
    size_t offset = reinterpret_cast<size_t>(virt_addr) & (page_size - 1);

    // Find how many pages we need to free.
    size_t pages = (size + offset) / page_size + 1;

    //global_kernel->syslog()->info("Freeing %u pages at virtual address %p.\n", pages, v_addr);

    // Do the freeing, but don't free the physical memory.
    for (size_t i = 0; i < pages; ++i)
    {
        free(v_addr, false);
        v_addr = reinterpret_cast<void*>(
            reinterpret_cast<size_t>(v_addr) + page_size
        );
    }
}

/******************************************************************************/

void PageDescriptorTable::new_page_table(const void* virt_addr, uint32_t conf)
{
    // Fail if the configuration is not sensible.
    if (!(conf & static_cast<uint32_t>(PdeSettings::present)) ||
        conf & 0xFFFFF000)
        global_kernel->panic("Bad configuration for temporary page table");

    // Free the virtual address for the temporary page table. We don't want to
    // free the physical space: that could be bad as it might be where the
    // previously added page table exists.
    size_t pdt_index = reinterpret_cast<size_t>(temp_page_table) >> 22;
    PageTable* pt = get(pdt_index);
    pt->free(temp_page_table);

    // Allocate a new entry. This is a recursive call to allocate, but as long
    // as the temp_page_table is within an existing Page Table, it should be
    // fine. temp_page_table was set aside in assembly to be within a Page
    // Table.
    if (!allocate(static_cast<void*>(temp_page_table),
        static_cast<uint32_t>(PdeSettings::present) |
        static_cast<uint32_t>(PdeSettings::writable)))
        global_kernel->panic("Unable to allocate temporary page table");

    // Now we have a Page Table in an undefined state sitting somewhere in
    // physical memory and at the temporary page table store in virtual memory.

    // Put the Page Table into a valid state.
    temp_page_table = new (temp_page_table) PageTable{};

    // Add the new page table to the PDT, where it will cover the requested
    // address.
    pdt_index = reinterpret_cast<size_t>(virt_addr) >> 22;
    if (!set(temp_page_table, pdt_index, conf))
        global_kernel->panic("Failed to add temporary page table to the PDT");

    // Now we have new Page Table with it's physical address loaded into the PDT
    // that covers the virtual region requested. This is a 'safe' state, but we
    // want to be able to virtually access the new Page Table even after another
    // new Page Table is made.

    // We want to add the Page Table to the heap. This is a little complicated.
    // Firstly, we're probably in the middle of a heap allocation that meant we
    // had to create a new Page Table. Secondly, some idiot might have decided
    // to initialise the heap in a new 4MB section, meaning the heap might not
    // be fully functional yet.

    if (global_kernel->get_heap() == nullptr)
    {
        // This is the special case where the heap doesn't exist yet. This could
        // only happen if we try to initialise the heap in a 4MB
        // unmapped section. If we asked to initialise the heap in the middle of
        // a 4MB section, we'll assume it's ok to put it at the start of the
        // section instead. This avoids a horrible scenario. Putting the Page
        // Table into the heap needs three pages (one for the meta data for the
        // Page Table, one for the Page Table, and one for the terminating meta
        // data). If the address given is in the last two pages, the second or
        // third required page might be unmapped. Then when malloc asks for a
        // new page, we'll get back here creating another new Page Table and
        // temp_page_table will get overwritten. Ouch.

        // Create a mapping for the first page of the table. Calling allocate
        // recursively again, but we know the Page Table exists as we just made
        // it.
        allocate(reinterpret_cast<void*>(pdt_index << 22), conf);

        // Manual heap initialisation. Yes, I know it's not pretty. Any better
        // ideas? This should be enough to be able to call malloc.
        global_kernel->get_heap()->initialise(
            reinterpret_cast<void*>(pdt_index << 22)
        );
    }

    // Ask the heap for aligned memory. This will probably cause malloc to ask
    // for two new pages, but that should be fine as the mapping exists (we've
    // just created it) and the Page Table will get updated at temp_page_table.
    // TODO: Am I sure about this? What if the new PT is for user space, but
    // creating the PT requires a new PT for the kernel heap?
    pt = static_cast<PageTable*>(
        global_kernel->get_heap()->malloc(sizeof(PageTable), page_size));
    if (pt == nullptr)
        global_kernel->panic("Failed to get heap space for new Page Table");
    if ((reinterpret_cast<size_t>(pt) & 0x00000FFF) != 0)
        global_kernel->panic(
            "Memory allocated for new Page Table was not 4096 byte aligned");

    // Get the indices for the new virtual address for the Page Table. Note
    // this isn't necessarily within the new Page Table, as malloc might have
    // found a free block somewhere else.
    size_t pdt_index_addr = reinterpret_cast<size_t>(pt) >> 22;
    PageTable* pt_addr = get(pdt_index_addr);
    size_t pt_index = (reinterpret_cast<size_t>(pt) >> 12) & 0x000003FF;
    void* phys_addr = translate(temp_page_table);

    // Update whichever Page Table.
    pt_addr->set(phys_addr, pt_index,
        static_cast<uint32_t>(PdeSettings::present) |
        static_cast<uint32_t>(PdeSettings::writable), pt);

    // Update the PDT virtual entries. Note the index is for where the new Page
    // Table covers, not where it resides.
    virt_entries[pdt_index] = pt;

    // We could free the temporary address here, but let's leave it occupied to
    // make sure no one else accidentally uses it (assembly initiation marks it
    // as in use too).
}

/******************************************************************************/

bool PageDescriptorTable::set(PageTable* pt, size_t n, uint32_t conf)
{
    if (n >= sz)
        return false;

    uint32_t entry = reinterpret_cast<uint32_t>(translate(pt));
    if (entry & 0x00000FFF)
        return false;

    if (conf & 0xFFFFF000)
        return false;

    entry |= conf;
    entries[n] = entry;
    virt_entries[n] = pt;

    return true;
}

/******************************************************************************/

void* PageDescriptorTable::translate(const void* virt_addr, bool print) const
{
    size_t v_addr = reinterpret_cast<size_t>(virt_addr);
    if (print) global_kernel->syslog()->info("translate: v_addr == %X\n", v_addr);
    void* phys_addr;

    // The top 10 bits are the index for the PDT
    size_t pdt_index = v_addr >> 22;

    if (print) global_kernel->syslog()->info("translate: pdt_index == %X\n", pdt_index);

    // Check to see whether the Page Descriptor Entry exists
    if (!(entries[pdt_index] & static_cast<uint32_t>(PdeSettings::present)))
    {
        if (print) global_kernel->syslog()->info("translate: PDE does not exist\n");
        return nullptr;
    }

    if (entries[pdt_index] & static_cast<uint32_t>(PdeSettings::large))
    {
        if (print) global_kernel->syslog()->info("translate: PDE is large\n");
        // The Page Descriptor Entry refers to a large page.
        // Get the physical address of the page start.
        size_t page_start = (entries[pdt_index] & 0xFFFFF000);
        // Get the offset within the page.
        size_t offset = v_addr & 0x003FFFFF;
        phys_addr = reinterpret_cast<void*>(page_start + offset);
    }
    else
    {
        if (print) global_kernel->syslog()->info("translate: PDE is a PT\n");
        // The Page Descriptor Entry refers to a Page Table.
        // Get the Page Table.
        const PageTable* pt = get(pdt_index);
        if (pt == nullptr)
            return nullptr;

        if (print) global_kernel->syslog()->info("translate: PT is at %p virtual\n", pt);
        if (print) global_kernel->syslog()->info("translate: PT is at %p physical\n", entries[pdt_index] & 0xFFFFF000);
        // Bits 12 to 21 give the index for the PT.
        size_t pt_index = (v_addr >> 12) & 0x000003FF;
        if (print) global_kernel->syslog()->info("translate: pt_index == %X\n", pt_index);
        if (print) global_kernel->syslog()->info("translate: (*pt)[pt_index] == %X\n", (*pt)[pt_index]);
        // Check to see whether the Page exists.
        if (!((*pt)[pt_index] &
              static_cast<uint32_t>(PdeSettings::present)))
            return nullptr;

        // Get the physical address of the page start.
        size_t page_start = ((*pt)[pt_index] & 0xFFFFF000);
        // Bits 0 to 11 give the offset within the page.
        size_t offset = v_addr & 0x00000FFF;
        phys_addr = reinterpret_cast<void*>(page_start + offset);
    }

    return phys_addr;
}

/******************************************************************************/

void PageDescriptorTable::update_user_space(
    const PageDescriptorTable& other, const void* end)
{
    // Work out the index of the last user entry.
    const uintptr_t e = reinterpret_cast<uintptr_t>(end) / large_page_size;

    // Copy the physical and virtual entries.
    for (size_t i = 0; i < e; ++i)
    {
        entries[i] = other.entries[i];
        virt_entries[i] = other.virt_entries[i];
    }

    // Invalidate the TLB
    load();
}

/******************************************************************************/

    PageTable* PageDescriptorTable::temp_page_table;

/******************************************************************************
 ******************************************************************************/

void trim_pdt(PageDescriptorTable* pdt, size_t end, size_t max)
{
    // Round the end up to a 4K page boundary.
    end = ((end >> 12) + 1) << 12;

    // Free all pages from the end to the max.
    for ( ; end < max; end += PageDescriptorTable::page_size)
    {
        pdt->free(reinterpret_cast<const void*>(end));
    }
}

/******************************************************************************
 ******************************************************************************/
