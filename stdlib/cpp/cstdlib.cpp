#include "../include/cstdlib"
#include "../include/cctype"
#include "../include/cmath"
#include "../include/cstring"
#include "../include/limits"
#include "../include/UserHeap.h"

namespace NMSP {

/******************************************************************************
 ******************************************************************************/

long strtol(const char* str, char **str_end, int base)
{
    long long llret_val = strtoll(str, str_end, base);

    if (llret_val > numeric_limits<long>::max())
        return numeric_limits<long>::max();

    if (llret_val < numeric_limits<long>::min())
        return numeric_limits<long>::min();

    return static_cast<long>(llret_val);
}

/******************************************************************************/

long long strtoll(const char* str, char **str_end, int base)
{
    const char* ptr = str;

    // Gobble leading space.
    while (isspace(*ptr))
        ++ptr;

    // Check whether we have a leading minus.
    bool is_neg = false;
    if (*ptr == '+')
        ++ptr;
    else if (*ptr == '-')
    {
        is_neg = true;
        ++ptr;
    }

    // We need to fail if the next character is a space. Otherwise they'll be
    // gobbled by strtoull.
    if (isspace(*ptr))
    {
        if (str_end != nullptr)
            *str_end = const_cast<char*>(str);
        return 0;
    }

    // Get the numerical value.
    unsigned long long ullret_val = strtoull(ptr, str_end, base);

    // Check for overflow.
    if (ullret_val > static_cast<unsigned long long>(
        numeric_limits<long long>::max()) && !is_neg)
        return numeric_limits<long long>::max();

    if (ullret_val > static_cast<unsigned long long>(
        -(numeric_limits<long long>::min() + 1)) + 1 && is_neg)
        return numeric_limits<long long>::min();

    long long ret_val = static_cast<long long>(ullret_val);

    return (is_neg ? - ret_val : ret_val);
}

/******************************************************************************/

unsigned long strtoul(const char* str, char **str_end, int base)
{
    unsigned long ullret_val = strtoull(str, str_end, base);

    if (ullret_val > numeric_limits<unsigned long>::max())
        return numeric_limits<unsigned long>::max();

    return static_cast<unsigned long>(ullret_val);
}

/******************************************************************************/

unsigned long long strtoull(const char* str, char **str_end, int base)
{
    const char* endptr = str;

    // Check whether we have a valid base.
    if (base < 0 || base == 1 || base > 36)
    {
        if (str_end != nullptr)
            *str_end = const_cast<char*>(str);
        return 0;
    }

    // Gobble any leading whitespace.
    const char* ptr = str;
    while (isspace(*ptr))
        ++ptr;

    // Check whether we have a leading minus.
    bool is_neg = false;
    if (*ptr == '+')
        ++ptr;
    else if (*ptr == '-')
    {
        is_neg = true;
        ++ptr;
    }

    // Check for a 0x or 0 prefix and set the base if necessary.
    if (*ptr == '0')
    {
        ++ptr;
        endptr = ptr;

        if (base == 16 && (*ptr == 'x' || *ptr == 'X'))
            ++ptr;
        else if (base == 0)
        {
            if (*ptr == 'x' || *ptr == 'X')
            {
                base = 16;
                ++ptr;
            }
            else
                base = 8;
        }
    }
    else if (base == 0)
        base = 10;

    // Accumulator for the result.
    unsigned long long accum = 0;

    // Things to deal with overflows.
    unsigned long long cutoff =
        numeric_limits<unsigned long long>::max() / base;
    bool overflow = false;

    // Loop through the characters.
    while (true)
    {
        // Work out the current digit. Break if invalid.
        int digit;
        if (isdigit(*ptr))
            digit = *ptr - '0';
        else if (islower(*ptr))
            digit = *ptr - 'a' + 10;
        else if (isupper(*ptr))
            digit = *ptr - 'A' + 10;
        else
            break;

        // Check whether the digit exceeds the base.
        if (digit >= base)
            break;

        // Advance the pointer and the end pointer.
        endptr = ++ptr;

        // Check whether we overflowed last round. We still need to advance
        // the endptr to the first non-digit character in the case of overflow,
        // unless str_end is nullptr, in which case we can skip this.
        if (overflow)
        {
            if (str_end == nullptr)
                break;
            continue;
        }

        // Check for incipient overflow.
        if (accum > cutoff / base || accum * base > cutoff - digit)
        {
            overflow = true;
            accum = cutoff;
        }
        else
            accum = accum * base + digit;
    }

    if (overflow)
        accum = numeric_limits<unsigned long long>::max();

    else if (is_neg)
        accum = -accum;

    if (str_end != nullptr)
        *str_end = const_cast<char*>(endptr);

    return accum;
}

/******************************************************************************
 ******************************************************************************/

int atoi(const char* str)
{
    long lret_val = strtol(str, nullptr, 10);

    if (lret_val > numeric_limits<int>::max())
        return numeric_limits<int>::max();

    if (lret_val < numeric_limits<int>::min())
        return numeric_limits<int>::min();

    return static_cast<int>(lret_val);
}

/******************************************************************************/

long atol(const char* str)
{
    return strtol(str, nullptr, 10);
}

/******************************************************************************/

long long atoll(const char* str)
{
    return strtoll(str, nullptr, 10);
}

/******************************************************************************
 ******************************************************************************/

float strtof(const char* str, char** str_end)
{
    // Get strtold to do the heavy lifting.
    long double ld_val = strtold(str, str_end);

    // Deal with special cases.
    if (isnan(ld_val))
        return numeric_limits<float>::quiet_NaN();
    if (ld_val == numeric_limits<long double>::infinity())
        return numeric_limits<float>::infinity();
    if (ld_val == -numeric_limits<long double>::infinity())
        return -numeric_limits<float>::infinity();

    // Overflow
    if (ld_val > numeric_limits<float>::max())
        return numeric_limits<float>::max();
    if (ld_val < numeric_limits<float>::lowest())
        return numeric_limits<float>::lowest();

    // Underflow
    if (ld_val > 0.0L && ld_val < numeric_limits<float>::min())
        return numeric_limits<float>::min();
    if (ld_val < 0.0L && ld_val > -numeric_limits<float>::min())
        return -numeric_limits<float>::min();

    return static_cast<float>(ld_val);
}

/******************************************************************************/

double strtod(const char* str, char** str_end)
{
    // Get strtold to do the heavy lifting.
    long double ld_val = strtold(str, str_end);

    // Deal with special cases.
    if (isnan(ld_val))
        return numeric_limits<double>::quiet_NaN();
    if (ld_val == numeric_limits<long double>::infinity())
        return numeric_limits<double>::infinity();
    if (ld_val == -numeric_limits<long double>::infinity())
        return -numeric_limits<double>::infinity();

    // Overflow
    if (ld_val > numeric_limits<double>::max())
        return numeric_limits<double>::max();
    if (ld_val < numeric_limits<double>::lowest())
        return numeric_limits<double>::lowest();

    // Underflow
    if (ld_val > 0.0L && ld_val < numeric_limits<double>::min())
        return numeric_limits<double>::min();
    if (ld_val < 0.0L && ld_val > -numeric_limits<double>::min())
        return -numeric_limits<double>::min();

    return static_cast<double>(ld_val);
}

/******************************************************************************/

namespace helper {
    // Converts a string of digits into a long double.
    long double conv(const char* str, const char** str_end, int base)
    {
        if (base != 10 && base != 16)
        {
            *str_end = const_cast<char*>(str);
            return 0.0; 
        }

        long double result = 0.0;

        const char* ptr = str;
        while (true)
        {
            int digit;
            if ('0' <= *ptr && *ptr <= '9')
                digit = *ptr - '0';
            else if (base == 16 && 'a' <= *ptr && *ptr <= 'f')
                digit = *ptr - 'a' + 10;
            else if (base == 16 && 'A' <= *ptr && *ptr <= 'F')
                digit = *ptr - 'A' + 10;
            else
                break;

            result = result * base + digit;
            ++ptr;        
        }

        if (ptr == str)
        {
            *str_end = const_cast<char*>(str);
            return 0.0;
        }

        *str_end = const_cast<char*>(ptr);
        return result;

    }
} // helper namespace

long double strtold(const char* str, char** str_end)
{
    // Gobble leading whitespace.
    const char* ptr = str;
    while (isspace(*ptr))
        ++ptr;

    // Check for a plus or minus sign.
    int sign = 1;
    if (*ptr == '+')
        ++ptr;
    else if (*ptr == '-')
    {
        sign = - 1;
        ++ptr;
    }

    // Check for a hexadecimal 0x prefix (this is the only way to get a non-
    // decimal conversion).
    int base = 10;
    if (strncmp(ptr, "0x", 2) == 0 || strncmp(ptr, "0X", 2) == 0)
    {
        base = 16;
        ptr += 2;
    }

    // Check for the special cases of NaN and INF.
    {
        constexpr size_t len = 8;
        char c[len];
        memset(c, '\0', len);
        for (size_t i = 0; i < len && ptr[i] != '\0'; ++i)
            c[i] = toupper(ptr[i]);

        if (strncmp(c, "INFINITY", len) == 0)
        {
            if (str_end != nullptr)
                *str_end = const_cast<char*>(ptr + len);
            return sign * numeric_limits<long double>::infinity();
        }
    }
    {
        constexpr size_t len = 3;
        char c[len];
        memset(c, '\0', len);
        for (size_t i = 0; i < len && ptr[i] != '\0'; ++i)
            c[i] = toupper(ptr[i]);

        if (strncmp(c, "NAN", len) == 0)
        {
            if (str_end != nullptr)
                *str_end = const_cast<char*>(ptr + len);
            return numeric_limits<long double>::quiet_NaN();
        }
        else if (strncmp(c, "INF", len) == 0)
        {
            if (str_end != nullptr)
                *str_end = const_cast<char*>(ptr + len);
            return sign * numeric_limits<long double>::infinity();
        }
    }

    // Get the part before the decimal. It would be nice to use strtoull for
    // this, but I think that is overly restrictive on the range.
    long double before_point = 0.0;
    {
        const char* temp = ptr;
        const char** ptr2 = &temp;
        before_point = helper::conv(ptr, ptr2, base);

        // Invalid, return. The special case is if the first character is a .
        // and there is no before . number.
        if (before_point == 0.0 && *ptr2 == ptr && *ptr != '.')
        {
            if (str_end != nullptr)
                *str_end = const_cast<char*>(str);
            return 0.0;
        }

        // Advance ptr.
        ptr = *ptr2;
    }

    // Check for a decimal point and get the fractional part.
    long double after_point = 0.0;
    if (*ptr == '.')
    {
        ++ptr;

        // Preserve the number of zeros.
        int power = 0;
        for (; *ptr == '0'; ++power, ++ptr) ;

        const char* temp = ptr;
        const char** ptr2 = &temp;
        after_point = helper::conv(ptr, ptr2, base);

        // Invalid is permitted here.
        // Advance ptr.
        ptr = *ptr2;

        // Divide down fractional part.
        while (after_point >= 1.0)
            after_point /= base;

        // Divide down further for leading zeroes.
        for (; power > 0; --power)
            after_point /= base;
    }

    // Assemble the number (before exponentiation).
    long double result = before_point + after_point;

    // Check for an exponent and get it. Here we're ok with using strtol as if
    // the exponent is too big (small) we're definitely over(under)flowing
    // anyway.
    long exponent = 0;
    if ((base == 10 && (*ptr == 'E' || *ptr == 'e')) ||
        (base == 16 && (*ptr == 'P' || *ptr == 'p')))
    {
        ++ptr;
        char **ptr2 = const_cast<char**>(&ptr);
        // Just to confuse us, the exponent must be in decimal.
        exponent = strtol(ptr, ptr2, 10);

        // Invalid, return.
        if (exponent == 0 && *ptr2 == ptr)
        {
            if (str_end != nullptr)
                *str_end = const_cast<char*>(ptr - 1);
            return result;
        }

        // Advance ptr.
        ptr = *ptr2;
    }

    // The exponent is a power of 2 for hex (just to confuse everyone).
    if (base == 16)
        base = 2;

    // Horribly inefficient, but I don't really care.
    for (; exponent > 0; --exponent)
    {
        if (numeric_limits<long double>::max() / base < result)
        {
            result = numeric_limits<long double>::max();
            break;
        }
        result *= base;
    }
    for (; exponent < 0; ++exponent)
    {
        if (numeric_limits<long double>::min() * base > result)
        {
            result = numeric_limits<long double>::min();
            break;
        }
        result /= base;
    }

    // Apply the sign.
    result *= sign;

    if (str_end != nullptr)
        *str_end = const_cast<char*>(ptr);

    return result;
}

/******************************************************************************
 ******************************************************************************/

#ifndef KLIB

/******************************************************************************
 ******************************************************************************/

void* malloc(size_t size)
{
    return helper::user_heap->malloc(size);
}

/******************************************************************************
 ******************************************************************************/

void free(void* ptr)
{
    return helper::user_heap->free(ptr);
}

/******************************************************************************
 ******************************************************************************/

void abort()
{
    // TODO execute system call to end the process.
}

/******************************************************************************
 ******************************************************************************/

void exit(int status)
{
    // TODO run functions registered with atexit().

    // The _fini function must be called to call global destructors.
    std::helper::_fini();

    // TODO execute system call to end the process.
    (void)status;
} 

/******************************************************************************
 ******************************************************************************/

#endif /* not KLIB */

/******************************************************************************
 ******************************************************************************/

} // NMSP namespace
