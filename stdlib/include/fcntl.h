#ifndef FNCTL_H
#define FNCTL_H

// Use std as the default namespace.
#ifndef NMSP
#define NMSP std
#endif /* NMSP */

// unistd has no use in the kernel library.
#ifndef KLIB

// These functions are in the default namespace and have C linkage.
extern "C" {

// List of file syscalls. These functions are defined in syscalls.s in assembly.

/**
    Opens a file.

    @param filename Name of the file to open, from %ebx.
    @param flags Various controls on the behaviour, from %ecx.
    @param mode The permissions to set on the file if it is to be created, from
           %edx.
    @return New file descriptor on success, -1 on failure.
 */
#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR 3
#define O_APPEND 4
#define O_TRUNC 8
int32_t open(const char* filename, int flags, int mode);

} // end extern "C"
#endif /* not KLIB */
#endif /* FNCTL_H */
