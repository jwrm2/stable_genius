.extern deadbeef_data
.extern deadbabe_rodata

.section .data
hello: .string "hello from user mode\n"
expected_ret: .string "eax return as expected\n"
unexpected_ret: .string "eax return not as expected\n"
digits: .string "0123456789ABCDEF"
newline: .string "\n"
hex_pre: .string "  0x"
serial_port: .string "/dev/ttyS0"
pid_string: .string "PID is:\n"
other_proc: .string "/bin/sgsh\0"
child_str: .string "I am the child, my PID is:\n"
parent_str1: .string "I am the parent, the child's PID is:\n"
parent_str2: .string "I am the parent, my PID is:\n"
exec_str: .string "EXEC\n"
fork_str: .string "FORK\n"

.section .text
# Entry point, called from crt0.s
.global main
main:
    # Testing
    #mov $0xCAFEBABE, %ecx
    #.test: jmp .test
# Read from the .data
    mov deadbeef_data, %eax
# Read from the .rodata
    mov deadbabe_rodata, %ecx

# Call C function, including saving register
    push %eax
    push %ecx
    call cxx_main
# Put return value into edx
    mov %eax, %edx
# Restore eax and ecx
    pop %ecx
    pop %eax

# Try to open the serial port for writing. 2 in %ecx indicates write only.
    mov $5, %eax
    mov $serial_port, %ebx
    mov $2, %ecx
    mov $0, %edx
    int $0x80

# Try to fire off a system call interrupt to print out a string.
    mov $4, %eax
    mov $0, %ebx
    mov $hello, %ecx
    mov $21, %edx
    int $0x80

# We hope eax will have value 21 after the syscall returns.
    cmp $21, %eax
    jne 0f

# Fire a second system call interrupt, to print the response.
    mov $4, %eax
    mov $0, %ebx
    mov $expected_ret, %ecx
    mov $23, %edx
    int $0x80
    jmp 1f

0:
    mov $4, %eax
    mov $0, %ebx
    mov $unexpected_ret, %ecx
    mov $27, %edx
    int $0x80

1:
# Get the current PID.
    mov $0x14, %eax
    int $0x80

# Print it
    push %eax
    mov $4, %eax
    mov $0, %ebx
    mov $pid_string, %ecx
    mov $8, %edx
    int $0x80

    call print_register
    pop %eax

# Try a fork.
    mov $4, %eax
    mov $0, %ebx
    mov $fork_str, %ecx
    mov $5, %edx
    int $0x80
    mov $2, %eax
    int $0x80

after_fork:
# Test whether we are the child or parent (%eax is zero for the child)
    test %eax, %eax
    jnz parent_after_fork

# Get the child PID.
    mov $0x14, %eax
    int $0x80

# Print out the child PID
    push %eax
    mov $4, %eax
    mov $0, %ebx
    mov $child_str, %ecx
    mov $27, %edx
    int $0x80
    call print_register
    pop %eax

# Try to exec the other process.
    mov $4, %eax
    mov $0, %ebx
    mov $exec_str, %ecx
    mov $5, %edx
    int $0x80
    mov $0xB, %eax
    mov $other_proc, %ebx
    mov $0, %ecx
    mov $0, %edx
    int $0x80

# If we've got here, the exec failed.
    mov $0xBADBADAD, %eax
    mov $0xBADBADAD, %ecx
    mov $0xBADBADAD, %edx
    0: jmp 0b

parent_after_fork:
# Print out the child PID
    push %eax
    mov $4, %eax
    mov $0, %ebx
    mov $parent_str1, %ecx
    mov $37, %edx
    int $0x80
    call print_register
    pop %eax

# Get the parent PID.
    mov $0x14, %eax
    int $0x80

# Print it
    push %eax
    mov $4, %eax
    mov $0, %ebx
    mov $parent_str2, %ecx
    mov $28, %edx
    int $0x80

    call print_register
    pop %eax

# Parent is finished, infinite loop.
    mov $0xDEADBEEF, %edx
    0: jmp 0b

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
    mov $0, %ebx
    mov %esp, %ecx
    mov $13, %edx
    int $0x80

    mov %ebp, %esp
    pop %ebp
    ret
