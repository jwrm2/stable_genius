# Loads a Global Descriptor Table
# Address of table at %esp + 4
# Size of table at %esp + 8
.global load_gdt
load_gdt:
    push %ebp
    mov %esp, %ebp
    sub $8, %esp

    mov 8(%ebp), %eax
    mov %eax, -6(%ebp)
    mov 12(%ebp), %ax
    mov %ax, -8(%ebp)
    lgdt -8(%ebp)

    mov %ebp, %esp
    pop %ebp
    ret

# Resets all the segment registers
# Value for code segment register at %esp + 4
# Value for data segment registers at %esp + 8
.global reset_segments
reset_segments:
    push %ebp
    mov %esp, %ebp
    sub $8, %esp

    # Data segment registers
    mov 12(%ebp), %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # Code segment register
    mov 8(%ebp), %ax
    mov %ax, -4(%ebp)
    movl $reload_CS, -8(%ebp)
    ljmp *-8(%ebp)
reload_CS:
    mov %ebp, %esp
    pop %ebp
    ret

# Loads the TSS register.
# Offset of the TSS segement in the GDT at %esp + 4
.global load_tss
load_tss:
    # Move segment selector into ax. This should already have the correct index
    # and privilege level.
    mov 4(%esp), %ax

    # Load the value into the task register.
    ltr %ax

    ret
