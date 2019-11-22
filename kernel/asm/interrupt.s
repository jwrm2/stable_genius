# External, contains the base address of the kernel.
.global kernel_virtual_base

# Loads an Interrupt Descriptor Table
# Address of table at %esp + 4
# Size of table at %esp + 8
.global load_idt
load_idt:
    push %ebp
    mov %esp, %ebp
    sub $8, %esp

    mov 8(%ebp), %eax
    mov %eax, -6(%ebp)
    mov 12(%ebp), %ax
    mov %ax, -8(%ebp)
    lidt -8(%ebp)

    mov %ebp, %esp
    pop %ebp
    ret

# Disables all interrupts
.global disable_interrupts
disable_interrupts:
    cli
    ret

# Enables all interrupts not otherwise disabled
.global enable_interrupts
enable_interrupts:
    sti
    ret

# Interrupt handler when there is no error code.
# Push an extra zero onto the stack to simulate an error code.
# Push the interrupt number onto the stack.
.macro no_error_code_interrupt_handler inum
    .global interrupt_handler_\inum            
    interrupt_handler_\inum:
        pushl $0x0
        pushl $\inum
        jmp common_interrupt_handler
.endm

# Interrupt handler when there is an error code.
# No need for an extra zero.
# Push the interrupt number onto the stack.
.macro with_error_code_interrupt_handler inum
    .global interrupt_handler_\inum            
    interrupt_handler_\inum:
        pushl $\inum
        jmp common_interrupt_handler
.endm

# Common interrupt handler
# ss                   at %esp + 24 if we interrupted from user mode
# esp                  at %esp + 20 if we interrupted from user mode
# eflags               at %esp + 16
# cs                   at %esp + 12
# eip                  at %esp + 8
# code (or zero)       at %esp + 4
# interrupt number     at %esp
common_interrupt_handler:
    # Save the current data registers. Puts 32 bytes on the stack.
    pushal

    # Determine whether we came from user mode. We have several options for
    # this, but the simplest seems to be to test whether the old %eip is in user
    # space or kernel space.
    mov kernel_virtual_base, %eax
    cmp %eax, 40(%esp)
    jb end_pre

    # We've come from kernel mode. %ss and %esp are not on the stack, but the
    # interrupt handler may need that information if it's doing a task switch.
    # We'll shift the data on the stack down and insert the values.
    # 2 values to insert.
    sub $8, %esp
    mov $0, %ecx
    shift_loop_pre:
        # Load the value at %esp + 4*%ecx + 8 into %eax.
        mov 8(%esp, %ecx, 4), %eax
        # Load the value of %eax into %esp + 4*$ecx.
        mov %eax, (%esp, %ecx, 4)
        # Increment %ecx
        inc %ecx
        # We have thirteen values to move.
        cmp $13, %ecx
        jne shift_loop_pre
    # Put the old %esp value onto the stack. We've moved %esp down 60 bytes
    # since the interrupt fired.
    mov %esp, %eax
    add $60, %eax
    mov %eax, 52(%esp)
    # Put the old %ss on the stack. Since we came from kernel mode, we can just
    # use the current %ss.
    mov $0, %eax
    pushw %ss
    popw %ax
    mov %eax, 56(%esp)

    end_pre:
    # If we've come from user mode, %ss and %cs will have been changed back to
    # kernel values by the TSS, but %ds etc. will not have. If we've come from
    # kernel mode, they'd normally be already in kernel mode, but not if, for
    # example, an interrupt was generated in the interrupt handler coming from
    # user mode before they got changed.
    pushw %ss
    popw %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    # Go to C++ for the main handling.
    call interrupt_handler

    # If this was a syscall (interrupt number 0x80), there is a return value in
    # eax.
    cmpl $0x80, 32(%esp)
    jne 0f
    mov %eax, 28(%esp)
    0:

    # If we've got here then a task switch has not occured and we'll be calling
    # iret. Determine whether we are returning to user mode.
    mov kernel_virtual_base, %eax
    cmp %eax, 40(%esp)
    jb user_space_post

    # We're returning to kernel space. We need to get rid of %esp and %ss from
    # the stack, as the iret won't pop them.
    # 13 values to move.
    mov $13, %ecx
    shift_loop_post:
        # Load the value at %esp + 4*(%ecx - 1) into %eax.
        mov -4(%esp, %ecx, 4), %eax
        # Load the value in %eax into %esp + 4*(%ecx - 1) + 8.
        mov %eax, 4(%esp, %ecx, 4)
        # Decrement %ecx
        dec %ecx
        jnz shift_loop_post
    # Clear the two unused spaces from the bottom of the stack.
    add $8, %esp

    jmp end_post

    user_space_post:
    # We're returning to user space. Restore previous data segment selectors.
    mov 56(%esp), %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    # Restore the data registers.
    end_post: popal

    # Wipe the interrupt number and error code.
    add $8, %esp

    # Jump back, using the saved eip, cs and eflags (and esp and ss).
    iret

# Generate the list of functions, letting the macro do the work.
no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7
with_error_code_interrupt_handler 8
no_error_code_interrupt_handler 9
with_error_code_interrupt_handler 10
with_error_code_interrupt_handler 11
with_error_code_interrupt_handler 12
with_error_code_interrupt_handler 13
with_error_code_interrupt_handler 14
no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
with_error_code_interrupt_handler 17
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19
no_error_code_interrupt_handler 20
no_error_code_interrupt_handler 21
no_error_code_interrupt_handler 22
no_error_code_interrupt_handler 23
no_error_code_interrupt_handler 24
no_error_code_interrupt_handler 25
no_error_code_interrupt_handler 26
no_error_code_interrupt_handler 27
no_error_code_interrupt_handler 28
no_error_code_interrupt_handler 29
with_error_code_interrupt_handler 30
no_error_code_interrupt_handler 31
no_error_code_interrupt_handler 32
no_error_code_interrupt_handler 33
no_error_code_interrupt_handler 34
no_error_code_interrupt_handler 35
no_error_code_interrupt_handler 36
no_error_code_interrupt_handler 37
no_error_code_interrupt_handler 38
no_error_code_interrupt_handler 39
no_error_code_interrupt_handler 40
no_error_code_interrupt_handler 41
no_error_code_interrupt_handler 42
no_error_code_interrupt_handler 43
no_error_code_interrupt_handler 44
no_error_code_interrupt_handler 45
no_error_code_interrupt_handler 46
no_error_code_interrupt_handler 47
no_error_code_interrupt_handler 48
no_error_code_interrupt_handler 49
no_error_code_interrupt_handler 50
no_error_code_interrupt_handler 51
no_error_code_interrupt_handler 52
no_error_code_interrupt_handler 53
no_error_code_interrupt_handler 54
no_error_code_interrupt_handler 55
no_error_code_interrupt_handler 56
no_error_code_interrupt_handler 57
no_error_code_interrupt_handler 58
no_error_code_interrupt_handler 59
no_error_code_interrupt_handler 60
no_error_code_interrupt_handler 61
no_error_code_interrupt_handler 62
no_error_code_interrupt_handler 63
no_error_code_interrupt_handler 64
no_error_code_interrupt_handler 65
no_error_code_interrupt_handler 66
no_error_code_interrupt_handler 67
no_error_code_interrupt_handler 68
no_error_code_interrupt_handler 69
no_error_code_interrupt_handler 70
no_error_code_interrupt_handler 71
no_error_code_interrupt_handler 72
no_error_code_interrupt_handler 73
no_error_code_interrupt_handler 74
no_error_code_interrupt_handler 75
no_error_code_interrupt_handler 76
no_error_code_interrupt_handler 77
no_error_code_interrupt_handler 78
no_error_code_interrupt_handler 79
no_error_code_interrupt_handler 80
no_error_code_interrupt_handler 81
no_error_code_interrupt_handler 82
no_error_code_interrupt_handler 83
no_error_code_interrupt_handler 84
no_error_code_interrupt_handler 85
no_error_code_interrupt_handler 86
no_error_code_interrupt_handler 87
no_error_code_interrupt_handler 88
no_error_code_interrupt_handler 89
no_error_code_interrupt_handler 90
no_error_code_interrupt_handler 91
no_error_code_interrupt_handler 92
no_error_code_interrupt_handler 93
no_error_code_interrupt_handler 94
no_error_code_interrupt_handler 95
no_error_code_interrupt_handler 96
no_error_code_interrupt_handler 97
no_error_code_interrupt_handler 98
no_error_code_interrupt_handler 99
no_error_code_interrupt_handler 100
no_error_code_interrupt_handler 101
no_error_code_interrupt_handler 102
no_error_code_interrupt_handler 103
no_error_code_interrupt_handler 104
no_error_code_interrupt_handler 105
no_error_code_interrupt_handler 106
no_error_code_interrupt_handler 107
no_error_code_interrupt_handler 108
no_error_code_interrupt_handler 109
no_error_code_interrupt_handler 110
no_error_code_interrupt_handler 111
no_error_code_interrupt_handler 112
no_error_code_interrupt_handler 113
no_error_code_interrupt_handler 114
no_error_code_interrupt_handler 115
no_error_code_interrupt_handler 116
no_error_code_interrupt_handler 117
no_error_code_interrupt_handler 118
no_error_code_interrupt_handler 119
no_error_code_interrupt_handler 120
no_error_code_interrupt_handler 121
no_error_code_interrupt_handler 122
no_error_code_interrupt_handler 123
no_error_code_interrupt_handler 124
no_error_code_interrupt_handler 125
no_error_code_interrupt_handler 126
no_error_code_interrupt_handler 127
no_error_code_interrupt_handler 128
no_error_code_interrupt_handler 129
no_error_code_interrupt_handler 130
no_error_code_interrupt_handler 131
no_error_code_interrupt_handler 132
no_error_code_interrupt_handler 133
no_error_code_interrupt_handler 134
no_error_code_interrupt_handler 135
no_error_code_interrupt_handler 136
no_error_code_interrupt_handler 137
no_error_code_interrupt_handler 138
no_error_code_interrupt_handler 139
no_error_code_interrupt_handler 140
no_error_code_interrupt_handler 141
no_error_code_interrupt_handler 142
no_error_code_interrupt_handler 143
no_error_code_interrupt_handler 144
no_error_code_interrupt_handler 145
no_error_code_interrupt_handler 146
no_error_code_interrupt_handler 147
no_error_code_interrupt_handler 148
no_error_code_interrupt_handler 149
no_error_code_interrupt_handler 150
no_error_code_interrupt_handler 151
no_error_code_interrupt_handler 152
no_error_code_interrupt_handler 153
no_error_code_interrupt_handler 154
no_error_code_interrupt_handler 155
no_error_code_interrupt_handler 156
no_error_code_interrupt_handler 157
no_error_code_interrupt_handler 158
no_error_code_interrupt_handler 159
no_error_code_interrupt_handler 160
no_error_code_interrupt_handler 161
no_error_code_interrupt_handler 162
no_error_code_interrupt_handler 163
no_error_code_interrupt_handler 164
no_error_code_interrupt_handler 165
no_error_code_interrupt_handler 166
no_error_code_interrupt_handler 167
no_error_code_interrupt_handler 168
no_error_code_interrupt_handler 169
no_error_code_interrupt_handler 170
no_error_code_interrupt_handler 171
no_error_code_interrupt_handler 172
no_error_code_interrupt_handler 173
no_error_code_interrupt_handler 174
no_error_code_interrupt_handler 175
no_error_code_interrupt_handler 176
no_error_code_interrupt_handler 177
no_error_code_interrupt_handler 178
no_error_code_interrupt_handler 179
no_error_code_interrupt_handler 180
no_error_code_interrupt_handler 181
no_error_code_interrupt_handler 182
no_error_code_interrupt_handler 183
no_error_code_interrupt_handler 184
no_error_code_interrupt_handler 185
no_error_code_interrupt_handler 186
no_error_code_interrupt_handler 187
no_error_code_interrupt_handler 188
no_error_code_interrupt_handler 189
no_error_code_interrupt_handler 190
no_error_code_interrupt_handler 191
no_error_code_interrupt_handler 192
no_error_code_interrupt_handler 193
no_error_code_interrupt_handler 194
no_error_code_interrupt_handler 195
no_error_code_interrupt_handler 196
no_error_code_interrupt_handler 197
no_error_code_interrupt_handler 198
no_error_code_interrupt_handler 199
no_error_code_interrupt_handler 200
no_error_code_interrupt_handler 201
no_error_code_interrupt_handler 202
no_error_code_interrupt_handler 203
no_error_code_interrupt_handler 204
no_error_code_interrupt_handler 205
no_error_code_interrupt_handler 206
no_error_code_interrupt_handler 207
no_error_code_interrupt_handler 208
no_error_code_interrupt_handler 209
no_error_code_interrupt_handler 210
no_error_code_interrupt_handler 211
no_error_code_interrupt_handler 212
no_error_code_interrupt_handler 213
no_error_code_interrupt_handler 214
no_error_code_interrupt_handler 215
no_error_code_interrupt_handler 216
no_error_code_interrupt_handler 217
no_error_code_interrupt_handler 218
no_error_code_interrupt_handler 219
no_error_code_interrupt_handler 220
no_error_code_interrupt_handler 221
no_error_code_interrupt_handler 222
no_error_code_interrupt_handler 223
no_error_code_interrupt_handler 224
no_error_code_interrupt_handler 225
no_error_code_interrupt_handler 226
no_error_code_interrupt_handler 227
no_error_code_interrupt_handler 228
no_error_code_interrupt_handler 229
no_error_code_interrupt_handler 230
no_error_code_interrupt_handler 231
no_error_code_interrupt_handler 232
no_error_code_interrupt_handler 233
no_error_code_interrupt_handler 234
no_error_code_interrupt_handler 235
no_error_code_interrupt_handler 236
no_error_code_interrupt_handler 237
no_error_code_interrupt_handler 238
no_error_code_interrupt_handler 239
no_error_code_interrupt_handler 240
no_error_code_interrupt_handler 241
no_error_code_interrupt_handler 242
no_error_code_interrupt_handler 243
no_error_code_interrupt_handler 244
no_error_code_interrupt_handler 245
no_error_code_interrupt_handler 246
no_error_code_interrupt_handler 247
no_error_code_interrupt_handler 248
no_error_code_interrupt_handler 249
no_error_code_interrupt_handler 250
no_error_code_interrupt_handler 251
no_error_code_interrupt_handler 252
no_error_code_interrupt_handler 253
no_error_code_interrupt_handler 254
no_error_code_interrupt_handler 255
