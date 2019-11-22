.section .text
# Fires an interrupt to yield remaining time. It's a syscall, with index 0x9e.
.global asm_yield
asm_yield:
    mov $0x9e, %eax
    int $0x80
    ret
