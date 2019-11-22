#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
    Base virtual address used for the kernel (not the same as the virtual
    address of the start of the kernel).
 */
extern const size_t kernel_virtual_base;

/**
    Highest virtual address allocated in assembly before transfer to C.
 */
extern const size_t kernel_virtual_max;

/**
    Loads a Page Descriptor Table.

    @param pdt Address of the PDT to load.
 */
void load_pdt(const void* pdt);

/**
    Enables paging.
 */
void enable_paging();

/**
    Enables Page Size Extension (PSE) ie 4MB pages.
 */
void enable_pse();

/**
    Invladiates the Translation Lookahead Buffer (TLB) for the given virtual
    memory address, which must be at a 4KB page boundary.

    @param virt_addr Virtual address to invalidate.
 */
void invalidate_page(const void* virt_addr);

/**
    Fetch the constents of the CR2 register, which contains the virtual address
    that caused the last page fault.

    @return Current contents of the CR2 register.
 */
uint32_t get_cr2();

#ifdef __cplusplus
}
#endif

#endif
