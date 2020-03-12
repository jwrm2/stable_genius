#include "../include/cstdlib"
#include "../include/fstream"
#include "../include/ios"
#include "../include/new"
#include "../include/unistd.h"
#include "../include/UserHeap.h"

namespace NMSP {

// Standard library initialisation steps are only performed for user space.
#ifndef KLIB

namespace helper {

// This function is nothrow, as we can't handle exceptions in the calling
// assembly routine. All exceptions must be caught.
void initialise_standard_library()
{
    try {
        // The nothrow oject is used by new for allocations, but is a global
        // object. We need to contruct it here or we'll get problems. It doesn't
        // matter if it gets initialised again.
        nothrow = nothrow_t {};

        // Initialise the heap. We can get the initial programme break point by
        // calling brk(0).
        helper::user_heap = helper::UserHeap {reinterpret_cast<void*>(brk(0))};

        // Initialise the C-style standard streams. Must be after heap
        // initialisation, as they use heap buffers. The file descriptors are
        // assumed to be open (they should be inherited from the exec'd process)
        // and setup according the standard numbers from unistd.h.
        // If this is the init process, these descriptors will not exist. That
        // shouldn't matter, as long as init opens them in the right order with
        // a direct syscall before trying to write to any of them.
        stdin = new (nothrow) FILE {};
        if (stdin != nullptr)
            stdin->open(STDIN_FILENO, ios_base::in);
        stdout = new (nothrow) FILE {};
        if (stdout != nullptr)
            stdout->open(STDOUT_FILENO, ios_base::out);
        stderr = new (nothrow) FILE {};
        if (stderr != nullptr)
            stderr->open(STDERR_FILENO, ios_base::out);
    }
    catch (...) { /* TODO we should possibly abort here... */ }
}

} // end helper namespace

#endif /* KLIB not defined */

} // end NMSP namespace
