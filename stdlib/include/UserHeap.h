#ifndef USER_HEAP_H
#define USER_HEAP_H

// Use std as the default namespace.
#ifndef NMSP
#define NMSP std
#endif /* NMSP */

#include <stddef.h>

namespace NMSP {

// User heap is only defined for the user space library.
#ifndef KLIB

namespace helper {

/**
    Class to manage dynamic memory allocation.
 */
class UserHeap {
public:
    /**
        Construction only needs to know the start of the heap.

        @param s Memory address for the start of the heap.
     */
    explicit UserHeap(void* s);

    /**
        Default constructor. Sets the start of the heap to nullptr so that
        future allocations will fail.
     */
    UserHeap() : start {nullptr}, last {nullptr}, data_size {0} {}

    /**
        Allocates memory of the size given and initialises to zero.

        @param nitems Number of elements to allocate.
        @param size Size of a single element.
        @return Pointer to the start of the allocated memory, or nullptr for
                failure.
     */
    void* calloc(size_t nitems, size_t size);

    /**
        Frees the memory at the given pointer previously allocated with malloc,
        calloc or realloc.

        @param ptr Address to free. Nothing happens if it's nullptr.
     */
    void free(void* ptr);

    /**
        Allocates memory of the size given.

        @param size Memory size (in bytes) to allocate.
        @return Pointer to the start of the allocated memory, or nullptr for
                failure.
     */
    void* malloc(size_t size);

    /**
        Reallocates the memory at the address given to the new size.

        @param ptr Pointer to an address previously allocated with malloc,
                   calloc or realloc. If nullptr, a new block of the given size
                   is allocated.
        @param size Size of the new memory to allocate. If 0 and ptr is not
                    nullptr, the existing block will be deallocated and the
                    nullptr returned.
        @return Pointer to the new memory, or nullptr for failure.
     */
    void* realloc(void* ptr, size_t size);

private:
    // Stores meta information about each block.
    struct BlockData {
        // Size of the block, in bytes.
        size_t size = 0;
        // Pointer to the next block.
        BlockData* next = nullptr;
        // Whether the block is currently available.
        bool free = true;
    };

    // Memory address of the start of the heap.
    BlockData* start;
    // Memory address of the end of the heap. This will be a valid BlockData,
    // with it's size as 0 and it's next as nullptr.
    BlockData* last;

    // Size of the BlockData. Even though we expect this to be 12 bytes, we'll
    // set it up in the constructor to be aligned to heap_align.
    size_t data_size;

    // This is the value to align the heap to. Having the power is convenient
    // for bitwise operations.
    static constexpr size_t heap_align = 16;
    static constexpr size_t heap_align_power = 4;

    // Starting from the beginning of the heap, return the first free block
    // of sufficient size, or last if none is found. Splits the block if there
    // is enough trailing space.
    // If the optional alignment is given, only return a free block if there's
    // space to fit in the aligned data. Splits the start off, if there's enough
    // leading space.
    BlockData* find_free_block(size_t size);

    // Creates a new block of the specified size at the end of the heap. If
    // align is not zero, the space created will be aligned as specified. This
    // may mean creating two new blocks.
    BlockData* new_block(size_t size);
};

/**
    Global heap. This can't be on the heap, so we put a pointer to it here, and
    also reserve some space to it. The pointer is fiddled in the standard
    library initialisation.
 */
extern UserHeap* user_heap;
extern char user_heap_space[sizeof(UserHeap) / sizeof(char)];

} // end helper namespace

#endif /* KLIB not defined */

} // end NMSP namespace

#endif /* USER_HEAP_H */
