.section .text

# Here we provide the assembly definitions of the system calls. This must be
# done in assembly to use the int instruction. Each function must put parameters
# from the stack into appropriate registers. Callee-saved registers are
# preserved by the system call, so we only need to save registers if we're
# putting values into them. %eax, %ecx and %edx are caller-saved, so we never
# need to worry about saving them. After the interrupt, it must restore any
# saved registers and clear the stack. The interrupt will put the return value
# in %eax, so no adjustment is required for that.

# Forks the process.
# No parameters.
.global fork
fork:
    mov $0x2, %eax
    int $0x80
    ret

# Reads characters.
# File descriptor at %esp + 4 goes into %ebx
# Buffer location at %esp + 8 goes into %ecx
# Count at %esp + 12 goes into %edx
.global read
read:
    push %ebx
    mov $0x3, %eax
    mov 8(%esp), %ebx
    mov 12(%esp), %ecx
    mov 16(%esp), %edx
    int $0x80
    pop %ebx
    ret

# Writes characters.
# File descriptor at %esp + 4 goes into %ebx
# Buffer location at %esp + 8 goes into %ecx
# Count at %esp + 12 goes into %edx
.global write
write:
    push %ebx
    mov $0x4, %eax
    mov 8(%esp), %ebx
    mov 12(%esp), %ecx
    mov 16(%esp), %edx
    int $0x80
    pop %ebx
    ret

# Opens a file.
# File name at %esp + 4 goes into %ebx
# Flags at %esp + 8 goes into %ecx
# Mode at %esp + 12 goes into %edx
.global open
open:
    push %ebx
    mov $0x5, %eax
    mov 8(%esp), %ebx
    mov 12(%esp), %ecx
    mov 16(%esp), %edx
    int $0x80
    pop %ebx
    ret

# Closes a file.
# File descriptor at %esp + 4 goes into %ebx
.global close
close:
    push %ebx
    mov $0x6, %eax
    mov 8(%esp), %ebx
    int $0x80
    pop %ebx
    ret

# Waits for a process.
# PID to wait for at %esp + 4 goes into %ebx
# wstatus at %esp + 8 goes into %ecx
# options at %esp + 12 goes into %edx
.global wait
wait:
    push %ebx
    mov $0x7, %eax
    mov 8(%esp), %ebx
    mov 12(%esp), %ecx
    mov 16(%esp), %edx
    int $0x80
    pop %ebx
    ret

# Unlinks (deletes) a file.
# File name at %esp + 4 goes into %ebx
.global unlink
unlink:
    push %ebx
    mov $0xa, %eax
    mov 8(%esp), %ebx
    int $0x80
    pop %ebx
    ret

# Execs a process.
# File name at %esp + 4 goes into %ebx
# Command line arguments at %esp + 8 go into %ecx
# Environment variables at %esp + 12 go into %edx
.global execve
execve:
    push %ebx
    mov $0xb, %eax
    mov 8(%esp), %ebx
    mov 12(%esp), %ecx
    mov 16(%esp), %edx
    int $0x80
    pop %ebx
    ret

# Gets the current PID.
# No parameters.
.global getpid
getpid:
    mov $0x14, %eax
    int $0x80
    ret

# Creates a directory.
# File name at %esp + 4 goes into %ebx
# mode at %esp + 8 goes into %ecx
.global mkdir
mkdir:
    push %ebx
    mov $0x27, %eax
    mov 8(%esp), %ebx
    mov 12(%esp), %ecx
    int $0x80
    pop %ebx
    ret

# Removes a directory.
# File name at %esp + 4 goes into %ebx
.global rmdir
rmdir:
    push %ebx
    mov $0x28, %eax
    mov 8(%esp), %ebx
    int $0x80
    pop %ebx
    ret

# Changes the break point.
# New address at %esp + 4 goes into %ebx
.global brk
brk:
    push %ebx
    mov $0x2d, %eax
    mov 8(%esp), %ebx
    int $0x80
    pop %ebx
    ret

# Changes the offset of an open file.
# File descriptor at %esp + 4 goes into %ebx
# High 32 bits of the new offset at %esp + 8 goes into %ecx
# Low 32 bits of the new offset at %esp + 12 goes into %edx
# Pointer to the result at %esp + 16 goes into %esi
# Origin of the offset at %esp + 20 goes into %edi
.global llseek
llseek:
    push %ebx
    push %esi
    push %edi
    mov $0x8c, %eax
    mov 16(%esp), %ebx
    mov 20(%esp), %ecx
    mov 24(%esp), %edx
    mov 28(%esp), %esi
    mov 32(%esp), %edi
    int $0x80
    pop %edi
    pop %esi
    pop %ebx
    ret

# Returns control to the scheduler for the next process.
# No parameters.
.global yield
yield:
    mov $0x9e, %eax
    int $0x80
    ret
