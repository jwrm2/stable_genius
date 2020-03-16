#include "KernelHeap.h"

#include <ostream>

#include "Kernel.h"
#include "Logger.h"
#include "PageDescriptorTable.h"

/******************************************************************************
 ******************************************************************************/

KernelHeap::KernelHeap(void* s, PageDescriptorTable* p) : pdt{p}
{
    // Set the block data size as heap_align aligned.
    data_size = (((sizeof(BlockData) - 1) >> heap_align_power) + 1) <<
        heap_align_power;

    // Put the heap into an empty state in case we have to call malloc to
    // add a new page table.
    start = nullptr;
    last = nullptr;
    next_page_addr = nullptr;

    // We can round s down to a page boundary. If it was in the middle of a
    // partially occupied page, we'll skip that page below. If it was in the
    // middle of a free page, we might as well use the whole page.
    size_t v_addr = (reinterpret_cast<size_t>(s) >> 12) << 12;

    // Cycle upwards in pages until we find one that is free. Make sure that one
    // gets a virtual mapping. This allocate gets really complicated if it needs
    // to make a new Page Table, but that's dealt with in
    // PageDescriptorTable::new_page_table.
    uint32_t conf = static_cast<uint32_t>(PdeSettings::present) |
        static_cast<uint32_t>(PdeSettings::writable);
    while (!(pdt->allocate(reinterpret_cast<void*>(v_addr), conf)))
    {
        v_addr += PageDescriptorTable::page_size;
        // Check for out of memory.
        if (v_addr < reinterpret_cast<size_t>(s))
            global_kernel->panic(
                "No memory available to initialise kernel heap");
    }

    // Initialise a BlockData, if it wasn't already done by adding a new page
    // table.
    initialise(reinterpret_cast<void*>(v_addr));
}

/******************************************************************************/

KernelHeap::KernelHeap() : 
    start{nullptr},
    last{nullptr},
    next_page_addr{nullptr},
    pdt{nullptr},
    data_size{0}
{}
    

/******************************************************************************/

void* KernelHeap::calloc(size_t nitems, size_t size)
{
    void* result = malloc(nitems * size);

    if (result == nullptr)
        return nullptr;

    // Set all the memory to zero.
    for (size_t i = 0; i < nitems*size; ++i)
    {
        char* ptr = static_cast<char*>(result) + i;
        *ptr = 0;
    }

    return result;
}

/******************************************************************************/

void KernelHeap::dump(klib::ostream& dest)
{
    dest << "Dumping heap state\n";
    dest << "Start of heap at " << start << "; end of heap at " << last;
    dest << "\n";
    dest << "Size of metadata is " << data_size << " bytes\n";

    BlockData* current = start;
    for (size_t i = 0; current != nullptr; ++i)
    {
        dest << "Block number " << i << ": location " << current;
        dest << ", size " << current->size << " bytes";
        dest << ", free " << current->free;
        dest << ", next " << current->next;
        if (current->magic != 0)
            dest << ", magic " << current->magic;
        dest << "\n";

        if (current->next != nullptr &&
            static_cast<size_t>((current->next - current)*sizeof(BlockData)) != 
            current->size + data_size)
        {
            dest << "HEAP CORRUPTION: space for block is ";
            dest << static_cast<size_t>((current->next - current) *
                sizeof(BlockData))  << " but size (incl metadata) is ";
            dest << current->size + data_size << "\n";
        }

        current = current->next;
    }

    dest.flush();
}

/******************************************************************************/

void KernelHeap::free(void* addr)
{
    if (addr == nullptr)
        return;

    // If the pointer given is not to something that was malloced/calloced/
    // realloced, the static_cast will get us in trouble. Fortunately, the
    // C standard says free only has to work for previously allocated memory, so
    // it's the caller's fault if they've done something stupid.
    // We need to reduce the address so we're pointing at the metadata, not the
    // actual value.
    BlockData* current = reinterpret_cast<BlockData*>(
        reinterpret_cast<size_t>(addr) - data_size);
    current->free = true;
    current->magic = 0;

    // Let's see if we can merge with the block after. Checking the block
    // before is prohibitively slow unless we change BlockData to be a doubly-
    // linked list. Don't merge with the last BlockData.
    while (current->next->free && current->next->next != nullptr)
    {
        current->size += data_size + current->next->size;
        current->next = current->next->next;
    }
}

/******************************************************************************/

void KernelHeap::initialise(void* virt_addr)
{
    // Do nothing if we're already initialised.
    if (start != nullptr)
        return;

    // Align the address, rounding upwards.
    size_t v_addr = (((reinterpret_cast<size_t>(virt_addr) - 1) >> 
        heap_align_power) + 1) << heap_align_power;

    // Set up start and last block.
    start = reinterpret_cast<BlockData*>(v_addr);
    start->size = 0;
    start->next = nullptr;
    start->free = true;
    start->magic = 0;
    last = start;

    // Set up the next page address. Assumes that only the current page is
    // allocated.
    v_addr = (v_addr >> 12) << 12;
    next_page_addr = 
        reinterpret_cast<void*>(v_addr + PageDescriptorTable::page_size);
}

/******************************************************************************/

void* KernelHeap::malloc(size_t size, size_t align, size_t magic)
{
    // Do nothing if size is zero.
    if (size <= 0)
        return nullptr;

    // Align size to heap_align bytes.
    size = (((size - 1) >> heap_align_power) + 1) << heap_align_power;

    BlockData* result = find_free_block(size, align);

    if (result == last)
        // No exisiting blocks large enough, create a new one.
        result = new_block(size, align);

    // Set the magic number.
    if (result != nullptr)
        result->magic = magic;

    // Return a pointer to the space, not the metadata.
    return
        reinterpret_cast<void*>(reinterpret_cast<size_t>(result) + data_size);
}

/******************************************************************************/

KernelHeap::BlockData* KernelHeap::find_free_block(size_t size, size_t align)
{
    // Start from the beginning of the heap.
    BlockData* current = start;

    // Iterate through the heap. Simple if we don't have to worry about
    // alignment.
    if (align == 0)
    {
        while (current->next != nullptr && (size > current->size ||
            !current->free))
            current = current->next;
    }
    else
    {
        // We can only align to powers of 2. Find which power.
        size_t power = 0;
        while (align >>= 1)
            ++power;

        // Increase the power if the requested alignment is less than
        // heap_align.
        power = (heap_align_power > power ? heap_align_power : power);

        // More complicated iteration as we have to check the size after
        // alignment.
        BlockData* prev = nullptr;
        while (current->next != nullptr)
        {
            // Easy continue if not free or the total size is too small.
            if (!current->free || size > current->size)
            {
                prev = current;
                current = current->next;
                continue;
            }

            // Round the start of the data up to the specified alignment.
            size_t s = reinterpret_cast<size_t>(current) + data_size;
            size_t as = (((s - 1) >> power) + 1) << power;
            // Get the end.
            size_t e = reinterpret_cast<size_t>(current) + data_size +
                current->size;
            // Check the aligned size.

            if (size <= e - as)
            {
                // Check if there's enough space to split the start off the
                // block.
                if (as == s) ;
                    // Block is already aligned, skip.
                else if (as - s >= data_size + heap_align)
                {
                    // Enough space. Split off the start into a new block.
                    prev = current;
                    prev->size = as - s - data_size;
                    prev->free = true;
                    prev->magic = 0;
                    current = reinterpret_cast<BlockData*>(as - data_size);
                    current->next = prev->next;
                    prev->next = current;
                    current->size = e - as;
                    current->magic = 0;
                }
                else
                {
                    // Not enough space. Move up the current block up.
                    BlockData temp = *current;
                    current = reinterpret_cast<BlockData*>(as - data_size);
                    current->next = temp.next;
                    current->size = temp.size - (as - s);
                    if (prev != nullptr)
                    {
                        // Expand the previous block.
                        prev->next = current;
                        prev->size += (as - s);
                    }
                    else
                        // We were at the start of the heap and we moved up. We
                        // lose max heap_align bytes from the start of the heap.
                        // Oh no.
                        start = current;
                }
                break;
            }
            else
            {
                // Not enough space after alignment.
                prev = current;
                current = current->next;
            }
        }
    }

    // Check whether there's enough space at the end of the block to split off.
    if (current->next != nullptr &&
        current->size >= size + data_size + heap_align)
    {
        BlockData* nb = reinterpret_cast<BlockData*>(
            reinterpret_cast<size_t>(current) + size + data_size);
        nb->free = true;
        nb->size = current->size - size - data_size;
        nb->next = current->next;
        nb->magic = 0;
        current->size = size;
        current->next = nb;
    }

    if (current->next != nullptr)
        current->free = false;

    return current;
}

/******************************************************************************/

KernelHeap::BlockData* KernelHeap::new_block(size_t size, size_t align)
{
    // Allocate new pages if required. We could reduce this a bit if we worked
    // out the proper alignment size, rather than using the maximum possible,
    // but that's only an issue if we're almost out of pages, in which case we'd
    // probably hit trouble soon anyway.
    while((reinterpret_cast<size_t>(last) + data_size + size + align) >=
        reinterpret_cast<size_t>(next_page_addr))
    {
        next_page();
    }

    if (align != 0)
    {
        // We need to make sure the space is aligned as specified. We can only
        // align to powers of 2. Get the power.
        size_t power = 0;
        while (align >>= 1)
            ++power;

        // Increase the power if the requested alignment is less than
        // heap_align.
        power = (heap_align_power > power ? heap_align_power : power);

        // Get where we need the space to start.
        size_t ls = reinterpret_cast<size_t>(last) + data_size;
        size_t as = (((ls - 1) >> power) + 1) << power;

        // Check whether there's enough space for a new block.
        if (as - data_size - ls >= data_size + heap_align)
        {
            // There's enough space. Make a new block.
            BlockData* nl = reinterpret_cast<BlockData*>(as - data_size);
            last->next = nl;
            last->free = true;
            last->size = as - data_size - ls;
            last = nl;
            last->next = nullptr;
            last->free = true;
            last->size = 0;
        }
        else
        {
            // Not enough space. Shift up the start of the current last block.
            BlockData* prev_last = last;
            last = reinterpret_cast<BlockData*>(as - data_size);
            last->free = true;
            last->next = nullptr;
            last->size = 0;

            // We need to find the previous block to move its next pointer.
            // Slow, but this is an edge case.
            if (start == prev_last)
            {
                // There are no blocks. Shift up the start of the heap. Lose
                // heap_align bytes maximum. Oh no.
                start = last;
            }
            else
            {
                // Adjust the next pointer and size of the second last block.
                BlockData* prev = start;
                while (prev->next != prev_last)
                    prev = prev->next;
                prev->next = last;
                prev->size = reinterpret_cast<size_t>(last) -
                    reinterpret_cast<size_t>(prev) - data_size;
            }
        }
    }

    // Change last to reflect the new block.
    last->size = size;
    last->free = false;
    BlockData* prev_last = last;

    // Create a new empty last.
    last = reinterpret_cast<BlockData*>(reinterpret_cast<size_t>(last) + size
        + data_size);
    last->size = 0;
    last->free = true;
    last->next = nullptr;
    prev_last->next = last;

    return prev_last;
}

/******************************************************************************/

void KernelHeap::next_page()
{
    // Try to get the next page.
    uint32_t conf = static_cast<uint32_t>(PdeSettings::present) |
        static_cast<uint32_t>(PdeSettings::writable);
    bool recursive_call = false;
    if (!pdt->allocate(next_page_addr, conf, nullptr, &recursive_call))
    {
        // Unable to get a new page.
        global_kernel->panic("Unable to allocate a new page for the heap");
    }

    // Update next_page_addr to reflect the new space. This is not entirely
    // trivial, as allocating new Page Tables can cause recursive calls to this
    // function. The bool* in the allocate is used to indicate this special
    // case.
    if (!recursive_call)
    {
        next_page_addr = reinterpret_cast<void*>(
            reinterpret_cast<size_t>(next_page_addr) +
            PageDescriptorTable::page_size);
    }
}

/******************************************************************************
 ******************************************************************************/
