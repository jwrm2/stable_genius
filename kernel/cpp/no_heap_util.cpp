#include "no_heap_util.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/******************************************************************************
 ******************************************************************************/

void itoa(signed long long int value, char* buffer, unsigned int radix)
{
    if (!buffer)
        return;

    if (radix == 0 || radix > 36)
        return;

    static char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Get the sign, then make sure value is positive
    bool neg = false;
    if (value < 0)
    {
        neg = true;
        value = -value;
    }

    // Generate digits in reverse order
    int i;
    if (value == 0)
    {
        buffer[0] = '0';
        i = 1;
    }
    else
    {
        for (i = 0; value; ++i)
        {
            buffer[i] = digits[value % radix];
            value /= radix;
        }
    }

    // Include sign
    if (neg)
    {
        buffer[i] = '-';
        ++i;
    }

    // Null termination
    buffer[i] = '\0';

    // Reverse digits.
    reverse(buffer);
}

/******************************************************************************/

void uitoa(unsigned long long int value, char* buffer, unsigned int radix)
{
    if (!buffer)
        return;

    if (radix == 0 || radix > 36)
        return;

    static char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Generate digits in reverse order
    int i;
    if (value == 0)
    {
        buffer[0] = '0';
        i = 1;
    }
    else
    {
        for (i = 0; value; ++i)
        {
            buffer[i] = digits[value % radix];
            value /= radix;
        }
    }

    // Null termination
    buffer[i] = '\0';

    // Reverse digits.
    reverse(buffer);
}

/******************************************************************************/

void dtoa(long double input, char* buffer, char style)
{
    const int max_dp = 14;
    const int radix = 10;

    if (input < 0.0)
    {
        buffer[0] = '-';
        buffer[1] = '\0';
        input = klib::fabs(input);
    }
    else
        buffer[0] = '\0';

    if (style != 'e' && style != 'f')
    {
        if (klib::fabs(input) >= 1000000.0 || klib::fabs(input) < 0.001)
            style = 'e';
        else
            style = 'f';
    }

    if (style == 'e')
    {
        int exponent = 0;
        while (input > radix)
        {
            input /= radix;
            ++exponent;
        }
        while (input < 1.0)
        {
            input *= radix;
            --exponent;
        }

        char number_part[2];
        char exponent_part[5];
        itoa(static_cast<long long int>(input), number_part, radix);
        itoa(exponent, exponent_part, radix);

        char decimal_part[max_dp + 1];
        input = klib::fabs(input - klib::trunc(input));
        for (int i = max_dp; i > 0; --i)
            input *= radix;
        while (klib::fmod(input, radix) < 1)
            input /= radix;
        itoa(static_cast<long long int>(
            klib::round(input)), decimal_part, radix);

        buffer = klib::strcat(buffer, number_part);
        buffer = klib::strcat(buffer, ".");
        buffer = klib::strcat(buffer, decimal_part);
        buffer = klib::strcat(buffer, "E");
        buffer = klib::strcat(buffer, exponent_part);
    }
    else
    {
        char number_part[309];
        itoa(static_cast<long long int>(input), number_part, radix);

        char decimal_part[max_dp + 1];
        input = klib::fabs(input - klib::trunc(input));
        for (int i = max_dp; i > 0; --i)
            input *= radix;
        while (klib::fmod(input, radix) < 1)
            input /= radix;
        itoa(static_cast<long long int>(
            klib::round(input)), decimal_part, radix);

        buffer = klib::strcat(buffer, number_part);
        buffer = klib::strcat(buffer, ".");
        buffer = klib::strcat(buffer, decimal_part);
    }
}

/******************************************************************************
 ******************************************************************************/

void reverse(char* str)
{
    for (int i = 0, j = klib::strlen(str) - 1; i < j; ++i, --j)
    {
        char c = str[i];
        str[i] = str[j];
        str[j] = c;
    }
}

/******************************************************************************
 ******************************************************************************/

int kprintf(CharacterDevice* dest, const char* fmt, ...)
{
    // Initialise the varibale argument list
    va_list arg;
    va_start(arg, fmt);
    // Pass on to kvprintf
    int ret_val = kvprintf(dest, fmt, arg);
    // Clean up
    va_end(arg);
    return ret_val;
}

int kvprintf(CharacterDevice* dest, const char* fmt, va_list arg)
{
    if (!dest || !fmt)
        return EOF;

    int ret_val = 0;

    for(const char* traverse = fmt; *traverse; ++traverse) 
    {
        if (*traverse != '%')
        {
            dest->write_char(*traverse);
            ++ret_val;
            continue;
        }

        // Move onto the format character
        bool in_format = true;
        bool long_format = false;
        bool long_long_format = false;
        bool long_double_format = false;
        while (in_format)
        {
            ++traverse;
            // Call the appropriate conversion
            switch(*traverse) 
            {
            // Char
            case 'c':
            {
                char c = static_cast<char>(va_arg(arg, int));
                dest->write_char(c);
                ++ret_val;
                in_format = false;
                break;
            }
            // Signed decimal integer
            case 'd': case 'i':
            {
                char buffer[50];
                if (long_long_format)
                {
                    long long int i = va_arg(arg, long long int);
                    itoa(i, buffer, 10);
                }
                else if (long_format)
                {
                    long int i = va_arg(arg, long int);
                    itoa(i, buffer, 10);
                }
                else
                {
                    int i = va_arg(arg, int);
                    itoa(i, buffer, 10);
                }
                for (const char* c = buffer; *c != '\0'; ++c)
                    dest->write_char(*c);
                ret_val += klib::strlen(buffer);
                in_format = false;
                break;
            }
            // Long specifier
            case 'l':
            {
                if (long_format)
                    long_long_format = true;
                else
                    long_format = true;
                break;
            }
            // Long double specifier
            case 'L':
            {
                long_double_format = true;
                break;
            }
            // Unsigned decimal integer
            case 'u':
            {
                char buffer[50];
                if (long_long_format)
                {
                    unsigned long long int i = 
                        va_arg(arg, unsigned long long int);
                    uitoa(i, buffer, 10);
                }
                else if (long_format)
                {
                    unsigned long int i = va_arg(arg, unsigned long int);
                    uitoa(i, buffer, 10);
                }
                else
                {
                    unsigned int i = va_arg(arg, unsigned int);
                    uitoa(i, buffer, 10);
                }
                for (const char* c = buffer; *c != '\0'; ++c)
                    dest->write_char(*c);
                ret_val += klib::strlen(buffer);
                in_format = false;
                break;
            }
            // Unsigned octal integer
            case 'o':
            {
                char buffer[50];
                if (long_long_format)
                {
                    unsigned long long int i = 
                        va_arg(arg, unsigned long long int);
                    uitoa(i, buffer, 8);
                }
                else if (long_format)
                {
                    unsigned long int i = va_arg(arg, unsigned long int);
                    uitoa(i, buffer, 8);
                }
                else
                {
                    unsigned int i = va_arg(arg, unsigned int);
                    uitoa(i, buffer, 8);
                }
                dest->write_char('0');
                ++ret_val;
                for (const char* c = buffer; *c != '\0'; ++c)
                    dest->write_char(*c);
                ret_val += klib::strlen(buffer);
                in_format = false;
                break;
            }
            // Unsigned hexadecimal integer
            case 'X':
            {
                char buffer[50];
                if (long_long_format)
                {
                    unsigned long long int i = 
                        va_arg(arg, unsigned long long int);
                    uitoa(i, buffer, 16);
                }
                else if (long_format)
                {
                    unsigned long int i = va_arg(arg, unsigned long int);
                    uitoa(i, buffer, 16);
                }
                else
                {
                    unsigned int i = va_arg(arg, unsigned int);
                    uitoa(i, buffer, 16);
                };
                dest->write_char('0');
                dest->write_char('X');
                ret_val += 2;
                for (const char* c = buffer; *c != '\0'; ++c)
                    dest->write_char(*c);
                ret_val += klib::strlen(buffer);
                in_format = false;
                break;
            }
            // C-style string
            case 's':
            {
                const char* s = va_arg(arg, const char *);
                for (const char* c = s; *c != '\0'; ++c)
                    dest->write_char(*c);
                ret_val += klib::strlen(s);
                in_format = false;
                break;
            }
            // Pointer
            case 'p':
            {
                char buffer[50];
                void* ptr = va_arg(arg, void*);
                uintptr_t i = reinterpret_cast<uintptr_t>(ptr);
                itoa(i, buffer, 16);
                dest->write_char('0');
                dest->write_char('X');
                ret_val += 2;
                for (const char* c = buffer; *c != '\0'; ++c)
                    dest->write_char(*c);
                ret_val += klib::strlen(buffer);
                in_format = false;
                break;
            }
            // Fixed point double
            case 'f':
            {
                char buffer[315];
                if (long_double_format)
                {
                    long double d = va_arg(arg, long double);
                    dtoa(d, buffer, 'f');
                }
                else
                {
                    double d = va_arg(arg, double);
                    dtoa(d, buffer, 'f');
                }
                for (const char* c = buffer; *c != '\0'; ++c)
                    dest->write_char(*c);
                ret_val += klib::strlen(buffer);
                in_format = false;
                long_double_format = false;
                break;
            }
            // Exponential double
            case 'e':
            {
                char buffer[23];
                if (long_double_format)
                {
                    long double d = va_arg(arg, long double);
                    dtoa(d, buffer, 'e');
                }
                else
                {
                    double d = va_arg(arg, double);
                    dtoa(d, buffer, 'e');
                }
                for (const char* c = buffer; *c != '\0'; ++c)
                    dest->write_char(*c);
                ret_val += klib::strlen(buffer);
                in_format = false;
                long_double_format = false;
                break;
            }
            // Fixed point double
            case 'g':
            {
                char buffer[23];
                if (long_double_format)
                {
                    long double d = va_arg(arg, long double);
                    dtoa(d, buffer, 'g');
                }
                else
                {
                    double d = va_arg(arg, double);
                    dtoa(d, buffer, 'g');
                }
                for (const char* c = buffer; *c != '\0'; ++c)
                    dest->write_char(*c);
                ret_val += klib::strlen(buffer);
                in_format = false;
                long_double_format = false;
                break;
            }
            // Literal %
            case '%':
            {
                dest->write_char('%');
                ++ret_val;
                in_format = false;
                break;
            }
            // Unrecognised format
            default:
            {
                const char* buffer = "Bad Format";
                for (const char* c = buffer; *c != '\0'; ++c)
                    dest->write_char(*c);
                ret_val += 10;
                in_format = false;
            }
            }
        } 
    }

    return ret_val; 
}

/******************************************************************************
 ******************************************************************************/

