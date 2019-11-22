.extern switch_blocked_for_switch

# Transfers to a user mode process, using an iret (ie fooling the processor into
# thinking it was already in user mode).
# Value for edi at %esp + 4
# Value for esi at %esp + 8
# Value for ebp at %esp + 12
# Placeholder for esp at %esp + 16
# Value for ebx at %esp + 20
# Value for edx at %esp + 24
# Value for ecx at %esp + 28
# Value for eax at %esp + 32
# Value for eip at %esp + 36
# Value for cs at %esp + 40
# Value for eflags at %esp + 44
# Value for esp at %esp + 48
# Value for ss (and other segment registers) at %esp + 52
# For the iret, the stack state must be:
#  %esp + 16: ss
#  %esp + 12: esp
#  %esp +  8: eflags
#  %esp +  4: cs
#  %esp     : eip
# We just need to do a little fiddling to set that up.
.global launch_process
launch_process:
    # Get rid of the return address, since we won't be needing it.
    add $4, %esp

    # Restore general purpose register values. The value for esp is ignored, so
    # the effect on esp is just to add 32.
    popal

    # The stack is now correctly set for the iret

    # Unblock task switching for switch in progress. We do this at the last
    # possible moment.
    movl $0, switch_blocked_for_switch

    # Segment registers other than cs and ss.
    push %eax
    mov 20(%esp), %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    pop %eax

    # Testing
#    mov $0xCAFEBABE, %ecx
#    .test: jmp .test

    # The stack and segment selectors are ready. Jump to user mode.
    iret

    # We should never get here. There's nothing we can do anyway, as we've lost
    # the return address.

# Transfers to a kernel mode process, using an iret (we want an iret to restore
# the value of eflags).
# Value for edi at %esp + 4
# Value for esi at %esp + 8
# Value for ebp at %esp + 12
# Placeholder for esp at %esp + 16
# Value for ebx at %esp + 20
# Value for edx at %esp + 24
# Value for ecx at %esp + 28
# Value for eax at %esp + 32
# Value for eip at %esp + 36
# Value for cs at %esp + 40
# Value for eflags at %esp + 44
# Value for esp at %esp + 48
# For the iret, the stack state must be:
#  %esp +  8: eflags
#  %esp +  4: cs
#  %esp     : eip
# We just need to do a little fiddling to set that up.
.global launch_kernel_process
launch_kernel_process:
    # Get rid of the return address, since we won't be needing it.
    add $4, %esp

    # Restore general purpose register values. The value for esp is ignored, so
    # the effect on esp is just to add 32.
    popal

    # No need to do ss here. However, we need to set up the iret stack at the
    # new esp address, as iret without a privilege change doesn't change the
    # stack pointer.

    # Save the current esp into ebp. We need to save the ebp value, since we've
    # already restored the process one above. I think doing it this way is
    # quicker than copying all the register values to the new stack space.
    push %ebp
    mov %esp, %ebp

    # Change stack to the resume address.
    mov 16(%ebp), %esp
    # eflags
    pushl 12(%ebp)
    # cs
    pushl 8(%ebp)
    # eip
    pushl 4(%ebp)

    # We're done with the old esp value. Restore ebp to the correct process
    # value.
    mov (%ebp), %ebp 

    # Unblock task switching for switch in progress. We do this at the last
    # possible moment.
    movl $0, switch_blocked_for_switch

    # Testing
#    mov $0xCAFEBABE, %ecx
#    .test: jmp .test

    # The stack is ready, time to iret, which conveniently leaves esp in the 
    # right place.
    iret

    # We should never get here. There's nothing we can do anyway, as we've lost
    # the return address.

# Returns the current value of the EFLAGS register.
.global get_eflags
get_eflags:
    # Push flags onto the stack.
    pushfl
    # Pop into eax for returning.
    pop %eax

    ret
