#include <stdint.h>

#include "../include/cstdio"
#include "../include/algorithm"
#include "../include/cmath"
#include "../include/limits"
#include "../include/string"

#ifdef HOSTED_TEST
#include <cstdio>
#endif /* HOSTED_TEST */

#ifndef KLIB
#include "../include/fcntl.h"
#include "../include/fstream"
#include "../include/iostream"
#include "../include/unistd.h"
#endif /* KLIB not defined */

namespace NMSP {

// Negative fail value.
constexpr unsigned int fail_val = -1;

/******************************************************************************
 ******************************************************************************/

#ifndef KLIB
FILE* stdin;
FILE* stdout;
FILE* stderr;
#endif /* KLIB not defined */

/******************************************************************************
 ******************************************************************************/

#ifndef HOSTED_TEST
int printf(const char* format, ...)
{
    if (format == nullptr)
        return fail_val;

    // Initialise the variable argument list
    va_list vlist;
    va_start(vlist, format);
    // Pass on to vprintf, which is not implemented in klib.
    int ret_val = vprintf(format, vlist);
    // Clean up
    va_end(vlist);
    return ret_val;
}
#endif /* HOSTED_TEST */

/******************************************************************************
 ******************************************************************************/

int sprintf(char* buffer, const char* format, ...)
{
    if (format == nullptr || buffer == nullptr)
        return fail_val;

    // Initialise the variable argument list
    va_list vlist;
    va_start(vlist, format);
    // Pass on to vsprintf.
    int ret_val = vsprintf(buffer, format, vlist);
    // Clean up
    va_end(vlist);
    return ret_val;
}

/******************************************************************************/

#ifndef KLIB
// This is implemented in the kernel for klib, as it goes to the kernel log.
// Here it goes to stdout.
int vprintf(const char* format, va_list vlist)
{
    int ret_val = EOF;

    if (!format)
        return ret_val;

    if (cout)
    {
        // Perform the substitutions, then print to the destination.
        string msg;
        helper::vstrprintf(msg, format, vlist);

        cout << msg;
        if (cout)
            ret_val = msg.size();
    }

    return ret_val;
}
#endif /* KLIB defined */

/******************************************************************************/

int vsprintf(char* buffer, const char* format, va_list vlist)
{
    if (format == nullptr || buffer == nullptr)
        return fail_val;

    // Call vstrprintf
    string result;
    helper::vstrprintf(result, format, vlist);
    // Copy data from result to buffer. Need to include null terminator.
    char_traits<string::value_type>::copy(buffer, result.c_str(),
        result.size() + 1);
    
    return result.size();
}

/******************************************************************************
 ******************************************************************************/

int snprintf(char* buffer, size_t buf_size, const char* format, ...)
{
    if (format == nullptr || (buffer == nullptr && buf_size != 0))
        return fail_val;

    // Initialise the variable argument list
    va_list vlist;
    va_start(vlist, format);
    // Pass on to vsprintf.
    int ret_val = vsnprintf(buffer, buf_size, format, vlist);
    // Clean up
    va_end(vlist);
    return ret_val;
}

/******************************************************************************/

int vsnprintf(char* buffer, size_t buf_size, const char* format, va_list vlist)
{
    if (format == nullptr || (buffer == nullptr && buf_size != 0))
        return fail_val;

    // Call vstrprintf
    string result;
    helper::vstrprintf(result, format, vlist);
    // Copy data from result to buffer. Either the whole string, or the buffer
    // size, whichever is smaller.
    if (buffer != nullptr)
    {
        size_t data_size = min(buf_size, result.size() + 1);
        char_traits<string::value_type>::copy(buffer, result.c_str(),
            data_size - 1);
        char_traits<string::value_type>::assign(buffer[data_size - 1], '\0');
    }

    return result.size();
}

/******************************************************************************
 ******************************************************************************/

#ifndef KLIB
// Utility function to convert a C-style mode into a C++ openmode. Only required
// in user space.
static ios_base::openmode cmode_to_cxxmode(const char* m)
{
    if (strcmp(m, "r") == 0)
        return ios_base::in;
    else if (strcmp(m, "w") == 0)
        return ios_base::out;
    else if (strcmp(m, "a") == 0)
        return ios_base::app;
    else if (strcmp(m, "r+") == 0)
        return ios_base::in | ios_base::out;
    else if (strcmp(m, "w+") == 0)
        return ios_base::in | ios_base::out | ios_base::trunc;
    else if (strcmp(m, "a+") == 0)
        return ios_base::in | ios_base::app;
    else if (strcmp(m, "rb") == 0)
        return ios_base::in | ios_base::binary;
    else if (strcmp(m, "wb") == 0)
        return ios_base::out | ios_base::binary;
    else if (strcmp(m, "ab") == 0)
        return ios_base::app | ios_base::binary;
    else if (strcmp(m, "r+b") == 0)
        return ios_base::in | ios_base::out | ios_base::binary;
    else if (strcmp(m, "w+b") == 0)
        return
            ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary;
    else if (strcmp(m, "a+b") == 0)
        return ios_base::in | ios_base::app | ios_base::binary;

    // No match. Return 0, which is invalid.
    return static_cast<ios_base::openmode>(0);
}
#endif /* KLIB not defined */

/******************************************************************************
 ******************************************************************************/

#ifdef HOSTED_TEST
FILE* fopen(const char* filename, const char* mode)
{
    return std::fopen(filename, mode);
}

int fclose(FILE* stream)
{
    return std::fclose(stream);
}

size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream)
{
    return std::fwrite(buffer, size, count, stream);
}

size_t fread(void* buffer, size_t size, size_t count, FILE* stream)
{
    return std::fread(buffer, size, count, stream);
}

int fflush(FILE* stream)
{
    return std::fflush(stream);
}

int fgetpos(FILE* stream, fpos_t* pos)
{
    return std::fgetpos(stream, pos);
}

int fseek(FILE* stream, long offset, int origin)
{
    return std::fseek(stream, offset, origin);
}
#else /* HOSTED_TEST */
#ifndef KLIB
// fopen is implemented in the kernel, but we need an implementation here for
// user space.
FILE* fopen(const char* filename, const char* mode)
{
    // Defulat construct a file buffer and then open it.
    FILE* temp = new FILE {};
    FILE* ret =
        static_cast<FILE*>(temp->open(filename, cmode_to_cxxmode(mode)));
    if (ret == nullptr)
        delete temp;
    return ret;
}
#endif /* KLIB defined */
int fclose(FILE* stream)
{
// stream->close() returns an int in the kernel library, but a pointer to a FILE
// in the user library. Silly, I know, but FILE is rather different in the two.
#ifdef KLIB
    int ret_val = stream->close();
#else /* KLIB defined */
    int ret_val = (stream->close() == nullptr ? fail_val : 0);
#endif /* KLIB defined */
    delete stream;
    return ret_val;
}

size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream)
{
    return stream->write(buffer, size, count);
}

size_t fread(void* buffer, size_t size, size_t count, FILE* stream)
{
    return stream->read(buffer, size, count);
}

int fflush(FILE* stream)
{
    return stream->flush();
}

int fgetpos(FILE* stream, fpos_t* pos)
{
    return stream->getpos(pos);
}

int fseek(FILE* stream, long offset, int origin)
{
    return stream->seek(offset, origin);
} 
#endif /* HOSTED_TEST */

/******************************************************************************
 ******************************************************************************/

#ifdef KLIB
// File only exists for the kernel. In user space, FILE is a wrapper around
// std::basic_filebuf<char>.

File::File(const char* mode) :
    buffered {false},
    buffer {nullptr},
    reading {false},
    writing {false},
    append {false},
    binary {false},
    eof {false},
    position {0}
{
    string m {mode};

    if (m == "r")
        reading = true;
    else if (m == "w")
        writing = true;
    else if (m == "a")
    {
        writing = true;
        append = true;
    }
    else if (m == "r+")
    {
        reading = true;
        writing = true;
    }
    else if (m == "w+")
    {
        reading = true;
        writing = true;
    }
    else if (m == "a+")
    {
        reading = true;
        writing = true;
        append = true;
    }
    else if (m == "rb")
    {
        reading = true;
        binary = true;
    }
    else if (m == "wb")
    {
        writing = true;
        binary = true;
    }
    else if (m == "ab")
    {
        writing = true;
        append = true;
        binary = true;
    }
    else if (m == "r+b")
    {
        reading = true;
        writing = true;
        binary = true;
    }
    else if (m == "w+b")
    {
        reading = true;
        writing = true;
        binary = true;
    }
    else if (m == "a+b")
    {
        reading = true;
        writing = true;
        append = true;
        binary = true;
    }
    // If not any of the above, everything remains false.
}

/******************************************************************************/

int File::close()
{
    int ret_val = File::flush();

    // Free the buffer.
    delete[] buffer;
    buffer = nullptr;

    // Set flags so future operations fail.
    reading = false;
    writing = false;

    return ret_val;
}

#endif /* KLIB defined */

/******************************************************************************
 ******************************************************************************/

namespace helper {

string itostr(signed long long value, unsigned int radix)
{
    string rev_str;

    if (radix < 2 || radix > 36)
        return rev_str;

    static char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    bool neg = false;
    // Special case for the most negative possible value, because that doesn't
    // have a positive equivalent. We'll divide by the radix, then add the final
    // digit back manually.
    bool neg_min = false;
    long long final_digit = 0;
    if (value == numeric_limits<long long>::min())
    {
        neg = true;
        neg_min = true;
        final_digit = (value / radix) * radix - value;
        value = - (value / radix);
    }

    // Get the sign, then make sure value is positive
    if (value < 0)
    {
        neg = true;
        value = -value;
    }

    // Generate digits in reverse order
    if (value == 0)
        rev_str = "0";
    else
    {
        while (value)
        {
            rev_str.push_back(digits[value % radix]);
            value /= radix;
        }
    }

    // Deal with the negative minimum. We need insert the final digit at the
    // front.
    if (neg_min)
        rev_str.insert(0, itostr(final_digit, radix));

    // Include sign
    if (neg)
        rev_str.push_back('-');

    // Reverse digits.
    reverse(rev_str.begin(), rev_str.end());
    return rev_str;
}

/******************************************************************************/

string uitostr(unsigned long long value, unsigned int radix)
{
    string rev_str;

    if (radix < 2 || radix > 36)
        return rev_str;

    static char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Generate digits in reverse order
    int i;
    if (value == 0)
    {
        rev_str = "0";
        i = 1;
    }
    else
    {
        for (i = 0; value; ++i)
        {
            rev_str.push_back(digits[value % radix]);
            value /= radix;
        }
    }

    // Reverse digits.
    reverse(rev_str.begin(), rev_str.end());
    return rev_str;
}

/******************************************************************************/

string dtostr(long double input, char style)
{
    string ret_str;

    const int max_dp = 14;
    const int radix = 10;

    if (isnan(input))
    {
        return string {"NaN"};
    }
    else if (input == numeric_limits<long double>::infinity())
    {
        return string {"INF"};
    }
    else if (input == -numeric_limits<long double>::infinity())
    {
        return string {"-INF"};
    }

    if (input < 0.0)
    {
        ret_str = "-";
        input = -input;
    }

    if (style != 'e' && style != 'f')
    {
        if (input == 0)
            return string {'0'};
        else if (fabs(input) >= 1000000.0 || fabs(input) < 0.001)
            style = 'e';
        else
            style = 'f';
    }

    if (style == 'e')
    {
        if (input == 0)
            return string {"0E0"};

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

        string number_part = itostr(static_cast<long long int>(input), radix);
        string exponent_part = itostr(exponent, radix);

        input = fabs(input - trunc(input));

        if (input != 0)
        {
            for (int i = max_dp; i > 0; --i)
                input *= radix;
            while (fmod(input, radix) < 1)
                input /= radix;
        }
        string decimal_part =
            itostr(static_cast<long long int>(round(input)), radix);

        ret_str.append(number_part + "." + decimal_part + "E" + exponent_part);
    }
    else
    {
        if (input == 0)
            return string {'0'};

        string number_part = itostr(static_cast<long long int>(input), radix);

        input = fabs(input - trunc(input));
        int pow = 0;
        for (int i = max_dp; i > 0; --i)
        {
            input *= radix;
            if (input < 1 && input != 0)
                ++pow;
        }
        while (fmod(input, radix) < 1 && input != 0)
            input /= radix;
        string decimal_part =
            itostr(static_cast<long long int>(round(input)), radix);

        ret_str.append(number_part + ".");

        for (; pow > 0; --pow)
            ret_str.append("0");

        ret_str.append(decimal_part); 
    }

    return ret_str;
}

/******************************************************************************
 ******************************************************************************/

void strprintf(string& str, const char* format, ...)
{
    if (format == nullptr)
    {
        str = string{};
        return;
    }

    // Initialise the variable argument list
    va_list vlist;
    va_start(vlist, format);
    // Pass on to vsprintf.
    vstrprintf(str, format, vlist);
    // Clean up
    va_end(vlist);
}

/******************************************************************************/

void vstrprintf(string& str, const char* format, va_list vlist)
{
    // Wipe the output string.
    str.clear();

    // Loop over the characters in the format string.
    for(const char* traverse = format; *traverse; ++traverse) 
    { 
        if (*traverse != '%')
        {
            str.push_back(*traverse);
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
                char c = static_cast<char>(va_arg(vlist, int));
                str.push_back(c);
                in_format = false;
                break;
            }
            // Signed decimal integer
            case 'd': case 'i':
            {
                if (long_long_format)
                {
                    long long int i = va_arg(vlist, long long int);
                    str.append(helper::itostr(i, 10));
                }
                else if (long_format)
                {
                    long int i = va_arg(vlist, long int);
                    str.append(helper::itostr(i, 10));
                }
                else
                {
                    int i = va_arg(vlist, int);
                    str.append(helper::itostr(i, 10));
                }
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
                if (long_long_format)
                {
                    unsigned long long int i = 
                        va_arg(vlist, unsigned long long int);
                    str.append(helper::uitostr(i, 10));
                }
                else if (long_format)
                {
                    unsigned long int i = va_arg(vlist, unsigned long int);
                    str.append(helper::uitostr(i, 10));
                }
                else
                {
                    unsigned int i = va_arg(vlist, unsigned int);
                    str.append(helper::uitostr(i, 10));
                }
                in_format = false;
                break;
            }
            // Unsigned octal integer
            case 'o':
            {
                str.push_back('0');
                if (long_long_format)
                {
                    unsigned long long int i = 
                        va_arg(vlist, unsigned long long int);
                    str.append(helper::uitostr(i, 8));
                }
                else if (long_format)
                {
                    unsigned long int i = va_arg(vlist, unsigned long int);
                    str.append(helper::uitostr(i, 8));
                }
                else
                {
                    unsigned int i = va_arg(vlist, unsigned int);
                    str.append(helper::uitostr(i, 8));
                }
                in_format = false;
                break;
            }
            // Unsigned hexadecimal integer
            case 'X':
            {
                str.append("0x");
                if (long_long_format)
                {
                    unsigned long long int i = 
                        va_arg(vlist, unsigned long long int);
                    str.append(helper::uitostr(i, 16));
                }
                else if (long_format)
                {
                    unsigned long int i = va_arg(vlist, unsigned long int);
                    str.append(helper::uitostr(i, 16));
                }
                else
                {
                    unsigned int i = va_arg(vlist, unsigned int);
                    str.append(helper::uitostr(i, 16));
                };
                in_format = false;
                break;
            }
            // C-style string
            case 's':
            {
                const char* s = va_arg(vlist, const char *);
                str.append(s);
                in_format = false;
                break;
            }
            // Pointer
            case 'p':
            {
                str.append("0x");
                void* ptr = va_arg(vlist, void*);
                uintptr_t i = reinterpret_cast<uintptr_t>(ptr);
                str.append(helper::itostr(i, 16));
                in_format = false;
                break;
            }
            // Fixed point double
            case 'f':
            {
                if (long_double_format)
                {
                    long double d = va_arg(vlist, long double);
                    str.append(helper::dtostr(d, 'f'));
                }
                else
                {
                    double d = va_arg(vlist, double);
                    str.append(helper::dtostr(d, 'f'));
                }
                in_format = false;
                long_double_format = false;
                break;
            }
            // Exponential double
            case 'e':
            {
                if (long_double_format)
                {
                    long double d = va_arg(vlist, long double);
                    str.append(helper::dtostr(d, 'e'));
                }
                else
                {
                    double d = va_arg(vlist, double);
                    str.append(helper::dtostr(d, 'e'));
                }
                in_format = false;
                long_double_format = false;
                break;
            }
            // Fixed point double
            case 'g':
            {
                if (long_double_format)
                {
                    long double d = va_arg(vlist, long double);
                    str.append(helper::dtostr(d, 'g'));
                }
                else
                {
                    double d = va_arg(vlist, double);
                    str.append(helper::dtostr(d, 'g'));
                }
                in_format = false;
                long_double_format = false;
                break;
            }
            // Literal %
            case '%':
            {
                str.push_back('%');
                in_format = false;
                break;
            }
            // Unrecognised format
            default:
            {
                str.append("Bad Format");
                in_format = false;
            }
            }
        } 
    } 
}

} // end helper namespace

/******************************************************************************
 ******************************************************************************/

} // end klib namespace
