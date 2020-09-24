#include "../include/initialise.h"

#include "../include/cstdlib"
#include "../include/fstream"
#include "../include/ios"
#include "../include/unistd.h"
#include "../include/UserHeap.h"

namespace NMSP {

// Standard library initialisation steps are only performed for user space.
#ifndef KLIB

namespace helper {

extern "C" void hang(int);

// This function is nothrow, as we can't handle exceptions in the calling
// assembly routine. All exceptions must be caught.
void initialise_standard_library() noexcept
{
    try {
        // The nothrow oject is used by new for allocations, but is a global
        // object. We need to contruct it here or we'll get problems. It doesn't
        // matter if it gets initialised again.
        nothrow = nothrow_t {};

        // Initialise the heap. We can get the initial programme break point by
        // calling brk(0). We need to call the constructor here, so it is ready
        // for standard stream initialisation, but we also need to prevent the
        // constructor being called later from the list of global objects. We
        // have a pointer to the heap (user_heap) as well as a char[] of the
        // correct size and alignment. We construct the heap in place on the
        // char[], then set the pointer to the right location.
        helper::user_heap = new (helper::user_heap_space) helper::UserHeap
            {reinterpret_cast<void*>(brk(0))};

        // Initialise the C-style standard streams. Must be after heap
        // initialisation, as they use heap buffers. The file descriptors are
        // assumed to be open (they should be inherited from the exec'd process)
        // and setup according the standard numbers from unistd.h.
        // If this is the init process, these descriptors will not exist. That
        // shouldn't matter, as long as init opens them in the right order with
        // a direct syscall before trying to do anything to any of them.
        stdin = new FILE {};
        if (stdin->open(STDIN_FILENO, ios_base::in) == nullptr)
            hang(1);
        stdout = new FILE {};
        if (stdout->open(STDOUT_FILENO, ios_base::out) == nullptr)
            hang(2);
        stderr = new FILE {};
        if (stderr->open(STDERR_FILENO, ios_base::out) == nullptr)
            hang(3);
    }
    catch (...) { /* TODO we should possibly abort here... */ }
}

} // end helper namespace

#endif /* KLIB not defined */

} // end NMSP namespace
