#ifndef KERNEL_HEAP_H
#define KERNEL_HEAP_H

#include <stddef.h>

#include <ostream>

// Forward declarations
class PageDescriptorTable;

/**
    Class to manage kernel dynamic memory allocation.
 */
class KernelHeap {
public:
    /**
        Construction only needs to know the start of the heap and the Page
        Descriptor Table, for getting more memory. The constructor will create
        the virtual mapping for the start of the heap if it doesn't already
        exist.

        @param s Memory address for the start of the heap.
        @param p Page Descriptor Table to use for memory allocation.
     */
    KernelHeap(void* s, PageDescriptorTable* p);

    /**
        We need a default constructor so the kernel can create an empty heap
        before it's had a chance to make a real one.
     */
    KernelHeap();

    /**
        Allocates memory of the size given and initialises to zero.

        @param nitems Number of elements to allocate.
        @param size Size of a single element.
        @return Pointer to the start of the allocated memory, or nullptr for
                failure.
     */
    void* calloc(size_t nitems, size_t size);

    /**
        Write the current status of the heap, for debugging purposes.

        @param dest Location to send the information.
     */
    void dump(klib::ostream& dest);

    /**
        Frees the memory at the given pointer previously allocated with malloc,
        calloc or realloc.

        @param ptr Address to free. Nothing happens if it's nullptr.
     */
    void free(void* ptr);

    /**
        Gets the start of the heap, to check whether the heap is in a valid
        state.

        @return Virtual address of the beginning of the heap, or nullptr if
                this is a non-initialised heap.
     */
    const void* get_start() const { return start; }

    /**
        Initialises with the start of the heap at the given address. Shouldn't
        normally be called from outside the constructor, but is necessary if
        the heap begins a new Page Table. Does nothing if the heap is already
        initialised.

        @param virt_addr Virtual address of the start of the heap.
     */
    void initialise(void* virt_addr);

    /**
        Allocates memory of the size given.

        @param size Memory size (in bytes) to allocate.
        @param align Specify that the start of the data must be aligned to a
                     the given number of bytes. Most useful for Page Tables,
                     which must be aligned to 4096 bytes.
        @param magic A number that will be recorded in the heap metadata for
               the allocated block, allowing tracing of where memory was
               allocated from.
        @return Pointer to the start of the allocated memory, or nullptr for
                failure.
     */
    void* malloc(size_t size, size_t align = 0, size_t magic = 0);

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
        // A magic number for allocation tracing.
        size_t magic = 0;
    };

    // Memory address of the start of the heap.
    BlockData* start;
    // Memory address of the end of the heap. This will be a valid BlockData,
    // with it's size as 0 and it's next as nullptr.
    BlockData* last;
    // Memory address of the start of the page above the current highest
    // allocated page.
    void* next_page_addr;
    // Page Descriptor Table for memory allocation.
    PageDescriptorTable* pdt;

    // Size of the BlockData. Even though we expect this to be 16 bytes, we'll
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
    BlockData* find_free_block(size_t size, size_t align = 0);

    // Creates a new block of the specified size at the end of the heap. If
    // align is not zero, the space created will be aligned as specified. This
    // may mean creating two new blocks.
    BlockData* new_block(size_t size, size_t align = 0);

    // Allocates the next page of virtual memory. If it fails (ie the page is
    // already in use) cause a panic.
    void next_page();
};
#endif
