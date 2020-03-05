#ifndef UNISTD_H
#define UNISTD_H

// Use std as the default namespace.
#ifndef NMSP
#define NMSP std
#endif /* NMSP */

// unistd has no use in the kernel library.
#ifndef KLIB

#include <stddef.h>
#include <stdint.h>

// Forward declarations.
namespace std {
    template<typename> class fpos;
    class mbstate_t; 
    using fpos_t = fpos<mbstate_t>;
}

/**
    Symbolic constants specifying the file descriptors for the standard streams.
 */
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

// These functions are in the default namespace and have C linkage.
extern "C" {

// List of syscalls. These functions are defined in syscalls.s in assembly.

/**
    Creates a copy of the currently running process. A new memory space is
    created and the current state of registers and memory is copied.

    @return 0 in the child process, PID of child process in the parent process.
            On error, no new process is created and the -1 is returned in the
            parent.
 */
int32_t fork();

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
    Attempts to create a new directory.

    @param pathname Name to create the new directory with.
    @param mode Permisions for the new directory.

    @return 0 on success, -1 on error.
 */
int32_t mkdir(const char* pathname, int mode);

/**
    Removes (unlinks) a directory. Deletes the directory if the number of hard
    links is reduced to zero. The delete is postponed until all open file
    descriptors for the file are closed, or happens immediately if there are
    none. Fails if the directory is not empty.

    @param pathname Directory to delete.
    @return 0 on success, -1 on error.
 */
int32_t rmdir(const char* pathname);

/**
    Changes the break point (end of the heap) to the specified address. May fail
    if not enough memory is available, if the requested address is into the
    stack, or if the requested address is before the start of the heap. As a
    special case, if the value 0 is passed, instead returns the current break
    point.

    @param addr New address to set, from %ebx.
    @return 0 on success, -1 on error, or current break point if addr was 0.
 */
int32_t brk(void* addr);

/**
    Repositions the offset in an open file description.

    @param fd File descriptor to operate on, from %ebx.
    @param offset_high High 32 bits of the new offset, from %ecx.
    @param offset_low Low 32 bits of the new offset, from %edx.
    @param result Pointer to the location to store the new position, from %esi.
    @param whence Origin for the new offset, may be SEEK_SET, SEEK_CUR or
           SEEK_END, from %edi.
    @return 0 on success, -1 on error.
 */
int32_t llseek(int fd, int32_t offset_high, int32_t offset_low,
    std::fpos_t* result, uint32_t whence);

/**
    Calls the scheduler to move onto the next available process.

    @return 0 on success, -1 on failure. This function will only return after
            the process gets time again, which suggests it was successful.
 */
int32_t yield();


} // end extern "C"
#endif /* not KLIB */
#endif /* UNISTD_H */
