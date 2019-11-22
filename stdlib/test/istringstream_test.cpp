#include <iostream>
#include <iomanip>

#include "../include/ios"
#include "../include/limits"
#include "../include/sstream"
#include "../include/string"
#include "../include/type_traits"

using namespace klib;

// Tests for istringstream. Note that the basic functionality inherited from
// ios_base and ios is tested in the ostringstream tests, so are not duplicated
// here.
int main()
{
    size_t fail_count = 0;

    // Types
    if (!is_same<istringstream::char_type, char>::value)
    {
        ++fail_count;
        std::cout << "FAILED istringstream::char_type\n";
    }
    if (!is_same<istringstream::traits_type, char_traits<char>>::value)
    {
        ++fail_count;
        std::cout << "FAILED istringstream::traits_type\n";
    }
    if (!is_same<istringstream::int_type, char_traits<char>::int_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED istringstream::int_type\n";
    }
    if (!is_same<istringstream::pos_type, char_traits<char>::pos_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED istringstream::pos_type\n";
    }
    if (!is_same<istringstream::off_type, char_traits<char>::off_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED istringstream::off_type\n";
    }
    if (!is_same<istringstream::allocator_type, allocator<char>>::value)
    {
        ++fail_count;
        std::cout << "FAILED istringstream::allocator_type\n";
    }
    if (!is_same<istringstream::event_callback,
        void (*)(istringstream::event type, ios_base& ios, int index)>::value)
    {
        ++fail_count;
        std::cout << "FAILED istringstream::event_callback\n";
    }
    // Constructor
    {
        // Default constructor.
        istringstream s1 {};
        if (s1.flags() != (ios_base::skipws | ios_base::dec) ||
            s1.precision() != 6 || s1.width() != 0 ||
            s1.rdstate() != ios_base::goodbit ||
            s1.exceptions() != ios_base::goodbit || s1.tie() != nullptr ||
            s1.rdbuf() == nullptr || s1.fill() != ' ' || s1.str() != "")
        {
            ++fail_count;
            std::cout << "FAILED istringstream default constructor\n";
        }
        // Constructor with mode
        istringstream s2 {ios_base::out};
        if (s2.flags() != (ios_base::skipws | ios_base::dec) ||
            s2.precision() != 6 || s2.width() != 0 ||
            s2.rdstate() != ios_base::goodbit ||
            s2.exceptions() != ios_base::goodbit || s2.tie() != nullptr ||
            s2.rdbuf() == nullptr || s2.fill() != ' ' || s2.str() != "")
        {
            ++fail_count;
            std::cout << "FAILED istringstream constructor with mode\n";
        }
        // Constructor with string.
        string str1 {"hello"};
        istringstream s3 {str1};
        if (s3.flags() != (ios_base::skipws | ios_base::dec) ||
            s3.precision() != 6 || s3.width() != 0 ||
            s3.rdstate() != ios_base::goodbit ||
            s3.exceptions() != ios_base::goodbit || s3.tie() != nullptr ||
            s3.rdbuf() == nullptr || s3.fill() != ' ' || s3.str() != "hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream constructor with string\n";
        }
        // Constructor with string and truncation (shouldn't actually truncate).
        string str2 {"hello"};
        istringstream s4 {str2, ios_base::trunc};
        if (s4.flags() != (ios_base::skipws | ios_base::dec) ||
            s4.precision() != 6 || s4.width() != 0 ||
            s4.rdstate() != ios_base::goodbit ||
            s4.exceptions() != ios_base::goodbit || s4.tie() != nullptr ||
            s4.rdbuf() == nullptr || s4.fill() != ' ' || s4.str() != "hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream constructor with string and truncation\n";
        }
        // Move constructor.
        string str3 {"hello"};
        istringstream s5 {str3};
        s5.flags(ios_base::hex);
        s5.precision(5);
        s5.width(1);
        s5.setstate(ios_base::eofbit);
        s5.exceptions(ios_base::failbit);
        ostringstream s6 {};
        s5.tie(&s6);
        s5.fill('a');
        istringstream s7 {move(s5)};
        if (s7.flags() != ios_base::hex || s7.precision() != 5 ||
            s7.width() != 1 || s7.rdstate() != ios_base::eofbit ||
            s7.exceptions() != ios_base::failbit || s7.tie() != &s6 ||
            s7.rdbuf() == nullptr || s7.fill() != 'a' || s7.str() != "hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream move constructor\n";
        }
    }
    // Assignment
    {
        // Move assignment.
        string str1 {"hello"};
        istringstream s1 {str1};
        s1.flags(ios_base::hex);
        s1.precision(5);
        s1.width(1);
        s1.setstate(ios_base::eofbit);
        s1.exceptions(ios_base::failbit);
        ostringstream s2 {};
        s1.tie(&s2);
        s1.fill('a');
        istringstream s3 {};
        s3 = move(s1);
        if (s3.flags() != ios_base::hex || s3.precision() != 5 ||
            s3.width() != 1 || s3.rdstate() != ios_base::eofbit ||
            s3.exceptions() != ios_base::failbit || s3.tie() != &s2 ||
            s3.rdbuf() == nullptr || s3.fill() != 'a' || s3.str() != "hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream move assignment\n";
        }
    }
    // swap
    {
        // swap member
        string str1 {"hello"};
        istringstream s1 {str1};
        s1.flags(ios_base::hex);
        s1.precision(5);
        s1.width(1);
        s1.setstate(ios_base::eofbit);
        s1.exceptions(ios_base::failbit);
        ostringstream s2 {};
        s1.tie(&s2);
        s1.fill('a');
        string str2 {"goodbye"};
        istringstream s3 {str2};
        s3.swap(s1);
        if (s3.flags() != ios_base::hex || s3.precision() != 5 ||
            s3.width() != 1 || s3.rdstate() != ios_base::eofbit ||
            s3.exceptions() != ios_base::failbit || s3.tie() != &s2 ||
            s3.rdbuf() == nullptr || s3.fill() != 'a' || s3.str() != "hello" ||
            s1.flags() != (ios_base::skipws | ios_base::dec) ||
            s1.precision() != 6 || s1.width() != 0 ||
            s1.rdstate() != ios_base::goodbit ||
            s1.exceptions() != ios_base::goodbit || s1.tie() != nullptr ||
            s1.rdbuf() == nullptr || s1.fill() != ' ' || s1.str() != "goodbye")
        {
            ++fail_count;
            std::cout << "FAILED istringstream swap member\n";
        }
        // swap non-member
        string str3 {"hello"};
        istringstream s4 {str3};
        s4.flags(ios_base::hex);
        s4.precision(5);
        s4.width(1);
        s4.setstate(ios_base::eofbit);
        s4.exceptions(ios_base::failbit);
        ostringstream s5 {};
        s4.tie(&s5);
        s4.fill('a');
        string str4 {"goodbye"};
        istringstream s6 {str4};
        swap(s4, s6);
        if (s6.flags() != ios_base::hex || s6.precision() != 5 ||
            s6.width() != 1 || s6.rdstate() != ios_base::eofbit ||
            s6.exceptions() != ios_base::failbit || s6.tie() != &s5 ||
            s6.rdbuf() == nullptr || s6.fill() != 'a' || s6.str() != "hello" ||
            s4.flags() != (ios_base::skipws | ios_base::dec) ||
            s4.precision() != 6 || s4.width() != 0 ||
            s4.rdstate() != ios_base::goodbit ||
            s4.exceptions() != ios_base::goodbit || s4.tie() != nullptr ||
            s4.rdbuf() == nullptr || s4.fill() != ' ' || s4.str() != "goodbye")
        {
            ++fail_count;
            std::cout << "FAILED istringstream swap non-member\n";
        }
    }
    // rdbuf
    {
        istringstream s1 {"hello"};
        if (s1.rdbuf()->str() != "hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream rdbuf\n";
        }
    }
    // str
    {
        // str get
        istringstream s1 {"hello"};
        if (s1.str() != "hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream str get\n";
        }
        // str set
        istringstream s2 {"hello"};
        s2.str("goodbye");
        if (s2.str() != "goodbye")
        {
            ++fail_count;
            std::cout << "FAILED istringstream str set\n";
        }
    }
    // operator >>, short
    {
        // dec, positive
        istringstream s1 {"123"};
        short sh1;
        s1 >> sh1;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || sh1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, dec, positive\n";
        }
        // dec, negative
        istringstream s2 {"-123"};
        short sh2;
        s2 >> sh2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() || sh2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, dec, negative\n";
        }
        // oct, positive
        istringstream s3 {"123"};
        short sh3;
        s3 >> oct >> sh3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || sh3 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, oct, positive\n";
        }
        // oct, negative
        istringstream s4 {"-123"};
        short sh4;
        s4 >> oct >> sh4;
        if (!s4 || !s4.eof() || s4.fail() || s4.bad() || sh4 != -83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, oct, negative\n";
        }
        // oct, with base
        istringstream s5 {"0123"};
        short sh5;
        s5 >> oct >> sh5;
        if (!s5 || !s5.eof() || s5.fail() || s5.bad() ||  sh5 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, oct, with base\n";
        }
        // hex, positive
        istringstream s6 {"123"};
        short sh6;
        s6 >> hex >> sh6;
        if (!s6 || !s6.eof() || s6.fail() || s6.bad() || sh6 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, hex, positive\n";
        }
        // hex, negative
        istringstream s7 {"-123"};
        short sh7;
        s7 >> hex >> sh7;
        if (!s7 || !s7.eof() || s7.fail() || s7.bad() || sh7 != -291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, hex, negative\n";
        }
        // hex, with base
        istringstream s8 {"0x123"};
        short sh8;
        s8 >> hex >> sh8;
        if (!s8 || !s8.eof() || s8.fail() || s8.bad() || sh8 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, hex, with base\n";
        }
        // with leading whitespace
        istringstream s9 {"\t   \r\n  123"};
        short sh9;
        s9 >> sh9;
        if (!s9 || !s9.eof() || s9.fail() || s9.bad() || sh9 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, with leading whitespace\n";
        }
        // fail, no exception
        istringstream s10 {"abc"};
        short sh10 = 1;
        s10 >> sh10;
        if (s10 || !s10.fail() || s10.eof() || s10.bad() ||
            s10.str() != "abc" || sh10 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, fail, no exception\n";
        }
        // fail, exception
        istringstream s11 {"abc"};
        s11.exceptions(ios_base::failbit);
        short sh11 = 1;
        try {
            s11 >> sh11;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s11 || !s11.fail() || s11.eof() || s11.bad() ||
                s11.str() != "abc" || sh11 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> short, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        istringstream s12 {""};
        short sh12 = 1;
        s12 >> sh12;
        if (s12 || !s12.fail() || !s12.eof() || s12.bad() ||
            s12.str() != "" || sh12 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, eof, no exception\n";
        }
        // eof, exception
        istringstream s13 {""};
        s13.exceptions(ios_base::eofbit);
        short sh13 = 1;
        try {
            s13 >> sh13;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s13 || !s13.fail() || !s13.eof() || s13.bad() ||
                s13.str() != "" || sh13 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> short, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, eof, exception, wrong exception type\n";
        }
        // Partial value
        istringstream s14 {"123abc"};
        short sh14;
        s14 >> sh14;
        char c1;
        s14 >> c1;
        if (!s14 || sh14 != 123 || c1 != 'a')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, partial value\n";
        }
        // Mulitple values
        istringstream s15 {"123 \t 456 "};
        short sh15, sh16;
        s15 >> sh15 >> sh16;
        if (!s15 || s15.eof() || s15.fail() || s15.bad() || sh15 != 123 ||
            sh16 != 456)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, partial value\n";
        }
        // Exceed numeric limits, positive
        istringstream s16 {"40000"};
        short sh17;
        s16 >> sh17;
        if (!s16 || !s16.eof() || s16.fail() || s16.bad() || sh17 != numeric_limits<short>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, exceed numeric limits, positive\n";
        }
        // Exceed numeric limits, negative
        istringstream s17 {"-40000"};
        short sh18;
        s17 >> sh18;
        if (!s17 || !s17.eof() || s17.fail() || s17.bad() || sh18 != numeric_limits<short>::lowest())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> short, exceed numeric limits, negative\n";
        }
    }
    // operator >>, int
    {
        // dec, positive
        istringstream s1 {"123"};
        int i1;
        s1 >> i1;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || i1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, dec, positive\n";
        }
        // dec, negative
        istringstream s2 {"-123"};
        int i2;
        s2 >> i2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() || i2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, dec, negative\n";
        }
        // oct, positive
        istringstream s3 {"123"};
        int i3;
        s3 >> oct >> i3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || i3 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, oct, positive\n";
        }
        // oct, negative
        istringstream s4 {"-123"};
        int i4;
        s4 >> oct >> i4;
        if (!s4 || !s4.eof() || s4.fail() || s4.bad() || i4 != -83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, oct, negative\n";
        }
        // oct, with base
        istringstream s5 {"0123"};
        int i5;
        s5 >> oct >> i5;
        if (!s5 || !s5.eof() || s5.fail() || s5.bad() ||  i5 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, oct, with base\n";
        }
        // hex, positive
        istringstream s6 {"123"};
        int i6;
        s6 >> hex >> i6;
        if (!s6 || !s6.eof() || s6.fail() || s6.bad() || i6 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, hex, positive\n";
        }
        // hex, negative
        istringstream s7 {"-123"};
        int i7;
        s7 >> hex >> i7;
        if (!s7 || !s7.eof() || s7.fail() || s7.bad() || i7 != -291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, hex, negative\n";
        }
        // hex, with base
        istringstream s8 {"0x123"};
        int i8;
        s8 >> hex >> i8;
        if (!s8 || !s8.eof() || s8.fail() || s8.bad() || i8 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, hex, with base\n";
        }
        // with leading whitespace
        istringstream s9 {"\t   \r\n  123"};
        int i9;
        s9 >> i9;
        if (!s9 || !s9.eof() || s9.fail() || s9.bad() || i9 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, with leading whitespace\n";
        }
        // fail, no exception
        istringstream s10 {"abc"};
        int i10 = 1;
        s10 >> i10;
        if (s10 || !s10.fail() || s10.eof() || s10.bad() ||
            s10.str() != "abc" || i10 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, fail, no exception\n";
        }
        // fail, exception
        istringstream s11 {"abc"};
        s11.exceptions(ios_base::failbit);
        int i11 = 1;
        try {
            s11 >> i11;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s11 || !s11.fail() || s11.eof() || s11.bad() ||
                s11.str() != "abc" || i11 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> int, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        istringstream s12 {""};
        int i12 = 1;
        s12 >> i12;
        if (s12 || !s12.fail() || !s12.eof() || s12.bad() ||
            s12.str() != "" || i12 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, eof, no exception\n";
        }
        // eof, exception
        istringstream s13 {""};
        s13.exceptions(ios_base::eofbit);
        int i13 = 1;
        try {
            s13 >> i13;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s13 || !s13.fail() || !s13.eof() || s13.bad() ||
                s13.str() != "" || i13 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> int, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, eof, exception, wrong exception type\n";
        }
        // Partial value
        istringstream s14 {"123abc"};
        int i14;
        s14 >> i14;
        char c1;
        s14 >> c1;
        if (!s14 || i14 != 123 || c1 != 'a')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, partial value\n";
        }
        // Mulitple values
        istringstream s15 {"123 \t 456 "};
        int i15, i16;
        s15 >> i15 >> i16;
        if (!s15 || s15.eof() || s15.fail() || s15.bad() || i15 != 123 ||
            i16 != 456)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, partial value\n";
        }
        // Exceed numeric limits, positive
        istringstream s16 {"4000000000"};
        int i17;
        s16 >> i17;
        if (!s16 || !s16.eof() || s16.fail() || s16.bad() || i17 != numeric_limits<int>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, exceed numeric limits, positive\n";
        }
        // Exceed numeric limits, negative
        istringstream s17 {"-4000000000"};
        int i18;
        s17 >> i18;
        if (!s17 || !s17.eof() || s17.fail() || s17.bad() || i18 != numeric_limits<int>::lowest())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> int, exceed numeric limits, negative\n";
        }
    }
    // operator >>, long
    {
        // dec, positive
        istringstream s1 {"123"};
        long l1;
        s1 >> l1;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || l1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, dec, positive\n";
        }
        // dec, negative
        istringstream s2 {"-123"};
        long l2;
        s2 >> l2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() || l2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, dec, negative\n";
        }
        // oct, positive
        istringstream s3 {"123"};
        long l3;
        s3 >> oct >> l3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || l3 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, oct, positive\n";
        }
        // oct, negative
        istringstream s4 {"-123"};
        long l4;
        s4 >> oct >> l4;
        if (!s4 || !s4.eof() || s4.fail() || s4.bad() || l4 != -83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, oct, negative\n";
        }
        // oct, with base
        istringstream s5 {"0123"};
        long l5;
        s5 >> oct >> l5;
        if (!s5 || !s5.eof() || s5.fail() || s5.bad() ||  l5 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, oct, with base\n";
        }
        // hex, positive
        istringstream s6 {"123"};
        long l6;
        s6 >> hex >> l6;
        if (!s6 || !s6.eof() || s6.fail() || s6.bad() || l6 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, hex, positive\n";
        }
        // hex, negative
        istringstream s7 {"-123"};
        long l7;
        s7 >> hex >> l7;
        if (!s7 || !s7.eof() || s7.fail() || s7.bad() || l7 != -291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, hex, negative\n";
        }
        // hex, with base
        istringstream s8 {"0x123"};
        long l8;
        s8 >> hex >> l8;
        if (!s8 || !s8.eof() || s8.fail() || s8.bad() || l8 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, hex, with base\n";
        }
        // with leading whitespace
        istringstream s9 {"\t   \r\n  123"};
        long l9;
        s9 >> l9;
        if (!s9 || !s9.eof() || s9.fail() || s9.bad() || l9 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, with leading whitespace\n";
        }
        // fail, no exception
        istringstream s10 {"abc"};
        long l10 = 1;
        s10 >> l10;
        if (s10 || !s10.fail() || s10.eof() || s10.bad() ||
            s10.str() != "abc" || l10 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, fail, no exception\n";
        }
        // fail, exception
        istringstream s11 {"abc"};
        s11.exceptions(ios_base::failbit);
        long l11 = 1;
        try {
            s11 >> l11;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s11 || !s11.fail() || s11.eof() || s11.bad() ||
                s11.str() != "abc" || l11 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> long, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        istringstream s12 {""};
        long l12 = 1;
        s12 >> l12;
        if (s12 || !s12.fail() || !s12.eof() || s12.bad() ||
            s12.str() != "" || l12 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, eof, no exception\n";
        }
        // eof, exception
        istringstream s13 {""};
        s13.exceptions(ios_base::eofbit);
        long l13 = 1;
        try {
            s13 >> l13;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s13 || !s13.fail() || !s13.eof() || s13.bad() ||
                s13.str() != "" || l13 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> long, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, eof, exception, wrong exception type\n";
        }
        // Partial value
        istringstream s14 {"123abc"};
        long l14;
        s14 >> l14;
        char c1;
        s14 >> c1;
        if (!s14 || l14 != 123 || c1 != 'a')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, partial value\n";
        }
        // Mulitple values
        istringstream s15 {"123 \t 456 "};
        long l15, l16;
        s15 >> l15 >> l16;
        if (!s15 || s15.eof() || s15.fail() || s15.bad() || l15 != 123 ||
            l16 != 456)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, partial value\n";
        }
        // Exceed numeric limits, positive
        istringstream s16 {"10000000000000000000"};
        long l17;
        s16 >> l17;
        if (!s16 || !s16.eof() || s16.fail() || s16.bad() || l17 != numeric_limits<long>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, exceed numeric limits, positive\n";
        }
        // Exceed numeric limits, negative
        istringstream s17 {"-10000000000000000000"};
        long l18;
        s17 >> l18;
        if (!s17 || !s17.eof() || s17.fail() || s17.bad() || l18 != numeric_limits<long>::lowest())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long, exceed numeric limits, negative\n";
        }
    }
    // operator >>, long long
    {
        // dec, positive
        istringstream s1 {"123"};
        long long ll1;
        s1 >> ll1;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || ll1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, dec, positive\n";
        }
        // dec, negative
        istringstream s2 {"-123"};
        long long ll2;
        s2 >> ll2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() || ll2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, dec, negative\n";
        }
        // oct, positive
        istringstream s3 {"123"};
        long long ll3;
        s3 >> oct >> ll3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || ll3 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, oct, positive\n";
        }
        // oct, negative
        istringstream s4 {"-123"};
        long long ll4;
        s4 >> oct >> ll4;
        if (!s4 || !s4.eof() || s4.fail() || s4.bad() || ll4 != -83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, oct, negative\n";
        }
        // oct, with base
        istringstream s5 {"0123"};
        long long ll5;
        s5 >> oct >> ll5;
        if (!s5 || !s5.eof() || s5.fail() || s5.bad() ||  ll5 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, oct, with base\n";
        }
        // hex, positive
        istringstream s6 {"123"};
        long long ll6;
        s6 >> hex >> ll6;
        if (!s6 || !s6.eof() || s6.fail() || s6.bad() || ll6 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, hex, positive\n";
        }
        // hex, negative
        istringstream s7 {"-123"};
        long long ll7;
        s7 >> hex >> ll7;
        if (!s7 || !s7.eof() || s7.fail() || s7.bad() || ll7 != -291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, hex, negative\n";
        }
        // hex, with base
        istringstream s8 {"0x123"};
        long long ll8;
        s8 >> hex >> ll8;
        if (!s8 || !s8.eof() || s8.fail() || s8.bad() || ll8 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, hex, with base\n";
        }
        // with leading whitespace
        istringstream s9 {"\t   \r\n  123"};
        long long ll9;
        s9 >> ll9;
        if (!s9 || !s9.eof() || s9.fail() || s9.bad() || ll9 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, with leading whitespace\n";
        }
        // fail, no exception
        istringstream s10 {"abc"};
        long long ll10 = 1;
        s10 >> ll10;
        if (s10 || !s10.fail() || s10.eof() || s10.bad() ||
            s10.str() != "abc" || ll10 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, fail, no exception\n";
        }
        // fail, exception
        istringstream s11 {"abc"};
        s11.exceptions(ios_base::failbit);
        long long ll11 = 1;
        try {
            s11 >> ll11;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s11 || !s11.fail() || s11.eof() || s11.bad() ||
                s11.str() != "abc" || ll11 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> long long, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        istringstream s12 {""};
        long long ll12 = 1;
        s12 >> ll12;
        if (s12 || !s12.fail() || !s12.eof() || s12.bad() ||
            s12.str() != "" || ll12 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, eof, no exception\n";
        }
        // eof, exception
        istringstream s13 {""};
        s13.exceptions(ios_base::eofbit);
        long long ll13 = 1;
        try {
            s13 >> ll13;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s13 || !s13.fail() || !s13.eof() || s13.bad() ||
                s13.str() != "" || ll13 != 0)
            {
                std::cout << "ll13 = " << ll13 << '\n';
                ++fail_count;
                std::cout << "FAILED istringstream operator >> long long, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, eof, exception, wrong exception type\n";
        }
        // Partial value
        istringstream s14 {"123abc"};
        long long ll14;
        s14 >> ll14;
        char c1;
        s14 >> c1;
        if (!s14 || ll14 != 123 || c1 != 'a')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, partial value\n";
        }
        // Mulitple values
        istringstream s15 {"123 \t 456 "};
        long long ll15, ll16;
        s15 >> ll15 >> ll16;
        if (!s15 || s15.eof() || s15.fail() || s15.bad() || ll15 != 123 ||
            ll16 != 456)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, partial value\n";
        }
        // Exceed numeric limits, positive
        istringstream s16 {"10000000000000000000"};
        long long ll17;
        s16 >> ll17;
        if (!s16 || !s16.eof() || s16.fail() || s16.bad() || ll17 != numeric_limits<long long>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, exceed numeric limits, positive\n";
        }
        // Exceed numeric limits, negative
        istringstream s17 {"-10000000000000000000"};
        long long ll18;
        s17 >> ll18;
        if (!s17 || !s17.eof() || s17.fail() || s17.bad() || ll18 != numeric_limits<long long>::lowest())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long long, exceed numeric limits, negative\n";
        }
    }
    // operator >>, unsigned short
    {
        // dec, positive
        istringstream s1 {"123"};
        unsigned short ush;
        s1 >> ush;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || ush != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, dec, positive\n";
        }
        // dec, negative
        istringstream s2 {"-123"};
        unsigned short ush2;
        s2 >> ush2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() ||
            ush2 != numeric_limits<unsigned short>::max() - 123 + 1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, dec, negative\n";
        }
        // oct, positive
        istringstream s3 {"123"};
        unsigned short ush3;
        s3 >> oct >> ush3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || ush3 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, oct, positive\n";
        }
        // oct, negative
        istringstream s4 {"-123"};
        unsigned short ush4;
        s4 >> oct >> ush4;
        if (!s4 || !s4.eof() || s4.fail() || s4.bad() ||
            ush4 != numeric_limits<unsigned short>::max() - 83 + 1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, oct, negative\n";
        }
        // oct, with base
        istringstream s5 {"0123"};
        unsigned short ush5;
        s5 >> oct >> ush5;
        if (!s5 || !s5.eof() || s5.fail() || s5.bad() ||  ush5 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, oct, with base\n";
        }
        // hex, positive
        istringstream s6 {"123"};
        unsigned short ush6;
        s6 >> hex >> ush6;
        if (!s6 || !s6.eof() || s6.fail() || s6.bad() || ush6 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, hex, positive\n";
        }
        // hex, negative
        istringstream s7 {"-123"};
        unsigned short ush7;
        s7 >> hex >> ush7;
        if (!s7 || !s7.eof() || s7.fail() || s7.bad() ||
            ush7 != numeric_limits<unsigned short>::max() - 291 + 1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, hex, negative\n";
        }
        // hex, with base
        istringstream s8 {"0x123"};
        unsigned short ush8;
        s8 >> hex >> ush8;
        if (!s8 || !s8.eof() || s8.fail() || s8.bad() || ush8 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, hex, with base\n";
        }
        // with leading whitespace
        istringstream s9 {"\t   \r\n  123"};
        unsigned short ush9;
        s9 >> ush9;
        if (!s9 || !s9.eof() || s9.fail() || s9.bad() || ush9 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, with leading whitespace\n";
        }
        // fail, no exception
        istringstream s10 {"abc"};
        unsigned short ush10 = 1;
        s10 >> ush10;
        if (s10 || !s10.fail() || s10.eof() || s10.bad() ||
            s10.str() != "abc" || ush10 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, fail, no exception\n";
        }
        // fail, exception
        istringstream s11 {"abc"};
        s11.exceptions(ios_base::failbit);
        unsigned short ush11 = 1;
        try {
            s11 >> ush11;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s11 || !s11.fail() || s11.eof() || s11.bad() ||
                s11.str() != "abc" || ush11 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> unsigned short, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        istringstream s12 {""};
        unsigned short ush12 = 1;
        s12 >> ush12;
        if (s12 || !s12.fail() || !s12.eof() || s12.bad() ||
            s12.str() != "" || ush12 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, eof, no exception\n";
        }
        // eof, exception
        istringstream s13 {""};
        s13.exceptions(ios_base::eofbit);
        unsigned short ush13 = 1;
        try {
            s13 >> ush13;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s13 || !s13.fail() || !s13.eof() || s13.bad() ||
                s13.str() != "" || ush13 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> unsigned short, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, eof, exception, wrong exception type\n";
        }
        // Partial value
        istringstream s14 {"123abc"};
        unsigned short ush14;
        s14 >> ush14;
        char c1;
        s14 >> c1;
        if (!s14 || ush14 != 123 || c1 != 'a')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, partial value\n";
        }
        // Mulitple values
        istringstream s15 {"123 \t 456 "};
        unsigned short ush15, ush16;
        s15 >> ush15 >> ush16;
        if (!s15 || s15.eof() || s15.fail() || s15.bad() || ush15 != 123 ||
            ush16 != 456)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, partial value\n";
        }
        // Exceed numeric limits, positive
        istringstream s16 {"70000"};
        unsigned short ush17;
        s16 >> ush17;
        if (!s16 || !s16.eof() || s16.fail() || s16.bad() || ush17 != numeric_limits<unsigned short>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, exceed numeric limits, positive\n";
        }
        // Exceed numeric limits, negative
        istringstream s17 {"-70000"};
        unsigned short ush18;
        s17 >> ush18;
        if (!s17 || !s17.eof() || s17.fail() || s17.bad() || ush18 != numeric_limits<unsigned short>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned short, exceed numeric limits, negative\n";
        }
    }
    // operator >>, unsigned int
    {
        // dec, positive
        istringstream s1 {"123"};
        unsigned int ush;
        s1 >> ush;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || ush != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, dec, positive\n";
        }
        // dec, negative
        istringstream s2 {"-123"};
        unsigned int ui2;
        s2 >> ui2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() ||
            ui2 != numeric_limits<unsigned int>::max() - 123 + 1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, dec, negative\n";
        }
        // oct, positive
        istringstream s3 {"123"};
        unsigned int ui3;
        s3 >> oct >> ui3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || ui3 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, oct, positive\n";
        }
        // oct, negative
        istringstream s4 {"-123"};
        unsigned int ui4;
        s4 >> oct >> ui4;
        if (!s4 || !s4.eof() || s4.fail() || s4.bad() ||
            ui4 != numeric_limits<unsigned int>::max() - 83 + 1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, oct, negative\n";
        }
        // oct, with base
        istringstream s5 {"0123"};
        unsigned int ui5;
        s5 >> oct >> ui5;
        if (!s5 || !s5.eof() || s5.fail() || s5.bad() ||  ui5 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, oct, with base\n";
        }
        // hex, positive
        istringstream s6 {"123"};
        unsigned int ui6;
        s6 >> hex >> ui6;
        if (!s6 || !s6.eof() || s6.fail() || s6.bad() || ui6 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, hex, positive\n";
        }
        // hex, negative
        istringstream s7 {"-123"};
        unsigned int ui7;
        s7 >> hex >> ui7;
        if (!s7 || !s7.eof() || s7.fail() || s7.bad() ||
            ui7 != numeric_limits<unsigned int>::max() - 291 + 1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, hex, negative\n";
        }
        // hex, with base
        istringstream s8 {"0x123"};
        unsigned int ui8;
        s8 >> hex >> ui8;
        if (!s8 || !s8.eof() || s8.fail() || s8.bad() || ui8 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, hex, with base\n";
        }
        // with leading whitespace
        istringstream s9 {"\t   \r\n  123"};
        unsigned int ui9;
        s9 >> ui9;
        if (!s9 || !s9.eof() || s9.fail() || s9.bad() || ui9 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, with leading whitespace\n";
        }
        // fail, no exception
        istringstream s10 {"abc"};
        unsigned int ui10 = 1;
        s10 >> ui10;
        if (s10 || !s10.fail() || s10.eof() || s10.bad() ||
            s10.str() != "abc" || ui10 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, fail, no exception\n";
        }
        // fail, exception
        istringstream s11 {"abc"};
        s11.exceptions(ios_base::failbit);
        unsigned int ui11 = 1;
        try {
            s11 >> ui11;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s11 || !s11.fail() || s11.eof() || s11.bad() ||
                s11.str() != "abc" || ui11 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> unsigned int, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        istringstream s12 {""};
        unsigned int ui12 = 1;
        s12 >> ui12;
        if (s12 || !s12.fail() || !s12.eof() || s12.bad() ||
            s12.str() != "" || ui12 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, eof, no exception\n";
        }
        // eof, exception
        istringstream s13 {""};
        s13.exceptions(ios_base::eofbit);
        unsigned int ui13 = 1;
        try {
            s13 >> ui13;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s13 || !s13.fail() || !s13.eof() || s13.bad() ||
                s13.str() != "" || ui13 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> unsigned int, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, eof, exception, wrong exception type\n";
        }
        // Partial value
        istringstream s14 {"123abc"};
        unsigned int ui14;
        s14 >> ui14;
        char c1;
        s14 >> c1;
        if (!s14 || ui14 != 123 || c1 != 'a')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, partial value\n";
        }
        // Mulitple values
        istringstream s15 {"123 \t 456 "};
        unsigned int ui15, ui16;
        s15 >> ui15 >> ui16;
        if (!s15 || s15.eof() || s15.fail() || s15.bad() || ui15 != 123 ||
            ui16 != 456)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, partial value\n";
        }
        // Exceed numeric limits, positive
        istringstream s16 {"5000000000"};
        unsigned int ui17;
        s16 >> ui17;
        if (!s16 || !s16.eof() || s16.fail() || s16.bad() || ui17 != numeric_limits<unsigned int>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, exceed numeric limits, positive\n";
        }
        // Exceed numeric limits, negative
        istringstream s17 {"-5000000000"};
        unsigned int ui18;
        s17 >> ui18;
        if (!s17 || !s17.eof() || s17.fail() || s17.bad() || ui18 != numeric_limits<unsigned int>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned int, exceed numeric limits, negative\n";
        }
    }
    // operator >>, unsigned long
    {
        // dec, positive
        istringstream s1 {"123"};
        unsigned long ush;
        s1 >> ush;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || ush != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, dec, positive\n";
        }
        // dec, negative
        istringstream s2 {"-123"};
        unsigned long ul2;
        s2 >> ul2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() ||
            ul2 != numeric_limits<unsigned long>::max() - 123 + 1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, dec, negative\n";
        }
        // oct, positive
        istringstream s3 {"123"};
        unsigned long ul3;
        s3 >> oct >> ul3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || ul3 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, oct, positive\n";
        }
        // oct, negative
        istringstream s4 {"-123"};
        unsigned long ul4;
        s4 >> oct >> ul4;
        if (!s4 || !s4.eof() || s4.fail() || s4.bad() ||
            ul4 != numeric_limits<unsigned long>::max() - 83 + 1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, oct, negative\n";
        }
        // oct, with base
        istringstream s5 {"0123"};
        unsigned long ul5;
        s5 >> oct >> ul5;
        if (!s5 || !s5.eof() || s5.fail() || s5.bad() ||  ul5 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, oct, with base\n";
        }
        // hex, positive
        istringstream s6 {"123"};
        unsigned long ul6;
        s6 >> hex >> ul6;
        if (!s6 || !s6.eof() || s6.fail() || s6.bad() || ul6 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, hex, positive\n";
        }
        // hex, negative
        istringstream s7 {"-123"};
        unsigned long ul7;
        s7 >> hex >> ul7;
        if (!s7 || !s7.eof() || s7.fail() || s7.bad() ||
            ul7 != numeric_limits<unsigned long>::max() - 291 + 1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, hex, negative\n";
        }
        // hex, with base
        istringstream s8 {"0x123"};
        unsigned long ul8;
        s8 >> hex >> ul8;
        if (!s8 || !s8.eof() || s8.fail() || s8.bad() || ul8 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, hex, with base\n";
        }
        // with leading whitespace
        istringstream s9 {"\t   \r\n  123"};
        unsigned long ul9;
        s9 >> ul9;
        if (!s9 || !s9.eof() || s9.fail() || s9.bad() || ul9 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, with leading whitespace\n";
        }
        // fail, no exception
        istringstream s10 {"abc"};
        unsigned long ul10 = 1;
        s10 >> ul10;
        if (s10 || !s10.fail() || s10.eof() || s10.bad() ||
            s10.str() != "abc" || ul10 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, fail, no exception\n";
        }
        // fail, exception
        istringstream s11 {"abc"};
        s11.exceptions(ios_base::failbit);
        unsigned long ul11 = 1;
        try {
            s11 >> ul11;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s11 || !s11.fail() || s11.eof() || s11.bad() ||
                s11.str() != "abc" || ul11 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> unsigned long, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        istringstream s12 {""};
        unsigned long ul12 = 1;
        s12 >> ul12;
        if (s12 || !s12.fail() || !s12.eof() || s12.bad() ||
            s12.str() != "" || ul12 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, eof, no exception\n";
        }
        // eof, exception
        istringstream s13 {""};
        s13.exceptions(ios_base::eofbit);
        unsigned long ul13 = 1;
        try {
            s13 >> ul13;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s13 || !s13.fail() || !s13.eof() || s13.bad() ||
                s13.str() != "" || ul13 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> unsigned long, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, eof, exception, wrong exception type\n";
        }
        // Partial value
        istringstream s14 {"123abc"};
        unsigned long ul14;
        s14 >> ul14;
        char c1;
        s14 >> c1;
        if (!s14 || ul14 != 123 || c1 != 'a')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, partial value\n";
        }
        // Mulitple values
        istringstream s15 {"123 \t 456 "};
        unsigned long ul15, ul16;
        s15 >> ul15 >> ul16;
        if (!s15 || s15.eof() || s15.fail() || s15.bad() || ul15 != 123 ||
            ul16 != 456)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, partial value\n";
        }
        // Exceed numeric limits, positive
        istringstream s16 {"20000000000000000000"};
        unsigned long ul17;
        s16 >> ul17;
        if (!s16 || !s16.eof() || s16.fail() || s16.bad() || ul17 != numeric_limits<unsigned long>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, exceed numeric limits, positive\n";
        }
        // Exceed numeric limits, negative
        istringstream s17 {"-20000000000000000000"};
        unsigned long ul18;
        s17 >> ul18;
        if (!s17 || !s17.eof() || s17.fail() || s17.bad() || ul18 != numeric_limits<unsigned long>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long, exceed numeric limits, negative\n";
        }
    }
    // operator >>, unsigned long long
    {
        // dec, positive
        istringstream s1 {"123"};
        unsigned long long ush;
        s1 >> ush;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || ush != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, dec, positive\n";
        }
        // dec, negative
        istringstream s2 {"-123"};
        unsigned long long ull2;
        s2 >> ull2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() ||
            ull2 != numeric_limits<unsigned long long>::max() - 123 + 1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, dec, negative\n";
        }
        // oct, positive
        istringstream s3 {"123"};
        unsigned long long ull3;
        s3 >> oct >> ull3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || ull3 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, oct, positive\n";
        }
        // oct, negative
        istringstream s4 {"-123"};
        unsigned long long ull4;
        s4 >> oct >> ull4;
        if (!s4 || !s4.eof() || s4.fail() || s4.bad() ||
            ull4 != numeric_limits<unsigned long long>::max() - 83 + 1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, oct, negative\n";
        }
        // oct, with base
        istringstream s5 {"0123"};
        unsigned long long ull5;
        s5 >> oct >> ull5;
        if (!s5 || !s5.eof() || s5.fail() || s5.bad() ||  ull5 != 83)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, oct, with base\n";
        }
        // hex, positive
        istringstream s6 {"123"};
        unsigned long long ull6;
        s6 >> hex >> ull6;
        if (!s6 || !s6.eof() || s6.fail() || s6.bad() || ull6 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, hex, positive\n";
        }
        // hex, negative
        istringstream s7 {"-123"};
        unsigned long long ull7;
        s7 >> hex >> ull7;
        if (!s7 || !s7.eof() || s7.fail() || s7.bad() ||
            ull7 != numeric_limits<unsigned long long>::max() - 291 + 1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, hex, negative\n";
        }
        // hex, with base
        istringstream s8 {"0x123"};
        unsigned long long ull8;
        s8 >> hex >> ull8;
        if (!s8 || !s8.eof() || s8.fail() || s8.bad() || ull8 != 291)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, hex, with base\n";
        }
        // with leading whitespace
        istringstream s9 {"\t   \r\n  123"};
        unsigned long long ull9;
        s9 >> ull9;
        if (!s9 || !s9.eof() || s9.fail() || s9.bad() || ull9 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, with leading whitespace\n";
        }
        // fail, no exception
        istringstream s10 {"abc"};
        unsigned long long ull10 = 1;
        s10 >> ull10;
        if (s10 || !s10.fail() || s10.eof() || s10.bad() ||
            s10.str() != "abc" || ull10 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, fail, no exception\n";
        }
        // fail, exception
        istringstream s11 {"abc"};
        s11.exceptions(ios_base::failbit);
        unsigned long long ull11 = 1;
        try {
            s11 >> ull11;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s11 || !s11.fail() || s11.eof() || s11.bad() ||
                s11.str() != "abc" || ull11 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> unsigned long long, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        istringstream s12 {""};
        unsigned long long ull12 = 1;
        s12 >> ull12;
        if (s12 || !s12.fail() || !s12.eof() || s12.bad() ||
            s12.str() != "" || ull12 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, eof, no exception\n";
        }
        // eof, exception
        istringstream s13 {""};
        s13.exceptions(ios_base::eofbit);
        unsigned long long ull13 = 1;
        try {
            s13 >> ull13;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s13 || !s13.fail() || !s13.eof() || s13.bad() ||
                s13.str() != "" || ull13 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> unsigned long long, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, eof, exception, wrong exception type\n";
        }
        // Partial value
        istringstream s14 {"123abc"};
        unsigned long long ull14;
        s14 >> ull14;
        char c1;
        s14 >> c1;
        if (!s14 || ull14 != 123 || c1 != 'a')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, partial value\n";
        }
        // Mulitple values
        istringstream s15 {"123 \t 456 "};
        unsigned long long ull15, ull16;
        s15 >> ull15 >> ull16;
        if (!s15 || s15.eof() || s15.fail() || s15.bad() || ull15 != 123 ||
            ull16 != 456)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, partial value\n";
        }
        // Exceed numeric limits, positive
        istringstream s16 {"20000000000000000000"};
        unsigned long long ull17;
        s16 >> ull17;
        if (!s16 || !s16.eof() || s16.fail() || s16.bad() || ull17 != numeric_limits<unsigned long long>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, exceed numeric limits, positive\n";
        }
        // Exceed numeric limits, negative
        istringstream s17 {"-20000000000000000000"};
        unsigned long long ull18;
        s17 >> ull18;
        if (!s17 || !s17.eof() || s17.fail() || s17.bad() || ull18 != numeric_limits<unsigned long long>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> unsigned long long, exceed numeric limits, negative\n";
        }
    }
    // operator >>, float
    {
        // no exponential, no decimal, positive
        istringstream s1 {"123"};
        float f1;
        s1 >> f1;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || f1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, no exponential, no decimal, positive\n";
        }
        // no exponential, no decimal, negative
        istringstream s2 {"-123"};
        float f2;
        s2 >> f2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() || f2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, no exponential, no decimal, negative\n";
        }
        // no exponential, decimal, positive
        istringstream s3 {"123.123"};
        float f3;
        s3 >> f3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || f3 != 123.123f)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, no exponential, decimal, positive\n";
        }
        // no exponential, decimal, negative
        istringstream s4 {"-123.123"};
        float f4;
        s4 >> f4;
        if (!s4 || !s4.eof() || s4.fail() || s4.bad() || f4 != -123.123f)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, no exponential, decimal, negative\n";
        }
        // exponential, no decimal, positive
        istringstream s5 {"123E2"};
        float f5;
        s5 >> f5;
        if (!s5 || !s5.eof() || s5.fail() || s5.bad() || f5 != 12300.f)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, exponential, no decimal, positive\n";
        }
        // exponential, no decimal, negative
        istringstream s6 {"-123E-2"};
        float f6;
        s6 >> f6;
        if (!s6 || !s6.eof() || s6.fail() || s6.bad() || f6 != -1.23f)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, exponential, no decimal, negative\n";
        }
        // exponential, decimal, positive
        istringstream s7 {"123.123e+02"};
        float f7;
        s7 >> f7;
        if (!s7 || !s7.eof() || s7.fail() || s7.bad() || f7 != 12312.3f)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, exponential, decimal, positive\n";
        }
        // exponential, decimal, negative
        istringstream s8 {"-123.123e-01"};
        float f8;
        s8 >> f8;
        if (!s8 || !s8.eof() || s8.fail() || s8.bad() || f8 != -12.3123f)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, exponential, decimal, negative\n";
        }
        // Two decimals
        istringstream s9 {"123.123.123"};
        float f9;
        s9 >> f9;
        if (!s9 || s9.eof() || s9.fail() || s9.bad() || f9 != 123.123f ||
            s9.peek() != '.')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, two decimals\n";
        }
        // Two exponentials
        istringstream s10 {"123E1E2"};
        float f10;
        s10 >> f10;
        if (!s10 || s10.eof() || s10.fail() || s10.bad() || f10 != 1230.f ||
            s10.peek() != 'E')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, two exponentials\n";
        }
        // Decimal in exponential
        istringstream s11 {"123E1.2"};
        float f11;
        s11 >> f11;
        if (!s11 || s11.eof() || s11.fail() || s11.bad() || f11 != 1230.f ||
            s11.peek() != '.')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, decimal in exponential\n";
        }
        // fail, no exception
        istringstream s12 {"abc"};
        float f12 = 1;
        s12 >> f12;
        if (s12 || !s12.fail() || s12.eof() || s12.bad() ||
            s12.str() != "abc" || f12 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, fail, no exception\n";
        }
        // fail, exception
        istringstream s13 {"abc"};
        s13.exceptions(ios_base::failbit);
        float f13 = 1;
        try {
            s13 >> f13;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s13 || !s13.fail() || s13.eof() || s13.bad() ||
                s13.str() != "abc" || f13 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> float, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        std::cout << "Test start\n";
        istringstream s14 {""};
        float f14 = 1;
        s14 >> f14;
        if (s14 || !s14.fail() || !s14.eof() || s14.bad() ||
            s14.str() != "" || f14 != 0.0f)
        {
            std::cout << "s14 == " << static_cast<bool>(s14) << '\n';
            std::cout << "s14.fail() == " << s14.fail() << '\n';
            std::cout << "s14.eof() == " << s14.eof() << '\n';
            std::cout << "s14.bad() == " << s14.bad() << '\n';
            std::cout << "s14.str() == " << s14.str() << '\n';
            std::cout << "f14 == " << f14 << '\n';
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, eof, no exception\n";
        }
        // eof, exception
        istringstream s15 {""};
        s15.exceptions(ios_base::eofbit);
        float f15 = 1;
        try {
            s15 >> f15;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s15 || !s15.fail() || !s15.eof() || s15.bad() ||
                s15.str() != "" || f15 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> float, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, eof, exception, wrong exception type\n";
        }
        // Exceed numeric limits, overflow, positive
        istringstream s16 {"1.0E39"};
        float f16;
        s16 >> f16;
        if (!s16 || !s16.eof() || s16.fail() || s16.bad() || f16 != numeric_limits<float>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, exceed numeric limits, overflow, positive\n";
        }
        // Exceed numeric limits, overflow, negative
        istringstream s17 {"-1.0E39"};
        float f17;
        s17 >> f17;
        if (!s17 || !s17.eof() || s17.fail() || s17.bad() || f17 != -numeric_limits<float>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float exceed numeric limits, overflow, negative\n";
        }
        // Exceed numeric limits, underflow, positive
        istringstream s18 {"1.0E-39"};
        float f18;
        s18 >> f18;
        if (!s18 || !s18.eof() || s18.fail() || s18.bad() || f18 != numeric_limits<float>::min())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, exceed numeric limits, underflow, positive\n";
        }
        // Exceed numeric limits, underflow, negative
        istringstream s19 {"-1.0E-39"};
        float f19;
        s19 >> f19;
        if (!s19 || !s19.eof() || s19.fail() || s19.bad() || f19 != -numeric_limits<float>::min())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, exceed numeric limits, underflow, negative\n";
        }
        // zero, simple
        istringstream s20 {"0"};
        float f20;
        s20 >> f20;
        if (!s20 || s20.fail() || !s20.eof() || s20.bad() || f20 != 0)
        {
            std::cout << "f20 == " << f20 << '\n';
            std::cout << "s20 == " << static_cast<bool>(s20) << '\n';
            std::cout << "s20.eof() == " << s20.eof() << '\n';
            std::cout << "s20.fail() == " << s20.fail() << '\n';
            std::cout << "s20.bad() == " << s20.bad() << '\n';
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, zero, simple\n";
        }
        // zero, complicated
        istringstream s21 {"0000000.000000000E-0000000000"};
        float f21;
        s21 >> f21;
        if (!s21 || s21.fail() || !s21.eof() || s21.bad() || f21 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, zero, complicated\n";
        }
        // Leading whitespace
        istringstream s22 {"  \t \r \n   123.123 \t  "};
        float f22;
        s22 >> f22;
        if (!s22 || s22.fail() || s22.eof() || s22.bad() || f22 != 123.123f)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, leading whitespace\n";
        }
        // Two values
        istringstream s23 {"123.123 \t \r  456.456"};
        float f23, f24;
        s23 >> f23 >> f24;
        if (!s23 || s23.fail() || !s23.eof() || s23.bad() || f23 != 123.123f ||
            f24 != 456.456f)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> float, two values\n";
        }
    }
    // operator >>, double
    {
        // no exponential, no decimal, positive
        istringstream s1 {"123"};
        double d1;
        s1 >> d1;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || d1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, no exponential, no decimal, positive\n";
        }
        // no exponential, no decimal, negative
        istringstream s2 {"-123"};
        double d2;
        s2 >> d2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() || d2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, no exponential, no decimal, negative\n";
        }
        // no exponential, decimal, positive
        istringstream s3 {"123.123"};
        double d3;
        s3 >> d3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || d3 != 123.123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, no exponential, decimal, positive\n";
        }
        // no exponential, decimal, negative
        istringstream s4 {"-123.123"};
        double d4;
        s4 >> d4;
        if (!s4 || !s4.eof() || s4.fail() || s4.bad() || d4 != -123.123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, no exponential, decimal, negative\n";
        }
        // exponential, no decimal, positive
        istringstream s5 {"123E2"};
        double d5;
        s5 >> d5;
        if (!s5 || !s5.eof() || s5.fail() || s5.bad() || d5 != 12300.f)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, exponential, no decimal, positive\n";
        }
        // exponential, no decimal, negative
        istringstream s6 {"-123E-2"};
        double d6;
        s6 >> d6;
        if (!s6 || !s6.eof() || s6.fail() || s6.bad() || d6 != -1.23)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, exponential, no decimal, negative\n";
        }
        // exponential, decimal, positive
        istringstream s7 {"123.123e+02"};
        double d7;
        s7 >> d7;
        if (!s7 || !s7.eof() || s7.fail() || s7.bad() || d7 != 12312.3)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, exponential, decimal, positive\n";
        }
        // exponential, decimal, negative
        istringstream s8 {"-123.123e-01"};
        double d8;
        s8 >> d8;
        if (!s8 || !s8.eof() || s8.fail() || s8.bad() || d8 != -12.3123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, exponential, decimal, negative\n";
        }
        // Two decimals
        istringstream s9 {"123.123.123"};
        double d9;
        s9 >> d9;
        if (!s9 || s9.eof() || s9.fail() || s9.bad() || d9 != 123.123 ||
            s9.peek() != '.')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, two decimals\n";
        }
        // Two exponentials
        istringstream s10 {"123E1E2"};
        double d10;
        s10 >> d10;
        if (!s10 || s10.eof() || s10.fail() || s10.bad() || d10 != 1230.f ||
            s10.peek() != 'E')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, two exponentials\n";
        }
        // Decimal in exponential
        istringstream s11 {"123E1.2"};
        double d11;
        s11 >> d11;
        if (!s11 || s11.eof() || s11.fail() || s11.bad() || d11 != 1230.f ||
            s11.peek() != '.')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, decimal in exponential\n";
        }
        // fail, no exception
        istringstream s12 {"abc"};
        double d12 = 1;
        s12 >> d12;
        if (s12 || !s12.fail() || s12.eof() || s12.bad() ||
            s12.str() != "abc" || d12 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, fail, no exception\n";
        }
        // fail, exception
        istringstream s13 {"abc"};
        s13.exceptions(ios_base::failbit);
        double d13 = 1;
        try {
            s13 >> d13;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s13 || !s13.fail() || s13.eof() || s13.bad() ||
                s13.str() != "abc" || d13 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> double, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        istringstream s14 {""};
        double d14 = 1;
        s14 >> d14;
        if (s14 || !s14.fail() || !s14.eof() || s14.bad() ||
            s14.str() != "" || d14 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, eof, no exception\n";
        }
        // eof, exception
        istringstream s15 {""};
        s15.exceptions(ios_base::eofbit);
        double d15 = 1;
        try {
            s15 >> d15;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s15 || !s15.fail() || !s15.eof() || s15.bad() ||
                s15.str() != "" || d15 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> double, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, eof, exception, wrong exception type\n";
        }
        // Exceed numeric limits, overflow, positive
        istringstream s16 {"1.0E309"};
        double d16;
        s16 >> d16;
        if (!s16 || !s16.eof() || s16.fail() || s16.bad() || d16 != numeric_limits<double>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, exceed numeric limits, overflow, positive\n";
        }
        // Exceed numeric limits, overflow, negative
        istringstream s17 {"-1.0E309"};
        double d17;
        s17 >> d17;
        if (!s17 || !s17.eof() || s17.fail() || s17.bad() || d17 != -numeric_limits<double>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double exceed numeric limits, overflow, negative\n";
        }
        // Exceed numeric limits, underflow, positive
        istringstream s18 {"1.0E-309"};
        double d18;
        s18 >> d18;
        if (!s18 || !s18.eof() || s18.fail() || s18.bad() || d18 != numeric_limits<double>::min())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, exceed numeric limits, underflow, positive\n";
        }
        // Exceed numeric limits, underflow, negative
        istringstream s19 {"-1.0E-309"};
        double d19;
        s19 >> d19;
        if (!s19 || !s19.eof() || s19.fail() || s19.bad() || d19 != -numeric_limits<double>::min())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, exceed numeric limits, underflow, negative\n";
        }
        // zero, simple
        istringstream s20 {"0"};
        double d20;
        s20 >> d20;
        if (!s20 || s20.fail() || !s20.eof() || s20.bad() || d20 != 0)
        {
            std::cout << "d20 == " << d20 << '\n';
            std::cout << "s20 == " << static_cast<bool>(s20) << '\n';
            std::cout << "s20.eof() == " << s20.eof() << '\n';
            std::cout << "s20.fail() == " << s20.fail() << '\n';
            std::cout << "s20.bad() == " << s20.bad() << '\n';
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, zero, simple\n";
        }
        // zero, complicated
        istringstream s21 {"0000000.000000000E-0000000000"};
        double d21;
        s21 >> d21;
        if (!s21 || s21.fail() || !s21.eof() || s21.bad() || d21 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, zero, complicated\n";
        }
        // Leading whitespace
        istringstream s22 {"  \t \r \n   123.123 \t  "};
        double d22;
        s22 >> d22;
        if (!s22 || s22.fail() || s22.eof() || s22.bad() || d22 != 123.123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, leading whitespace\n";
        }
        // Two values
        istringstream s23 {"123.123 \t \r  456.456"};
        double d23, d24;
        s23 >> d23 >> d24;
        if (!s23 || s23.fail() || !s23.eof() || s23.bad() || d23 != 123.123 ||
            d24 != 456.456)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> double, two values\n";
        }
    }
    // operator >>, long double
    {
        // no exponential, no decimal, positive
        istringstream s1 {"123"};
        long double ld1;
        s1 >> ld1;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || ld1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, no exponential, no decimal, positive\n";
        }
        // no exponential, no decimal, negative
        istringstream s2 {"-123"};
        long double ld2;
        s2 >> ld2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() || ld2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, no exponential, no decimal, negative\n";
        }
        // no exponential, decimal, positive
        istringstream s3 {"123.123"};
        long double ld3;
        s3 >> ld3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || ld3 != 123.123l)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, no exponential, decimal, positive\n";
        }
        // no exponential, decimal, negative
        istringstream s4 {"-123.123"};
        long double ld4;
        s4 >> ld4;
        if (!s4 || !s4.eof() || s4.fail() || s4.bad() || ld4 != -123.123l)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, no exponential, decimal, negative\n";
        }
        // exponential, no decimal, positive
        istringstream s5 {"123E2"};
        long double ld5;
        s5 >> ld5;
        if (!s5 || !s5.eof() || s5.fail() || s5.bad() || ld5 != 12300.f)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, exponential, no decimal, positive\n";
        }
        // exponential, no decimal, negative
        istringstream s6 {"-123E-2"};
        long double ld6;
        s6 >> ld6;
        if (!s6 || !s6.eof() || s6.fail() || s6.bad() || ld6 != -1.23l)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, exponential, no decimal, negative\n";
        }
        // exponential, decimal, positive
        istringstream s7 {"123.123e+02"};
        long double ld7;
        s7 >> ld7;
        if (!s7 || !s7.eof() || s7.fail() || s7.bad() || ld7 != 12312.3l)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, exponential, decimal, positive\n";
        }
        // exponential, decimal, negative
        istringstream s8 {"-123.123e-01"};
        long double ld8;
        s8 >> ld8;
        if (!s8 || !s8.eof() || s8.fail() || s8.bad() || ld8 != -12.312299999999999999475419620864613534649834036827l)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, exponential, decimal, negative\n";
        }
        // Two decimals
        istringstream s9 {"123.123.123"};
        long double ld9;
        s9 >> ld9;
        if (!s9 || s9.eof() || s9.fail() || s9.bad() || ld9 != 123.123l ||
            s9.peek() != '.')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, two decimals\n";
        }
        // Two exponentials
        istringstream s10 {"123E1E2"};
        long double ld10;
        s10 >> ld10;
        if (!s10 || s10.eof() || s10.fail() || s10.bad() || ld10 != 1230.f ||
            s10.peek() != 'E')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, two exponentials\n";
        }
        // Decimal in exponential
        istringstream s11 {"123E1.2"};
        long double ld11;
        s11 >> ld11;
        if (!s11 || s11.eof() || s11.fail() || s11.bad() || ld11 != 1230.f ||
            s11.peek() != '.')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, decimal in exponential\n";
        }
        // fail, no exception
        istringstream s12 {"abc"};
        long double ld12 = 1;
        s12 >> ld12;
        if (s12 || !s12.fail() || s12.eof() || s12.bad() ||
            s12.str() != "abc" || ld12 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, fail, no exception\n";
        }
        // fail, exception
        istringstream s13 {"abc"};
        s13.exceptions(ios_base::failbit);
        long double ld13 = 1;
        try {
            s13 >> ld13;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s13 || !s13.fail() || s13.eof() || s13.bad() ||
                s13.str() != "abc" || ld13 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> long double, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        istringstream s14 {""};
        long double ld14 = 1;
        s14 >> ld14;
        if (s14 || !s14.fail() || !s14.eof() || s14.bad() ||
            s14.str() != "" || ld14 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, eof, no exception\n";
        }
        // eof, exception
        istringstream s15 {""};
        s15.exceptions(ios_base::eofbit);
        long double ld15 = 1;
        try {
            s15 >> ld15;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s15 || !s15.fail() || !s15.eof() || s15.bad() ||
                s15.str() != "" || ld15 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> long double, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, eof, exception, wrong exception type\n";
        }
        // Exceed numeric limits, overflow, positive
        istringstream s16 {"1.0E4933"};
        long double ld16;
        s16 >> ld16;
        if (!s16 || !s16.eof() || s16.fail() || s16.bad() || ld16 != numeric_limits<long double>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, exceed numeric limits, overflow, positive\n";
        }
        // Exceed numeric limits, overflow, negative
        istringstream s17 {"-1.0E4933"};
        long double ld17;
        s17 >> ld17;
        if (!s17 || !s17.eof() || s17.fail() || s17.bad() || ld17 != -numeric_limits<long double>::max())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double exceed numeric limits, overflow, negative\n";
        }
        // Exceed numeric limits, underflow, positive
        istringstream s18 {"1.0E-4933"};
        long double ld18;
        s18 >> ld18;
        if (!s18 || !s18.eof() || s18.fail() || s18.bad() || ld18 != numeric_limits<long double>::min())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, exceed numeric limits, underflow, positive\n";
        }
        // Exceed numeric limits, underflow, negative
        istringstream s19 {"-1.0E-4933"};
        long double ld19;
        s19 >> ld19;
        if (!s19 || !s19.eof() || s19.fail() || s19.bad() || ld19 != -numeric_limits<long double>::min())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, exceed numeric limits, underflow, negative\n";
        }
        // zero, simple
        istringstream s20 {"0"};
        long double ld20;
        s20 >> ld20;
        if (!s20 || s20.fail() || !s20.eof() || s20.bad() || ld20 != 0)
        {
            std::cout << "ld20 == " << ld20 << '\n';
            std::cout << "s20 == " << static_cast<bool>(s20) << '\n';
            std::cout << "s20.eof() == " << s20.eof() << '\n';
            std::cout << "s20.fail() == " << s20.fail() << '\n';
            std::cout << "s20.bad() == " << s20.bad() << '\n';
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, zero, simple\n";
        }
        // zero, complicated
        istringstream s21 {"0000000.000000000E-0000000000"};
        long double ld21;
        s21 >> ld21;
        if (!s21 || s21.fail() || !s21.eof() || s21.bad() || ld21 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, zero, complicated\n";
        }
        // Leading whitespace
        istringstream s22 {"  \t \r \n   123.123 \t  "};
        long double ld22;
        s22 >> ld22;
        if (!s22 || s22.fail() || s22.eof() || s22.bad() || ld22 != 123.123l)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, leading whitespace\n";
        }
        // Two values
        istringstream s23 {"123.123 \t \r  456.456"};
        long double ld23, ld24;
        s23 >> ld23 >> ld24;
        if (!s23 || s23.fail() || !s23.eof() || s23.bad() || ld23 != 123.123l ||
            ld24 != 456.456l)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> long double, two values\n";
        }
    }
    // operator >>, bool
    {
        // noboolalpha, false, simple
        istringstream s1 {"0"};
        bool b1 = true;
        s1 >> b1;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || b1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, noboolalpha, false, simple\n";
        }
        // noboolalpha, false, complicated
        istringstream s2 {"000000000000000000"};
        bool b2 = true;
        s2 >> b2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() || b2)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, noboolalpha, false, complicated\n";
        }
        // noboolalpha, true, simple
        istringstream s3 {"1"};
        bool b3 = false;
        s3 >> b3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || !b3)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, noboolalpha, true, simple\n";
        }
        // noboolalpha, true, complicated
        istringstream s4 {"1983489343894"};
        bool b4 = false;
        s4 >> b4;
        if (s4 || !s4.eof() || !s4.fail() || s4.bad() || !b4)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, noboolalpha, true, complicated\n";
        }
        // boolalpha, false, simple
        istringstream s5 {"false"};
        bool b5 = true;
        s5 >> boolalpha >> b5;
        if (!s5 || s5.eof() || s5.fail() || s5.bad() || b5)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, boolalpha, false, simple\n";
        }
        // boolalpha, false, mixed case
        istringstream s6 {"FalSE"};
        bool b6 = true;
        s6 >> boolalpha >> b6;
        if (!s6 || s6.eof() || s6.fail() || s6.bad() || b6)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, boolalpha, false, mixed case\n";
        }
        // boolalpha, true, simple
        istringstream s7 {"true"};
        bool b7 = false;
        s7 >> boolalpha >> b7;
        if (!s7 || s7.eof() || s7.fail() || s7.bad() || !b7)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, boolalpha, true, simple\n";
        }
        // boolalpha, true, mixed case
        istringstream s8 {"tRUe"};
        bool b8 = true;
        s8 >> boolalpha >> b8;
        if (!s8 || s8.eof() || s8.fail() || s8.bad() || !b8)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, boolalpha, true, mixed case\n";
        }
        // with leading whitespace
        istringstream s9 {"\t   \r\n  1"};
        bool b9 = false;
        s9 >> b9;
        if (!s9 || !s9.eof() || s9.fail() || s9.bad() || !b9)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, with leading whitespace\n";
        }
        // fail, no exception
        istringstream s10 {"abc"};
        bool b10 = true;
        s10 >> b10;
        if (s10 || !s10.fail() || s10.eof() || s10.bad() ||
            s10.str() != "abc" || b10)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, fail, no exception\n";
        }
        // fail, exception
        istringstream s11 {"abc"};
        s11.exceptions(ios_base::failbit);
        bool b11 = true;
        try {
            s11 >> b11;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s11 || !s11.fail() || s11.eof() || s11.bad() ||
                s11.str() != "abc" || b11)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> bool, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        istringstream s12 {""};
        bool b12 = true;
        s12 >> b12;
        if (s12 || !s12.fail() || !s12.eof() || s12.bad() ||
            s12.str() != "" || b12)
        {
            std::cout << "b12 == " << b12 << '\n';
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, eof, no exception\n";
        }
        // eof, exception
        istringstream s13 {""};
        s13.exceptions(ios_base::eofbit);
        bool b13 = true;
        try {
            s13 >> b13;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s13 || !s13.fail() || !s13.eof() || s13.bad() ||
                s13.str() != "" || b13)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> bool, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, eof, exception, wrong exception type\n";
        }
        // Partial value, eof
        istringstream s14 {"fal"};
        bool b14 = true;
        s14 >> boolalpha >> b14;
        if (s14 || !s14.eof() || !s14.fail() || s14.bad() || b14)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, partial value, eof\n";
        }
        // Partial value, no eof
        istringstream s15 {"falgdrg"};
        bool b15 = true;
        s15 >> boolalpha >> b15;
        if (s15 || s15.eof() || !s15.fail() || s15.bad() || b15)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, partial value, no eof\n";
        }
        s15.clear();
        if (s15.peek() != 'g')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, partial value, no eof, wrong position\n";
        }
        // Two values
        istringstream s16 {"\t000  \r\n   false  blob"};
        bool b16 = true, b17 = true;
        s16 >> b16 >> boolalpha >> b17;
        if (!s16 || s16.fail() || s16.eof() || s16.bad() || b16 || b17)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> bool, two values\n";
        }
    }
    // operator >>, pointer
    {
        // no base, positive
        istringstream s1 {"123"};
        void* p1;
        s1 >> p1;
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() ||
            p1 != reinterpret_cast<void*>(0x123))
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> pointer, no base, positive\n";
        }
        // base, positive
        istringstream s2 {"0x123"};
        void* p2;
        s2 >> p2;
        if (!s2 || !s2.eof() || s2.fail() || s2.bad() ||
            p2 != reinterpret_cast<void*>(0x123))
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> pointer, base, positive\n";
        }
        // no base, negative
        istringstream s3 {"-123"};
        void* p3;
        s3 >> p3;
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() ||
            p3 != reinterpret_cast<void*>(numeric_limits<uintptr_t>::max() -
            0x123 + 1))
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> pointer, no base, negative\n";
        }
        // base, negative
        istringstream s4 {"-0x123"};
        void* p4;
        s4 >> p4;
        if (!s4 || !s4.eof() || s4.fail() || s4.bad() ||
            p4 != reinterpret_cast<void*>(numeric_limits<uintptr_t>::max() -
            0x123 + 1))
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> pointer, base, negative\n";
        }
        // fail, no exception
        istringstream s5 {"ghi"};
        void* v5 = reinterpret_cast<void*>(1);
        s5 >> v5;
        if (s5 || !s5.fail() || s5.eof() || s5.bad() || s5.str() != "ghi" ||
            v5 != nullptr)
        {
            std::cout << "v5 = " << v5 << '\n';
            ++fail_count;
            std::cout << "FAILED istringstream operator >> pointer, fail, no exception\n";
        }
        // fail, exception
        istringstream s6 {"ghi"};
        s6.exceptions(ios_base::failbit);
        void* v6 = reinterpret_cast<void*>(1);
        try {
            s6 >> v6;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> pointer, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s6 || !s6.fail() || s6.eof() || s6.bad() || s6.str() != "ghi" ||
                v6 != nullptr)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> pointer, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> pointer, fail, exception, wrong exception type\n";
        }
        // eof, no exception
        istringstream s7 {""};
        void* v7 = reinterpret_cast<void*>(1);
        s7 >> v7;
        if (s7 || !s7.fail() || !s7.eof() || s7.bad() || s7.str() != "" ||
            v7 != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> pointer, eof, no exception\n";
        }
        // eof, exception
        istringstream s8 {""};
        s8.exceptions(ios_base::eofbit);
        void* v8 = reinterpret_cast<void*>(1);
        try {
            s8 >> v8;
            ++fail_count;
            std::cout << "FAILED istringstream operator >> pointer, eof, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s8 || !s8.fail() || !s8.eof() || s8.bad() || s8.str() != "" ||
                v8 != 0)
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> pointer, eof, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> pointer, eof, exception, wrong exception type\n";
        }
        // Leading whitespace
        istringstream s9 {"    \t \r  123\n"};
        void* p9;
        s9 >> p9;
        if (!s9 || s9.eof() || s9.fail() || s9.bad() ||
            p9 != reinterpret_cast<void*>(0x123))
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> pointer, leading whitespace\n";
        }
        // Two values
        istringstream s10 {"\t0x123    \t\r  \n  0X456  "};
        void *p10, *p11;
        s10 >> p10 >> p11;
        if (!s10 || s10.eof() || s10.fail() || s10.bad() ||
            p10 != reinterpret_cast<void*>(0x123) ||
            p11 != reinterpret_cast<void*>(0x456))
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> pointer, two values\n";
        }
    }
    // operator >>, streambuf
    {
        // should work
        istringstream s1 {"Hello, world!"};
        ostringstream s2 {""};
        s1 >> s2.rdbuf();
        if (!s1 || !s1.eof() || s1.fail() || s1.bad() || s1.gcount() != 13 ||
            !s2 || s2.eof() || s2.fail() || s2.bad() ||
            s2.str() != "Hello, world!")
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> streambuf, should work\n";
        }
        // fail, no exception
        istringstream s3 {"Hello, world!"};
        istringstream s4 {""};
        s3 >> s4.rdbuf();
        if (s3 || s3.eof() || !s3.fail() || s3.bad() || s3.gcount() != 0 ||
            !s4 || s4.eof() || s4.fail() || s4.bad() || s4.str() != "")
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> streambuf, fail, no exception\n";
        }
        // fail, exception
        istringstream s5 {"Hello, world!"};
        s5.exceptions(ios_base::failbit);
        istringstream s6 {""};
        try {
            s5 >> s6.rdbuf();
            ++fail_count;
            std::cout << "FAILED istringstream operator >> streambuf, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (s5 || s5.eof() || !s5.fail() || s5.bad() || s5.gcount() != 0 ||
                !s6 || s6.eof() || s6.fail() || s6.bad() || s6.str() != "")
            {
                ++fail_count;
                std::cout << "FAILED istringstream operator >> streambuf, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator >> streambuf, fail, exception, wrong exception type\n";
        }    
    }
    // get
    {
        // Single character read. Success.
        istringstream s1 {"Hello, world"};
        int i1;
        i1 = s1.get();
        if (!s1 || s1.eof() || s1.fail() || s1.bad() || s1.gcount() != 1 ||
            i1 != 'H' || s1.peek() != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, single character read, success\n";
        }
        // Single character read. Failure.
        istringstream s2 {""};
        int i2;
        i2 = s2.get();
        if (s2 || !s2.eof() || !s2.fail() || s2.bad() || s2.gcount() != 0 ||
            i2 != -1)
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, single character read, failure\n";
        }
        // Single character reference read. Success.
        istringstream s3 {"Hello, world"};
        char c1 = 'a';
        s3.get(c1);
        if (!s3 || s3.eof() || s3.fail() || s3.bad() || s3.gcount() != 1 ||
            c1 != 'H' || s3.peek() != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, single character reference read, success\n";
        }
        // Single character reference read. Failure.
        istringstream s4 {""};
        char c2 = 'a';
        s4.get(c2);
        if (s4 || !s4.eof() || !s4.fail() || s4.bad() || s4.gcount() != 0 ||
            c2 != 'a')
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, single character reference read, failure\n";
        }
        // Buffer read, reach positive count
        istringstream s5 {"Hello, world!\nGoodbye."};
        char c3[5];
        s5.get(c3, 5);
        if (!s5 || s5.eof() || s5.fail() || s5.bad() || s5.gcount() != 4 ||
            strncmp(c3, "Hell", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, buffer read, reach positive count\n";
        }
        // Buffer read, zero count
        istringstream s6 {"Hello, world!\nGoodbye."};
        char c4[5] = {'a', 'b', 'c', 'd', '\0'};
        s6.get(c4, 0);
        if (s6 || s6.eof() || !s6.fail() || s6.bad() || s6.gcount() != 0 ||
            strncmp(c4, "abcd", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, buffer read, zero count\n";
        }
        // Buffer read, reach eof, some read
        istringstream s7 {"Hello, world!"};
        char c5[15];
        s7.get(c5, 15);
        if (!s7 || !s7.eof() || s7.fail() || s7.bad() || s7.gcount() != 13 ||
            strncmp(c5, "Hello, world!", 14) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, buffer read, reach eof, some read\n";
        }
        // Buffer read, reach eof, none read, positive count
        istringstream s8 {""};
        char c6[5] = {'a', 'b', 'c', 'd', '\0'};
        s8.get(c6, 5);
        if (s8 || !s8.eof() || !s8.fail() || s8.bad() || s8.gcount() != 0 ||
            strncmp(c6, "\0bcd", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, buffer read, reach eof, none read, positive count\n";
        }
        // Buffer read, reach eof, none read, zero count
        istringstream s9 {""};
        char c7[5] = {'a', 'b', 'c', 'd', '\0'};
        s9.get(c7, 0);
        if (s9 || s9.eof() || !s9.fail() || s9.bad() || s9.gcount() != 0 ||
            strncmp(c7, "abcd", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, buffer read, reach eof, none read, positive count\n";
        }
        // Buffer read, reach delimiter, default
        istringstream s10 {"Hello, world!\nGoodbye."};
        char c8[15];
        s10.get(c8, 15);
        if (!s10 || s10.eof() || s10.fail() || s10.bad() ||
            s10.gcount() != 13 || s10.peek() != '\n' ||
            strncmp(c8, "Hello, world!", 14) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, buffer read, reach delimiter, default\n";
        }
        // Buffer read, reach delimiter, custom
        istringstream s11 {"Hello, world!\nGoodbye."};
        char c9[20];
        s11.get(c9, 20, 'b');
        if (!s11 || s11.eof() || s11.fail() || s11.bad() ||
            s11.gcount() != 18 || s11.peek() != 'b' ||
            strncmp(c9, "Hello, world!\nGood", 19) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, buffer read, reach delimiter, custom\n";
        }
        // streambuf read, reach eof, some read
        istringstream s12 {"Hello, world!"};
        ostringstream os1 {""};
        s12.get(*os1.rdbuf());
        if (!s12 || !s12.eof() || s12.fail() || s12.bad() ||
            s12.gcount() != 13 || os1.str() != "Hello, world!")
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, streambuf read, reach eof, some read\n";
        }
        // streambuf read, reach eof, none read
        istringstream s13 {""};
        ostringstream os2 {""};
        s13.get(*os2.rdbuf());
        if (s13 || !s13.eof() || !s13.fail() || s13.bad() ||
            s13.gcount() != 0 || os2.str() != "")
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, streambuf read, reach eof, none read\n";
        }
        // streambuf read, reach delimiter, default
        istringstream s14 {"Hello, world!\nGoodbye."};
        ostringstream os3 {""};
        s14.get(*os3.rdbuf());
        if (!s14 || s14.eof() || s14.fail() || s14.bad() ||
            s14.gcount() != 13 || s14.peek() != '\n' ||
            os3.str() != "Hello, world!")
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, streambuf read, reach delimiter, default\n";
        }
        // streambuf read, reach delimiter, custom
        istringstream s15 {"Hello, world!\nGoodbye."};
        ostringstream os4 {""};
        s15.get(*os4.rdbuf(), 'b');
        if (!s15 || s15.eof() || s15.fail() || s15.bad() ||
            s15.gcount() != 18 || s15.peek() != 'b' ||
            os4.str() != "Hello, world!\nGood")
        {
            ++fail_count;
            std::cout << "FAILED istringstream get, streambuf read, reach delimiter, custom\n";
        }
    }
    // peek
    {
        // Basic
        istringstream s1 {"Hello"};
        if (s1.peek() != 'H' || !s1 || s1.eof() || s1.fail() || s1.bad() ||
            s1.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream peek, basic\n";
        }
        // Repeated
        istringstream s2 {"Hello"};
        if (s2.peek() != 'H' || s2.peek() != 'H' || !s2 || s2.eof() ||
            s2.fail() || s2.bad() || s2.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream peek, repeated\n";
        }
        // Repeated with read
        istringstream s3 {"Hello"};
        char c1 = s3.peek();
        s3.get();
        if (c1 != 'H' || s3.peek() != 'e' || !s3 || s3.eof() ||
            s3.fail() || s3.bad() || s3.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream peek, repeated with read\n";
        }
        // Empty
        istringstream s4 {};
        if (!istringstream::traits_type::eq_int_type(s4.peek(),
            istringstream::traits_type::eof()) || !s4 || !s4.eof() ||
            s4.fail() || s4.bad() || s4.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream peek, empty\n";
        }
        // End reached
        istringstream s5 {"Hello"};
        char c2[6];
        s5.get(c2, 6); 
        if (!istringstream::traits_type::eq_int_type(s5.peek(),
            istringstream::traits_type::eof()) || !s5 || !s5.eof() ||
            s5.fail() || s5.bad() || s5.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream peek, end reached\n";
        }
    }
    // unget
    {
        // Basic
        istringstream s1 {"Hello"};
        s1.get();
        s1.unget();
        if (s1.gcount() != 0 || s1.peek() != 'H' || !s1 || s1.eof() ||
            s1.fail() || s1.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream unget, basic\n";
        }
        // Repeated
        istringstream s2 {"Hello"};
        s2.get(); s2.get(); s2.get();
        s2.unget(); s2.unget(); 
        if (s2.gcount() != 0 || s2.peek() != 'e' || !s2 || s2.eof() ||
            s2.fail() || s2.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream unget, repeated\n";
        }
        // At eof
        istringstream s3 {"He"};
        s3.get(); s3.get();
        s3.peek();
        s3.unget();
        if (s3.gcount() != 0 || s3.peek() != 'e' || !s3 || s3.eof() ||
            s3.fail() || s3.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream unget, at eof\n";
        }
        // At start
        istringstream s4 {"Hello"};
        s4.unget();
        if (s4 || s4.eof() || !s4.fail() || !s4.bad() || s4.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream unget, at start\n";
        }
        // Empty
        istringstream s5 {""};
        s5.unget();
        if (s5 || s5.eof() || !s5.fail() || !s5.bad() || s5.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream unget, empty\n";
        }
    }
    // putback
    {
        // Non-modifying
        istringstream s1 {"Hello"};
        s1.get();
        s1.putback('H');
        if (s1.gcount() != 0 || s1.peek() != 'H' || !s1 || s1.eof() ||
            s1.fail() || s1.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream putback, non-modifying\n";
        }
        // Modifying
        istringstream s2 {"Hello"};
        s2.get();
        s2.putback('Y');
        if (s2.gcount() != 0 || s2 || s2.eof() || !s2.fail() || !s2.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream putback, modifying\n";
        }
        // At start
        istringstream s3 {"Hello"};
        s3.putback('Y');
        if (s3.gcount() != 0 || s3 || s3.eof() || !s3.fail() || !s3.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream putback, at start\n";
        }
        // Empty
        istringstream s4 {""};
        s4.putback('Y');
        if (s4.gcount() != 0 || s4 || s4.eof() || !s4.fail() || !s4.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream putback, empty\n";
        }
    }
    // getline
    {
        // Reach positive count
        istringstream s1 {"Hello, world!\nGoodbye."};
        char c1[5];
        s1.getline(c1, 5);
        if (s1 || s1.eof() || !s1.fail() || s1.bad() || s1.gcount() != 4 ||
            strncmp(c1, "Hell", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream getline, reach positive count\n";
        }
        // Zero count
        istringstream s2 {"Hello, world!\nGoodbye."};
        char c2[5] = {'a', 'b', 'c', 'd', '\0'};
        s2.getline(c2, 0);
        if (s2 || s2.eof() || !s2.fail() || s2.bad() || s2.gcount() != 0 ||
            strncmp(c2, "abcd", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream getline, zero count\n";
        }
        // Reach eof, some read
        istringstream s3 {"Hello, world!"};
        char c3[15];
        s3.getline(c3, 15);
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || s3.gcount() != 13 ||
            strncmp(c3, "Hello, world!", 14) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream getline, reach eof, some read\n";
        }
        // Reach eof, none read, positive count
        istringstream s4 {""};
        char c4[5] = {'a', 'b', 'c', 'd', '\0'};
        s4.getline(c4, 5);
        if (s4 || !s4.eof() || !s4.fail() || s4.bad() || s4.gcount() != 0 ||
            strncmp(c4, "\0bcd", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream getline, reach eof, none read, positive count\n";
        }
        // Reach eof, none read, zero count
        istringstream s5 {""};
        char c5[5] = {'a', 'b', 'c', 'd', '\0'};
        s5.getline(c5, 0);
        if (s5 || s5.eof() || !s5.fail() || s5.bad() || s5.gcount() != 0 ||
            strncmp(c5, "abcd", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream getline, reach eof, none read, positive count\n";
        }
        // Reach delimiter, default, buffer larger
        istringstream s6 {"Hello, world!\nGoodbye."};
        char c6[15];
        s6.getline(c6, 15);
        if (!s6 || s6.eof() || s6.fail() || s6.bad() || s6.gcount() != 14 ||
            s6.peek() != 'G' || strncmp(c6, "Hello, world!", 14) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream getline, reach delimiter, default, buffer larger\n";
        }
        // Reach delimiter, default, buffer exact
        istringstream s7 {"Hello, world!\nGoodbye."};
        char c7[14];
        s7.getline(c7, 14);
        if (!s7 || s7.eof() || s7.fail() || s7.bad() || s7.gcount() != 14 ||
            s7.peek() != 'G' || strncmp(c7, "Hello, world!", 14) != 0)
        {
            std::cout << "s7.fail() == " << s7.fail() << '\n';
            ++fail_count;
            std::cout << "FAILED istringstream getline, reach delimiter, default, buffer exact\n";
        }
        // Reach delimiter, custom
        istringstream s8 {"Hello, world!\nGoodbye."};
        char c8[20];
        s8.getline(c8, 20, 'b');
        if (!s8 || s8.eof() || s8.fail() || s8.bad() || s8.gcount() != 19 ||
            s8.peek() != 'y' || strncmp(c8, "Hello, world!\nGood", 19) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream getline, reach delimiter, custom\n";
        }
        // Empty line, with second line
        istringstream s9 {"\nGoodbye."};
        char c9[5] = {'a', 'b', 'c', 'd', '\0'};
        s9.getline(c9, 4);
        if (!s9 || s9.eof() || s9.fail() || s9.bad() || s9.gcount() != 1 ||
            s9.peek() != 'G' || strncmp(c9, "\0bcd", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream getline, empty line, with second line\n";
        }
        // Empty line, no second line
        istringstream s10 {"\n"};
        char c10[5] = {'a', 'b', 'c', 'd', '\0'};
        s10.getline(c10, 4);
        if (!s10 || s10.eof() || s10.fail() || s10.bad() || s10.gcount() != 1 ||
            !istringstream::traits_type::eq_int_type(s10.peek(),
            istringstream::traits_type::eof()) || strncmp(c10, "\0bcd", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream getline, empty line, no second line\n";
        }
    }
    // ignore
    {
        // default (count is 1, delim is ignored)
        istringstream s1 {"Hello, world!"};
        s1.ignore();
        if (!s1 || s1.eof() || s1.fail() || s1.bad() || s1.gcount() != 1 ||
            s1.peek() != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream ignore, default\n";
        }
        // reach count, no delim
        istringstream s2 {"Hello, world!"};
        s2.ignore(8);
        if (!s2 || s2.eof() || s2.fail() || s2.bad() || s2.gcount() != 8 ||
            s2.peek() != 'o')
        {
            ++fail_count;
            std::cout << "FAILED istringstream ignore, reach count, no delim\n";
        }
        // reach eof, no delim
        istringstream s3 {"Hello, world!"};
        s3.ignore(30);
        if (!s3 || !s3.eof() || s3.fail() || s3.bad() || s3.gcount() != 13)
        {
            ++fail_count;
            std::cout << "FAILED istringstream ignore, reach eof, no delim\n";
        }
        // zero count, no delim
        istringstream s4 {"Hello, world!"};
        s4.ignore(0);
        if (!s4 || s4.eof() || s4.fail() || s4.bad() || s4.gcount() != 0 ||
            s4.peek() != 'H')
        {
            ++fail_count;
            std::cout << "FAILED istringstream ignore, zero count, no delim\n";
        }
        // empty string, no delim
        istringstream s5 {""};
        s5.ignore();
        if (!s5 || !s5.eof() || s5.fail() || s5.bad() || s5.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream ignore, empty string, no delim\n";
        }
        // reach count, with delim
        istringstream s6 {"Hello, world!"};
        s6.ignore(8, 'd');
        if (!s6 || s6.eof() || s6.fail() || s6.bad() || s6.gcount() != 8 ||
            s6.peek() != 'o')
        {
            
            ++fail_count;
            std::cout << "FAILED istringstream ignore, reach count, with delim\n";
        }
        // reach delim
        istringstream s7 {"Hello, world!"};
        s7.ignore(8, 'o');
        if (!s7 || s7.eof() || s7.fail() || s7.bad() || s7.gcount() != 5 ||
            s7.peek() != ',')
        {
            ++fail_count;
            std::cout << "FAILED istringstream ignore, reach delim\n";
        }
        // reach eof, with delim
        istringstream s8 {"Hello, world!"};
        s8.ignore(30, 'b');
        if (!s8 || !s8.eof() || s8.fail() || s8.bad() || s8.gcount() != 13)
        {
            ++fail_count;
            std::cout << "FAILED istringstream ignore, reach eof, with delim\n";
        }
        // zero count, with delim
        istringstream s9 {"Hello, world!"};
        s9.ignore(0, 'l');
        if (!s9 || s9.eof() || s9.fail() || s9.bad() || s9.gcount() != 0 ||
            s9.peek() != 'H')
        {
            ++fail_count;
            std::cout << "FAILED istringstream ignore, zero count, with delim\n";
        }
        // empty string, with delim
        istringstream s10 {""};
        s10.ignore();
        if (!s10 || !s10.eof() || s10.fail() || s10.bad() || s10.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream ignore, empty string, with delim\n";
        }
        // leading whitespace, with delim
        istringstream s11 {"    Hello, world!"};
        s11.ignore(5, 'l');
        if (!s11 || s11.eof() || s11.fail() || s11.bad() || s11.gcount() != 5 ||
            s11.peek() != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream ignore, leading whitespace, with delim\n";
        }
    }
    // read
    {
        // basic
        istringstream s1 {"Hello, world!"};
        char c1[10];
        s1.read(c1, 10);
        if (!s1 || s1.eof() || s1.fail() || s1.bad() || s1.gcount() != 10 ||
            s1.peek() != 'l' || strncmp(c1, "Hello, wor", 10) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream read, basic\n";
        }
        // reach eof
        istringstream s2 {"Hello, world!"};
        char c2[15] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
            'l', 'm', 'n', 'o'};
        s2.read(c2, 15);
        if (s2 || !s2.eof() || !s2.fail() || s2.bad() || s2.gcount() != 13 ||
            strncmp(c2, "Hello, world!no", 15) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream read, reach eof\n";
        }
        // zero count
        istringstream s3 {"Hello, world!"};
        char c3[5] = {'a', 'b', 'c', 'd', 'e'};
        s3.read(c3, 0);
        if (!s3 || s3.eof() || s3.fail() || s3.bad() || s3.gcount() != 0 ||
            strncmp(c3, "abcde", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream read, zero count\n";
        }
        // empty string
        istringstream s4 {""};
        char c4[5] = {'a', 'b', 'c', 'd', 'e'};
        s4.read(c4, 5);
        if (s4 || !s4.eof() || !s4.fail() || s4.bad() || s4.gcount() != 0 ||
            strncmp(c4, "abcde", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream read, empty string\n";
        }
        // zero count, empty string
        istringstream s5 {""};
        char c5[5] = {'a', 'b', 'c', 'd', 'e'};
        s5.read(c5, 0);
        if (!s5 || s5.eof() || s5.fail() || s5.bad() || s5.gcount() != 0 ||
            strncmp(c5, "abcde", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream read, zero count, empty string\n";
        }
    }
    // readsome (the string contents is always immediately available)
    {
        // basic
        istringstream s1 {"Hello, world!"};
        char c1[10];
        s1.readsome(c1, 10);
        if (!s1 || s1.eof() || s1.fail() || s1.bad() || s1.gcount() != 10 ||
            s1.peek() != 'l' || strncmp(c1, "Hello, wor", 10) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream readsome, basic\n";
        }
        // reach eof
        istringstream s2 {"Hello, world!"};
        char c2[15] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
            'l', 'm', 'n', 'o'};
        s2.readsome(c2, 15);
        if (!s2 || s2.eof() || s2.fail() || s2.bad() || s2.gcount() != 13 ||
            strncmp(c2, "Hello, world!no", 15) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream readsome, reach eof\n";
        }
        // zero count
        istringstream s3 {"Hello, world!"};
        char c3[5] = {'a', 'b', 'c', 'd', 'e'};
        s3.readsome(c3, 0);
        if (!s3 || s3.eof() || s3.fail() || s3.bad() || s3.gcount() != 0 ||
            strncmp(c3, "abcde", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream readsome, zero count\n";
        }
        // empty string
        istringstream s4 {""};
        char c4[5] = {'a', 'b', 'c', 'd', 'e'};
        s4.readsome(c4, 5);
        if (!s4 || s4.eof() || s4.fail() || s4.bad() || s4.gcount() != 0 ||
            strncmp(c4, "abcde", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream readsome, empty string\n";
        }
        // zero count, empty string
        istringstream s5 {""};
        char c5[5] = {'a', 'b', 'c', 'd', 'e'};
        s5.readsome(c5, 0);
        if (!s5 || s5.eof() || s5.fail() || s5.bad() || s5.gcount() != 0 ||
            strncmp(c5, "abcde", 5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream readsome, zero count, empty string\n";
        }
    }
    // gcount
    {
        // move constructor
        istringstream s1 {"Hello, world!"};
        s1.ignore(5);
        istringstream s2 {move(s1)};
        if (s2.gcount() != 5)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, move constructor\n";
        }
        // swap
        istringstream s3 {"Hello, world!"};
        s3.ignore(5);
        istringstream s4 {"Goodbye"};
        s4.ignore(3);
        s3.swap(s4);
        if (s3.gcount() != 3 || s4.gcount() != 5)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, swap\n";
        }
        // get
        istringstream s5 {"Hello, world!"};
        char c1[5];
        s5.get(c1, 5);
        if (s5.gcount() != 4)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, get\n";
        }
        // getline
        istringstream s6 {"Hello, world!\nGoodbye"};
        char c2[15];
        s6.getline(c2, 15);
        if (s6.gcount() != 14)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, getline\n";
        }
        // ignore
        istringstream s7 {"Hello, world!"};
        s7.ignore(5);
        if (s7.gcount() != 5)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, ignore\n";
        }
        // read
        istringstream s8 {"Hello, world!"};
        char c3[5];
        s8.read(c3, 5);
        if (s8.gcount() != 5)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, read\n";
        }
        // readsome
        istringstream s9 {"Hello, world!"};
        char c4[5];
        s9.readsome(c4, 5);
        if (s9.gcount() != 5)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, readsome\n";
        }
        // operator>>
        istringstream s10 {"Hello, world!"};
        ostringstream o1 {};
        s10 >> o1.rdbuf();
        if (s10.gcount() != 13)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, operator>>\n";
        }
        // default constructor
        istringstream s11 {};
        if (s11.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, default constructor\n";
        }
        // string constructor
        istringstream s12 {"Hello, world!"};
        if (s12.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, string constructor\n";
        }
        // putback
        istringstream s13 {"Hello, world!"};
        s13.ignore(5);
        s13.putback('o');
        if (s13.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, putback\n";
        }
        // unget
        istringstream s14 {"Hello, world!"};
        s14.ignore(5);
        s14.unget();
        if (s14.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, unget\n";
        }
        // peek
        istringstream s15 {"Hello, world!"};
        s15.ignore(5);
        s15.peek();
        if (s15.gcount() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream gcount, peek\n";
        }
    }
    // tellg
    {
        // start
        istringstream s1 {"Hello, world!"};
        if (s1.tellg() != 0)
        {
            ++fail_count;
            std::cout << "FAILED istringstream tellg, start\n";
        }
        // middle
        istringstream s2 {"Hello, world!"};
        s2.ignore(5);
        if (s2.tellg() != 5)
        {
            ++fail_count;
            std::cout << "FAILED istringstream tellg, middle\n";
        }
        // eof
        istringstream s3 {"Hello, world!"};
        s3.ignore(15);
        if (s3.tellg() != istringstream::pos_type{-1})
        {
            ++fail_count;
            std::cout << "FAILED istringstream tellg, eof\n";
        }
    }
    // seekg
    {
        // absolute, start
        istringstream s1 {"Hello, world!"};
        s1.ignore(5);
        s1.seekg(0);
        if (s1.tellg() != 0 || s1.peek() != 'H')
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, absolute, start\n";
        }
        // absolute, middle
        istringstream s2 {"Hello, world!"};
        s2.seekg(5);
        if (s2.tellg() != 5 || s2.peek() != ',')
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, absolute, middle\n";
        }
        // absolute, past end
        istringstream s3 {"Hello, world!"};
        s3.seekg(20);
        if (s3.tellg() != istringstream::pos_type{-1} || s3 || s3.eof() ||
            !s3.fail() || s3.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, absolute, past end\n";
        }
        // relative beginning, start
        istringstream s4 {"Hello, world!"};
        s4.ignore(5);
        s4.seekg(0, ios_base::beg);
        if (s4.tellg() != 0 || s4.peek() != 'H')
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, relative beginning, start\n";
        }
        // relative beginning, middle
        istringstream s5 {"Hello, world!"};
        s5.seekg(5, ios_base::beg);
        if (s5.tellg() != 5 || s5.peek() != ',')
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, relative beginning, middle\n";
        }
        // relative beginning, past end
        istringstream s6 {"Hello, world!"};
        s6.seekg(20, ios_base::beg);
        if (s6.tellg() != istringstream::pos_type{-1} || s6 || s6.eof() ||
            !s6.fail() || s6.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, relative beginning, past end\n";
        }
        // relative current, start
        istringstream s7 {"Hello, world!"};
        s7.ignore(5);
        s7.seekg(-5, ios_base::cur);
        if (s7.tellg() != 0 || s4.peek() != 'H')
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, relative current, start\n";
        }
        // relative current, middle
        istringstream s8 {"Hello, world!"};
        s8.ignore(5);
        s8.seekg(5, ios_base::cur);
        if (s8.tellg() != 10 || s8.peek() != 'l')
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, relative current, middle\n";
        }
        // relative current, past end
        istringstream s9 {"Hello, world!"};
        s9.ignore(5);
        s9.seekg(10, ios_base::cur);
        if (s9.tellg() != istringstream::pos_type{-1} || s9 || s9.eof() ||
            !s9.fail() || s9.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, relative current, past end\n";
        }
        // relative end, start
        istringstream s10 {"Hello, world!"};
        s10.seekg(-13, ios_base::end);
        if (s10.tellg() != 0 || s10.peek() != 'H')
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, relative end, start\n";
        }
        // relative end, middle
        istringstream s11 {"Hello, world!"};
        s11.seekg(-5, ios_base::end);
        if (s11.tellg() != 8 || s11.peek() != 'o')
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, relative end, middle\n";
        }
        // relative end, past end
        istringstream s12 {"Hello, world!"};
        s12.seekg(1, ios_base::end);
        if (s12.tellg() != istringstream::pos_type{-1} || s12 || s12.eof() ||
            !s12.fail() || s12.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, relative end, past end\n";
        }
        // absolute, exact end
        istringstream s13 {"Hello, world!"};
        s13.seekg(13);
        if (s13.tellg() != 13 || !s13 || s13.eof() || s13.fail() || s13.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, absolute, exact end\n";
        }
        // relative end, exact end
        istringstream s14 {"Hello, world!"};
        s14.seekg(13, ios_base::beg);
        if (s14.tellg() != 13 || !s14 || s14.eof() || s14.fail() || s14.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, relative beginning, exact end\n";
        }
        // relative end, exact end
        istringstream s15 {"Hello, world!"};
        s15.ignore(5);
        s15.seekg(8, ios_base::cur);
        if (s15.tellg() != 13 || !s15 || s15.eof() || s15.fail() || s15.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, relative current, exact end\n";
        }
        // relative end, exact end
        istringstream s16 {"Hello, world!"};
        s16.seekg(0, ios_base::end);
        if (s16.tellg() != 13 || !s16 || s16.eof() || s16.fail() || s16.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream seekg, relative end, exact end\n";
        }
    }
    // sync (doesn't do anything)
    {
        // basic
        istringstream s1 {"Hello, world!"};
        s1.sync();
        if (!s1 || s1.eof() || s1.fail() || s1.bad() || s1.gcount() != 0 ||
            s1.peek() != 'H')
        {
            ++fail_count;
            std::cout << "FAILED istringstream sync, basic\n";
        }
    }
    // operator>>, non member
    {
        // Extract single character, CharT, skipws
        istringstream s1 {" Hello"};
        istringstream::char_type c1;
        s1 >> c1;
        if (c1 != 'H' || !s1 || s1.peek() != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>>, single character, CharT, skipws\n";
        }
        // Extract single character, CharT, noskipws
        istringstream s2 {" Hello"};
        istringstream::char_type c2;
        s2 >> noskipws >> c2;
        if (c2 != ' ' || !s2 || s2.peek() != 'H')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>>, single character, CharT, noskipws\n";
        }
        // Extract single character, signed char, skipws
        istringstream s3 {" Hello"};
        signed char c3;
        s3 >> c3;
        if (c3 != 'H' || !s3 || s3.peek() != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>>, single character, signed char, skipws\n";
        }
        // Extract single character, signed char, noskipws
        istringstream s4 {" Hello"};
        signed char c4;
        s4 >> noskipws >> c4;
        if (c4 != ' ' || !s4 || s4.peek() != 'H')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>>, single character, signed char, noskipws\n";
        }
        // Extract single character, unsigned char, skipws
        istringstream s5 {" Hello"};
        unsigned char c5;
        s5 >> c5;
        if (c5 != 'H' || !s5 || s5.peek() != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>>, single character, unsigned char, skipws\n";
        }
        // Extract single character, unsigned char, noskipws
        istringstream s6 {" Hello"};
        unsigned char c6;
        s6 >> noskipws >> c6;
        if (c6 != ' ' || !s6 || s6.peek() != 'H')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>>, single character, unsigned char, noskipws\n";
        }
        // Extract last character, CharT
        istringstream s7 {"Hello"};
        s7.ignore(4);
        istringstream::char_type c7;
        s7 >> c7;
        if (c7 != 'o' || !s7 || s7.eof())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>>, last character, CharT\n";
        }
        // EoF, CharT
        istringstream s8 {"Hello"};
        s8.ignore(5);
        istringstream::char_type c8;
        s8 >> c8;
        if (s8 || !s8.eof() || !s8.fail() || s8.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>>, EoF, CharT\n";
        }
        // Extract last character, signed char
        istringstream s9 {"Hello"};
        s9.ignore(4);
        signed char c9;
        s9 >> c9;
        if (c9 != 'o' || !s9 || s9.eof())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>>, last character, signed char\n";
        }
        // EoF, signed char
        istringstream s10 {"Hello"};
        s10.ignore(5);
        signed char c10;
        s10 >> c10;
        if (s10 || !s10.eof() || !s10.fail() || s10.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>>, EoF, signed char\n";
        }
        // Extract last character, unsigned char
        istringstream s11 {"Hello"};
        s11.ignore(4);
        unsigned char c11;
        s11 >> c11;
        if (c11 != 'o' || !s11 || s11.eof())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>>, last character, unsigned char\n";
        }
        // EoF, unsigned char
        istringstream s12 {"Hello"};
        s12.ignore(5);
        unsigned char c12;
        s12 >> c12;
        if (s12 || !s12.eof() || !s12.fail() || s12.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>>, EoF, unsigned char\n";
        }
        // Extract string, CharT, skipws, end at whitespace
        istringstream s13 {"  Hel 1"};
        istringstream::char_type c13[5] = {'a', 'b', 'c', 'd', 'e'};
        s13 >> c13;
        if (!s13 || s13.eof() || s13.peek() != ' ' || c13[0] != 'H' ||
            c13[1] != 'e' || c13[2] != 'l' || c13[3] != '\0' || c13[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> CharT, skipws, end at whitespace\n";
        }
        // Extract string, CharT, skipws, end at width
        istringstream s14 {"  Hel 1"};
        istringstream::char_type c14[5] = {'a', 'b', 'c', 'd', 'e'};
        s14.width(3);
        s14 >> c14;
        if (!s14 || s14.eof() || s14.peek() != 'l' || s14.width() != 0 ||
            c14[0] != 'H' || c14[1] != 'e' || c14[2] != '\0' || c14[3] != 'd' ||
            c14[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> CharT, skipws, end at width\n";
        }
        // Extract string, CharT, noskipws, ws start
        istringstream s15 {"  Hel 1"};
        istringstream::char_type c15[5] = {'a', 'b', 'c', 'd', 'e'};
        s15 >> noskipws;
        s15 >> c15;
        if (s15 || s15.eof() || c15[0] != '\0' || c15[1] != 'b' ||
            c15[2] != 'c' || c15[3] != 'd' || c15[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> CharT, noskipws, ws start\n";
        }
        // Extract string, CharT, noskipws, nows start
        istringstream s16 {"Hel 1"};
        istringstream::char_type c16[5] = {'a', 'b', 'c', 'd', 'e'};
        s16 >> noskipws;
        s16 >> c16;
        if (!s16 || s16.eof() || s16.peek() != ' ' || c16[0] != 'H' ||
            c16[1] != 'e' || c16[2] != 'l' || c16[3] != '\0' || c16[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> CharT, noskipws, nows start\n";
        }
        // Extract string, CharT, EoF
        istringstream s17 {"  Hel"};
        istringstream::char_type c17[5] = {'a', 'b', 'c', 'd', 'e'};
        s17 >> c17;
        if (!s17 || !s17.eof() || c17[0] != 'H' || c17[1] != 'e' ||
            c17[2] != 'l' || c17[3] != '\0' || c17[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> CharT, EoF\n";
        }
        // Extract string, signed char, skipws, end at whitespace
        istringstream s18 {"  Hel 1"};
        signed char c18[5] = {'a', 'b', 'c', 'd', 'e'};
        s18 >> c18;
        if (!s18 || s18.eof() || s18.peek() != ' ' || c18[0] != 'H' ||
            c18[1] != 'e' || c18[2] != 'l' || c18[3] != '\0' || c18[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> signed char, skipws, end at whitespace\n";
        }
        // Extract string, signed char, skipws, end at width
        istringstream s19 {"  Hel 1"};
        signed char c19[5] = {'a', 'b', 'c', 'd', 'e'};
        s19.width(3);
        s19 >> c19;
        if (!s19 || s19.eof() || s19.peek() != 'l' || s19.width() != 0 ||
            c19[0] != 'H' || c19[1] != 'e' || c19[2] != '\0' || c19[3] != 'd' ||
            c19[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> signed char, skipws, end at width\n";
        }
        // Extract string, signed char, noskipws, ws start
        istringstream s20 {"  Hel 1"};
        signed char c20[5] = {'a', 'b', 'c', 'd', 'e'};
        s20 >> noskipws;
        s20 >> c20;
        if (s20 || s20.eof() || c20[0] != '\0' || c20[1] != 'b' ||
            c20[2] != 'c' || c20[3] != 'd' || c20[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> signed char, noskipws, ws start\n";
        }
        // Extract string, signed char, noskipws, nows start
        istringstream s21 {"Hel 1"};
        signed char c21[5] = {'a', 'b', 'c', 'd', 'e'};
        s21 >> noskipws;
        s21 >> c21;
        if (!s21 || s21.eof() || s21.peek() != ' ' || c21[0] != 'H' ||
            c21[1] != 'e' || c21[2] != 'l' || c21[3] != '\0' || c21[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> signed char, noskipws, nows start\n";
        }
        // Extract string, signed char, EoF
        istringstream s22 {"  Hel"};
        signed char c22[5] = {'a', 'b', 'c', 'd', 'e'};
        s22 >> c22;
        if (!s22 || !s22.eof() || c22[0] != 'H' || c22[1] != 'e' ||
            c22[2] != 'l' || c22[3] != '\0' || c22[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> signed char, EoF\n";
        }
        // Extract string, unsigned char, skipws, end at whitespace
        istringstream s23 {"  Hel 1"};
        unsigned char c23[5] = {'a', 'b', 'c', 'd', 'e'};
        s23 >> c23;
        if (!s23 || s23.eof() || s23.peek() != ' ' || c23[0] != 'H' ||
            c23[1] != 'e' || c23[2] != 'l' || c23[3] != '\0' || c23[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> unsigned char, skipws, end at whitespace\n";
        }
        // Extract string, unsigned char, skipws, end at width
        istringstream s24 {"  Hel 1"};
        unsigned char c24[5] = {'a', 'b', 'c', 'd', 'e'};
        s24.width(3);
        s24 >> c24;
        if (!s24 || s24.eof() || s24.peek() != 'l' || s24.width() != 0 ||
            c24[0] != 'H' || c24[1] != 'e' || c24[2] != '\0' || c24[3] != 'd' ||
            c24[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> unsigned char, skipws, end at width\n";
        }
        // Extract string, unsigned char, noskipws, ws start
        istringstream s25 {"  Hel 1"};
        unsigned char c25[5] = {'a', 'b', 'c', 'd', 'e'};
        s25 >> noskipws;
        s25 >> c25;
        if (s25 || s25.eof() || c25[0] != '\0' || c25[1] != 'b' ||
            c25[2] != 'c' || c25[3] != 'd' || c25[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> unsigned char, noskipws, ws start\n";
        }
        // Extract string, unsigned char, noskipws, nows start
        istringstream s26 {"Hel 1"};
        unsigned char c26[5] = {'a', 'b', 'c', 'd', 'e'};
        s26 >> noskipws;
        s26 >> c26;
        if (!s26 || s26.eof() || s26.peek() != ' ' || c26[0] != 'H' ||
            c26[1] != 'e' || c26[2] != 'l' || c26[3] != '\0' || c26[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> unsigned char, noskipws, nows start\n";
        }
        // Extract string, unsigned char, EoF
        istringstream s27 {"  Hel"};
        unsigned char c27[5] = {'a', 'b', 'c', 'd', 'e'};
        s27 >> c27;
        if (!s27 || !s27.eof() || c27[0] != 'H' || c27[1] != 'e' ||
            c27[2] != 'l' || c27[3] != '\0' || c27[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> unsigned char, EoF\n";
        }
        // rvalue reference
        istringstream s28 {"Hel"};
        istringstream::char_type c28[5] = {'a', 'b', 'c', 'd', 'e'};
        klib::move(s28) >> c28;
        if (!s28 || !s28.eof() || c28[0] != 'H' || c28[1] != 'e' ||
            c28[2] != 'l' || c28[3] != '\0' || c28[4] != 'e')
        {
            ++fail_count;
            std::cout << "FAILED istringstream operator>> rvalue reference\n";
        }
    }
    // Non-member getline (from klib_string.h)
    {
        // Reach end of line
        istringstream s1 {"Hello\nGoodbye"};
        string str1 = "foo";
        getline(s1, str1);
        if (!s1 || s1.eof() || str1 != "Hello" || s1.peek() != 'G')
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member getline, reach end of line\n";
        }
        // Reach end of file
        istringstream s2 {"Hello"};
        string str2 = "foo";
        getline(s2, str2);
        if (!s2 || !s2.eof() || str2 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member getline, reach end of file\n";
        }
        // Empty
        istringstream s3 {""};
        string str3 = "foo";
        getline(s3, str3);
        if (s3 || !s3.eof() || str3 != "")
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member getline, empty\n";
        }
        // Leading whitespace
        istringstream s4 {"  Hello"};
        string str4 = "foo";
        getline(s4, str4);
        if (!s4 || !s4.eof() || str4 != "  Hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member getline, leading whitespace\n";
        }
        // After empty line
        istringstream s5 {"\nHello\n"};
        string str5 = "foo";
        getline(s5, str5);
        getline(s5, str5);
        if (!s5 || s5.eof() || str5 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member getline, after empty line\n";
        }
        // Reach custom delimiter
        istringstream s6 {"Helol"};
        string str6 = "foo";
        getline(s6, str6, 'l');
        if (!s6 || s6.eof() || str6 != "He" || s6.peek() != 'o')
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member getline, reach end custom delimiter\n";
        }
        // Reach end of file, custom delimiter
        istringstream s7 {"Hello"};
        string str7 = "foo";
        getline(s7, str7, 'a');
        if (!s7 || !s7.eof() || str7 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member getline, reach end of file, custom delimiter\n";
        }
        // After empty read, custom delimiter
        istringstream s8 {"aHelloa"};
        string str8 = "foo";
        getline(s8, str8, 'a');
        getline(s8, str8, 'a');
        if (!s8 || s8.eof() || str8 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member getline, after empty read, custom delimiter\n";
        }
    }
    // Non-member operator>> (from klib_string.h)
    {
        // Reach end of file
        istringstream s1 {"Hello"};
        string str1 = "foo";
        s1 >> str1;
        if (!s1 || !s1.eof() || str1 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member operator>> string, reach end of file\n";
        }
        // Reach whitespace
        istringstream s2 {"Hello Goodbye"};
        string str2 = "foo";
        s2 >> str2;
        if (!s2 || s2.eof() || s2.peek() != ' ' || str2 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member operator>> string, reach whitespace\n";
        }
        // Reach width before whitespace
        istringstream s3 {"Hello Goodbye"};
        string str3 = "foo";
        s3.width(4);
        s3 >> str3;
        if (!s3 || s3.eof() || s3.peek() != 'o' || str3 != "Hell")
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member operator>> string, reach width before whitespace\n";
        }
        // Reach whitespace before width
        istringstream s4 {"Hello Goodbye"};
        string str4 = "foo";
        s4.width(8);
        s4 >> str4;
        if (!s4 || s4.eof() || s4.peek() != ' ' || str4 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member operator>> string, reach whitespace before width\n";
        }
        // After width read
        istringstream s5 {"Hello Goodbye"};
        string str5 = "foo";
        s5.width(2);
        s5 >> str5;
        s5 >> str5;
        if (!s5 || s5.eof() || s5.peek() != ' ' || str5 != "llo")
        {
            ++fail_count;
            std::cout << "FAILED istringstream non-member operator>> string, after width read\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
