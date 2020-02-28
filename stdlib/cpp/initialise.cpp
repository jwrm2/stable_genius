#include "../include/cstdlib"
#include "../include/new"
#include "../include/unistd.h"
#include "../include/UserHeap.h"

namespace NMSP {

// Standard library initialisation steps are only performed for user space.
#ifndef KLIB

namespace helper {

/**
    This function (which does not appear in a header file) is called from the
    startup file crt0.o. Its job is to perform any initialisations of the
    standard library that must be performed before entry into main. This will
    include the heap and standard streams.
 */
extern "C"
void initialise_standard_library()
{
    // Initialise the heap. We can get the initial programme break point by
    // calling brk(0).
    helper::user_heap = helper::UserHeap {reinterpret_cast<void*>(brk(0))};

    // TODO standard streams.
}

} // end helper namespace

#endif /* KLIB not defined */

} // end NMSP namespace
