.section .init
.global _init
_init:
    push %ebp
    mov %esp, %ebp
    /** GCC puts its crtbegin init section here. */

.section .fini
.global _fini
_fini:
    push %ebp
    mov %esp, %ebp
    /** GCC puts its crtbegin fini section here. */

