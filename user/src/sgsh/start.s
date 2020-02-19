.section .data
hello: .string "Stable Genius Shell running on tty\n"
digits: .string "0123456789ABCDEF"
newline: .string "\n"
hex_pre: .string "  0x"
pid_string: .string "PID is:\n"
tty_string: .string "/dev/tty"
prompt: .string "belegost$ "
test_file: .string "/etc/foo.conf"
test_read_string: .string "Characters read from /etc/foo.conf: "
test_write_string: .string "12345"
test_error_read: .string "Unable to read from /etc/foo.conf\n"
test_error_write: .string "Unable to write to /etc/foo.conf\n"
test_error_open: .string "Unable to open /etc/foo.conf\n"
create_file: .string "/etc/test_dir/foobar.conf"
create_no_error_open: .string "Able to open non-existant file for reading\n"
create_error_open: .string "Not able to open non-existant file for writing\n"
create_error_open_r: .string "Unable to open /etc/test_dir/foobar.conf for reading\n"
create_error_read: .string "Unable to read from /etc/test_dir/foobar.conf\n"
create_error_write: .string "Unable to write to /etc/test_dir/foobar.conf\n"
create_read_string: .string "Characters read from /etc/test_dir/foobar.conf: "
create_write_string: .string "defgh"
delete_error: .string "Unable to delete file /etc/test_dir/foobar.conf\n"
create_directory: .string "/etc/test_dir"
create_directory_error: .string "Unable to create directory /etc/test_dir/\n"
rmdir_succeeded_error: .string "Able to delete non-empty directory /etc/test_dir/\n"
rmdir_error: .string "Unable to delete directory /etc/test_dir/\n"

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
    mov $35, %edx
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

# Create a new directory, /etc/test_dir
    mov $0x27, %eax
    mov $create_directory, %ebx
    mov $0, %ecx
    int $0x80

# If there was an error on mkdir, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $create_directory_error, %ecx
    mov $42, %edx
    int $0x80
    jmp 0f
1:

# Open a non-existent file /etc/test_dir/foobar.conf for writing.
    mov $5, %eax
    mov $create_file, %ebx
    mov $2, %ecx
    mov $0, %edx
    int $0x80

# If there was an error on open, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $create_error_open, %ecx
    mov $47, %edx
    int $0x80
    jmp 0f
1:
    push %eax

# Write 5 characters to /etc/test_dir/foobar.conf
    mov $4, %eax
    mov (%esp), %ebx
    mov $create_write_string, %ecx
    mov $5, %edx
    int $0x80

# If there was an error on write, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $create_error_write, %ecx
    mov $45, %edx
    int $0x80
    jmp 0f
1:

# Close /etc/test_dir/foobar.conf
    mov $6, %eax
    pop %ebx
    int $0x80

# Open /etc/test_dir/foobar.conf for reading. We should get back what we just
# wrote.
    mov $5, %eax
    mov $create_file, %ebx
    mov $1, %ecx
    mov $0, %edx
    int $0x80

# If there was an error on open, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $create_error_open_r, %ecx
    mov $53, %edx
    int $0x80
    jmp 0f
1:
    push %eax

# Read the first 10 characters of /etc/test_dir/foobar.conf
    mov $3, %eax
    mov (%esp), %ebx
    mov $read_buffer, %ecx
    mov $10, %edx
    int $0x80

# If there was an error on read, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $create_error_read, %ecx
    mov $46, %edx
    int $0x80
    jmp 0f
1:
    push %eax

# Print the characters to stdout.
    mov $4, %eax
    mov $1, %ebx
    mov $create_read_string, %ecx
    mov $48, %edx
    int $0x80
    mov $4, %eax
    mov $1, %ebx
    mov $read_buffer, %ecx
    pop %edx
    int $0x80
    mov $4, %eax
    mov $1, %ebx
    mov $newline, %ecx
    mov $1, %edx
    int $0x80

# Close /etc/test_dir/foobar.conf
    mov $6, %eax
    pop %ebx
    int $0x80

# Delete the directory /etc/test_dir, which should fail as the the directory is
# not empty.
    mov $0x28, %eax
    mov $create_directory, %ebx
    int $0x80

# If there was no error on rmdir, print and jump to the prompt.
    cmp $-1, %eax
    je 1f
    mov $4, %eax
    mov $1, %ebx
    mov $rmdir_succeeded_error, %ecx
    mov $50, %edx
    int $0x80
    jmp 0f
1:

# Delete the file we just created, /etc/test_dir/foobar.conf
    mov $10, %eax
    mov $create_file, %ebx
    int $0x80

# If there was an error on delete, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $delete_error, %ecx
    mov $49, %edx
    int $0x80
    jmp 0f
1:

# Open a non-existent file /etc/test_dir/foobar.conf for reading. Should fail.
    mov $5, %eax
    mov $create_file, %ebx
    mov $1, %ecx
    mov $0, %edx
    int $0x80

# If there was no error on open, print and jump to the prompt.
    cmp $-1, %eax
    je 1f
    mov $4, %eax
    mov $1, %ebx
    mov $create_no_error_open, %ecx
    mov $43, %edx
    int $0x80
    jmp 0f
1:

# Delete the directory /etc/test_dir, which should now succeed.
    mov $0x28, %eax
    mov $create_directory, %ebx
    int $0x80

# If there was an error on rmdir, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $rmdir_error, %ecx
    mov $42, %edx
    int $0x80
    jmp 0f
1:

# Open the file /etc/foo.conf for writing, which should truncate the file.
    mov $5, %eax
    mov $test_file, %ebx
    mov $2, %ecx
    mov $0, %edx
    int $0x80

# If there was an error on open, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $test_error_open, %ecx
    mov $29, %edx
    int $0x80
    jmp 0f
1:
    push %eax

# Write 5 characters to /etc/foo.conf
    mov $4, %eax
    mov (%esp), %ebx
    mov $test_write_string, %ecx
    mov $5, %edx
    int $0x80

# If there was an error on write, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $test_error_write, %ecx
    mov $33, %edx
    int $0x80
    jmp 0f
1:

# Close /etc/foo.conf
    mov $6, %eax
    pop %ebx
    int $0x80

# Open /etc/foo.conf for reading. We should get back what we just wrote.
    mov $5, %eax
    mov $test_file, %ebx
    mov $1, %ecx
    mov $0, %edx
    int $0x80

# If there was an error on open, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $test_error_open, %ecx
    mov $29, %edx
    int $0x80
    jmp 0f
1:
    push %eax

# Read the first 10 characters of /etc/foo.conf
    mov $3, %eax
    mov (%esp), %ebx
    mov $read_buffer, %ecx
    mov $10, %edx
    int $0x80

# If there was an error on read, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $test_error_read, %ecx
    mov $34, %edx
    int $0x80
    jmp 0f
1:
    push %eax

# Print the characters to stdout.
    mov $4, %eax
    mov $1, %ebx
    mov $test_read_string, %ecx
    mov $36, %edx
    int $0x80
    mov $4, %eax
    mov $1, %ebx
    mov $read_buffer, %ecx
    pop %edx
    int $0x80
    mov $4, %eax
    mov $1, %ebx
    mov $newline, %ecx
    mov $1, %edx
    int $0x80

# Close /etc/foo.conf
    mov $6, %eax
    pop %ebx
    int $0x80

# Now open /etc/foo.conf for writing again, which should truncate the file.
    mov $5, %eax
    mov $test_file, %ebx
    mov $2, %ecx
    mov $0, %edx
    int $0x80

# If there was an error on open, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $test_error_open, %ecx
    mov $29, %edx
    int $0x80
    jmp 0f
1:
    push %eax

# Close /etc/foo.conf
    mov $6, %eax
    pop %ebx
    int $0x80

# Now open /etc/foo.conf and try to read again. This time it should be empty.
    mov $5, %eax
    mov $test_file, %ebx
    mov $1, %ecx
    mov $0, %edx
    int $0x80

# If there was an error on open, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $test_error_open, %ecx
    mov $29, %edx
    int $0x80
    jmp 0f
1:
    push %eax

# Read the first 10 characters of /etc/foo.conf
    mov $3, %eax
    mov (%esp), %ebx
    mov $read_buffer, %ecx
    mov $10, %edx
    int $0x80

# If there was an error on read, print and jump to the prompt.
    cmp $-1, %eax
    jne 1f
    mov $4, %eax
    mov $1, %ebx
    mov $test_error_read, %ecx
    mov $34, %edx
    int $0x80
    jmp 0f
1:
    push %eax

# Print the characters to stdout.
    mov $4, %eax
    mov $1, %ebx
    mov $test_read_string, %ecx
    mov $36, %edx
    int $0x80
    mov $4, %eax
    mov $1, %ebx
    mov $read_buffer, %ecx
    pop %edx
    int $0x80
    mov $4, %eax
    mov $1, %ebx
    mov $newline, %ecx
    mov $1, %edx
    int $0x80

# Close /etc/foo.conf
    mov $6, %eax
    pop %ebx
    int $0x80

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
