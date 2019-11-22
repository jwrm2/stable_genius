.section .data
hello: .string "Hello from exec'd process\n"
digits: .string "0123456789ABCDEF"
newline: .string "\n"
hex_pre: .string "  0x"
pid_string: .string "PID is:\n"
tty_string: .string "/dev/tty"
prompt: .string "belegost$ "

.section .bss
read_buffer: .skip 1024

.section .text
# Entry point
.global _start
_start:

# We should have inherited the serial port as fd 0. Close it.
    mov $6, %eax
    mov $0, %ebx
    int $0x80

# Open the TTY as an input device. 1 in %ecx indicates read only.
    mov $5, %eax
    mov $tty_string, %ebx
    mov $1, %ecx
    mov $0, %edx
    int $0x80

# Open the TTY as an output device. 2 in %ecx indicates write only.
    mov $5, %eax
    mov $tty_string, %ebx
    mov $2, %ecx
    mov $0, %edx
    int $0x80

# Try to fire off a system call interrupt to print out a string.
    mov $4, %eax
    mov $1, %ebx
    mov $hello, %ecx
    mov $26, %edx
    int $0x80

# Get the current PID.
    mov $0x14, %eax
    int $0x80

# Print it
    push %eax
    mov $4, %eax
    mov $1, %ebx
    mov $pid_string, %ecx
    mov $8, %edx
    int $0x80

    call print_register
    pop %eax

# Infinite loop. Wait for reading in data, then echo it.
0:
    # Print the prompt, stdout is fd 1
    mov $4, %eax
    mov $1, %ebx
    mov $prompt, %ecx
    mov $10, %edx
    int $0x80

    # Read input, stdin is fd 0
    mov $3, %eax
    mov $0, %ebx
    mov $read_buffer, %ecx
    mov $1024, %edx
    int $0x80

    # Echo the input. Number of characters read is in %eax
    mov %eax, %edx
    mov $4, %eax
    mov $1, %ebx
    mov $read_buffer, %ecx
    int $0x80

    jmp 0b

# Value to print at %esp + 4
print_register:
    push %ebp
    mov %esp, %ebp

    mov 8(%esp), %eax

    pushl $0
    mov newline, %edx
    mov %dl, (%esp)

    xor %edx, %edx
    xor %ecx, %ecx
0:
    push %eax
    shr %cl, %eax
    and $0xF, %eax
    mov digits(%eax), %al
    push %ecx
    shl $1, %ecx
    neg %ecx
    add $24, %ecx
    shl %cl, %eax
    pop %ecx
    or %eax, %edx
    pop %eax
    add $4, %ecx
    cmp $16, %ecx
    jne 0b

    push %edx

    xor %ecx, %ecx
    xor %edx, %edx
0:
    push %eax
    add $16, %ecx
    shr %cl, %eax
    sub $16, %ecx
    and $0xF, %eax
    mov digits(%eax), %al
    push %ecx
    shl $1, %ecx
    neg %ecx
    add $24, %ecx
    shl %cl, %eax
    pop %ecx
    or %eax, %edx
    pop %eax
    add $4, %ecx
    cmp $16, %ecx
    jne 0b

    push %edx

    push hex_pre

    mov $4, %eax
    mov $1, %ebx
    mov %esp, %ecx
    mov $13, %edx
    int $0x80

    mov %ebp, %esp
    pop %ebp
    ret
