# Mulitboot header
.set MAGIC_NUMBER, 0x1BADB002 # Identifies this as multiboot to GRUB
.set FLAGS, 0x3 # Align modules, provide memory map
.set CHECKSUM, - (MAGIC_NUMBER + FLAGS) # Required to be this form

# 64 kilobyte kernel stack
.set KERNEL_STACK_SIZE, 0x10000

# This is the virtual memory address for the kernel base. The kernel will
# actally be loaded at 0x00100000 (1MB) but will think it's at 0xC0100000
# (3GB + 1MB)
.set KERNEL_VIRTUAL_BASE, 0xC0000000
# We can get the number of pages before the virtual kernel address by dividing
# by the size of a page (4MB = 2^22)
.set KERNEL_PAGE_NUMBER, KERNEL_VIRTUAL_BASE >> 22

# Put the multiboot header in it's own section so that we can make sure the
# linker puts it early.
.section .multiboot
.align 4
    .long MAGIC_NUMBER
    .long FLAGS
    .long CHECKSUM

.section .data
# Align to a page boundary
.align 4096
BootPdt:
# This is where we're storing the basic Page Directory Table. This table will
# contain entries to map the virtual first 8MB and the 8MB above 3GB, both
# to the real first 8MB. The first will be invalidated before transfering to C,
# we expect the later to remain.
# First we make the identity pages for the first 8MB. The address (upper 20
# bits) is the physical address of the Page Table defined below. 
# We set bit 1 (writable) and bit 0 (present).
    .long (BootPt - KERNEL_VIRTUAL_BASE) + 0x00000003
    .long (BootPt2 - KERNEL_VIRTUAL_BASE) + 0x00000003
# Now we need to add entries for all the space between 8MB and 3GB. All that
# really matters for these is that bit 0 (present) is not set, so we'll leave
# them all as zero. The number of bytes to leave is the number of pages before
# the virtual kernel address minus two (for the pages already used) all
# multiplied by 4 (4 bytes per entry). 
    .skip (KERNEL_PAGE_NUMBER - 2)*4, 0
# This is the entry for the virtual address of the kernel. Again the addresses
# are for the boot Page Tables (they points to the real space from 0 to 8MB)
# and the flags are the same as before.
    .long (BootPt - KERNEL_VIRTUAL_BASE) + 0x00000003
    .long (BootPt2 - KERNEL_VIRTUAL_BASE) + 0x00000003
# Now entries for the rest of the pages, that also should be blank. There are
# 1024 entries total.
    .skip (1024 - KERNEL_PAGE_NUMBER - 2)*4, 0
# In order to allow software mapping from virtual addresses to physical
# addresses, we need to store the pointers to the virtual addresses of the page
# tables, or a null pointer if the page directory entry is not present or is to
# a 4MB page. We can ignore the first one as it'll be unmapped before we get to
# C.
    .skip KERNEL_PAGE_NUMBER*4, 0
    .long BootPt
    .long BootPt2
    .skip (1024 - KERNEL_PAGE_NUMBER - 2)*4, 0
# There's another 4 bytes at the end which is the last_alloc of the Page Frame
# Allocator. We'll set it 0, as it's only for speed and we'll overwrite it in
# initialisation anyway.
    .long 0
# This is where we'll have our first kernel Page Table. For simplicity, we'll
# map the whole 4MB.
# The address (upper 20 bits) is 4096 times the entry we're on. We set bit 1
# (writable) and bit 0 (present).
.align 4096
BootPt:
    .macro  pt from, to
    .long   \from * 4096 + 3
    .if     \to-\from
        pt     "(\from+1)",\to
    .endif
    .endm
# GNU as limits the macro recursion depth...
    pt    0,  100
    pt  101,  200
    pt  201,  300
    pt  301,  400
    pt  401,  500
    pt  501,  600
    pt  601,  700
    pt  701,  800
    pt  801,  900
    pt  901, 1000
    pt 1001, 1023

# This is where we'll have our second kernel Page Table. For simplicity, we'll
# map the whole 4MB.
# The address (upper 20 bits) is 4096 times the entry we're on, plus 4194304
# (the space covered by the first page table). We set bit 1 (writable) and bit 0
# (present).
.align 4096
BootPt2:
    .macro  pt2 from, to
    .long   \from * 4096 + 4194304 + 3
    .if     \to-\from
        pt     "(\from+1)",\to
    .endif
    .endm
# GNU as limits the macro recursion depth...
    pt2    0,  100
    pt2  101,  200
    pt2  201,  300
    pt2  301,  400
    pt2  401,  500
    pt2  501,  600
    pt2  601,  700
    pt2  701,  800
    pt2  801,  900
    pt2  901, 1000
    pt2 1001, 1023

# Also include the KERNEL_VIRTUAL_BASE address for C++ access and the highest
# virtual address allocated.
.align 4
.global kernel_virtual_base
kernel_virtual_base: .long KERNEL_VIRTUAL_BASE
.global kernel_virtual_max
kernel_virtual_max: .long KERNEL_VIRTUAL_BASE + 0x00800000

.section .bss
# Here we reserve 64 kilobytes for the kernel stack, which must be 16 byte
# aligned.
.align 16
    stack_base: .skip KERNEL_STACK_SIZE
    stack_top:
# We reserve one page (4KB) to make sure there's always somewhere to create a
# new page table.
.align 4096
    page_table_temp: .skip 4096

.section .text
# This is where GRUB will transfer control to. Just to confuse everyone, even
# though this symbol will have a value around 0xC0100000, but be loaded around
# 0x0010000, GRUB will deal with this and find the correct place. The multiboot
# loader for QEMU does not do the mapping (which is what I would have expected).
.global loader
loader:
    # Disable interrupts
    cli

    # Load the PDT. We need to give the real rather than virtual address
    lea (BootPdt - KERNEL_VIRTUAL_BASE), %eax
    mov %eax, %cr3

    # Enable PSE (4MB pages)
    mov %cr4, %eax
    or $0x00000010, %eax
    mov %eax, %cr4

    # Enable paging
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0

    # Jump to a virtual address to continue. Note that real addresses are still
    # valid here because of the first page in BootPdt, otherwise we'd be in
    # trouble, with %eip pointing around virtual 0x00100000 and nothing being
    # there.
    lea StartInHigherHalf, %eax
    jmp *%eax
    
# This is where we get to after setting up the PDT. Now we are using virtual
# addresses
StartInHigherHalf:
    # Invalidate the first two pages
    movl $0, BootPdt
    movl $0, BootPdt + 4
    invlpg 0
    invlpg 4194304

    # Set up the stack for C
    mov $stack_top, %esp

    # Pass the location of the multiboot header. Note this a real, not virtual
    # address. Since it's not guaranteed to be in the first 4MB, we'll just
    # have to let the kernel sort it out later.
    push %ebx

    # Pass the location we reserved for temporary page tables.
    lea page_table_temp, %eax
    push %eax

    # Pass the location of the current PDT.
    lea BootPdt, %eax
    push %eax

    # Provide the linker defined starts and ends.
    push $kernel_physical_end
    push $kernel_physical_start
    push $kernel_virtual_end
    push $kernel_virtual_start

    # Testing
#    mov $0xCAFEBABE, %ecx
#    .test: jmp .test

    # Transfer to C++
    call kernel_main

    # If we've managed to get here, we've somehow broken out of the kernel main
    # and passed a kernel panic. That really shouldn't happen. We'll destroy
    # local static objects for neatness, then loop.
    pushl $0x0
    call __cxa_finalize
    add $4, %esp

    .loop:
    jmp .loop
