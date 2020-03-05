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

void initialise_standard_library()
{
    // Initialise the heap. We can get the initial programme break point by
    // calling brk(0).
    helper::user_heap = helper::UserHeap {reinterpret_cast<void*>(brk(0))};

    // Initialise the C-style standard streams. Must be after heap
    // initialisation, as they use heap buffers. The file descriptors are
    // assumed to be open (they should be inherited from the exec'd process) and
    // setup according the standard numbers from unistd.h.
    stdin = new FILE {};
    stdin->open(STDIN_FILENO, ios_base::in);
    stdout = new FILE {};
    stdout->open(STDOUT_FILENO, ios_base::out);
    stderr = new FILE {};
    stderr->open(STDERR_FILENO, ios_base::out);
}

} // end helper namespace

#endif /* KLIB not defined */

} // end NMSP namespace
