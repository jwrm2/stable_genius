#ifndef PAGE_FRAME_ALLOCATOR_H
#define PAGE_FRAME_ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>

#include <array>
#include <ostream>
#include <vector>

// Forward declaration
class MultiBootMap;
class PageDescriptorTable;

/**
    Manages the used pages and supplies new ones when requested.
 */
class PageFrameAllocator {
public:
    /**
        Used to intialise the used_pages array. Will set everything below 1MB
        and everything used by the kernel as used.
     */
    PageFrameAllocator();

    /**
        Find a free page and allocate it.

        @param large True for a large page.
        @return Physical address of the page found, or nullptr for a fail.
     */
    void* allocate(bool large = false);

    /**
        Free a given page. It's the caller's fault if there's still virtual
        memory mapping to this physical page (eg if multiple virtual address
        map to the same physical address) and those address become invalid.

        @param phys_addr Physical address to unmap. Will be rounded down to a
                         page boundary.
        @param large True for a 4MB page.
     */
    void free(const void* phys_addr, bool large = false);

    /**
        Number of pages, asuuming 4KB pages and 4GB of memory.
     */
    static constexpr size_t number_of_pages = 1048576;

    // These functions need access to the private static members.
    friend void pfa_dump_status(klib::ostream& dest);
    friend void pfa_initialise(const void*, const void*);
    friend void pfa_read_map(const klib::vector<MultiBootMap>& map);

private:
    // Size of normal pages (4KB)
    static const size_t page_size = 4096;
    // Size of large pages (4MB)
    static const size_t large_page_size = 4194304;

    // The bit map for which physical pages are used. Since there are 4GB of 
    // memory and each page is 4KB, there are 1 million ish pages. With a bit
    // for each one, we need 128KB to store the bit map. This is static data as
    // we're assuming there's only one set of physical 4GB RAM available.
    static klib::array<uint32_t, number_of_pages / sizeof(uint32_t)> used_pages;
    // Total available memory, as indicated by the highest address in the
    // MultiBoot memory map. Assumed to be 64MB to start with.
    static uint32_t mem_size;

    // Index of the last allocated free page.
    size_t last_alloc;

    // Physical memory address of the start and end of the kernel.
    static uint32_t kernel_physical_start;
    static uint32_t kernel_physical_end;

    // Used to prevent multiple initialisations.
    static bool initialised;
    // Used to prevent multiple mappings.
    static bool mapped;

    /**
        Checks whether the physical address provided is already allocated.

        @param addr Physcial address to check.
        @param large True for a large page.
        @return True if the page is already allocated.
     */
    bool check(const void* addr, bool large = false) const;
};

/**
    Helper function to dump some information on memory.

    @param dest Stream to write to.
 */
void pfa_dump_status(klib::ostream& dest);

/**
    Helper function to deal with static member initialisation.

    @param kps Physical memory address of the start of the kernel.
    @param kpe Physical memory address of the end of the kernel.
 */
void pfa_initialise(const void* kps, const void* kpe);

/**
    Helper function to process the MultiBoot map.

    @param map MultiBoot memory map.
 */
void pfa_read_map(const klib::vector<MultiBootMap>& map);

#endif
