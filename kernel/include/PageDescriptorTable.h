#ifndef PAGEDESCRIPTORTABLE_H
#define PAGEDESCRIPTORTABLE_H

#include <stddef.h>
#include <stdint.h>

#include <array>
#include <ostream>

#include "PageFrameAllocator.h"

// Forward declaration
class PageDescriptorTable;

/**
    Settings for various configuration bits in the PDEs and PTEs.
 */
enum class PdeSettings : uint32_t {
    // Set for a present entry
    present = 0x001,
    // Set for a writable entry
    writable = 0x002,
    // Set for access for user space
    user_access = 0x004,
    // Set for write through caching
    write_through = 0x008,
    // Set to disable caching
    disable_caching = 0x010,
    // Set if the entry has been accessed
    accessed = 0x020,
    // Set for dirty bit (PTE only)
    dirty = 0x040,
    // Set for 4MB pages (PDE only)
    large = 0x080
};

/**
    A Page Table (PT) that contains 1024 Page Table Entries (PTEs). The highest
    20 bits of each PTE are the address of a page. The lowest 12 bits are for
    configuration. Instances of this class must be aligned to a page boundary:
    good luck.
 */
class PageTable {
public:
    /**
        Constructor initialises all addresses to zero. Default configuration is
        for clean unaccessed pages, with write-back caching, only kernel
        accessible, writable and not present.
     */
    PageTable();

    /**
        Dump the entries for debugging purposes to the provided Stream.

        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

    /**
        Tests whether the Page Table is emtpy, that is all the entries are set
        to not present.

        @return True if the Page Table is empty, false otherwise.
     */
    bool empty() const;

    /**
        Removes the entry for the given virtual page. Will also deallocate the
        associated physical memory (which is unsafe if there are multiple
        virtual address for one physical address).

        @param virt_addr Virtual address to free. Will be rounded down to a page
                         boundary.
     */
    void free(const void* virt_addr);

    /**
        Add a Page Table Entry to the PT.

        @param page The physical address of the Page to load.
        @param n The position in the table to load at (bewteen 0 and 1023).
        @param conf Configuration to load.
        @param virt Virtual address, for TLB invalidation.
        @retun Whether the operation succeeded.
     */
    bool set(const void* page, size_t n, uint32_t conf, const void* virt);

    // PageDescriptorTable needs access to the entries.
    friend PageDescriptorTable;

private:
    // Number of entries
    static constexpr size_t sz = 1024;
    // The raw entries (these are physical addresses).
    klib::array<uint32_t, sz> entries;

    /**
        Gets the entry at the specified location.
     */
    uint32_t operator[](size_t n) const;
};

/**
    A Page Descriptor Table (PDT) that contains 1024 Page Descriptor Entries
    (PDEs). The highest 20 bits of each PDE are the address of a Page Table
    (PT). The lowest 12 bits are for configuration. Instances of this class must
    be aligned to a page boundary: good luck.
 */
class PageDescriptorTable {
public:
    /**
        Constructor initialises all addresses to zero. Default configuration is
        for 4KB unaccessed pages, with write-back caching, only kernel
        accessible, writable and not present.
     */
    PageDescriptorTable(const PageFrameAllocator& p = PageFrameAllocator());

    /**
        Copy constructor. This shouldn't really be necessary, but I think the
        array members get copied by memcpy() in the implicit constructor, which
        obviously doesn't exist.

        @param other PDT to copy data from.
     */
    PageDescriptorTable(const PageDescriptorTable& other);

    /**
        Copy assignment. See copy constructor.

        @param other PDT to copy data from.
     */
    PageDescriptorTable& operator=(const PageDescriptorTable& other);

    /**
        Sets up a page at the virtual memory address provided, with the
        configuration provided. Automatically rounds the address down to a page
        boundary. If no physical address is provided, an unused one is requested
        from the page frame allocator; if one is provided, that is mapped to,
        irrespective of whether it already has a mapping, after rounding down
        to the nearest page boundary.

        @param virt_addr Virtual memory address.
        @param conf Configuration.
        @param phys_addr Physical address to map to.
        @param recursive Set to true in the special case of the allocation
               creating a new Page Table that occupies the requested virtual
               memory.
        @return Whether the operation succeeded.
     */
    bool allocate(const void* virt_addr,
        uint32_t conf,
        const void* phys_addr = nullptr,
        bool* already_existed = nullptr);

    /**
        Clears any entries below the indicated index (in 4MB blocks). Does not
        free any physical memory or delete any page tables. Therefore this
        should not be used unless there exists another PDT with pointers to the
        user space PTs.

        @param end Virtual address to wipe up to, rounded down to a page table
               boundary.
     */
    void clean_user_space(const void* end);

    /**
        Dump the entries and virt_entries for debugging purposes to the provided
        Stream.

        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

    /**
        Duplicate the user space memory of the currently loaded PDT into this
        one. New page tables are put on the kernel heap managed by the currently
        loaded PDT. The pointer end is taken as the end of user space.

        @param end Only copy up to this address. The address is rounded up to a
               page table boundary.
     */
    void duplicate_user_space(const void* end);

    /**
        Removes the entry for the given virtual page. Will also deallocate the
        associated physical memory if asked (which is unsafe if there are
        multiple virtual address for one physical address).
        @param virt_addr Virtual address to free. Will be rounded down to a page
               boundary.
        @param phys_free Whether to free the physical memory.
     */
    void free(const void* virt_addr, bool phys_free = true);

    /**
        Removes all entires in user space, freeing page tables, up to the
        provided pointer. Also frees the physical memory if requested.

        @param end Virtual address to wipe up to. Rounded down to a page table
               boundary.
        @param phys_free Whether to free the physical memory.
     */
    void free_user_space(const void* virt_addr, bool phys_free = true);

    /**
        Set this table as the current PDT. Will not enable paging if not already
        enabled.
     */
    void load() const;

    /**
        Creates a new virtual mapping for the provided physical address, of at
        least the given size. Useful when we need to access a specific physical
        address. The mapping could usefully be created anywhere that's free, but
        we'll start searching at hint.

        @param phys_addr Physical address to create a mapping for.
        @param size Size of the physical memory we need to access.
        @param hint Where to start looking for a free virtual address.
     */ 
    void* map(const void* phys,
        size_t size,
        const void* hint = reinterpret_cast<void*>(0x10000000));

    /**
        Frees the virtual address provided, up to the size given. This had
        better have been allocated by the map function with the same size, or we
        run the risk of stomping on someone else's virtual memory. The physical
        memory is not freed.

        @param virt_addr Virtual address to free.
        @param size Memory size to free.
     */
    void unmap(const void* virt_addr, size_t size);

    /**
        Sets the Page Frame Allocator.

        @param Allocator to set. Will be copied. The only non-static data it
               contains is the last allocation, anyway.
     */
    void set_allocator(const PageFrameAllocator& p) { pfa = p; }

    /**
        Translates a virtual address into a physical address.

        @param virt_addr Virtual address to translate.
        @return Physical address the virt_addr is mapped to, or nullptr if the
                appropriate entry is not present.
     */
    void* translate(const void* virt_addr, bool print = false) const;

    /**
        Updates the user space mappings from the given other PDT. Useful for
        swapping a processes mappings into the active PDT.

        @param other Other PDT to copy the kernel space mappings from.
        @param end Virtual address to wipe up to.
     */
    void update_user_space(const PageDescriptorTable& other, const void* end);

    /**
        Location for creating a temporary page table. Guaranteed to be available
        for this purpose and shouldn't be used for anything else.
     */
    static PageTable* temp_page_table;

    /**
        Number of bytes in a page (4KB).
     */
    static constexpr size_t page_size = 4096;

    // Give the initialisation routine access to the private static members.
    friend void trim_pdt(PageDescriptorTable* pdt, size_t end, size_t max);

private:
    // Number of entries.
    static constexpr size_t sz = 1024;
    // Number of bytes in a large page (4MB).
    static constexpr size_t large_page_size = 4194304;
    // The raw entries (these are physical addresses).
    klib::array<uint32_t, sz> entries;
    // Pointers to the virtual address of the Page Tables (or nullptr if the 
    // PDE indicates not present or a 4MB page).
    klib::array<PageTable*, sz> virt_entries;
    // Allocator for getting physical memory.
    PageFrameAllocator pfa;

    /**
        Gets the Page Table for the given virtual address. Can be used to modify
        a Page Table.

        @param n Entry to access (between 0 and 1023).
        @return Page Table at the given virtual address, or nullptr if not
                present, or refers to a 4MB page.
     */
    PageTable* get(size_t n);
    const PageTable* get(size_t n) const;

    /**
        Creates a new Page Table in the temporary reserved space, making sure to
        include the provided virtual address. Adds it's physical address to the
        Page Descriptor Table and sets up a mapping to itself in a free slot
        within it's own Page Table. Frees the virtual temporary space.
        @param virt_addr Virtual address to be included in this Page Table. Will
                         be rounded down to a 4MB page boundary.
        @param conf Configuration to use for the new Page Table.
     */
    void new_page_table(const void* virt_addr, uint32_t conf);    

    /**
        Add a Page Descriptor Entry to the PDT.

        @param pt The virtual address of the Page Table to load.
        @param n The position in the table to load at (between 0 and 1023).
        @param conf Configuration to load (must not be a 4MB page).
        @param Whether the operation succeeded.
     */
    bool set(PageTable* pt, size_t n, uint32_t conf);

    /**
        Add a 4MB page to the PDT.

        @param page The physical address of the Page to load.
        @param n The position in the table to load at (between 0 and 1023).
        @param conf Configuration to load (must be a 4MB page).
        @param Whether the operation succeeded.
     */
    bool set(void* page, size_t n, uint32_t conf);
};

/**
    The assembly loader allocates a full 4MB for the kernel (excluding the
    heap), but this is unlikley to be full, so free everything from the end of
    the kernel to the maximum allocated address.
 */
void trim_pdt(PageDescriptorTable* pdt, size_t end, size_t max);

#endif
