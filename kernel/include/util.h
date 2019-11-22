#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#include <string>

/**
    Contains a few functions that are useful but don't really belong anywhere
    else. These functions do use the heap, unlike the ones in no_heap_utils.h.
 */

/**
    Changes a number into a smaller number with a multiple of bytes suffix,
    eg. 10MB or 15GB.

    @param bytes Number to format.
    @return String representation of the number.
 */
klib::string format_bytes(uint64_t bytes);

#endif /* UTIL_H */

