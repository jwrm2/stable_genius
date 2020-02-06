#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>
#include <stdint.h>

#include <ios>

#include "InterruptHandler.h"

/**
    Receive the interrupt stack and registers and decide which system call
    function to execute.

    @param ir Contains the contents of the interrupt registers when the
           syscall was made. Can be modified to provide the return value in eax.
    @param is Contains the contents of the interrupt stack just after the
           syscall was made.
    @return Return value for the syscall.
 */
void syscall(InterruptRegisters& ir, const InterruptStack& is);

/**
    List of the system call function indices. See the equivalent functions for
    descriptions.
 */
enum class syscall_ind {
    fork = 0x2,
    read = 0x3,
    write = 0x4,
    open = 0x5,
    close = 0x6,
    wait = 0x7,
    unlink = 0xa,
    execve = 0xb,
    getpid = 0x14,
    yield = 0x9e
};

/**
    List of flags for open.
 */
enum class open_flags {
    /** Empty flags. */
    none = 0x0,
    /** Read only mode. */
    rdonly = 0x1,
    /** Write only mode. */
    wronly = 0x2,
    /** Read and write. */
    rdwr = 0x3,
    /** Append. */
    append = 0x4,
    /** Truncate. */
    trunc = 0x8
};

/**
    Template overloads to allow open_flags enum to be used as Bitfields. We need
    to use the klib bitwise operators, since open_flags is not a member of klib
    and Argument-Dependent Lookup will not be triggered.
 */
namespace klib {
template <>
struct BitmaskEnable<open_flags> : public true_type {};
}
using klib::operator|;
using klib::operator&;
using klib::operator^;
using klib::operator~;
using klib::operator|=;
using klib::operator&=;
using klib::operator^=;

namespace syscalls {

/**
    Creates a copy of the currently running process. A new memory space is
    created and the current state of registers and memory is copied.

    @param ir Contains the contents of the interrupt registers when the
           syscall was made. Needed to set the values for the child process, as
           the arent values shouldn't be updated.
    @param is Contains the contents of the interrupt stack just after the
           syscall was made. Needed to set the values for the child process, as
           the arent values shouldn't be updated.
    @return 0 in the child process, PID of child process in the parent process.
            On error, no new process is created and the -1 is returned in the
            parent.
 */
int32_t fork(const InterruptRegisters& ir, const InterruptStack& is);

/**
    Reads up to count characters from the provided file into the provided
    pointer. May read less than count charaters if EOF is reached or characters
    are not available.

    @param fd File descriptor to read from, from %ebx.
    @param buf Location to place the read data, from %ecx.
    @param count Maximum number of characters to read, from %edx.
    @param The number of characters read. -1 on error.
 */
int32_t read(int fd, char* buf, size_t count);

/**
    Writes count characters from the provided pointer to the specified file
    descriptor for the active process.

    @param fd File descriptor to write to, from %ebx.
    @param buf Location of the string to write, from %ecx.
    @param count Number of characters to write, from %edx.
    @return The number of characters written. -1 on error.
 */
int32_t write(int fd, const char* buf, size_t count);

/**
    Opens a file.

    @param filename Name of the file to open, from %ebx.
    @param flags Various controls on the behaviour, from %ecx.
    @param mode The permissions to set on the file if it is to be created, from
           %edx.
    @return New file descriptor on success, -1 on failure.
 */
int32_t open(const char* filename, open_flags flags, int mode);

/**
    Closes a file.

    @param fd File descriptor to close, from %ebx.
    @return 0 on success, -1 on failure.
 */
int32_t close(int fd);

/**
    Waits for an event to happen on a child process. Normally termination of the
    process.

    @param pid PID of the child process to wait for. -1 indicates any child
           process, from %ebx.
    @param wstatus Address of a field that will get updated with the return
           information. nullptr indicates the information is not required, from
           %ecx.
    @param options Field specifying the nature of the event.
    @return PID of the child process that had an event on success, -1 on
            failure.
 */
int32_t wait(int pid, int* wstatus, int options);

/**
    Unlink a file. Deletes the file if the number of hard links is reduced to
    zero. The delete is postponed until all open file descriptors for the file
    are closed, or happens immediately if there are none.

    @param filename Name of the file to delete, from %ebx.
    @return 0 on success, -1 on failure.
 */
int32_t unlink(const char* filename);

/**
    Replaces the current process with a new one.

    @param filename Name of the executable to launch, from %ebx.
    @param argv Command line arguments for the new executable. Must be null
           terminated, from %ecx.
    @param envp Environment variables for the new executable. Must be null
           terminated, from %edx.

    @return -1 on error. Otherwise doesn't return.
 */
int32_t execve(const char* filename, char* const argv[], char* const envp[]);

/**
    Gets the PID of the current process. Looks in the scheduler for the last
    process to get time.

    @return The PID of the current process.
 */
int32_t getpid();

/**
    Calls the scheduler to move onto the next available process.

    @param ir Contains the contents of the interrupt registers when the
           syscall was made.
    @param is Contains the contents of the interrupt stack just after the
           syscall was made.
    @return 0 on success, -1 on failure. This function will only return after
            the process gets time again, which suggests it was successful.
 */
int32_t yield(const InterruptRegisters& ir, const InterruptStack& is);

}

#endif /* SYSCALL_H */
