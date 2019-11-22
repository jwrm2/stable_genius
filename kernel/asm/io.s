# Sends a byte of data to an I/O port
# Data to send at %esp + 4
# Address to send data to at %esp + 8
.global outb
outb:
    mov 8(%esp), %dx # Move the address (2nd parameter) into the register
    mov 4(%esp), %al # Move the data (1st parameter) into the register
    outb %al, %dx    # Send the data    
    ret

# Gets a byte data from an I/O port
# Address to fetch from at %esp + 4
.global inb
inb:
    mov 4(%esp), %dx # Move the address (1st parameter) into the register
    inb %dx, %al     # Put the data into eax, so it is returned
    ret

# Sends 2 bytes of data to an I/O port
# Data to send at %esp + 4
# Address to send data to at %esp + 8
.global outw
outw:
    mov 8(%esp), %dx # Move the address (2nd parameter) into the register
    mov 4(%esp), %ax # Move the data (1st parameter) into the register
    outw %ax, %dx    # Send the data    
    ret

# Gets 2 bytes of data from an I/O port
# Address to fetch from at %esp + 4
.global inw
inw:
    mov 4(%esp), %dx # Move the address (1st parameter) into the register
    inw %dx, %ax     # Put the data into eax, so it is returned
    ret

# Sends 4 bytes of data to an I/O port
# Data to send at %esp + 4
# Address to send data to at %esp + 8
.global outl
outl:
    mov 8(%esp), %dx # Move the address (2nd parameter) into the register
    mov 4(%esp), %eax # Move the data (1st parameter) into the register
    outl %eax, %dx    # Send the data    
    ret

# Gets 4 bytes of data from an I/O port
# Address to fetch from at %esp + 4
.global inl
inl:
    mov 4(%esp), %dx # Move the address (1st parameter) into the register
    inl %dx, %eax     # Put the data into eax, so it is returned
    ret

# Sends a sequence of 2 byte words to an I/O port.
# Port to write to at %esp + 4
# Address fetch from to at %esp + 8
# Number of words to read at %esp + 12
.global outsw
outsw:
    push %ebp
    mov %esp, %ebp

    # In cdecl, %esi is callee saved.
    push %esi

    # Set the registers for the call. Strictly data is fetched from %ds:%esi,
    # but we'll just assume %ds is set correctly.
    mov 8(%ebp), %dx # Move the port address (1st parameter) into the register.
    mov 12(%ebp), %esi # Move the source address into the register.
    mov 16(%ebp), %ecx # Move the counter into the register.

    # We need to make sure the direction flag (DF) is clear, but we want to
    # restore it afterwards.
    pushfl
    cld

    # Do the write
    rep outsw

    # Restore flags and %edi
    popfl
    pop %esi

    # Clean stack
    mov %ebp, %esp
    pop %ebp
    ret

# Gets a sequence of 2 byte words from an I/O port.
# Port to fetch from at %esp + 4
# Address to write to at %esp + 8
# Number of words to read at %esp + 12
.global insw
insw:
    push %ebp
    mov %esp, %ebp

    # In cdecl, %edi is callee saved.
    push %edi

    # Set the registers for the call. Strictly data is written to %es:%edi, but
    # we'll just assume %es is set correctly.
    mov 8(%ebp), %dx # Move the port address (1st parameter) into the register.
    mov 12(%ebp), %edi # Move the destination address into the register.
    mov 16(%ebp), %ecx # Move the counter into the register.

    # We need to make sure the direction flag (DF) is clear, but we want to
    # restore it afterwards.
    pushfl
    cld

    # Do the read
    rep insw

    # Restore flags and %edi
    popfl
    pop %edi

    # Clean stack
    mov %ebp, %esp
    pop %ebp
    ret

# Gets a sequence of double words from an I/O port.
# Address to fetch from at %esp + 4
# Address to write to at %esp + 8
# Number of double words to read at %esp + 12
.global insl
insl:
    push %ebp
    mov %esp, %ebp

    # In cdecl, %edi is callee saved.
    push %edi

    # Set the registers for the call. Strictly data is written to %es:%edi, but
    # we'll just assume %es is set correctly.
    mov 8(%ebp), %dx # Move the port address (1st parameter) into the register.
    mov 12(%ebp), %edi # Move the destination address into the register.
    mov 16(%ebp), %ecx # Move the counter into the register.

    # We need to make sure the direction flag (DF) is clear, but we want to
    # restore it afterwards.
    pushfl
    cld

    # Do the read
    rep insl

    # Restore flags and %edi
    popfl
    pop %edi

    # Clean stack
    mov %ebp, %esp
    pop %ebp
    ret

# Waits for io to complete by sending to the hopefully unused port 0x80
.global io_wait
io_wait:
    mov $0xAB, %al
    mov $0x80, %dx
    outb %al, %dx
    ret
