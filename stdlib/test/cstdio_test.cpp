#include <iostream>

#include "../include/cstdio"

#include "../include/cstring"
#include "../include/string"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // sprintf
    // All the format tests go here. The actual conversion methods are tested
    // elsewhere.
    {
        char result[256];
        int length;
        // No formatting
        length = klib::sprintf(result, "Hello");
        if (strcmp(result, "Hello") || length != 5)
        {
            ++fail_count;
            std::cout << "FAILED sprintf no format" << '\n';
        }
        // char format
        char c1 = 'l';
        length = klib::sprintf(result, "He%c%co", c1, c1);
        if (strcmp(result, "Hello") || length != 5)
        {
            ++fail_count;
            std::cout << "FAILED sprintf char format" << '\n';
        }
        // Signed decimal integer
        int i1 = 10;
        int i2 = 30;
        length = klib::sprintf(result, "Hello %d %i", i1, i2);
        if (strcmp(result, "Hello 10 30") || length != 11)
        {
            ++fail_count;
            std::cout << "FAILED sprintf signed decimal int format" << '\n';
        }
        // Signed decimal long
        long l1 = 10;
        long l2 = 30;
        length = klib::sprintf(result, "Hello %ld %li", l1, l2);
        if (strcmp(result, "Hello 10 30") || length != 11)
        {
            ++fail_count;
            std::cout << "FAILED sprintf signed decimal long format" << '\n';
        }
        // Signed decimal long long
        long long ll1 = 10;
        long long ll2 = 30;
        length = klib::sprintf(result, "Hello %lld %lli", ll1, ll2);
        if (strcmp(result, "Hello 10 30") || length != 11)
        {
            ++fail_count;
            std::cout << "FAILED sprintf signed decimal long long format" << '\n';
        }
        // Unsigned decimal integer
        unsigned int ui1 = 10;
        unsigned int ui2 = 30;
        length = klib::sprintf(result, "Hello %u %u", ui1, ui2);
        if (strcmp(result, "Hello 10 30") || length != 11)
        {
            ++fail_count;
            std::cout << "FAILED sprintf unsigned decimal int format" << '\n';
        }
        // Unsigned decimal long
        unsigned long ul1 = 10;
        unsigned long ul2 = 30;
        length = klib::sprintf(result, "Hello %lu %lu", ul1, ul2);
        if (strcmp(result, "Hello 10 30")|| length != 11)
        {
            ++fail_count;
            std::cout << "FAILED sprintf unsigned decimal long format" << '\n';
        }
        // Unsigned decimal long long
        unsigned long long ull1 = 10;
        unsigned long long ull2 = 30;
        length = klib::sprintf(result, "Hello %llu %llu", ull1, ull2);
        if (strcmp(result, "Hello 10 30") || length != 11)
        {
            ++fail_count;
            std::cout << "FAILED sprintf unsigned decimal long long format" << '\n';
        }
        // Unsigned octal integer
        unsigned int ui3 = 10;
        unsigned int ui4 = 30;
        length = klib::sprintf(result, "Hello %o %o", ui3, ui4);
        if (strcmp(result, "Hello 012 036") || length != 13)
        {
            ++fail_count;
            std::cout << "FAILED sprintf unsigned octal int format" << '\n';
        }
        // Unsigned octal long
        unsigned long ul3 = 10;
        unsigned long ul4 = 30;
        length = klib::sprintf(result, "Hello %lo %lo", ul3, ul4);
        if (strcmp(result, "Hello 012 036") || length != 13)
        {
            ++fail_count;
            std::cout << "FAILED sprintf unsigned octal long format" << '\n';
        }
        // Unsigned octal long long
        unsigned long long ull3 = 10;
        unsigned long long ull4 = 30;
        length = klib::sprintf(result, "Hello %llo %llo", ull3, ull4);
        if (strcmp(result, "Hello 012 036") || length != 13)
        {
            ++fail_count;
            std::cout << "FAILED sprintf unsigned octal long long format" << '\n';
        }
        // Unsigned hexadecimal integer
        unsigned int ui5 = 10;
        unsigned int ui6 = 30;
        length = klib::sprintf(result, "Hello %X %X", ui5, ui6);
        if (strcmp(result, "Hello 0xA 0x1E") || length != 14)
        {
            ++fail_count;
            std::cout << "FAILED sprintf unsigned hexadecimal int format" << '\n';
        }
        // Unsigned hexadecimal long
        unsigned long ul5 = 10;
        unsigned long ul6 = 30;
        length = klib::sprintf(result, "Hello %lX %lX", ul5, ul6);
        if (strcmp(result, "Hello 0xA 0x1E") || length != 14)
        {
            ++fail_count;
            std::cout << "FAILED sprintf unsigned hexadecimal long format" << '\n';
        }
        // Unsigned hexadecimal long long
        unsigned long long ull5 = 10;
        unsigned long long ull6 = 30;
        length = klib::sprintf(result, "Hello %llX %llX", ull5, ull6);
        if (strcmp(result, "Hello 0xA 0x1E") || length != 14)
        {
            ++fail_count;
            std::cout << "FAILED sprintf unsigned hexadecimal long long format" << '\n';
        }
        // C string
        const char s1[] = "world";
        const char s2[] = "goodbye";
        length = klib::sprintf(result, "Hello %s %s", s1, s2);
        if (strcmp(result, "Hello world goodbye") || length != 19)
        {
            ++fail_count;
            std::cout << "FAILED sprintf char* format" << '\n';
        }
        // Pointer
        void* p1 = reinterpret_cast<void*>(0xDEADBEEF);
        void* p2 = reinterpret_cast<void*>(0xCAFEBABE);
        length = klib::sprintf(result, "Hello %p %p", p1, p2);
        if (strcmp(result, "Hello 0xDEADBEEF 0xCAFEBABE") || length != 27)
        {
            ++fail_count;
            std::cout << "FAILED sprintf pointer format" << '\n';
        }
        // Fixed point double
        double d1 = 123456789.01234500110149;
        double d2 = -0.0000001234;
        length = klib::sprintf(result, "Hello %f %f", d1, d2);
        if (strcmp(result, "Hello 123456789.01234500110149 -0.0000001234") ||
            length != 44)
        {
            ++fail_count;
            std::cout << "FAILED sprintf fixed double format" << '\n';
        }
        // Fixed point long double
        long double ld1 = 123456789.01234500110149;
        long double ld2 = -0.0000001234;
        length = klib::sprintf(result, "Hello %Lf %Lf", ld1, ld2);
        if (strcmp(result, "Hello 123456789.01234500110149 -0.0000001234") ||
            length != 44)
        {
            ++fail_count;
            std::cout << "FAILED sprintf fixed long double format" << '\n';
        }
        // Exponential double
        double d3 = 123456789.012345;
        double d4 = -0.0000001234;
        length = klib::sprintf(result, "Hello %e %e", d3, d4);
        if (strcmp(result, "Hello 1.23456789012345E8 -1.234E-7") ||
            length != 34)
        {
            ++fail_count;
            std::cout << "FAILED sprintf exponential double format" << '\n';
        }
        // Exponential long double
        long double ld3 = 123456789.012345;
        long double ld4 = -0.0000001234;
        length = klib::sprintf(result, "Hello %Le %Le", ld3, ld4);
        if (strcmp(result, "Hello 1.23456789012345E8 -1.234E-7") ||
            length != 34)
        {
            ++fail_count;
            std::cout << "FAILED sprintf exponential long double format" << '\n';
        }
        // General double
        double d5 = 123456.78901234500518;
        double d6 = -0.0000001234;
        length = klib::sprintf(result, "Hello %g %g", d5, d6);
        if (strcmp(result, "Hello 123456.78901234500518 -1.234E-7") ||
            length != 37)
        {
            ++fail_count;
            std::cout << "FAILED sprintf general double format" << '\n';
        }
        // General long double
        long double ld5 = 123456.78901234500518;
        long double ld6 = -0.0000001234;
        length = klib::sprintf(result, "Hello %Lg %Lg", ld5, ld6);
        if (strcmp(result, "Hello 123456.78901234500518 -1.234E-7") ||
            length != 37)
        {
            ++fail_count;
            std::cout << "FAILED sprintf general long double format" << '\n';
        }
        // Literal %
        length = klib::sprintf(result, "Hello %%%%");
        if (strcmp(result, "Hello %%") || length != 8)
        {
            ++fail_count;
            std::cout << "FAILED sprintf literal %% format" << '\n';
        }
        // Bad format
        length = klib::sprintf(result, "Hello %z %j");
        if (strcmp(result, "Hello Bad Format Bad Format") || length != 27)
        {
            ++fail_count;
            std::cout << "FAILED sprintf bad format" << '\n';
        }
        // nullptr
        length = klib::sprintf(nullptr, nullptr);
        if (length != -1)
        {
            ++fail_count;
            std::cout << "FAILED sprintf nullptr" << '\n';
        }
    }
    // snprintf
    // We'll assume that the format is sufficently tested by sprintf (they're
    // using the same function for it) and just test what's different.
    {
        char result[256];
        int length;
        // Long enough
        length = klib::snprintf(result, 10, "Hello");
        if (strcmp(result, "Hello") || length != 5)
        {
            ++fail_count;
            std::cout << "FAILED snprintf long enough" << '\n';
        }
        // Just long enough
        length = klib::snprintf(result, 6, "Hello");
        if (strcmp(result, "Hello") || length != 5)
        {
            ++fail_count;
            std::cout << "FAILED snprintf just long enough" << '\n';
        }
        // Not long enough
        length = klib::snprintf(result, 3, "Hello");
        if (strcmp(result, "He") || length != 5)
        {
            ++fail_count;
            std::cout << "FAILED snprintf not long enough" << '\n';
        }
        // nullptr input
        length = klib::snprintf(result, 10, nullptr);
        if (length != -1)
        {
            ++fail_count;
            std::cout << "FAILED snprintf nullptr input" << '\n';
        }
        // nullptr output
        length = klib::snprintf(nullptr, 10, "Hello");
        if (length != -1)
        {
            ++fail_count;
            std::cout << "FAILED snprintf nullptr output" << '\n';
        }
        // nullptr output, length zero
        length = klib::snprintf(nullptr, 0, "Hello");
        if (length != 5)
        {
            ++fail_count;
            std::cout << "FAILED snprintf nullptr output, length zero" << '\n';
        }
    }
    // itostr
    {
        // Simple int
        if (helper::itostr(123, 10) != "123")
        {
            ++fail_count;
            std::cout << "FAILED itostr simple int" << '\n';
        }
        // Negative int
        if (helper::itostr(-123, 10) != "-123")
        {
            ++fail_count;
            std::cout << "FAILED itostr negative int" << '\n';
        }
        // Zero int
        if (helper::itostr(0, 10) != "0")
        {
            ++fail_count;
            std::cout << "FAILED itostr zero int" << '\n';
        }
        // Simple hexadecimal int
        if (helper::itostr(123, 16) != "7B")
        {
            ++fail_count;
            std::cout << "FAILED itostr simple hexadecimal int" << '\n';
        }
        // Negative octal int
        if (helper::itostr(-123, 8) != "-173")
        {
            ++fail_count;
            std::cout << "FAILED itostr negative octal int" << '\n';
        }
        // Simple long
        if (helper::itostr(123l, 10) != "123")
        {
            ++fail_count;
            std::cout << "FAILED itostr simple long" << '\n';
        }
        // Negative long
        if (helper::itostr(-123l, 10) != "-123")
        {
            ++fail_count;
            std::cout << "FAILED itostr negative long" << '\n';
        }
        // Zero long
        if (helper::itostr(0l, 10) != "0")
        {
            ++fail_count;
            std::cout << "FAILED itostr zero long" << '\n';
        }
        // Simple hexadecimal long
        if (helper::itostr(123l, 16) != "7B")
        {
            ++fail_count;
            std::cout << "FAILED itostr simple hexadecimal long" << '\n';
        }
        // Negative octal long
        if (helper::itostr(-123l, 8) != "-173")
        {
            ++fail_count;
            std::cout << "FAILED itostr negative octal long" << '\n';
        }
        // Simple long long
        if (helper::itostr(123ll, 10) != "123")
        {
            ++fail_count;
            std::cout << "FAILED itostr simple long long" << '\n';
        }
        // Negative long long
        if (helper::itostr(-123ll, 10) != "-123")
        {
            ++fail_count;
            std::cout << "FAILED itostr negative long long" << '\n';
        }
        // Zero long long
        if (helper::itostr(0ll, 10) != "0")
        {
            ++fail_count;
            std::cout << "FAILED itostr zero long long" << '\n';
        }
        // Simple hexadecimal long long
        if (helper::itostr(123ll, 16) != "7B")
        {
            ++fail_count;
            std::cout << "FAILED itostr simple hexadecimal long long" << '\n';
        }
        // Negative octal long long
        if (helper::itostr(-123ll, 8) != "-173")
        {
            ++fail_count;
            std::cout << "FAILED itostr negative octal long long" << '\n';
        }
        // Bad base, too small
        if (helper::itostr(123, 1) != "")
        {
            ++fail_count;
            std::cout << "FAILED itostr bad base, too small" << '\n';
        }
        // Bad base, too big
        if (helper::itostr(123, 37) != "")
        {
            ++fail_count;
            std::cout << "FAILED itostr bad base, too small" << '\n';
        }
        // Bad base, negative
        if (helper::itostr(123, -1) != "")
        {
            ++fail_count;
            std::cout << "FAILED itostr bad base, negative" << '\n';
        }
    }
    // uitostr
    {
        // Simple int
        if (helper::uitostr(123, 10) != "123")
        {
            ++fail_count;
            std::cout << "FAILED uitostr simple int" << '\n';
        }
        // Zero int
        if (helper::uitostr(0, 10) != "0")
        {
            ++fail_count;
            std::cout << "FAILED uitostr zero int" << '\n';
        }
        // Simple hexadecimal int
        if (helper::uitostr(123, 16) != "7B")
        {
            ++fail_count;
            std::cout << "FAILED uitostr simple hexadecimal int" << '\n';
        }
        // Simple long
        if (helper::uitostr(123l, 10) != "123")
        {
            ++fail_count;
            std::cout << "FAILED uitostr simple long" << '\n';
        }
        // Zero long
        if (helper::uitostr(0l, 10) != "0")
        {
            ++fail_count;
            std::cout << "FAILED uitostr zero long" << '\n';
        }
        // Simple hexadecimal long
        if (helper::uitostr(123l, 16) != "7B")
        {
            ++fail_count;
            std::cout << "FAILED uitostr simple hexadecimal long" << '\n';
        }
        // Simple long long
        if (helper::uitostr(123ll, 10) != "123")
        {
            ++fail_count;
            std::cout << "FAILED uitostr simple long long" << '\n';
        }
        // Zero long long
        if (helper::uitostr(0ll, 10) != "0")
        {
            ++fail_count;
            std::cout << "FAILED uitostr zero long long" << '\n';
        }
        // Simple hexadecimal long long
        if (helper::uitostr(123ll, 16) != "7B")
        {
            ++fail_count;
            std::cout << "FAILED uitostr simple hexadecimal long long" << '\n';
        }
        // Bad base, too small
        if (helper::uitostr(123, 1) != "")
        {
            ++fail_count;
            std::cout << "FAILED uitostr bad base, too small" << '\n';
        }
        // Bad base, too big
        if (helper::uitostr(123, 37) != "")
        {
            ++fail_count;
            std::cout << "FAILED uitostr bad base, too small" << '\n';
        }
        // Bad base, negative
        if (helper::uitostr(123, -1) != "")
        {
            ++fail_count;
            std::cout << "FAILED uitostr bad base, negative" << '\n';
        }
    }
    // dtostr
    {
        // fixed small positive float
        if (helper::dtostr(0.00000320000004f, 'f') != "0.00000320000004")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed small positive float" << '\n';
        }
        // fixed normal positive float
        if (helper::dtostr(123.32099914550781f, 'f') != "123.32099914550781")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed normal positive float" << '\n';
        }
        // fixed large positive float
        if (helper::dtostr(123456792.0f, 'f') != "123456792.0")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed large positive float" << '\n';
        }
        // fixed small negative float
        if (helper::dtostr(-0.00000320000004f, 'f') != "-0.00000320000004")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed small negative float" << '\n';
        }
        // fixed normal negative float
        if (helper::dtostr(-123.32099914550781f, 'f') != "-123.32099914550781")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed normal negative float" << '\n';
        }
        // fixed large negative float
        if (helper::dtostr(-123456792.0f, 'f') != "-123456792.0")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed large negative float" << '\n';
        }
        // exp small positive float
        if (helper::dtostr(0.00000320000003739551f, 'e') != "3.20000003739551E-6")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp small positive float" << '\n';
        }
        // exp normal positive float
        if (helper::dtostr(123.320999145508f, 'e') != "1.23320999145508E2")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp normal positive float" << '\n';
        }
        // exp large positive float
        if (helper::dtostr(123456792.0f, 'e') != "1.23456792E8")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp large positive float" << '\n';
        }
        // exp small negative float
        if (helper::dtostr(-0.00000320000003739551f, 'e') != "-3.20000003739551E-6")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp small negative float" << '\n';
        }
        // exp normal negative float
        if (helper::dtostr(-123.320999145508f, 'e') != "-1.23320999145508E2")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp normal negative float" << '\n';
        }
        // exp large negative float
        if (helper::dtostr(-123456792.0f, 'e') != "-1.23456792E8")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp large negative float" << '\n';
        }
        // general small positive float
        if (helper::dtostr(0.00000320000003739551f, 'g') != "3.20000003739551E-6")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general small positive float" << '\n';
        }
        // general normal positive float
        if (helper::dtostr(123.32099914550781f, 'g') != "123.32099914550781")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general normal positive float" << '\n';
        }
        // general large positive float
        if (helper::dtostr(123456792.0f, 'g') != "1.23456792E8")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general large positive float" << '\n';
        }
        // general small negative float
        if (helper::dtostr(-0.00000320000003739551f, 'g') != "-3.20000003739551E-6")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general small negative float" << '\n';
        }
        // general normal negative float
        if (helper::dtostr(-123.32099914550781f, 'g') != "-123.32099914550781")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general normal negative float" << '\n';
        }
        // general large negative float
        if (helper::dtostr(-123456792.0f, 'g') != "-1.23456792E8")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general large negative float" << '\n';
        }
        // fixed small positive double
        if (helper::dtostr(0.00000320000004, 'f') != "0.00000320000004")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed small positive double" << '\n';
        }
        // fixed normal positive double
        if (helper::dtostr(123.32099914550781, 'f') != "123.32099914550781")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed normal positive double" << '\n';
        }
        // fixed large positive double
        if (helper::dtostr(123456792.0, 'f') != "123456792.0")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed large positive double" << '\n';
        }
        // fixed small negative double
        if (helper::dtostr(-0.00000320000004, 'f') != "-0.00000320000004")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed small negative double" << '\n';
        }
        // fixed normal negative double
        if (helper::dtostr(-123.32099914550781, 'f') != "-123.32099914550781")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed normal negative double" << '\n';
        }
        // fixed large negative double
        if (helper::dtostr(-123456792.0, 'f') != "-123456792.0")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed large negative double" << '\n';
        }
        // exp small positive double
        if (helper::dtostr(0.00000320000003739551, 'e') != "3.20000003739551E-6")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp small positive double" << '\n';
        }
        // exp normal positive double
        if (helper::dtostr(123.320999145508, 'e') != "1.23320999145508E2")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp normal positive double" << '\n';
        }
        // exp large positive double
        if (helper::dtostr(123456792.0, 'e') != "1.23456792E8")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp large positive double" << '\n';
        }
        // exp small negative double
        if (helper::dtostr(-0.00000320000003739551, 'e') != "-3.20000003739551E-6")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp small negative double" << '\n';
        }
        // exp normal negative double
        if (helper::dtostr(-123.320999145508, 'e') != "-1.23320999145508E2")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp normal negative double" << '\n';
        }
        // exp large negative double
        if (helper::dtostr(-123456792.0, 'e') != "-1.23456792E8")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp large negative double" << '\n';
        }
        // general small positive double
        if (helper::dtostr(0.00000320000003739551, 'g') != "3.20000003739551E-6")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general small positive double" << '\n';
        }
        // general normal positive double
        if (helper::dtostr(123.32099914550781, 'g') != "123.32099914550781")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general normal positive double" << '\n';
        }
        // general large positive double
        if (helper::dtostr(123456792.0, 'g') != "1.23456792E8")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general large positive double" << '\n';
        }
        // general small negative double
        if (helper::dtostr(-0.00000320000003739551, 'g') != "-3.20000003739551E-6")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general small negative double" << '\n';
        }
        // general normal negative double
        if (helper::dtostr(-123.32099914550781, 'g') != "-123.32099914550781")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general normal negative double" << '\n';
        }
        // general large negative double
        if (helper::dtostr(-123456792.0, 'g') != "-1.23456792E8")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general large negative double" << '\n';
        }
        // fixed small positive long double
        if (helper::dtostr(0.00000320000004, 'f') != "0.00000320000004")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed small positive long double" << '\n';
        }
        // fixed normal positive long double
        if (helper::dtostr(123.32099914550781, 'f') != "123.32099914550781")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed normal positive long double" << '\n';
        }
        // fixed large positive long double
        if (helper::dtostr(123456792.0, 'f') != "123456792.0")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed large positive long double" << '\n';
        }
        // fixed small negative long double
        if (helper::dtostr(-0.00000320000004, 'f') != "-0.00000320000004")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed small negative long double" << '\n';
        }
        // fixed normal negative long double
        if (helper::dtostr(-123.32099914550781, 'f') != "-123.32099914550781")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed normal negative long double" << '\n';
        }
        // fixed large negative long double
        if (helper::dtostr(-123456792.0, 'f') != "-123456792.0")
        {
            ++fail_count;
            std::cout << "FAILED dtostr fixed large negative long double" << '\n';
        }
        // exp small positive long double
        if (helper::dtostr(0.00000320000003739551, 'e') != "3.20000003739551E-6")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp small positive long double" << '\n';
        }
        // exp normal positive long double
        if (helper::dtostr(123.320999145508, 'e') != "1.23320999145508E2")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp normal positive long double" << '\n';
        }
        // exp large positive long double
        if (helper::dtostr(123456792.0, 'e') != "1.23456792E8")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp large positive long double" << '\n';
        }
        // exp small negative long double
        if (helper::dtostr(-0.00000320000003739551, 'e') != "-3.20000003739551E-6")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp small negative long double" << '\n';
        }
        // exp normal negative long double
        if (helper::dtostr(-123.320999145508, 'e') != "-1.23320999145508E2")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp normal negative long double" << '\n';
        }
        // exp large negative long double
        if (helper::dtostr(-123456792.0, 'e') != "-1.23456792E8")
        {
            ++fail_count;
            std::cout << "FAILED dtostr exp large negative long double" << '\n';
        }
        // general small positive long double
        if (helper::dtostr(0.00000320000003739551, 'g') != "3.20000003739551E-6")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general small positive long double" << '\n';
        }
        // general normal positive long double
        if (helper::dtostr(123.32099914550781, 'g') != "123.32099914550781")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general normal positive long double" << '\n';
        }
        // general large positive long double
        if (helper::dtostr(123456792.0, 'g') != "1.23456792E8")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general large positive long double" << '\n';
        }
        // general small negative long double
        if (helper::dtostr(-0.00000320000003739551, 'g') != "-3.20000003739551E-6")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general small negative long double" << '\n';
        }
        // general normal negative long double
        if (helper::dtostr(-123.32099914550781, 'g') != "-123.32099914550781")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general normal negative long double" << '\n';
        }
        // general large negative long double
        if (helper::dtostr(-123456792.0, 'g') != "-1.23456792E8")
        {
            ++fail_count;
            std::cout << "FAILED dtostr general large negative long double" << '\n';
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
