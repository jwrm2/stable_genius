#include "../include/string"

#include "../include/cstdlib"
#include "../include/cstring"
#include "../include/istream"
#include "../include/ostream"

namespace NMSP {

/******************************************************************************
 ******************************************************************************/

void char_traits<char>::assign(char_traits<char>::char_type& r,
    const char_traits<char>::char_type& a)
{
    r = a;
}

char_traits<char>::char_type* char_traits<char>::assign(
    char_traits<char>::char_type* p,
    size_t count,
    char_traits<char>::char_type a)
{
    for(size_t i = 0; i < count; ++i)
        *(p + i) = a;

    return p;
}

/******************************************************************************/

char_traits<char>::char_type* char_traits<char>::move(
    char_traits<char>::char_type* dest, const char_traits<char>::char_type* src,
    size_t count)
{
    return static_cast<char_traits<char>::char_type*>(
        memmove(dest, src, count)
    );
}

/******************************************************************************/

char_traits<char>::char_type* char_traits<char>::copy(
    char_traits<char>::char_type* dest, const char_traits<char>::char_type* src,
    size_t count)
{
    return static_cast<char_traits<char>::char_type*>(memcpy(dest, src, count));
}

/******************************************************************************/

int char_traits<char>::compare(const char_traits<char>::char_type* s1,
    const char_traits<char>::char_type* s2, size_t count)
{
    return memcmp(s1, s2, count);
}

/******************************************************************************/

size_t char_traits<char>::length(const char_traits<char>::char_type* s)
{
    return strlen(s);
}

/******************************************************************************/

const char_traits<char>::char_type* char_traits<char>::find(
    const char_traits<char>::char_type* p,
    size_t count,
    const char_traits<char>::char_type& ch
)
{
    return static_cast<const char_traits<char>::char_type*>(
        memchr(p, ch, count)
    );
}

/******************************************************************************
 ******************************************************************************/

int stoi(const string& str, size_t* pos, int base)
{
    char* ptr;
    int retval = strtol(str.c_str(), &ptr, base);
    if (pos != nullptr)
        *pos = ptr - str.c_str();

    return retval;
}

/******************************************************************************/

long stol(const string& str, size_t* pos, int base)
{
    char* ptr;
    long retval = strtol(str.c_str(), &ptr, base);
    if (pos != nullptr)
        *pos = ptr - str.c_str();

    return retval;
}

/******************************************************************************/

long long stoll(const string& str, size_t* pos, int base)
{
    char* ptr;
    long long retval = strtoll(str.c_str(), &ptr, base);
    if (pos != nullptr)
        *pos = ptr - str.c_str();

    return retval;
}

/******************************************************************************/

unsigned int stoui(const string& str, size_t* pos, int base)
{
    char* ptr;
    unsigned int retval = strtoul(str.c_str(), &ptr, base);
    if (pos != nullptr)
        *pos = ptr - str.c_str();

    return retval;
}

/******************************************************************************/

unsigned long stoul(const string& str, size_t* pos, int base)
{
    char* ptr;
    unsigned long retval = strtoul(str.c_str(), &ptr, base);
    if (pos != nullptr)
        *pos = ptr - str.c_str();

    return retval;
}

/******************************************************************************/

unsigned long long stoull(const string& str, size_t* pos, int base)
{
    char* ptr;
    unsigned long long retval = strtoull(str.c_str(), &ptr, base);
    if (pos != nullptr)
        *pos = ptr - str.c_str();

    return retval;
}

/******************************************************************************/

float stof(const string& str, size_t* pos)
{
    char* ptr;
    float retval = strtof(str.c_str(), &ptr);
    if (pos != nullptr)
        *pos = ptr - str.c_str();

    return retval;
}

/******************************************************************************/

double stod(const string& str, size_t* pos)
{
    char* ptr;
    double retval = strtod(str.c_str(), &ptr);
    if (pos != nullptr)
        *pos = ptr - str.c_str();

    return retval;
}

/******************************************************************************/

long double stold(const string& str, size_t* pos)
{
    char* ptr;
    long double retval = strtold(str.c_str(), &ptr);
    if (pos != nullptr)
        *pos = ptr - str.c_str();

    return retval;
}

/******************************************************************************
 ******************************************************************************/

string to_string(char value)
{
    string ret_val;
    helper::strprintf(ret_val, "%c", value);
    return ret_val;
}

string to_string(signed char value)
{
    string ret_val;
    helper::strprintf(ret_val, "%c", value);
    return ret_val;
}

string to_string(int value)
{
    string ret_val;
    helper::strprintf(ret_val, "%d", value);
    return ret_val;
}

string to_string(long value)
{
    string ret_val;
    helper::strprintf(ret_val, "%ld", value);
    return ret_val;
}

string to_string(long long value)
{
    string ret_val;
    helper::strprintf(ret_val, "%lld", value);
    return ret_val;
}

string to_string(unsigned char value)
{
    string ret_val;
    helper::strprintf(ret_val, "%c", value);
    return ret_val;
}

string to_string(unsigned int value)
{
    string ret_val;
    helper::strprintf(ret_val, "%u", value);
    return ret_val;
}

string to_string(unsigned long value)
{
    string ret_val;
    helper::strprintf(ret_val, "%lu", value);
    return ret_val;
}

string to_string(unsigned long long value)
{
    string ret_val;
    helper::strprintf(ret_val, "%llu", value);
    return ret_val;
}

string to_string(float value)
{
    string ret_val;
    helper::strprintf(ret_val, "%g", value);
    return ret_val;
}

string to_string(double value)
{
    string ret_val;
    helper::strprintf(ret_val, "%g", value);
    return ret_val;
}

string to_string(long double value)
{
    string ret_val;
    helper::strprintf(ret_val, "%Lg", value);
    return ret_val;
}

/******************************************************************************
 ******************************************************************************/
/*
inline namespace literals {
inline namespace string_literals {
    klib::string operator""s(const char* str, size_t len)
    {
        return string(str, len);
    }
} // string_literals namespace
} // literals namespace
*/
/******************************************************************************
 ******************************************************************************/

#ifdef HOSTED_TEST
std::ostream& operator<<(std::ostream& os, const string& str)
{
    return (os << str.c_str());
} 
#endif /* HOSTED_TEST */

/******************************************************************************
 ******************************************************************************/

} // NMSP namespace
