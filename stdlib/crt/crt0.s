.section .text

# Program entry point
.global _start
_start:
    # Create a terminating zero in the stack frame linked list.
    mov $0, %ebp
    push %ebp # Gives a zero return address (%eip).
    push %ebp # Gives a zero stack base address (%ebp).
    mov %esp, %ebp # Sets stack base address appropriately for this function.

    # TODO Call standard library initialistion here.
    # call initialize_standard_library

    # Run the global contructors.
    call _init

    # TODO Might need to fiddle with argc and argv here when they exist.
    # Call the main program.
    call main

    # Call the exit function with the return value of main. The exit function
    # must call the _fini function.
    push %eax
    call exit
