#include <iostream>
#include <iomanip>

#include "../include/cmath"
#include "../include/cstdlib"
#include "../include/limits"
#include "../include/string"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // strtol
    {
        long l1 = klib::strtol("123", nullptr, 10);
        if (l1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 10) positive no spaces\n";
        }
        long l2 = klib::strtol("-123", nullptr, 10);
        if (l2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 10) negative no spaces\n";
        }
        long l3 = klib::strtol("   123 ", nullptr, 10);
        if (l3 != 123)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 10) positive with spaces\n";
        }
        long l4 = klib::strtol("   -123 ", nullptr, 10);
        if (l4 != -123)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 10) negative with spaces\n";
        }
        long l5 = klib::strtol("0xA6", nullptr, 0);
        if (l5 != 166)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 0) assumed hex\n";
        }
        long l6 = klib::strtol("076", nullptr, 0);
        if (l6 != 62)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 0) assumed octal\n";
        }
        long l7 = klib::strtol("123", nullptr, 0);
        if (l7 != 123)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 0) assumed decimal\n";
        }
        long l8 = klib::strtol("0xA6", nullptr, 16);
        if (l8 != 166)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 16) explicit hex\n";
        }
        long l9 = klib::strtol("076", nullptr, 8);
        if (l9 != 62)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 8) explicit octal\n";
        }
        char* st1;
        const char* c1 = "07697";
        long l10 = klib::strtol(c1, &st1, 8);
        if (l10 != 62 || (st1 - c1) != 3)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, char**, 8) octal with bad digit\n";
        }
        char* st2;
        const char* c2 = "123";
        long l11 = klib::strtol(c2, &st2, 10);
        if (l11 != 123 || (st2 - c2) != 3)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, char**, 10) simple digit count\n";
        }
        char* st3;
        const char* c3 = "   123a  ";
        long l12 = klib::strtol(c3, &st3, 10);
        if (l12 != 123 || (st3 - c3) != 6)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, char**, 10) digit count with spaces\n";
        }
        string s1 = to_string(numeric_limits<long>::max()) + "0";
        long l13 = klib::strtol(s1.c_str(), nullptr, 10);
        if (l13 != numeric_limits<long>::max())
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 10) long positive overflow\n";
        }
        string s2 = to_string(numeric_limits<long>::min()) + "0";
        long l14 = klib::strtol(s2.c_str(), nullptr, 10);
        if (l14 != numeric_limits<long>::min())
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 10) long negative overflow\n";
        }
        string s3 = to_string(numeric_limits<long long>::max()) + "0";
        long l15 = klib::strtol(s3.c_str(), nullptr, 10);
        if (l15 != numeric_limits<long>::max())
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 10) long long positive overflow\n";
        }
        string s4 = to_string(numeric_limits<long long>::min()) + "0";
        long l16 = klib::strtol(s4.c_str(), nullptr, 10);
        if (l16 != numeric_limits<long>::min())
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 10) long long negative overflow\n";
        }
        long l17 = klib::strtol("0", nullptr, 10);
        if (l17 != 0)
        {
            ++fail_count;
            std::cout << "FAILED strtol(char*, nullptr, 10) zero\n";
        }
    }
    // strtoll
    {
        long long l1 = klib::strtoll("123", nullptr, 10);
        if (l1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, nullptr, 10) positive no spaces\n";
        }
        long long l2 = klib::strtoll("-123", nullptr, 10);
        if (l2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, nullptr, 10) negative no spaces\n";
        }
        long long l3 = klib::strtoll("   123 ", nullptr, 10);
        if (l3 != 123)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, nullptr, 10) positive with spaces\n";
        }
        long long l4 = klib::strtoll("   -123 ", nullptr, 10);
        if (l4 != -123)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, nullptr, 10) negative with spaces\n";
        }
        long long l5 = klib::strtoll("0xA6", nullptr, 0);
        if (l5 != 166)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, nullptr, 0) assumed hex\n";
        }
        long long l6 = klib::strtoll("076", nullptr, 0);
        if (l6 != 62)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, nullptr, 0) assumed octal\n";
        }
        long long l7 = klib::strtoll("123", nullptr, 0);
        if (l7 != 123)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, nullptr, 0) assumed decimal\n";
        }
        long long l8 = klib::strtoll("0xA6", nullptr, 16);
        if (l8 != 166)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, nullptr, 16) explicit hex\n";
        }
        long long l9 = klib::strtoll("076", nullptr, 8);
        if (l9 != 62)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, nullptr, 8) explicit octal\n";
        }
        char* st1;
        const char* c1 = "07697";
        long long l10 = klib::strtoll(c1, &st1, 8);
        if (l10 != 62 || (st1 - c1) != 3)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, char**, 8) octal with bad digit\n";
        }
        char* st2;
        const char* c2 = "123";
        long long l11 = klib::strtoll(c2, &st2, 10);
        if (l11 != 123 || (st2 - c2) != 3)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, char**, 10) simple digit count\n";
        }
        char* st3;
        const char* c3 = "   123a  ";
        long long l12 = klib::strtoll(c3, &st3, 10);
        if (l12 != 123 || (st3 - c3) != 6)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, char**, 10) digit count with spaces\n";
        }
        string s1 = to_string(numeric_limits<long long>::max()) + "0";
        long long l13 = klib::strtoll(s1.c_str(), nullptr, 10);
        if (l13 != numeric_limits<long long>::max())
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, nullptr, 10) long long positive overflow\n";
        }
        string s2 = to_string(numeric_limits<long long>::min()) + "0";
        long long l14 = klib::strtoll(s2.c_str(), nullptr, 10);
        if (l14 != numeric_limits<long long>::min())
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, nullptr, 10) long long negative overflow\n";
        }
        long long l15 = klib::strtoll("0", nullptr, 10);
        if (l15 != 0)
        {
            ++fail_count;
            std::cout << "FAILED strtoll(char*, nullptr, 10) zero\n";
        }
    }
    // strtoul
    {
        unsigned long ul1 = klib::strtoul("123", nullptr, 10);
        if (ul1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, nullptr, 10) positive no spaces\n";
        }
        unsigned long ul2 = klib::strtoul("   123 ", nullptr, 10);
        if (ul2 != 123)
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, nullptr, 10) positive with spaces\n";
        }
        unsigned long ul3 = klib::strtoul("0xA6", nullptr, 0);
        if (ul3 != 166)
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, nullptr, 0) assumed hex\n";
        }
        unsigned long ul4 = klib::strtoul("076", nullptr, 0);
        if (ul4 != 62)
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, nullptr, 0) assumed octal\n";
        }
        unsigned long ul5 = klib::strtoul("123", nullptr, 0);
        if (ul5 != 123)
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, nullptr, 0) assumed decimal\n";
        }
        unsigned long ul6 = klib::strtoul("0xA6", nullptr, 16);
        if (ul6 != 166)
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, nullptr, 16) explicit hex\n";
        }
        unsigned long ul7 = klib::strtoul("076", nullptr, 8);
        if (ul7 != 62)
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, nullptr, 8) explicit octal\n";
        }
        char* st1;
        const char* c1 = "07697";
        unsigned long ul8 = klib::strtoul(c1, &st1, 8);
        if (ul8 != 62 || (st1 - c1) != 3)
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, char**, 8) octal with bad digit\n";
        }
        char* st2;
        const char* c2 = "123";
        unsigned long ul9 = klib::strtoul(c2, &st2, 10);
        if (ul9 != 123 || (st2 - c2) != 3)
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, char**, 10) simple digit count\n";
        }
        char* st3;
        const char* c3 = "   123a  ";
        unsigned long ul10 = klib::strtoul(c3, &st3, 10);
        if (ul10 != 123 || (st3 - c3) != 6)
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, char**, 10) digit count with spaces\n";
        }
        string s1 = to_string(numeric_limits<unsigned long>::max()) + "0";
        unsigned long ul11 = klib::strtoul(s1.c_str(), nullptr, 10);
        if (ul11 != numeric_limits<unsigned long>::max())
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, nullptr, 10) unsigned long overflow\n";
        }
        string s2 = to_string(numeric_limits<unsigned long long>::max()) + "0";
        unsigned long ul12 = klib::strtoul(s2.c_str(), nullptr, 10);
        if (ul12 != numeric_limits<unsigned long>::max())
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, nullptr, 10) unsigned long long overflow\n";
        }
        unsigned long ul13 = klib::strtoul("0", nullptr, 10);
        if (ul13 != 0)
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, nullptr, 10) zero\n";
        }
        unsigned long ul14 = klib::strtoul("-123", nullptr, 10);
        if (ul14 != numeric_limits<unsigned long>::max() - 122)
        {
            ++fail_count;
            std::cout << "FAILED strtoul(char*, nullptr, 10) negative\n";
        }
    }
    // strtoull
    {
        unsigned long long ul1 = klib::strtoull("123", nullptr, 10);
        if (ul1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, nullptr, 10) positive no spaces\n";
        }
        unsigned long long ul2 = klib::strtoull("   123 ", nullptr, 10);
        if (ul2 != 123)
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, nullptr, 10) positive with spaces\n";
        }
        unsigned long long ul3 = klib::strtoull("0xA6", nullptr, 0);
        if (ul3 != 166)
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, nullptr, 0) assumed hex\n";
        }
        unsigned long long ul4 = klib::strtoull("076", nullptr, 0);
        if (ul4 != 62)
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, nullptr, 0) assumed octal\n";
        }
        unsigned long long ul5 = klib::strtoull("123", nullptr, 0);
        if (ul5 != 123)
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, nullptr, 0) assumed decimal\n";
        }
        unsigned long long ul6 = klib::strtoull("0xA6", nullptr, 16);
        if (ul6 != 166)
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, nullptr, 16) explicit hex\n";
        }
        unsigned long long ul7 = klib::strtoull("076", nullptr, 8);
        if (ul7 != 62)
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, nullptr, 8) explicit octal\n";
        }
        char* st1;
        const char* c1 = "07697";
        unsigned long long ul8 = klib::strtoull(c1, &st1, 8);
        if (ul8 != 62 || (st1 - c1) != 3)
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, char**, 8) octal with bad digit\n";
        }
        char* st2;
        const char* c2 = "123";
        unsigned long long ul9 = klib::strtoull(c2, &st2, 10);
        if (ul9 != 123 || (st2 - c2) != 3)
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, char**, 10) simple digit count\n";
        }
        char* st3;
        const char* c3 = "   123a  ";
        unsigned long long ul10 = klib::strtoull(c3, &st3, 10);
        if (ul10 != 123 || (st3 - c3) != 6)
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, char**, 10) digit count with spaces\n";
        }
        string s1 = to_string(numeric_limits<unsigned long long>::max()) + "0";
        unsigned long long ul11 = klib::strtoull(s1.c_str(), nullptr, 10);
        if (ul11 != numeric_limits<unsigned long long>::max())
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, nullptr, 10) unsigned long long overflow\n";
        }
        unsigned long long ul12 = klib::strtoull("0", nullptr, 10);
        if (ul12 != 0)
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, nullptr, 10) zero\n";
        }
        unsigned long long ul14 = klib::strtoull("-123", nullptr, 10);
        if (ul14 != numeric_limits<unsigned long long>::max() - 122)
        {
            ++fail_count;
            std::cout << "FAILED strtoull(char*, nullptr, 10) negative\n";
        }
    }
    // atoi
    {
        int i1 = klib::atoi("123");
        if (i1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED atoi(char*) positive no spaces\n";
        }
        int i2 = klib::atoi("-123");
        if (i2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED atoi(char*) negative no spaces\n";
        }
        int i3 = klib::atoi("   123 ");
        if (i3 != 123)
        {
            ++fail_count;
            std::cout << "FAILED atoi(char*) positive with spaces\n";
        }
        int i4 = klib::atoi("   -123 ");
        if (i4 != -123)
        {
            ++fail_count;
            std::cout << "FAILED atoi(char*) negative with spaces\n";
        }
        string s1 = to_string(numeric_limits<int>::max()) + "0";
        int i5 = klib::atoi(s1.c_str());
        if (i5 != numeric_limits<int>::max())
        {
            ++fail_count;
            std::cout << "FAILED atoi(char*) int positive overflow\n";
        }
        string s2 = to_string(numeric_limits<int>::min()) + "0";
        int i6 = klib::atoi(s2.c_str());
        if (i6 != numeric_limits<int>::min())
        {
            ++fail_count;
            std::cout << "FAILED atoi(char*) int negative overflow\n";
        }
        string s3 = to_string(numeric_limits<long>::max()) + "0";
        int i7 = klib::atoi(s3.c_str());
        if (i7 != numeric_limits<int>::max())
        {
            ++fail_count;
            std::cout << "FAILED atoi(char*) long positive overflow\n";
        }
        string s4 = to_string(numeric_limits<long>::min()) + "0";
        int i8 = klib::atoi(s4.c_str());
        if (i8 != numeric_limits<int>::min())
        {
            ++fail_count;
            std::cout << "FAILED atoi(char*) long negative overflow\n";
        }
        string s5 = to_string(numeric_limits<long long>::max()) + "0";
        int i9 = klib::atoi(s5.c_str());
        if (i9 != numeric_limits<int>::max())
        {
            ++fail_count;
            std::cout << "FAILED atoi(char*) long long positive overflow\n";
        }
        string s6 = to_string(numeric_limits<long long>::min()) + "0";
        int i10 = klib::atoi(s6.c_str());
        if (i10 != numeric_limits<int>::min())
        {
            ++fail_count;
            std::cout << "FAILED atoi(char*) long long negative overflow\n";
        }
        int i11 = klib::atoi("0");
        if (i11 != 0)
        {
            ++fail_count;
            std::cout << "FAILED atoi(char*) zero\n";
        }
    }
    // atol
    {
        long l1 = klib::atol("123");
        if (l1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED atol(char*) positive no spaces\n";
        }
        long l2 = klib::atol("-123");
        if (l2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED atol(char*) negative no spaces\n";
        }
        long l3 = klib::atol("   123 ");
        if (l3 != 123)
        {
            ++fail_count;
            std::cout << "FAILED atol(char*) positive with spaces\n";
        }
        long l4 = klib::atol("   -123 ");
        if (l4 != -123)
        {
            ++fail_count;
            std::cout << "FAILED atol(char*) negative with spaces\n";
        }
        string s1 = to_string(numeric_limits<long>::max()) + "0";
        long l5 = klib::atol(s1.c_str());
        if (l5 != numeric_limits<long>::max())
        {
            ++fail_count;
            std::cout << "FAILED atol(char*) long positive overflow\n";
        }
        string s2 = to_string(numeric_limits<long>::min()) + "0";
        long l6 = klib::atol(s2.c_str());
        if (l6 != numeric_limits<long>::min())
        {
            ++fail_count;
            std::cout << "FAILED atol(char*) long negative overflow\n";
        }
        string s3 = to_string(numeric_limits<long long>::max()) + "0";
        long l7 = klib::atol(s3.c_str());
        if (l7 != numeric_limits<long>::max())
        {
            ++fail_count;
            std::cout << "FAILED atol(char*) long long positive overflow\n";
        }
        string s4 = to_string(numeric_limits<long long>::min()) + "0";
        long l8 = klib::atol(s4.c_str());
        if (l8 != numeric_limits<long>::min())
        {
            ++fail_count;
            std::cout << "FAILED atol(char*) long long negative overflow\n";
        }
        long l9 = klib::atol("0");
        if (l9 != 0)
        {
            ++fail_count;
            std::cout << "FAILED atol(char*) zero\n";
        }
    }
    // atoll
    {
        long long l1 = klib::atoll("123");
        if (l1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED atoll(char*) positive no spaces\n";
        }
        long long l2 = klib::atoll("-123");
        if (l2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED atoll(char*) negative no spaces\n";
        }
        long long l3 = klib::atoll("   123 ");
        if (l3 != 123)
        {
            ++fail_count;
            std::cout << "FAILED atoll(char*) positive with spaces\n";
        }
        long long l4 = klib::atoll("   -123 ");
        if (l4 != -123)
        {
            ++fail_count;
            std::cout << "FAILED atoll(char*) negative with spaces\n";
        }
        string s1 = to_string(numeric_limits<long long>::max()) + "0";
        long long l5 = klib::atoll(s1.c_str());
        if (l5 != numeric_limits<long>::max())
        {
            ++fail_count;
            std::cout << "FAILED atoll(char*) long long positive overflow\n";
        }
        string s2 = to_string(numeric_limits<long long>::min()) + "0";
        long long l6 = klib::atoll(s2.c_str());
        if (l6 != numeric_limits<long>::min())
        {
            ++fail_count;
            std::cout << "FAILED atoll(char*) long long negative overflow\n";
        }
        long long l7 = klib::atoll("0");
        if (l7 != 0)
        {
            ++fail_count;
            std::cout << "FAILED atoll(char*) zero\n";
        }
    }
    // strtof
    {
        float f1 = klib::strtof("123.456", nullptr);
        if (f1 != 123.456f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) simple positive\n";
        }
        float f2 = klib::strtof("-123.456", nullptr);
        if (f2 != -123.456f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) simple negative\n";
        }
        float f3 = klib::strtof("    123.456fkj  ", nullptr);
        if (f3 != 123.456f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) positive with spaces\n";
        }
        float f4 = klib::strtof("    -123.456gyjt  ", nullptr);
        if (f4 != -123.456f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) negative with spaces\n";
        }
        float f5 = klib::strtof("123", nullptr);
        if (f5 != 123.f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) no decimal\n";
        }
        float f6 = klib::strtof("123.", nullptr);
        if (f6 != 123.f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) nothing after decimal\n";
        }
        float f7 = klib::strtof(".456", nullptr);
        if (f7 != .456f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) nothing before decimal\n";
        }
        float f8 = klib::strtof("123.00456", nullptr);
        if (f8 != 123.00456f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) zeros after decimal\n";
        }
        float f9 = klib::strtof("000.00456", nullptr);
        if (f9 != .00456f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) zeros before decimal\n";
        }
        float f10 = klib::strtof("123.456E2", nullptr);
        if (f10 != 12345.6f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) positive exponential\n";
        }
        float f11 = klib::strtof("123.456E-2", nullptr);
        if (f11 != 1.23456f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) negative exponential\n";
        }
        float f12 = klib::strtof("123E2", nullptr);
        if (f12 != 12300.f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) no decimal, positive exponential\n";
        }
        float f13 = klib::strtof("123.E2", nullptr);
        if (f13 != 12300.f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) nothing after decimal, positive exponential\n";
        }
        float f14 = klib::strtof(".456E2", nullptr);
        if (f14 != 45.6f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) nothing before decimal, positive exponential\n";
        }
        float f15 = klib::strtof("123.00456E2", nullptr);
        if (f15 != 12300.456f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) zeros after decimal, positive exponential\n";
        }
        float f16 = klib::strtof("000.00456E2", nullptr);
        if (f16 != .456f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) zeros before decimal, positive exponential\n";
        }
        float f17 = klib::strtof("0x7B.C", nullptr);
        if (f17 != 123.75f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) positive hex\n";
        }
        float f18 = klib::strtof("-0x7B.C", nullptr);
        if (f18 != -123.75f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) negative hex\n";
        }
        float f19 = klib::strtof("0x7B", nullptr);
        if (f19 != 123.f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) hex, no decimal\n";
        }
        float f20 = klib::strtof("0x7B.", nullptr);
        if (f20 != 123.f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) hex, nothing after decimal\n";
        }
        float f21 = klib::strtof("0x.C", nullptr);
        if (f21 != .75f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) hex, nothing before decimal\n";
        }
        float f22 = klib::strtof("0x7B.CP2", nullptr);
        if (f22 != 495.f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) hex, positive exponential\n";
        }
        float f23 = klib::strtof("0x7B.CP-2", nullptr);
        if (f23 != 30.9375f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) hex, negative exponential\n";
        }
        float f24 = klib::strtof("InF", nullptr);
        if (f24 != numeric_limits<float>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) inf\n";
        }
        float f25 = klib::strtof("-InF", nullptr);
        if (f25 != -numeric_limits<float>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) -inf\n";
        }
        float f26 = klib::strtof("InFinITy", nullptr);
        if (f26 != numeric_limits<float>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) infinity\n";
        }
        float f27 = klib::strtof("nAn", nullptr);
        if (!isnan(f27))
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) nan\n";
        }
        float f28 = klib::strtof("000.0000E12", nullptr);
        if (f28 != 0.f)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) zero\n";
        }
        char* st1;
        const char* c1 = "  abc";
        float f29 = klib::strtof(c1, &st1);
        if (f29 != 0.f || (st1 - c1) != 0)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) invalid start, digit count\n";
        }
        char* st2;
        const char* c2 = "56dkjg";
        float f30 = klib::strtof(c2, &st2);
        if (f30 != 56.f || (st2 - c2) != 2)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) invalid before decimal, digit count\n";
        }
        char* st3;
        const char* c3 = "56.wer";
        float f31 = klib::strtof(c3, &st3);
        if (f31 != 56.f || (st3 - c3) != 3)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) invalid at decimal, digit count\n";
        }
        char* st4;
        const char* c4 = "56.78wer";
        float f32 = klib::strtof(c4, &st4);
        if (f32 != 56.78f || (st4 - c4) != 5)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) invalid after decimal, digit count\n";
        }
        char* st5;
        const char* c5 = "56.78egewr";
        float f33 = klib::strtof(c5, &st5);
        if (f33 != 56.78f || (st5 - c5) != 5)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) invalid at exponential, digit count\n";
        }
        char* st6;
        const char* c6 = "56.78e1kj";
        float f34 = klib::strtof(c6, &st6);
        if (f34 != 567.8f || (st6 - c6) != 7)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) invalid after exponential, digit count\n";
        }
        char* st7;
        const char* c7 = "123.456";
        float f35 = klib::strtof(c7, &st7);
        if (f35 != 123.456f || (st7 - c7) != 7)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) simple digit count\n";
        }
        char* st8;
        const char* c8 = "   123.456 ";
        float f36 = klib::strtof(c8, &st8);
        if (f36 != 123.456f || (st8 - c8) != 10)
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) digit count with spaces\n";
        }
        string s1 = to_string(numeric_limits<float>::max()) + "1";
        float f37 = klib::strtof(s1.c_str(), nullptr);
        if (f37 != numeric_limits<float>::max())
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) positive overflow\n";
        }
        string s2 = to_string(numeric_limits<float>::lowest()) + "1";
        float f38 = klib::strtof(s2.c_str(), nullptr);
        if (f38 != numeric_limits<float>::lowest())
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) negative overflow\n";
        }
        string s3 = to_string(numeric_limits<float>::min()) + "1";
        float f39 = klib::strtof(s3.c_str(), nullptr);
        if (f39 != numeric_limits<float>::min())
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) positive underflow\n";
        }
        string s4 = to_string(-numeric_limits<float>::min()) + "1";
        float f40 = klib::strtof(s4.c_str(), nullptr);
        if (f40 != -numeric_limits<float>::min())
        {
            ++fail_count;
            std::cout << "FAILED strtof(char*, nullptr) negative underflow\n";
        }
    }
    // strtod
    {
        double d1 = klib::strtod("123.456", nullptr);
        if (d1 != 123.456)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) simple positive\n";
        }
        double d2 = klib::strtod("-123.456", nullptr);
        if (d2 != -123.456)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) simple negative\n";
        }
        double d3 = klib::strtod("    123.456fkj  ", nullptr);
        if (d3 != 123.456)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) positive with spaces\n";
        }
        double d4 = klib::strtod("    -123.456gyjt  ", nullptr);
        if (d4 != -123.456)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) negative with spaces\n";
        }
        double d5 = klib::strtod("123", nullptr);
        if (d5 != 123.)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) no decimal\n";
        }
        double d6 = klib::strtod("123.", nullptr);
        if (d6 != 123.)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) nothing after decimal\n";
        }
        double d7 = klib::strtod(".456", nullptr);
        if (d7 != .456)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) nothing before decimal\n";
        }
        double d8 = klib::strtod("123.00456", nullptr);
        if (d8 != 123.00456)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) zeros after decimal\n";
        }
        double d9 = klib::strtod("000.00456", nullptr);
        if (d9 != .00456)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) zeros before decimal\n";
        }
        double d10 = klib::strtod("123.456E2", nullptr);
        if (d10 != 12345.6)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) positive exponential\n";
        }
        double d11 = klib::strtod("123.456E-2", nullptr);
        if (d11 != 1.23456)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) negative exponential\n";
        }
        double d12 = klib::strtod("123E2", nullptr);
        if (d12 != 12300.)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) no decimal, positive exponential\n";
        }
        double d13 = klib::strtod("123.E2", nullptr);
        if (d13 != 12300.)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) nothing after decimal, positive exponential\n";
        }
        double d14 = klib::strtod(".456E2", nullptr);
        if (d14 != 45.6)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) nothing before decimal, positive exponential\n";
        }
        double d15 = klib::strtod("123.00456E2", nullptr);
        if (d15 != 12300.456)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) zeros after decimal, positive exponential\n";
        }
        double d16 = klib::strtod("000.00456E2", nullptr);
        if (d16 != .456)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) zeros before decimal, positive exponential\n";
        }
        double d17 = klib::strtod("0x7B.C", nullptr);
        if (d17 != 123.75)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) positive hex\n";
        }
        double d18 = klib::strtod("-0x7B.C", nullptr);
        if (d18 != -123.75)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) negative hex\n";
        }
        double d19 = klib::strtod("0x7B", nullptr);
        if (d19 != 123.)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) hex, no decimal\n";
        }
        double d20 = klib::strtod("0x7B.", nullptr);
        if (d20 != 123.)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) hex, nothing after decimal\n";
        }
        double d21 = klib::strtod("0x.C", nullptr);
        if (d21 != .75)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) hex, nothing before decimal\n";
        }
        double d22 = klib::strtod("0x7B.CP2", nullptr);
        if (d22 != 495.)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) hex, positive exponential\n";
        }
        double d23 = klib::strtod("0x7B.CP-2", nullptr);
        if (d23 != 30.9375)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) hex, negative exponential\n";
        }
        double d24 = klib::strtod("InF", nullptr);
        if (d24 != numeric_limits<double>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) inf\n";
        }
        double d25 = klib::strtod("-InF", nullptr);
        if (d25 != -numeric_limits<double>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) -inf\n";
        }
        double d26 = klib::strtod("InFinITy", nullptr);
        if (d26 != numeric_limits<double>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) infinity\n";
        }
        double d27 = klib::strtod("nAn", nullptr);
        if (!isnan(d27))
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) nan\n";
        }
        double d28 = klib::strtod("000.0000E12", nullptr);
        if (d28 != 0.)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) zero\n";
        }
        char* st1;
        const char* c1 = "  abc";
        double d29 = klib::strtod(c1, &st1);
        if (d29 != 0. || (st1 - c1) != 0)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) invalid start, digit count\n";
        }
        char* st2;
        const char* c2 = "56dkjg";
        float d30 = klib::strtod(c2, &st2);
        if (d30 != 56. || (st2 - c2) != 2)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) invalid before decimal, digit count\n";
        }
        char* st3;
        const char* c3 = "56.wer";
        double d31 = klib::strtod(c3, &st3);
        if (d31 != 56. || (st3 - c3) != 3)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) invalid at decimal, digit count\n";
        }
        char* st4;
        const char* c4 = "56.78wer";
        double d32 = klib::strtod(c4, &st4);
        if (d32 != 56.78 || (st4 - c4) != 5)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) invalid after decimal, digit count\n";
        }
        char* st5;
        const char* c5 = "56.78egewr";
        double d33 = klib::strtod(c5, &st5);
        if (d33 != 56.78 || (st5 - c5) != 5)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) invalid at exponential, digit count\n";
        }
        char* st6;
        const char* c6 = "56.78e1kj";
        double d34 = klib::strtod(c6, &st6);
        if (d34 != 567.8 || (st6 - c6) != 7)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) invalid after exponential, digit count\n";
        }
        char* st7;
        const char* c7 = "123.456";
        double d35 = klib::strtod(c7, &st7);
        if (d35 != 123.456 || (st7 - c7) != 7)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) simple digit count\n";
        }
        char* st8;
        const char* c8 = "   123.456 ";
        double d36 = klib::strtod(c8, &st8);
        if (d36 != 123.456 || (st8 - c8) != 10)
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) digit count with spaces\n";
        }
        string s1 = to_string(numeric_limits<double>::max()) + "1";
        double d37 = klib::strtod(s1.c_str(), nullptr);
        if (d37 != numeric_limits<double>::max())
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) positive overflow\n";
        }
        string s2 = to_string(numeric_limits<double>::lowest()) + "1";
        double d38 = klib::strtod(s2.c_str(), nullptr);
        if (d38 != numeric_limits<double>::lowest())
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) negative overflow\n";
        }
        string s3 = to_string(numeric_limits<double>::min()) + "1";
        double d39 = klib::strtod(s3.c_str(), nullptr);
        if (d39 != numeric_limits<double>::min())
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) positive underflow\n";
        }
        string s4 = to_string(-numeric_limits<double>::min()) + "1";
        double d40 = klib::strtod(s4.c_str(), nullptr);
        if (d40 != -numeric_limits<double>::min())
        {
            ++fail_count;
            std::cout << "FAILED strtod(char*, nullptr) negative underflow\n";
        }
    }
    // strtold
    {
        long double ld1 = klib::strtold("123.456", nullptr);
        if (ld1 != 123.456L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) simple positive\n";
        }
        long double ld2 = klib::strtold("-123.456", nullptr);
        if (ld2 != -123.456L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) simple negative\n";
        }
        long double ld3 = klib::strtold("    123.456fkj  ", nullptr);
        if (ld3 != 123.456L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) positive with spaces\n";
        }
        long double ld4 = klib::strtold("    -123.456gyjt  ", nullptr);
        if (ld4 != -123.456L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) negative with spaces\n";
        }
        long double ld5 = klib::strtold("123", nullptr);
        if (ld5 != 123.L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) no decimal\n";
        }
        long double ld6 = klib::strtold("123.", nullptr);
        if (ld6 != 123.L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) nothing after decimal\n";
        }
        long double ld7 = klib::strtold(".456", nullptr);
        if (ld7 != .456L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) nothing before decimal\n";
        }
        long double ld8 = klib::strtold("123.00456", nullptr);
        if (ld8 != 123.00456L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) zeros after decimal\n";
        }
        long double ld9 = klib::strtold("000.00456", nullptr);
        if (ld9 != .00456L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) zeros before decimal\n";
        }
        long double ld10 = klib::strtold("1.234E2", nullptr);
        if (ld10 != 123.4L)
        {
            std::cout << "ld10 == " << ld10 << '\n';
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) positive exponential\n";
        }
        long double ld11 = klib::strtold("123.456E-2", nullptr);
        if (ld11 != 1.23456L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) negative exponential\n";
        }
        long double ld12 = klib::strtold("123E2", nullptr);
        if (ld12 != 12300.L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) no decimal, positive exponential\n";
        }
        long double ld13 = klib::strtold("123.E2", nullptr);
        if (ld13 != 12300.L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) nothing after decimal, positive exponential\n";
        }
        long double ld14 = klib::strtold(".456E2", nullptr);
        if (ld14 != 45.6L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) nothing before decimal, positive exponential\n";
        }
        long double ld15 = klib::strtold("1.00229999999999999996E2", nullptr);
        if (ld15 != 100.229999999999999996L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) zeros after decimal, positive exponential\n";
        }
        long double ld16 = klib::strtold("000.00456E2", nullptr);
        if (ld16 != .456L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) zeros before decimal, positive exponential\n";
        }
        long double ld17 = klib::strtold("0x7B.C", nullptr);
        if (ld17 != 123.75L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) positive hex\n";
        }
        long double ld18 = klib::strtold("-0x7B.C", nullptr);
        if (ld18 != -123.75L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) negative hex\n";
        }
        long double ld19 = klib::strtold("0x7B", nullptr);
        if (ld19 != 123.L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) hex, no decimal\n";
        }
        long double ld20 = klib::strtold("0x7B.", nullptr);
        if (ld20 != 123.L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) hex, nothing after decimal\n";
        }
        long double ld21 = klib::strtold("0x.C", nullptr);
        if (ld21 != .75L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) hex, nothing before decimal\n";
        }
        long double ld22 = klib::strtold("0x7B.CP2", nullptr);
        if (ld22 != 495.L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) hex, positive exponential\n";
        }
        long double ld23 = klib::strtold("0x7B.CP-2", nullptr);
        if (ld23 != 30.9375L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) hex, negative exponential\n";
        }
        long double ld24 = klib::strtold("InF", nullptr);
        if (ld24 != numeric_limits<long double>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) inf\n";
        }
        long double ld25 = klib::strtold("-InF", nullptr);
        if (ld25 != -numeric_limits<long double>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) -inf\n";
        }
        long double ld26 = klib::strtold("InFinITy", nullptr);
        if (ld26 != numeric_limits<long double>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) infinity\n";
        }
        long double ld27 = klib::strtold("nAn", nullptr);
        if (!isnan(ld27))
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) nan\n";
        }
        long double ld28 = klib::strtold("000.0000E12", nullptr);
        if (ld28 != 0.L)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) zero\n";
        }
        char* st1;
        const char* c1 = "  abc";
        long double ld29 = klib::strtold(c1, &st1);
        if (ld29 != 0.L || (st1 - c1) != 0)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) invalid start, digit count\n";
        }
        char* st2;
        const char* c2 = "56dkjg";
        float ld30 = klib::strtold(c2, &st2);
        if (ld30 != 56.L || (st2 - c2) != 2)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) invalid before decimal, digit count\n";
        }
        char* st3;
        const char* c3 = "56.wer";
        long double ld31 = klib::strtold(c3, &st3);
        if (ld31 != 56.L || (st3 - c3) != 3)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) invalid at decimal, digit count\n";
        }
        char* st4;
        const char* c4 = "56.78wer";
        long double ld32 = klib::strtold(c4, &st4);
        if (ld32 != 56.78L || (st4 - c4) != 5)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) invalid after decimal, digit count\n";
        }
        char* st5;
        const char* c5 = "56.78egewr";
        long double ld33 = klib::strtold(c5, &st5);
        if (ld33 != 56.78L || (st5 - c5) != 5)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) invalid at exponential, digit count\n";
        }
        char* st6;
        const char* c6 = "56.78e1kj";
        long double ld34 = klib::strtold(c6, &st6);
        if (ld34 != 567.8L || (st6 - c6) != 7)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) invalid after exponential, digit count\n";
        }
        char* st7;
        const char* c7 = "123.456";
        long double ld35 = klib::strtold(c7, &st7);
        if (ld35 != 123.456L || (st7 - c7) != 7)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) simple digit count\n";
        }
        char* st8;
        const char* c8 = "   123.456 ";
        long double ld36 = klib::strtold(c8, &st8);
        if (ld36 != 123.456L || (st8 - c8) != 10)
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) digit count with spaces\n";
        }
        string s1 = to_string(numeric_limits<long double >::max()) + "1";
        long double ld37 = klib::strtold(s1.c_str(), nullptr);
        if (ld37 != numeric_limits<long double >::max())
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) positive overflow\n";
        }
        string s2 = to_string(numeric_limits<long double>::lowest()) + "1";
        long double ld38 = klib::strtold(s2.c_str(), nullptr);
        if (ld38 != numeric_limits<long double >::lowest())
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) negative overflow\n";
        }
        string s3 = to_string(numeric_limits<long double >::min()) + "1";
        long double ld39 = klib::strtold(s3.c_str(), nullptr);
        if (ld39 != numeric_limits<long double >::min())
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) positive underflow\n";
        }
        string s4 = to_string(-numeric_limits<long double >::min()) + "1";
        long double ld40 = klib::strtold(s4.c_str(), nullptr);
        if (ld40 != -numeric_limits<long double >::min())
        {
            ++fail_count;
            std::cout << "FAILED strtold(char*, nullptr) negative underflow\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
