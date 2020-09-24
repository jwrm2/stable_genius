.section .init
.global _init
_init:
    push %ebp
    mov %esp, %ebp
    /** Newer versions of GCC (8.* onwards?) generate an empty .ctors section
      * and instead puts a list of function pointers to call the global
      * constructors in the .init_array section. Fortunately, the default linker
      * script generates symbols for the start and end of that list, so we can
      * cycle through and call the functions here. What a pain.
      */
    mov $__init_array_start, %ebx
0:  mov $__init_array_end, %eax
    cmp %ebx, %eax
    je 1f
    mov (%ebx), %eax
    call *%eax
    add $0x4, %ebx
    jmp 0b
1:
    /** GCC puts its crtbegin init section here. */

.section .fini
.global _fini
_fini:
    push %ebp
    mov %esp, %ebp
    /** Similarly to .init_array, we need to go through the .fini_array section.
      */
    mov $__fini_array_start, %ebx
0:  mov $__fini_array_end, %eax
    cmp %ebx, %eax
    je 1f
    mov (%ebx), %eax
    call *%eax
    add $0x4, %ebx
    jmp 0b
1:
    /** GCC puts its crtbegin fini section here. */

