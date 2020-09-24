#include "../include/unistd.h"
#include "../include/UserHeap.h"

#include <stddef.h>
#include <stdint.h>

namespace NMSP {

// User heap is only defined in user space.
#ifndef KLIB

namespace helper {

/******************************************************************************
 ******************************************************************************/

// Global UserHeap. This needs to be in the bss section, so we put it here.
// However, we also need to not call its global constructor, so the real object
// is actually a char[] of the right size and alignment.
UserHeap* user_heap;
alignas(UserHeap) char user_heap_space[sizeof(UserHeap) / sizeof(char)];

/******************************************************************************
 ******************************************************************************/

UserHeap::UserHeap(void* s) :
    start {static_cast<BlockData*>(s)},
    last {static_cast<BlockData*>(s)}
{
    // Set the block data size as heap_align aligned.
    data_size = (((sizeof(BlockData) - 1) >> heap_align_power) + 1) <<
        heap_align_power;

    // System call to allocate enough space for the empty block. Really this
    // will allocate a whole page, but since we don't know the page size, we
    // won't worry about that.
    if (brk(static_cast<uint8_t*>(s) + data_size) != 0)
    {
        // Failure. We don't have a heap. Setting start to nullptr will ensure
        // all allocations fail immediately.
        start = nullptr;
    }
    else
    {
        // Initialise a BlockData.
        start->size = 0;
        start->next = nullptr;
        start->free = true;
    }
}

/******************************************************************************/

void* UserHeap::calloc(size_t nitems, size_t size)
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

void UserHeap::free(void* addr)
{
    if (addr == nullptr || start == nullptr)
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

void* UserHeap::malloc(size_t size)
{
    // Check for heap validity.
    if (start == nullptr)
        return nullptr;

    // Do nothing if size is zero.
    if (size <= 0)
        return nullptr;

    // Align size to heap_align bytes.
    size = (((size - 1) >> heap_align_power) + 1) << heap_align_power;

    BlockData* result = find_free_block(size);

    if (result == last)
        // No exisiting blocks large enough, create a new one at the end.
        result = new_block(size);

    // Return a pointer to the space, not the metadata.
    return
        reinterpret_cast<void*>(reinterpret_cast<size_t>(result) + data_size);
}

/******************************************************************************/

UserHeap::BlockData* UserHeap::find_free_block(size_t size)
{
    // Start from the beginning of the heap.
    BlockData* current = start;

    // Iterate through the heap.
    while (current->next != nullptr && (size > current->size ||
        !current->free))
        current = current->next;

    // Check whether there's enough space at the end of the block to split off.
    if (current->next != nullptr &&
        current->size >= size + data_size + heap_align)
    {
        BlockData* nb = reinterpret_cast<BlockData*>(
            reinterpret_cast<size_t>(current) + size + data_size);
        nb->free = true;
        nb->size = current->size - size - data_size;
        nb->next = current->next;
        current->size = size;
        current->next = nb;
    }

    if (current->next != nullptr)
        current->free = false;

    return current;
}

/******************************************************************************/

UserHeap::BlockData* UserHeap::new_block(size_t size)
{
    // Expand the heap memory area.
    if (brk(reinterpret_cast<uint8_t*>(last) + 2 * data_size + size) != 0)
        return nullptr;

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

/******************************************************************************
 ******************************************************************************/

} // end helper namespace

#endif /* KLIB not defined */

} // end NMSP namespace
