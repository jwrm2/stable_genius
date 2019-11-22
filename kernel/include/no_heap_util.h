#ifndef NO_HEAP_UTIL_H
#define NO_HEAP_UTIL_H

#include <stdarg.h>

#include "Device.h"

/**
    Convert an integer to a C string representation. Doesn't use heap memory.
    Use klib::helper::itostr or klib::helper::uitostr if the heap is available.

    @param input The value to convert.
    @param buffer Where to store the result. Will be a null terminated string.
                  Must be large enough to hold the result.
                  Nothing happens if this is the null pointer.
    @param radix The base of the output (eg. 10 for decimal).
 */
void itoa(signed long long int input, char* buffer, unsigned int radix);
void uitoa(unsigned long long int input, char* buffer, unsigned int radix);

/**
    Convert a double to a C string representation. Doesn't use heap memory. Use
    klib::helper::dtostr if the heap is available.

    @param input The value to convert.
    @param buffer Where to store the result. Will be a null terminated string.
           Must be large enough to hold the result. Nothing happens if this is
           the null pointer.
    @param style String style, either 'f' for fixed point, 'e' for exponetial
           form, or g to use fixed point if 0.001 < value < 1000000 and
           exponential otherwise. g is assumed if the argument is invalid.
 */
void dtoa(long double input, char* buffer, char style);

/**
    Reverse a C string in place.

    @param str A null terminated C string.
 */
void reverse(char* str);

/**
    Prints a formatted string to a character device. Does not use heap memory.

    @param dest Where to send the output. Uses only the write_char(char c)
           function.
    @param fmt A string to print, with printf %d %s etc. substitutions.
    @param ... Values to substitute into the format string.
    @return Number of characters printed, or EOF on failure.
 */
int kprintf(CharacterDevice* dest, const char* fmt, ...);

/**
    Prints a formatted string to a character device. Does not use heap memory.

    @param dest Where to send the output. Uses only the write_char(char c)
           function.
    @param fmt A string to print, with printf %d %s etc. substitutions.
    @param arg Values to substitute into the format string. The caller is
               responsible for initialising and ending the list.
    @return Number of characters printed, or EOF on failure.
 */
int kvprintf(CharacterDevice* dest, const char* fmt, va_list arg);

#endif /* NO_HEAP_UTIL_H */
