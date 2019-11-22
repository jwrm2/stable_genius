/* the name of the entry label */
ENTRY(loader)

/* Manually specify the segments. */
PHDRS
{
  text PT_LOAD FILEHDR PHDRS;
  data PT_LOAD ;
  frame_header PT_GNU_EH_FRAME ;
}

SECTIONS {
    /* The kernel will be at the virtual address 3GB + 1MB, which will be mapped
       to 1MB in real space. */
    . = 0xC0100000;

    /* these labels get exported to the code files */
    kernel_virtual_start = .;
    kernel_physical_start = . - 0xC0000000;

    /* Leave space for the headers. Only has an effect if the headers are more
       than 4 KB, since the .text section is 4KB aligned. */
    . += SIZEOF_HEADERS;

    /* Align at 16 B, load the code physically around 1MB */
    .text ALIGN (0x0010) : AT(ADDR(.text) - 0xC0000000)
    {
        /* The special multiboot header, which must be early in the image */
        *(.multiboot)
        /* all text sections from all files, including random extra ones with
           C++ name mangling (.text._ZN6Serial3keyE7KeyCodeRK8KeyState) */
        *(.text*)
    } :text

    /* Align at 32 B, load the code physically around 1MB */
    .rodata ALIGN (0x0020) : AT(ADDR(.rodata) - 0xC0000000)
    {
        /* all read-only data sections from all files */
        *(.rodata*)
    } :text

    /* Align at 4 KB, load the code physically around 1MB */
    .data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000)
    {
        /* all data sections from all files, including random extra ones with
           C++ name mangling
           (.data.rel.ro._ZTIN10__cxxabiv117__class_type_info) */
        *(.data*)
    } :data

    /* Align at 4 KB, load the code physically around 1MB */
    .bss ALIGN (0x1000) : AT(ADDR(.bss) - 0xC0000000)
    {
        /* all bss sections from all files */
        *(.bss)
    } :data

    /* The linker controls the alignment and position of this section. */
    .eh_frame_hdr :
    {
        /* all eh_frame_hdr sections from all files */
        *(.eh_frame_hdr)
    } :text :frame_header

    /** We need the address of the start of the eh_frame. */
    start_eh_frame = .;

    /* The linker controls the alignment and position of this section. */
    .eh_frame :
    {
        /* all eh_frame sections from all files */
        *(.eh_frame)
        /* Needs to be terminated by 4 bytes of zeroes. */
        QUAD(0)
    } :text

    /* The linker controls the alignment and position of this section. */
    .gcc_except_table :
    {
        /* all gcc_except_table sections from all files */
        *(.gcc_except_table*)
    } :text

    /* Align at 1 B, load the code physically around 1MB */
    .comment ALIGN (0x0001) : AT(ADDR(.comment) - 0xC0000000)
    {
        /* all comment sections from all files */
        *(.comment)
    } :text

    /* Align at 1 B, load the code physically around 1MB */
    .debug_info ALIGN (0x0001) : AT(ADDR(.debug_info) - 0xC0000000)
    {
        /* all debug_info sections from all files */
        *(.debug_info)
    } :text

    /* Align at 1 B, load the code physically around 1MB */
    .debug_abbrev ALIGN (0x0001) : AT(ADDR(.debug_abbrev) - 0xC0000000)
    {
        /* all debug_abbrev sections from all files */
        *(.debug_abbrev)
    } :text

    /* Align at 1 B, load the code physically around 1MB */
    .debug_loc ALIGN (0x0001) : AT(ADDR(.debug_loc) - 0xC0000000)
    {
        /* all debug_loc sections from all files */
        *(.debug_loc)
    } :text

    /* Align at 1 B, load the code physically around 1MB */
    .debug_aranges ALIGN (0x0001) : AT(ADDR(.debug_aranges) - 0xC0000000)
    {
        /* all debug_aranges sections from all files */
        *(.debug_aranges)
    } :text

    /* Align at 1 B, load the code physically around 1MB */
    .debug_ranges ALIGN (0x0001) : AT(ADDR(.debug_ranges) - 0xC0000000)
    {
        /* all debug_ranges sections from all files */
        *(.debug_ranges)
    } :text

    /* Align at 1 B, load the code physically around 1MB */
    .debug_line ALIGN (0x0001) : AT(ADDR(.debug_line) - 0xC0000000)
    {
        /* all debug_line sections from all files */
        *(.debug_line)
    } :text

    /* Align at 1 B, load the code physically around 1MB */
    .debug_str ALIGN (0x0001) : AT(ADDR(.debug_str) - 0xC0000000)
    {
        /* all debug_str sections from all files */
        *(.debug_str)
    } :text

    /* these labels get exported to the code files */
    kernel_virtual_end = .;
    kernel_physical_end = . - 0xC0000000;
}
