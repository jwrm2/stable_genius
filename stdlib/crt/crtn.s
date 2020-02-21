.section .init
    /** GCC puts its crtend init section here. */
    pop %ebp
    ret

.section .fini
    /** GCC puts its crtend fini section here. */
    pop %ebp
    ret
