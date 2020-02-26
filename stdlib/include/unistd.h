#ifndef UNISTD_H
#define UNISTD_H

// Use std as the default namespace.
#ifndef NMSP
#define NMSP std
#endif /* NMSP */

// unistd has no use in the kernel library. Only have anything here if NMSP is
// std.
#if NMSP == std

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
    Opens a file.

    @param filename Name of the file to open, from %ebx.
    @param flags Various controls on the behaviour, from %ecx.
    @param mode The permissions to set on the file if it is to be created, from
           %edx.
    @return New file descriptor on success, -1 on failure.
 */
int32_t open(const char* filename, int flags, int mode);

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
    Calls the scheduler to move onto the next available process.

    @return 0 on success, -1 on failure. This function will only return after
            the process gets time again, which suggests it was successful.
 */
int32_t yield();


} // end extern "C"
#endif /* NMSP == std */
#endif /* UNISTD_H */
