#ifndef INITIALISE_H
#define INITIALISE_H

// Use std as the default namespace.
#ifndef NMSP
#define NMSP std
#endif /* NMSP */

namespace NMSP {
namespace helper {

#ifndef KLIB

/**
    This function is called from the startup file crt0.o. Its job is to perform
    any initialisations of the standard library that must be performed before
    entry into main. This includes the heap and standard streams.
 */
extern "C"
void initialise_standard_library() noexcept;

#endif /* KLIB not defined */
} // end helper namespace
} // end NMSP namespace

#endif /* INITIALISE_H */
