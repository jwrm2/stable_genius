#include <iostream>

#include "../include/cstring"
#include "../include/ios"
#include "../include/memory"
#include "../include/sstream"
#include "../include/string"
#include "../include/type_traits"

using namespace klib;

// Pointers for callback side effects.
int* cbp1;
int* cbp2;

// Functions for callback test.
void callback_erase(ostringstream::event ev, ios_base&, int index)
{
    if (ev == ios_base::erase_event)
        *cbp1 = index;
}
void callback_copy(ostringstream::event ev, ios_base&, int index)
{
    if (ev == ios_base::copyfmt_event)
        *cbp2 = index;
}

int main()
{
    size_t fail_count = 0;

    // Types
    if (!is_same<ostringstream::char_type, char>::value)
    {
        ++fail_count;
        std::cout << "FAILED ostringstream::char_type\n";
    }
    if (!is_same<ostringstream::traits_type, char_traits<char>>::value)
    {
        ++fail_count;
        std::cout << "FAILED ostringstream::traits_type\n";
    }
    if (!is_same<ostringstream::int_type, char_traits<char>::int_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED ostringstream::int_type\n";
    }
    if (!is_same<ostringstream::pos_type, char_traits<char>::pos_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED ostringstream::pos_type\n";
    }
    if (!is_same<ostringstream::off_type, char_traits<char>::off_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED ostringstream::off_type\n";
    }
    if (!is_same<ostringstream::allocator_type, allocator<char>>::value)
    {
        ++fail_count;
        std::cout << "FAILED ostringstream::allocator_type\n";
    }
    if (!is_same<ostringstream::event_callback,
        void (*)(ostringstream::event type, ios_base& ios, int index)>::value)
    {
        ++fail_count;
        std::cout << "FAILED ostringstream::event_callback\n";
    }

    // Constructor
    {
        // Default constructor.
        ostringstream s1 {};
        if (s1.flags() != (ios_base::skipws | ios_base::dec) ||
            s1.precision() != 6 || s1.width() != 0 ||
            s1.rdstate() != ios_base::goodbit ||
            s1.exceptions() != ios_base::goodbit || s1.tie() != nullptr ||
            s1.rdbuf() == nullptr || s1.fill() != ' ' || s1.str() != "")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream default constructor\n";
        }
        // Constructor with mode
        ostringstream s2 {ios_base::in};
        if (s2.flags() != (ios_base::skipws | ios_base::dec) ||
            s2.precision() != 6 || s2.width() != 0 ||
            s2.rdstate() != ios_base::goodbit ||
            s2.exceptions() != ios_base::goodbit || s2.tie() != nullptr ||
            s2.rdbuf() == nullptr || s2.fill() != ' ' || s2.str() != "")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream constructor with mode\n";
        }
        // Constructor with string.
        string str1 {"hello"};
        ostringstream s3 {str1};
        if (s3.flags() != (ios_base::skipws | ios_base::dec) ||
            s3.precision() != 6 || s3.width() != 0 ||
            s3.rdstate() != ios_base::goodbit ||
            s3.exceptions() != ios_base::goodbit || s3.tie() != nullptr ||
            s3.rdbuf() == nullptr || s3.fill() != ' ' || s3.str() != "hello")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream constructor with string\n";
        }
        // Constructor with string and truncation.
        string str2 {"hello"};
        ostringstream s4 {str2, ios_base::trunc};
        if (s4.flags() != (ios_base::skipws | ios_base::dec) ||
            s4.precision() != 6 || s4.width() != 0 ||
            s4.rdstate() != ios_base::goodbit ||
            s4.exceptions() != ios_base::goodbit || s4.tie() != nullptr ||
            s4.rdbuf() == nullptr || s4.fill() != ' ' || s4.str() != "")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream constructor with string and truncation\n";
        }
        // Move constructor.
        string str3 {"hello"};
        ostringstream s5 {str3};
        s5.flags(ios_base::hex);
        s5.precision(5);
        s5.width(1);
        s5.setstate(ios_base::eofbit);
        s5.exceptions(ios_base::failbit);
        ostringstream s6 {};
        s5.tie(&s6);
        s5.fill('a');
        ostringstream s7 {move(s5)};
        if (s7.flags() != ios_base::hex || s7.precision() != 5 ||
            s7.width() != 1 || s7.rdstate() != ios_base::eofbit ||
            s7.exceptions() != ios_base::failbit || s7.tie() != &s6 ||
            s7.rdbuf() == nullptr || s7.fill() != 'a' || s7.str() != "hello")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream move constructor\n";
        }
    }
    // Assignment
    {
        // Move assignment.
        string str1 {"hello"};
        ostringstream s1 {str1};
        s1.flags(ios_base::hex);
        s1.precision(5);
        s1.width(1);
        s1.setstate(ios_base::eofbit);
        s1.exceptions(ios_base::failbit);
        ostringstream s2 {};
        s1.tie(&s2);
        s1.fill('a');
        ostringstream s3 {};
        s3 = move(s1);
        if (s3.flags() != ios_base::hex || s3.precision() != 5 ||
            s3.width() != 1 || s3.rdstate() != ios_base::eofbit ||
            s3.exceptions() != ios_base::failbit || s3.tie() != &s2 ||
            s3.rdbuf() == nullptr || s3.fill() != 'a' || s3.str() != "hello")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream move assignment\n";
        }
    }
    // swap
    {
        // swap member
        string str1 {"hello"};
        ostringstream s1 {str1};
        s1.flags(ios_base::hex);
        s1.precision(5);
        s1.width(1);
        s1.setstate(ios_base::eofbit);
        s1.exceptions(ios_base::failbit);
        ostringstream s2 {};
        s1.tie(&s2);
        s1.fill('a');
        string str2 {"goodbye"};
        ostringstream s3 {str2};
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
            std::cout << "FAILED ostringstream swap member\n";
        }
        // swap non-member
        string str3 {"hello"};
        ostringstream s4 {str3};
        s4.flags(ios_base::hex);
        s4.precision(5);
        s4.width(1);
        s4.setstate(ios_base::eofbit);
        s4.exceptions(ios_base::failbit);
        ostringstream s5 {};
        s4.tie(&s5);
        s4.fill('a');
        string str4 {"goodbye"};
        ostringstream s6 {str4};
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
            std::cout << "FAILED ostringstream swap non-member\n";
        }
    }
    // rdbuf
    {
        ostringstream s1 {"hello"};
        if (s1.rdbuf()->str() != "hello")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream rdbuf\n";
        }
    }
    // str
    {
        // str get
        ostringstream s1 {"hello"};
        if (s1.str() != "hello")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream str get\n";
        }
        // str set
        ostringstream s2 {"hello"};
        s2.str("goodbye");
        if (s2.str() != "goodbye")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream str set\n";
        }
    }
    // operator <<, short
    {
        // Short, default positive
        ostringstream s1 {};
        short sh1 = 123;
        s1 << sh1;
        if (s1.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, default positive\n";
        }
        // Short, default negative
        ostringstream s2 {};
        short sh2 = -123;
        s2 << sh2;
        if (s2.str() != "-123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, default negative\n";
        }
        // Short, default zero
        ostringstream s3 {};
        short sh3 {};
        s3 << sh3;
        if (s3.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, default zero\n";
        }
        // Short, showpos positive
        ostringstream s4 {};
        short sh4 = 123;
        s4 << showpos << sh4;
        if (s4.str() != "+123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showpos positive\n";
        }
        // Short, showpos negative
        ostringstream s5 {};
        short sh5 = -123;
        s5 << showpos << sh5;
        if (s5.str() != "-123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showpos negative\n";
        }
        // Short, showpos zero
        ostringstream s6 {};
        short sh6 {};
        s6 << showpos << sh6;
        if (s6.str() != "+0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showpos zero\n";
        }
        // Short, hex positive
        ostringstream s7 {};
        short sh7 = 123;
        s7 << hex << sh7;
        if (s7.str() != "7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, hex positive\n";
        }
        // Short, hex negative
        ostringstream s8 {};
        short sh8 = -123;
        s8 << hex << sh8;
        if (s8.str() != "ff85")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, hex negative\n";
        }
        // Short, hex zero
        ostringstream s9 {};
        short sh9 {};
        s9 << hex << sh9;
        if (s9.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, hex zero\n";
        }
        // Short, oct positive
        ostringstream s10 {};
        short sh10 = 123;
        s10 << oct << sh10;
        if (s10.str() != "173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, oct positive\n";
        }
        // Short, oct negative
        ostringstream s11 {};
        short sh11 = -123;
        s11 << oct << sh11;
        if (s11.str() != "177605")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, oct negative\n";
        }
        // Short, oct zero
        ostringstream s12 {};
        short sh12 {};
        s12 << oct << sh12;
        if (s12.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, oct zero\n";
        }
        // Short, showbase, dec positive
        ostringstream s13 {};
        short sh13 = 123;
        s13 << showbase << sh13;
        if (s13.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showbase, dec positive\n";
        }
        // Short, showbase, dec negative
        ostringstream s14 {};
        short sh14 = -123;
        s14 << showbase << sh14;
        if (s14.str() != "-123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showbase, dec negative\n";
        }
        // Short, showbase, dec zero
        ostringstream s15 {};
        short sh15 {};
        s15 << showbase << sh15;
        if (s15.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showbase, dec zero\n";
        }
        // Short, showbase, hex positive
        ostringstream s16 {};
        short sh16 = 123;
        s16 << showbase << hex << sh16;
        if (s16.str() != "0x7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showbase, hex positive\n";
        }
        // Short, showbase, hex negative
        ostringstream s17 {};
        short sh17 = -123;
        s17 << showbase << hex << sh17;
        if (s17.str() != "0xff85")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showbase, hex negative\n";
        }
        // Short, showbase, hex zero
        ostringstream s18 {};
        short sh18 {};
        s18 << showbase << hex << sh18;
        if (s18.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showbase, hex zero\n";
        }
        // Short, showbase, oct positive
        ostringstream s19 {};
        short sh19 = 123;
        s19 << showbase << oct << sh19;
        if (s19.str() != "0173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showbase, oct positive\n";
        }
        // Short, showbase, oct negative
        ostringstream s20 {};
        short sh20 = -123;
        s20 << showbase << oct << sh20;
        if (s20.str() != "0177605")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showbase, oct negative\n";
        }
        // Short, showbase, oct zero
        ostringstream s21 {};
        short sh21 {};
        s21 << showbase << oct << sh21;
        if (s21.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showbase, oct zero\n";
        }
        // Short, uppercase, dec positive
        ostringstream s22 {};
        short sh22 = 123;
        s22 << uppercase << sh22;
        if (s22.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, uppercase, dec positive\n";
        }
        // Short, showbase, uppercase, hex positive
        ostringstream s23 {};
        short sh23 = 123;
        s23 << showbase << uppercase << hex << sh23;
        if (s23.str() != "0X7B")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showbase, uppercase, hex positive\n";
        }
        // Short, showbase, uppercase, hex negative
        ostringstream s24 {};
        short sh24 = -123;
        s24 << showbase << uppercase << hex << sh24;
        if (s24.str() != "0XFF85")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << short, showbase, uppercase, hex negative\n";
        }
    }
    // operator <<, int
    {
        // int, default positive
        ostringstream s1 {};
        int i1 = 123;
        s1 << i1;
        if (s1.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, default positive\n";
        }
        // int, default negative
        ostringstream s2 {};
        int i2 = -123;
        s2 << i2;
        if (s2.str() != "-123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, default negative\n";
        }
        // int, default zero
        ostringstream s3 {};
        int i3 {};
        s3 << i3;
        if (s3.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, default zero\n";
        }
        // int, showpos positive
        ostringstream s4 {};
        int i4 = 123;
        s4 << showpos << i4;
        if (s4.str() != "+123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showpos positive\n";
        }
        // int, showpos negative
        ostringstream s5 {};
        int i5 = -123;
        s5 << showpos << i5;
        if (s5.str() != "-123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showpos negative\n";
        }
        // int, showpos zero
        ostringstream s6 {};
        int i6 {};
        s6 << showpos << i6;
        if (s6.str() != "+0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showpos zero\n";
        }
        // int, hex positive
        ostringstream s7 {};
        int i7 = 123;
        s7 << hex << i7;
        if (s7.str() != "7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, hex positive\n";
        }
        // int, hex negative
        ostringstream s8 {};
        int i8 = -123;
        s8 << hex << i8;
        if (s8.str() != "ffffff85")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, hex negative\n";
        }
        // int, hex zero
        ostringstream s9 {};
        int i9 {};
        s9 << hex << i9;
        if (s9.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, hex zero\n";
        }
        // int, oct positive
        ostringstream s10 {};
        int i10 = 123;
        s10 << oct << i10;
        if (s10.str() != "173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, oct positive\n";
        }
        // int, oct negative
        ostringstream s11 {};
        int i11 = -123;
        s11 << oct << i11;
        if (s11.str() != "37777777605")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, oct negative\n";
        }
        // int, oct zero
        ostringstream s12 {};
        int i12 {};
        s12 << oct << i12;
        if (s12.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, oct zero\n";
        }
        // int, showbase, dec positive
        ostringstream s13 {};
        int i13 = 123;
        s13 << showbase << i13;
        if (s13.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showbase, dec positive\n";
        }
        // int, showbase, dec negative
        ostringstream s14 {};
        int i14 = -123;
        s14 << showbase << i14;
        if (s14.str() != "-123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showbase, dec negative\n";
        }
        // int, showbase, dec zero
        ostringstream s15 {};
        int i15 {};
        s15 << showbase << i15;
        if (s15.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showbase, dec zero\n";
        }
        // int, showbase, hex positive
        ostringstream s16 {};
        int i16 = 123;
        s16 << showbase << hex << i16;
        if (s16.str() != "0x7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showbase, hex positive\n";
        }
        // int, showbase, hex negative
        ostringstream s17 {};
        int i17 = -123;
        s17 << showbase << hex << i17;
        if (s17.str() != "0xffffff85")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showbase, hex negative\n";
        }
        // int, showbase, hex zero
        ostringstream s18 {};
        int i18 {};
        s18 << showbase << hex << i18;
        if (s18.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showbase, hex zero\n";
        }
        // int, showbase, oct positive
        ostringstream s19 {};
        int i19 = 123;
        s19 << showbase << oct << i19;
        if (s19.str() != "0173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showbase, oct positive\n";
        }
        // int, showbase, oct negative
        ostringstream s20 {};
        int i20 = -123;
        s20 << showbase << oct << i20;
        if (s20.str() != "037777777605")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showbase, oct negative\n";
        }
        // int, showbase, oct zero
        ostringstream s21 {};
        int i21 {};
        s21 << showbase << oct << i21;
        if (s21.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showbase, oct zero\n";
        }
        // int, uppercase, dec positive
        ostringstream s22 {};
        int i22 = 123;
        s22 << uppercase << i22;
        if (s22.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, uppercase, dec positive\n";
        }
        // int, showbase, uppercase, hex positive
        ostringstream s23 {};
        int i23 = 123;
        s23 << showbase << uppercase << hex << i23;
        if (s23.str() != "0X7B")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showbase, uppercase, hex positive\n";
        }
        // int, showbase, uppercase, hex negative
        ostringstream s24 {};
        int i24 = -123;
        s24 << showbase << uppercase << hex << i24;
        if (s24.str() != "0XFFFFFF85")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << int, showbase, uppercase, hex negative\n";
        }
    }
    // operator <<, long
    {
        // long, default positive
        ostringstream s1 {};
        long l1 = 123;
        s1 << l1;
        if (s1.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, default positive\n";
        }
        // long, default negative
        ostringstream s2 {};
        long l2 = -123;
        s2 << l2;
        if (s2.str() != "-123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, default negative\n";
        }
        // long, default zero
        ostringstream s3 {};
        long l3 {};
        s3 << l3;
        if (s3.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, default zero\n";
        }
        // long, showpos positive
        ostringstream s4 {};
        long l4 = 123;
        s4 << showpos << l4;
        if (s4.str() != "+123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showpos positive\n";
        }
        // long, showpos negative
        ostringstream s5 {};
        long l5 = -123;
        s5 << showpos << l5;
        if (s5.str() != "-123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showpos negative\n";
        }
        // long, showpos zero
        ostringstream s6 {};
        long l6 {};
        s6 << showpos << l6;
        if (s6.str() != "+0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showpos zero\n";
        }
        // long, hex positive
        ostringstream s7 {};
        long l7 = 123;
        s7 << hex << l7;
        if (s7.str() != "7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, hex positive\n";
        }
        // long, hex negative
        ostringstream s8 {};
        long l8 = -123;
        s8 << hex << l8;
        if ((sizeof(long) == 4 && s8.str() != "ffffff85") ||
            (sizeof(long) == 8 && s8.str() != "ffffffffffffff85"))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, hex negative\n";
        }
        // long, hex zero
        ostringstream s9 {};
        long l9 {};
        s9 << hex << l9;
        if (s9.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, hex zero\n";
        }
        // long, oct positive
        ostringstream s10 {};
        long l10 = 123;
        s10 << oct << l10;
        if (s10.str() != "173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, oct positive\n";
        }
        // long, oct negative
        ostringstream s11 {};
        long l11 = -123;
        s11 << oct << l11;
        if ((sizeof(long) == 4 && s11.str() != "37777777605") ||
            (sizeof(long) == 8 && s11.str() != "1777777777777777777605"))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, oct negative\n";
        }
        // long, oct zero
        ostringstream s12 {};
        long l12 {};
        s12 << oct << l12;
        if (s12.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, oct zero\n";
        }
        // long, showbase, dec positive
        ostringstream s13 {};
        long l13 = 123;
        s13 << showbase << l13;
        if (s13.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showbase, dec positive\n";
        }
        // long, showbase, dec negative
        ostringstream s14 {};
        long l14 = -123;
        s14 << showbase << l14;
        if (s14.str() != "-123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showbase, dec negative\n";
        }
        // long, showbase, dec zero
        ostringstream s15 {};
        long l15 {};
        s15 << showbase << l15;
        if (s15.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showbase, dec zero\n";
        }
        // long, showbase, hex positive
        ostringstream s16 {};
        long l16 = 123;
        s16 << showbase << hex << l16;
        if (s16.str() != "0x7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showbase, hex positive\n";
        }
        // long, showbase, hex negative
        ostringstream s17 {};
        long l17 = -123;
        s17 << showbase << hex << l17;
        if ((sizeof(long) == 4 && s17.str() != "0xffffff85") ||
            (sizeof(long) == 8 && s17.str() != "0xffffffffffffff85"))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showbase, hex negative\n";
        }
        // long, showbase, hex zero
        ostringstream s18 {};
        long l18 {};
        s18 << showbase << hex << l18;
        if (s18.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showbase, hex zero\n";
        }
        // long, showbase, oct positive
        ostringstream s19 {};
        long l19 = 123;
        s19 << showbase << oct << l19;
        if (s19.str() != "0173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showbase, oct positive\n";
        }
        // long, showbase, oct negative
        ostringstream s20 {};
        long l20 = -123;
        s20 << showbase << oct << l20;
        if ((sizeof(long) == 4 && s20.str() != "037777777605") ||
            (sizeof(long) == 8 && s20.str() != "01777777777777777777605"))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showbase, oct negative\n";
        }
        // long, showbase, oct zero
        ostringstream s21 {};
        long l21 {};
        s21 << showbase << oct << l21;
        if (s21.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showbase, oct zero\n";
        }
        // long, uppercase, dec positive
        ostringstream s22 {};
        long l22 = 123;
        s22 << uppercase << l22;
        if (s22.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, uppercase, dec positive\n";
        }
        // long, showbase, uppercase, hex positive
        ostringstream s23 {};
        long l23 = 123;
        s23 << showbase << uppercase << hex << l23;
        if (s23.str() != "0X7B")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showbase, uppercase, hex positive\n";
        }
        // long, showbase, uppercase, hex negative
        ostringstream s24 {};
        long l24 = -123;
        s24 << showbase << uppercase << hex << l24;
        if ((sizeof(long) == 4 && s24.str() != "0XFFFFFF85") ||
            (sizeof(long) == 8 && s24.str() != "0XFFFFFFFFFFFFFF85"))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long, showbase, uppercase, hex negative\n";
        }
    }
    // operator <<, long long
    {
        // long long, default positive
        ostringstream s1 {};
        long long ll1 = 123;
        s1 << ll1;
        if (s1.str() != "123")
        {
            std::cout << "s1 == " << s1.str() << '\n';
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, default positive\n";
        }
        // long long, default negative
        ostringstream s2 {};
        long long ll2 = -123;
        s2 << ll2;
        if (s2.str() != "-123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, default negative\n";
        }
        // long long, default zero
        ostringstream s3 {};
        long long ll3 {};
        s3 << ll3;
        if (s3.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, default zero\n";
        }
        // long long, showpos positive
        ostringstream s4 {};
        long long ll4 = 123;
        s4 << showpos << ll4;
        if (s4.str() != "+123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showpos positive\n";
        }
        // long long, showpos negative
        ostringstream s5 {};
        long long ll5 = -123;
        s5 << showpos << ll5;
        if (s5.str() != "-123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showpos negative\n";
        }
        // long long, showpos zero
        ostringstream s6 {};
        long long ll6 {};
        s6 << showpos << ll6;
        if (s6.str() != "+0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showpos zero\n";
        }
        // long long, hex positive
        ostringstream s7 {};
        long long ll7 = 123;
        s7 << hex << ll7;
        if (s7.str() != "7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, hex positive\n";
        }
        // long long, hex negative
        ostringstream s8 {};
        long long ll8 = -123;
        s8 << hex << ll8;
        if (s8.str() != "ffffffffffffff85")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, hex negative\n";
        }
        // long long, hex zero
        ostringstream s9 {};
        long long ll9 {};
        s9 << hex << ll9;
        if (s9.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, hex zero\n";
        }
        // long long, oct positive
        ostringstream s10 {};
        long long ll10 = 123;
        s10 << oct << ll10;
        if (s10.str() != "173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, oct positive\n";
        }
        // long long, oct negative
        ostringstream s11 {};
        long long ll11 = -123;
        s11 << oct << ll11;
        if (s11.str() != "1777777777777777777605")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, oct negative\n";
        }
        // long long, oct zero
        ostringstream s12 {};
        long long ll12 {};
        s12 << oct << ll12;
        if (s12.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, oct zero\n";
        }
        // long long, showbase, dec positive
        ostringstream s13 {};
        long long ll13 = 123;
        s13 << showbase << ll13;
        if (s13.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showbase, dec positive\n";
        }
        // long long, showbase, dec negative
        ostringstream s14 {};
        long long ll14 = -123;
        s14 << showbase << ll14;
        if (s14.str() != "-123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showbase, dec negative\n";
        }
        // long long, showbase, dec zero
        ostringstream s15 {};
        long long ll15 {};
        s15 << showbase << ll15;
        if (s15.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showbase, dec zero\n";
        }
        // long long, showbase, hex positive
        ostringstream s16 {};
        long long ll16 = 123;
        s16 << showbase << hex << ll16;
        if (s16.str() != "0x7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showbase, hex positive\n";
        }
        // long long, showbase, hex negative
        ostringstream s17 {};
        long long ll17 = -123;
        s17 << showbase << hex << ll17;
        if (s17.str() != "0xffffffffffffff85")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showbase, hex negative\n";
        }
        // long long, showbase, hex zero
        ostringstream s18 {};
        long long ll18 {};
        s18 << showbase << hex << ll18;
        if (s18.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showbase, hex zero\n";
        }
        // long long, showbase, oct positive
        ostringstream s19 {};
        long long ll19 = 123;
        s19 << showbase << oct << ll19;
        if (s19.str() != "0173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showbase, oct positive\n";
        }
        // long long, showbase, oct negative
        ostringstream s20 {};
        long long ll20 = -123;
        s20 << showbase << oct << ll20;
        if (s20.str() != "01777777777777777777605")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showbase, oct negative\n";
        }
        // long long, showbase, oct zero
        ostringstream s21 {};
        long long ll21 {};
        s21 << showbase << oct << ll21;
        if (s21.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showbase, oct zero\n";
        }
        // long long, uppercase, dec positive
        ostringstream s22 {};
        long long ll22 = 123;
        s22 << uppercase << ll22;
        if (s22.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, uppercase, dec positive\n";
        }
        // long long, showbase, uppercase, hex positive
        ostringstream s23 {};
        long long ll23 = 123;
        s23 << showbase << uppercase << hex << ll23;
        if (s23.str() != "0X7B")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showbase, uppercase, hex positive\n";
        }
        // long long, showbase, uppercase, hex negative
        ostringstream s24 {};
        long long ll24 = -123;
        s24 << showbase << uppercase << hex << ll24;
        if (s24.str() != "0XFFFFFFFFFFFFFF85")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long long, showbase, uppercase, hex negative\n";
        }
    }
    // operator <<, Unsigned short
    {
        // Unsigned short, default positive
        ostringstream s1 {};
        unsigned short ush1 = 123;
        s1 << ush1;
        if (s1.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, default positive\n";
        }
        // Unsigned short, default zero
        ostringstream s2 {};
        unsigned short ush2 {};
        s2 << ush2;
        if (s2.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, default zero\n";
        }
        // Unsigned short, showpos positive
        ostringstream s3 {};
        unsigned short ush3 = 123;
        s3 << showpos << ush3;
        if (s3.str() != "123")
        {
            std::cout << "s3 == " << s3.str() << '\n';
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, showpos positive\n";
        }
        // Unsigned short, showpos zero
        ostringstream s4 {};
        unsigned short ush4 {};
        s4 << showpos << ush4;
        if (s4.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, showpos zero\n";
        }
        // Unsigned short, hex positive
        ostringstream s5 {};
        unsigned short ush5 = 123;
        s5 << hex << ush5;
        if (s5.str() != "7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, hex positive\n";
        }
        // Unsigned short, hex zero
        ostringstream s6 {};
        unsigned short ush6 {};
        s6 << hex << ush6;
        if (s6.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, hex zero\n";
        }
        // Unsigned short, oct positive
        ostringstream s7 {};
        unsigned short ush7 = 123;
        s7 << oct << ush7;
        if (s7.str() != "173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, oct positive\n";
        }
        // Unsigned short, oct zero
        ostringstream s8 {};
        unsigned short ush8 {};
        s8 << oct << ush8;
        if (s8.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, oct zero\n";
        }
        // Unsigned short, showbase, dec positive
        ostringstream s9 {};
        unsigned short ush9 = 123;
        s9 << showbase << ush9;
        if (s9.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, showbase, dec positive\n";
        }
        // Unsigned short, showbase, dec zero
        ostringstream s10 {};
        unsigned short ush10 {};
        s10 << showbase << ush10;
        if (s10.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, showbase, dec zero\n";
        }
        // Unsigned short, showbase, hex positive
        ostringstream s11 {};
        unsigned short ush11 = 123;
        s11 << showbase << hex << ush11;
        if (s11.str() != "0x7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, showbase, hex positive\n";
        }
        // Unsigned short, showbase, hex zero
        ostringstream s12 {};
        unsigned short ush12 {};
        s12 << showbase << hex << ush12;
        if (s12.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, showbase, hex zero\n";
        }
        // Unsigned short, showbase, oct positive
        ostringstream s13 {};
        unsigned short ush13 = 123;
        s13 << showbase << oct << ush13;
        if (s13.str() != "0173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, showbase, oct positive\n";
        }
        // Unsigned short, showbase, oct zero
        ostringstream s14 {};
        unsigned short ush14 {};
        s14 << showbase << oct << ush14;
        if (s14.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, showbase, oct zero\n";
        }
        // Unsigned short, uppercase, dec positive
        ostringstream s15 {};
        unsigned short ush15 = 123;
        s15 << uppercase << ush15;
        if (s15.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, uppercase, dec positive\n";
        }
        // Unsigned short, showbase, uppercase, hex positive
        ostringstream s16 {};
        unsigned short ush16 = 123;
        s16 << showbase << uppercase << hex << ush16;
        if (s16.str() != "0X7B")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned short, showbase, uppercase, hex positive\n";
        }
    }
    // operator <<, Unsigned int
    {
        // Unsigned int, default positive
        ostringstream s1 {};
        unsigned int ui1 = 123;
        s1 << ui1;
        if (s1.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, default positive\n";
        }
        // Unsigned int, default zero
        ostringstream s2 {};
        unsigned int ui2 {};
        s2 << ui2;
        if (s2.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, default zero\n";
        }
        // Unsigned int, showpos positive
        ostringstream s3 {};
        unsigned int ui3 = 123;
        s3 << showpos << ui3;
        if (s3.str() != "123")
        {
            std::cout << "s3 == " << s3.str() << '\n';
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, showpos positive\n";
        }
        // Unsigned int, showpos zero
        ostringstream s4 {};
        unsigned int ui4 {};
        s4 << showpos << ui4;
        if (s4.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, showpos zero\n";
        }
        // Unsigned int, hex positive
        ostringstream s5 {};
        unsigned int ui5 = 123;
        s5 << hex << ui5;
        if (s5.str() != "7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, hex positive\n";
        }
        // Unsigned int, hex zero
        ostringstream s6 {};
        unsigned int ui6 {};
        s6 << hex << ui6;
        if (s6.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, hex zero\n";
        }
        // Unsigned int, oct positive
        ostringstream s7 {};
        unsigned int ui7 = 123;
        s7 << oct << ui7;
        if (s7.str() != "173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, oct positive\n";
        }
        // Unsigned int, oct zero
        ostringstream s8 {};
        unsigned int ui8 {};
        s8 << oct << ui8;
        if (s8.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, oct zero\n";
        }
        // Unsigned int, showbase, dec positive
        ostringstream s9 {};
        unsigned int ui9 = 123;
        s9 << showbase << ui9;
        if (s9.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, showbase, dec positive\n";
        }
        // Unsigned int, showbase, dec zero
        ostringstream s10 {};
        unsigned int ui10 {};
        s10 << showbase << ui10;
        if (s10.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, showbase, dec zero\n";
        }
        // Unsigned int, showbase, hex positive
        ostringstream s11 {};
        unsigned int ui11 = 123;
        s11 << showbase << hex << ui11;
        if (s11.str() != "0x7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, showbase, hex positive\n";
        }
        // Unsigned int, showbase, hex zero
        ostringstream s12 {};
        unsigned int ui12 {};
        s12 << showbase << hex << ui12;
        if (s12.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, showbase, hex zero\n";
        }
        // Unsigned int, showbase, oct positive
        ostringstream s13 {};
        unsigned int ui13 = 123;
        s13 << showbase << oct << ui13;
        if (s13.str() != "0173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, showbase, oct positive\n";
        }
        // Unsigned int, showbase, oct zero
        ostringstream s14 {};
        unsigned int ui14 {};
        s14 << showbase << oct << ui14;
        if (s14.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, showbase, oct zero\n";
        }
        // Unsigned int, uppercase, dec positive
        ostringstream s15 {};
        unsigned int ui15 = 123;
        s15 << uppercase << ui15;
        if (s15.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, uppercase, dec positive\n";
        }
        // Unsigned int, showbase, uppercase, hex positive
        ostringstream s16 {};
        unsigned int ui16 = 123;
        s16 << showbase << uppercase << hex << ui16;
        if (s16.str() != "0X7B")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned int, showbase, uppercase, hex positive\n";
        }
    }
    // operator <<, unsigned long
    {
        // unsigned long, default positive
        ostringstream s1 {};
        unsigned long ul1 = 123;
        s1 << ul1;
        if (s1.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, default positive\n";
        }
        // unsigned long, default zero
        ostringstream s2 {};
        unsigned long ul2 {};
        s2 << ul2;
        if (s2.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, default zero\n";
        }
        // unsigned long, showpos positive
        ostringstream s3 {};
        unsigned long ul3 = 123;
        s3 << showpos << ul3;
        if (s3.str() != "123")
        {
            std::cout << "s3 == " << s3.str() << '\n';
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, showpos positive\n";
        }
        // unsigned long, showpos zero
        ostringstream s4 {};
        unsigned long ul4 {};
        s4 << showpos << ul4;
        if (s4.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, showpos zero\n";
        }
        // unsigned long, hex positive
        ostringstream s5 {};
        unsigned long ul5 = 123;
        s5 << hex << ul5;
        if (s5.str() != "7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, hex positive\n";
        }
        // unsigned long, hex zero
        ostringstream s6 {};
        unsigned long ul6 {};
        s6 << hex << ul6;
        if (s6.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, hex zero\n";
        }
        // unsigned long, oct positive
        ostringstream s7 {};
        unsigned long ul7 = 123;
        s7 << oct << ul7;
        if (s7.str() != "173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, oct positive\n";
        }
        // unsigned long, oct zero
        ostringstream s8 {};
        unsigned long ul8 {};
        s8 << oct << ul8;
        if (s8.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, oct zero\n";
        }
        // unsigned long, showbase, dec positive
        ostringstream s9 {};
        unsigned long ul9 = 123;
        s9 << showbase << ul9;
        if (s9.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, showbase, dec positive\n";
        }
        // unsigned long, showbase, dec zero
        ostringstream s10 {};
        unsigned long ul10 {};
        s10 << showbase << ul10;
        if (s10.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, showbase, dec zero\n";
        }
        // unsigned long, showbase, hex positive
        ostringstream s11 {};
        unsigned long ul11 = 123;
        s11 << showbase << hex << ul11;
        if (s11.str() != "0x7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, showbase, hex positive\n";
        }
        // unsigned long, showbase, hex zero
        ostringstream s12 {};
        unsigned long ul12 {};
        s12 << showbase << hex << ul12;
        if (s12.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, showbase, hex zero\n";
        }
        // unsigned long, showbase, oct positive
        ostringstream s13 {};
        unsigned long ul13 = 123;
        s13 << showbase << oct << ul13;
        if (s13.str() != "0173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, showbase, oct positive\n";
        }
        // unsigned long, showbase, oct zero
        ostringstream s14 {};
        unsigned long ul14 {};
        s14 << showbase << oct << ul14;
        if (s14.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, showbase, oct zero\n";
        }
        // unsigned long, uppercase, dec positive
        ostringstream s15 {};
        unsigned long ul15 = 123;
        s15 << uppercase << ul15;
        if (s15.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, uppercase, dec positive\n";
        }
        // unsigned long, showbase, uppercase, hex positive
        ostringstream s16 {};
        unsigned long ul16 = 123;
        s16 << showbase << uppercase << hex << ul16;
        if (s16.str() != "0X7B")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long, showbase, uppercase, hex positive\n";
        }
    }
    // operator <<, unsigned long long
    {
        // unsigned long long, default positive
        ostringstream s1 {};
        unsigned long long ull1 = 123;
        s1 << ull1;
        if (s1.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, default positive\n";
        }
        // unsigned long long, default zero
        ostringstream s2 {};
        unsigned long long ull2 {};
        s2 << ull2;
        if (s2.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, default zero\n";
        }
        // unsigned long long, showpos positive
        ostringstream s3 {};
        unsigned long long ull3 = 123;
        s3 << showpos << ull3;
        if (s3.str() != "123")
        {
            std::cout << "s3 == " << s3.str() << '\n';
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, showpos positive\n";
        }
        // unsigned long long, showpos zero
        ostringstream s4 {};
        unsigned long long ull4 {};
        s4 << showpos << ull4;
        if (s4.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, showpos zero\n";
        }
        // unsigned long long, hex positive
        ostringstream s5 {};
        unsigned long long ull5 = 123;
        s5 << hex << ull5;
        if (s5.str() != "7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, hex positive\n";
        }
        // unsigned long long, hex zero
        ostringstream s6 {};
        unsigned long long ull6 {};
        s6 << hex << ull6;
        if (s6.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, hex zero\n";
        }
        // unsigned long long, oct positive
        ostringstream s7 {};
        unsigned long long ull7 = 123;
        s7 << oct << ull7;
        if (s7.str() != "173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, oct positive\n";
        }
        // unsigned long long, oct zero
        ostringstream s8 {};
        unsigned long long ull8 {};
        s8 << oct << ull8;
        if (s8.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, oct zero\n";
        }
        // unsigned long long, showbase, dec positive
        ostringstream s9 {};
        unsigned long long ull9 = 123;
        s9 << showbase << ull9;
        if (s9.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, showbase, dec positive\n";
        }
        // unsigned long long, showbase, dec zero
        ostringstream s10 {};
        unsigned long long ull10 {};
        s10 << showbase << ull10;
        if (s10.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, showbase, dec zero\n";
        }
        // unsigned long long, showbase, hex positive
        ostringstream s11 {};
        unsigned long long ull11 = 123;
        s11 << showbase << hex << ull11;
        if (s11.str() != "0x7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, showbase, hex positive\n";
        }
        // unsigned long long, showbase, hex zero
        ostringstream s12 {};
        unsigned long long ull12 {};
        s12 << showbase << hex << ull12;
        if (s12.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, showbase, hex zero\n";
        }
        // unsigned long long, showbase, oct positive
        ostringstream s13 {};
        unsigned long long ull13 = 123;
        s13 << showbase << oct << ull13;
        if (s13.str() != "0173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, showbase, oct positive\n";
        }
        // unsigned long long, showbase, oct zero
        ostringstream s14 {};
        unsigned long long ull14 {};
        s14 << showbase << oct << ull14;
        if (s14.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, showbase, oct zero\n";
        }
        // unsigned long long, uppercase, dec positive
        ostringstream s15 {};
        unsigned long long ull15 = 123;
        s15 << uppercase << ull15;
        if (s15.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, uppercase, dec positive\n";
        }
        // unsigned long long, showbase, uppercase, hex positive
        ostringstream s16 {};
        unsigned long long ull16 = 123;
        s16 << showbase << uppercase << hex << ull16;
        if (s16.str() != "0X7B")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << unsigned long long, showbase, uppercase, hex positive\n";
        }
    }
    // operator <<, float
    {
        // float, default positive
        ostringstream s1 {};
        float f1 = 123.123;
        s1 << f1;
        if (s1.str() != "123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, default positive\n";
        }
        // float, default positive, small
        ostringstream s2 {};
        float f2 = 0.000000123;
        s2 << f2;
        if (s2.str() != "1.23e-7")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, default positive, small\n";
        }
        // float, default positive, large
        ostringstream s3 {};
        float f3 = 12300000;
        s3 << f3;
        if (s3.str() != "1.23e7")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, default positive, large\n";
        }
        // float, fixed positive
        ostringstream s4 {};
        float f4 = 123.123;
        s4 << fixed << f4;
        if (s4.str() != "123.123001")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, fixed positive\n";
        }
        // float, fixed positive, small
        ostringstream s5 {};
        float f5 = 0.0000123;
        s5 << fixed << f5;
        if (s5.str() != "0.000012")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, fixed positive, small\n";
        }
        // float, fixed positive, large
        ostringstream s6 {};
        float f6 = 12300000;
        s6 << fixed << f6;
        if (s6.str() != "12300000.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, fixed positive, large\n";
        }
        // float, exp positive
        ostringstream s7 {};
        float f7 = 123.123;
        s7 << scientific << f7;
        if (s7.str() != "1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, scientific positive\n";
        }
        // float, exp positive, small
        ostringstream s8 {};
        float f8 = 0.000000123;
        s8 << scientific << f8;
        if (s8.str() != "1.230000e-07")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, scientific positive, small\n";
        }
        // float, exp positive, large
        ostringstream s9 {};
        float f9 = 12300000;
        s9 << scientific << f9;
        if (s9.str() != "1.230000e+07")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, scientific positive, large\n";
        }
        // float, default negative
        ostringstream s10 {};
        float f10 = -123.123;
        s10 << f10;
        if (s10.str() != "-123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, default negative\n";
        }
        // float, default negative, small
        ostringstream s11 {};
        float f11 = -0.000000123;
        s11 << f11;
        if (s11.str() != "-1.23e-7")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, default negative, small\n";
        }
        // float, default negative, large
        ostringstream s12 {};
        float f12 = -12300000;
        s12 << f12;
        if (s12.str() != "-1.23e7")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, default negative, large\n";
        }
        // float, fixed negative
        ostringstream s13 {};
        float f13 = -123.123;
        s13 << fixed << f13;
        if (s13.str() != "-123.123001")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, fixed negative\n";
        }
        // float, fixed negative, small
        ostringstream s14 {};
        float f14 = -0.0000123;
        s14 << fixed << f14;
        if (s14.str() != "-0.000012")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, fixed negative, small\n";
        }
        // float, fixed negative, large
        ostringstream s15 {};
        float f15 = -12300000;
        s15 << fixed << f15;
        if (s15.str() != "-12300000.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, fixed negative, large\n";
        }
        // float, exp negative
        ostringstream s16 {};
        float f16 = -123.123;
        s16 << scientific << f16;
        if (s16.str() != "-1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, scientific negative\n";
        }
        // float, exp negative, small
        ostringstream s17 {};
        float f17 = -0.000000123;
        s17 << scientific << f17;
        if (s17.str() != "-1.230000e-07")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, scientific negative, small\n";
        }
        // float, exp negative, large
        ostringstream s18 {};
        float f18 = -12300000;
        s18 << scientific << f18;
        if (s18.str() != "-1.230000e+07")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, scientific negative, large\n";
        }
        // float, default zero
        ostringstream s19 {};
        float f19 = 0;
        s19 << f19;
        if (s19.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, default zero\n";
        }
        // float, fixed zero
        ostringstream s20 {};
        float f20 = 0;
        s20 << fixed << f20;
        if (s20.str() != "0.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, fixed zero\n";
        }
        // float, exp zero
        ostringstream s21 {};
        float f21 = 0;
        s21 << scientific << f21;
        if (s21.str() != "0.000000e+00")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, scientific zero\n";
        }
        // float, showpos, default positive
        ostringstream s22 {};
        float f22 = 123.123;
        s22 << showpos << f22;
        if (s22.str() != "+123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpos, default positive\n";
        }
        // float, showpos, default negative
        ostringstream s23 {};
        float f23 = -123.123;
        s23 << showpos << f23;
        if (s23.str() != "-123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpos, default negative\n";
        }
        // float, showpos, default zero
        ostringstream s24 {};
        float f24 = 0;
        s24 << showpos << f24;
        if (s24.str() != "+0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpos, default zero\n";
        }
        // float, showpos, fixed positive
        ostringstream s25 {};
        float f25 = 123.123;
        s25 << fixed << showpos << f25;
        if (s25.str() != "+123.123001")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpos, fixed positive\n";
        }
        // float, showpos, fixed negative
        ostringstream s26 {};
        float f26 = -123.123;
        s26 << fixed << showpos << f26;
        if (s26.str() != "-123.123001")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpos, fixed negative\n";
        }
        // float, showpos, fixed zero
        ostringstream s27 {};
        float f27 = 0;
        s27 << fixed << showpos << f27;
        if (s27.str() != "+0.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpos, fixed zero\n";
        }
        // float, showpos, exp positive
        ostringstream s28 {};
        float f28 = 123.123;
        s28 << scientific << showpos << f28;
        if (s28.str() != "+1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpos, scientific positive\n";
        }
        // float, showpos, exp negative
        ostringstream s29 {};
        float f29 = -123.123;
        s29 << scientific << showpos << f29;
        if (s29.str() != "-1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpos, scientific negative\n";
        }
        // float, showpos, exp zero
        ostringstream s30 {};
        float f30 = 0;
        s30 << scientific << showpos << f30;
        if (s30.str() != "+0.000000e+00")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpos, scientific zero\n";
        }
        // float, uppercase, default positive
        ostringstream s31 {};
        float f31 = 123.123;
        s31 << uppercase << f31;
        if (s31.str() != "123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, uppercase, default positive\n";
        }
        // float, uppercase, fixed positive
        ostringstream s32 {};
        float f32 = 123.123;
        s32 << fixed << uppercase << f32;
        if (s32.str() != "123.123001")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpos, fixed positive\n";
        }
        // float, uppercase, exp positive
        ostringstream s33 {};
        float f33 = 123.123;
        s33 << scientific << uppercase << f33;
        if (s33.str() != "1.231230E+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpos, scientific positive\n";
        }
        // float, showpoint, default positive, with point
        ostringstream s34 {};
        float f34 = 123.123;
        s34 << showpoint << f34;
        if (s34.str() != "123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpoint, default positive, with point\n";
        }
        // float, showpoint, fixed positive, with point
        ostringstream s35 {};
        float f35 = 123.123;
        s35 << fixed << showpoint << f35;
        if (s35.str() != "123.123001")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpoint, fixed positive, with point\n";
        }
        // float, showpoint, exp positive, with point
        ostringstream s36 {};
        float f36 = 123.123;
        s36 << scientific << showpoint << f36;
        if (s36.str() != "1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpoint, scientific positive, with point\n";
        }
        // float, showpoint, default positive, without point
        ostringstream s37 {};
        float f37 = 123;
        s37 << showpoint << f37;
        if (s37.str() != "123.000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpoint, default positive, without point\n";
        }
        // float, showpoint, fixed positive, without point
        ostringstream s38 {};
        float f38 = 123;
        s38 << fixed << showpoint << f38;
        if (s38.str() != "123.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpoint, fixed positive, without point\n";
        }
        // float, showpoint, exp positive, without point
        ostringstream s39 {};
        float f39 = 1;
        s39 << scientific << showpoint << f39;
        if (s39.str() != "1.000000e+00")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << float, showpoint, scientific positive, without point\n";
        }
    }
    // operator <<, double
    {
        // double, default positive
        ostringstream s1 {};
        double d1 = 123.123;
        s1 << d1;
        if (s1.str() != "123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, default positive\n";
        }
        // double, default positive, small
        ostringstream s2 {};
        double d2 = 0.000000123;
        s2 << d2;
        if (s2.str() != "1.23e-7")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, default positive, small\n";
        }
        // double, default positive, large
        ostringstream s3 {};
        double d3 = 12300000;
        s3 << d3;
        if (s3.str() != "1.23e7")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, default positive, large\n";
        }
        // double, fixed positive
        ostringstream s4 {};
        double d4 = 123.123;
        s4 << fixed << d4;
        if (s4.str() != "123.123000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, fixed positive\n";
        }
        // double, fixed positive, small
        ostringstream s5 {};
        double d5 = 0.0000123;
        s5 << fixed << d5;
        if (s5.str() != "0.000012")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, fixed positive, small\n";
        }
        // double, fixed positive, large
        ostringstream s6 {};
        double d6 = 12300000;
        s6 << fixed << d6;
        if (s6.str() != "12300000.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, fixed positive, large\n";
        }
        // double, exp positive
        ostringstream s7 {};
        double d7 = 123.123;
        s7 << scientific << d7;
        if (s7.str() != "1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, scientific positive\n";
        }
        // double, exp positive, small
        ostringstream s8 {};
        double d8 = 0.000000123;
        s8 << scientific << d8;
        if (s8.str() != "1.230000e-07")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, scientific positive, small\n";
        }
        // double, exp positive, large
        ostringstream s9 {};
        double d9 = 12300000;
        s9 << scientific << d9;
        if (s9.str() != "1.230000e+07")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, scientific positive, large\n";
        }
        // double, default negative
        ostringstream s10 {};
        double d10 = -123.123;
        s10 << d10;
        if (s10.str() != "-123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, default negative\n";
        }
        // double, default negative, small
        ostringstream s11 {};
        double d11 = -0.000000123;
        s11 << d11;
        if (s11.str() != "-1.23e-7")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, default negative, small\n";
        }
        // double, default negative, large
        ostringstream s12 {};
        double d12 = -12300000;
        s12 << d12;
        if (s12.str() != "-1.23e7")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, default negative, large\n";
        }
        // double, fixed negative
        ostringstream s13 {};
        double d13 = -123.123;
        s13 << fixed << d13;
        if (s13.str() != "-123.123000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, fixed negative\n";
        }
        // double, fixed negative, small
        ostringstream s14 {};
        double d14 = -0.0000123;
        s14 << fixed << d14;
        if (s14.str() != "-0.000012")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, fixed negative, small\n";
        }
        // double, fixed negative, large
        ostringstream s15 {};
        double d15 = -12300000;
        s15 << fixed << d15;
        if (s15.str() != "-12300000.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, fixed negative, large\n";
        }
        // double, exp negative
        ostringstream s16 {};
        double d16 = -123.123;
        s16 << scientific << d16;
        if (s16.str() != "-1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, scientific negative\n";
        }
        // double, exp negative, small
        ostringstream s17 {};
        double d17 = -0.000000123;
        s17 << scientific << d17;
        if (s17.str() != "-1.230000e-07")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, scientific negative, small\n";
        }
        // double, exp negative, large
        ostringstream s18 {};
        double d18 = -12300000;
        s18 << scientific << d18;
        if (s18.str() != "-1.230000e+07")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, scientific negative, large\n";
        }
        // double, default zero
        ostringstream s19 {};
        double d19 = 0;
        s19 << d19;
        if (s19.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, default zero\n";
        }
        // double, fixed zero
        ostringstream s20 {};
        double d20 = 0;
        s20 << fixed << d20;
        if (s20.str() != "0.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, fixed zero\n";
        }
        // double, exp zero
        ostringstream s21 {};
        double d21 = 0;
        s21 << scientific << d21;
        if (s21.str() != "0.000000e+00")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, scientific zero\n";
        }
        // double, showpos, default positive
        ostringstream s22 {};
        double d22 = 123.123;
        s22 << showpos << d22;
        if (s22.str() != "+123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpos, default positive\n";
        }
        // double, showpos, default negative
        ostringstream s23 {};
        double d23 = -123.123;
        s23 << showpos << d23;
        if (s23.str() != "-123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpos, default negative\n";
        }
        // double, showpos, default zero
        ostringstream s24 {};
        double d24 = 0;
        s24 << showpos << d24;
        if (s24.str() != "+0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpos, default zero\n";
        }
        // double, showpos, fixed positive
        ostringstream s25 {};
        double d25 = 123.123;
        s25 << fixed << showpos << d25;
        if (s25.str() != "+123.123000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpos, fixed positive\n";
        }
        // double, showpos, fixed negative
        ostringstream s26 {};
        double d26 = -123.123;
        s26 << fixed << showpos << d26;
        if (s26.str() != "-123.123000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpos, fixed negative\n";
        }
        // double, showpos, fixed zero
        ostringstream s27 {};
        double d27 = 0;
        s27 << fixed << showpos << d27;
        if (s27.str() != "+0.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpos, fixed zero\n";
        }
        // double, showpos, exp positive
        ostringstream s28 {};
        double d28 = 123.123;
        s28 << scientific << showpos << d28;
        if (s28.str() != "+1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpos, scientific positive\n";
        }
        // double, showpos, exp negative
        ostringstream s29 {};
        double d29 = -123.123;
        s29 << scientific << showpos << d29;
        if (s29.str() != "-1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpos, scientific negative\n";
        }
        // double, showpos, exp zero
        ostringstream s30 {};
        double d30 = 0;
        s30 << scientific << showpos << d30;
        if (s30.str() != "+0.000000e+00")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpos, scientific zero\n";
        }
        // double, uppercase, default positive
        ostringstream s31 {};
        double d31 = 123.123;
        s31 << uppercase << d31;
        if (s31.str() != "123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, uppercase, default positive\n";
        }
        // double, uppercase, fixed positive
        ostringstream s32 {};
        double d32 = 123.123;
        s32 << fixed << uppercase << d32;
        if (s32.str() != "123.123000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, uppercase, fixed positive\n";
        }
        // double, uppercase, exp positive
        ostringstream s33 {};
        double d33 = 123.123;
        s33 << scientific << uppercase << d33;
        if (s33.str() != "1.231230E+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, uppercase, scientific positive\n";
        }
        // double, showpoint, default positive, with point
        ostringstream s34 {};
        double d34 = 123.123;
        s34 << showpoint << d34;
        if (s34.str() != "123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpoint, default positive, with point\n";
        }
        // double, showpoint, fixed positive, with point
        ostringstream s35 {};
        double d35 = 123.123;
        s35 << fixed << showpoint << d35;
        if (s35.str() != "123.123000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpoint, fixed positive, with point\n";
        }
        // double, showpoint, exp positive, with point
        ostringstream s36 {};
        double d36 = 123.123;
        s36 << scientific << showpoint << d36;
        if (s36.str() != "1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpoint, scientific positive, with point\n";
        }
        // double, showpoint, default positive, without point
        ostringstream s37 {};
        double d37 = 123;
        s37 << showpoint << d37;
        if (s37.str() != "123.000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpoint, default positive, without point\n";
        }
        // double, showpoint, fixed positive, without point
        ostringstream s38 {};
        double d38 = 123;
        s38 << fixed << showpoint << d38;
        if (s38.str() != "123.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpoint, fixed positive, without point\n";
        }
        // double, showpoint, exp positive, without point
        ostringstream s39 {};
        double d39 = 1;
        s39 << scientific << showpoint << d39;
        if (s39.str() != "1.000000e+00")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << double, showpoint, scientific positive, without point\n";
        }
    }
    // operator <<, long double
    {
        // long double, default positive
        ostringstream s1 {};
        long double ld1 = 123.123;
        s1 << ld1;
        if (s1.str() != "123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, default positive\n";
        }
        // long double, default positive, small
        ostringstream s2 {};
        long double ld2 = 0.000000123;
        s2 << ld2;
        if (s2.str() != "1.23e-7")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, default positive, small\n";
        }
        // long double, default positive, large
        ostringstream s3 {};
        long double ld3 = 12300000;
        s3 << ld3;
        if (s3.str() != "1.23e7")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, default positive, large\n";
        }
        // long double, fixed positive
        ostringstream s4 {};
        long double ld4 = 123.123;
        s4 << fixed << ld4;
        if (s4.str() != "123.123000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, fixed positive\n";
        }
        // long double, fixed positive, small
        ostringstream s5 {};
        long double ld5 = 0.0000123;
        s5 << fixed << ld5;
        if (s5.str() != "0.000012")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, fixed positive, small\n";
        }
        // long double, fixed positive, large
        ostringstream s6 {};
        long double ld6 = 12300000;
        s6 << fixed << ld6;
        if (s6.str() != "12300000.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, fixed positive, large\n";
        }
        // long double, exp positive
        ostringstream s7 {};
        long double ld7 = 123.123;
        s7 << scientific << ld7;
        if (s7.str() != "1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, scientific positive\n";
        }
        // long double, exp positive, small
        ostringstream s8 {};
        long double ld8 = 0.000000123;
        s8 << scientific << ld8;
        if (s8.str() != "1.230000e-07")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, scientific positive, small\n";
        }
        // long double, exp positive, large
        ostringstream s9 {};
        long double ld9 = 12300000;
        s9 << scientific << ld9;
        if (s9.str() != "1.230000e+07")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, scientific positive, large\n";
        }
        // long double, default negative
        ostringstream s10 {};
        long double ld10 = -123.123;
        s10 << ld10;
        if (s10.str() != "-123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, default negative\n";
        }
        // long double, default negative, small
        ostringstream s11 {};
        long double ld11 = -0.000000123;
        s11 << ld11;
        if (s11.str() != "-1.23e-7")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, default negative, small\n";
        }
        // long double, default negative, large
        ostringstream s12 {};
        long double ld12 = -12300000;
        s12 << ld12;
        if (s12.str() != "-1.23e7")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, default negative, large\n";
        }
        // long double, fixed negative
        ostringstream s13 {};
        long double ld13 = -123.123;
        s13 << fixed << ld13;
        if (s13.str() != "-123.123000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, fixed negative\n";
        }
        // long double, fixed negative, small
        ostringstream s14 {};
        long double ld14 = -0.0000123;
        s14 << fixed << ld14;
        if (s14.str() != "-0.000012")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, fixed negative, small\n";
        }
        // long double, fixed negative, large
        ostringstream s15 {};
        long double ld15 = -12300000;
        s15 << fixed << ld15;
        if (s15.str() != "-12300000.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, fixed negative, large\n";
        }
        // long double, exp negative
        ostringstream s16 {};
        long double ld16 = -123.123;
        s16 << scientific << ld16;
        if (s16.str() != "-1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, scientific negative\n";
        }
        // long double, exp negative, small
        ostringstream s17 {};
        long double ld17 = -0.000000123;
        s17 << scientific << ld17;
        if (s17.str() != "-1.230000e-07")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, scientific negative, small\n";
        }
        // long double, exp negative, large
        ostringstream s18 {};
        long double ld18 = -12300000;
        s18 << scientific << ld18;
        if (s18.str() != "-1.230000e+07")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, scientific negative, large\n";
        }
        // long double, default zero
        ostringstream s19 {};
        long double ld19 = 0;
        s19 << ld19;
        if (s19.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, default zero\n";
        }
        // long double, fixed zero
        ostringstream s20 {};
        long double ld20 = 0;
        s20 << fixed << ld20;
        if (s20.str() != "0.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, fixed zero\n";
        }
        // long double, exp zero
        ostringstream s21 {};
        long double ld21 = 0;
        s21 << scientific << ld21;
        if (s21.str() != "0.000000e+00")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, scientific zero\n";
        }
        // long double, showpos, default positive
        ostringstream s22 {};
        long double ld22 = 123.123;
        s22 << showpos << ld22;
        if (s22.str() != "+123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpos, default positive\n";
        }
        // long double, showpos, default negative
        ostringstream s23 {};
        long double ld23 = -123.123;
        s23 << showpos << ld23;
        if (s23.str() != "-123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpos, default negative\n";
        }
        // long double, showpos, default zero
        ostringstream s24 {};
        long double ld24 = 0;
        s24 << showpos << ld24;
        if (s24.str() != "+0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpos, default zero\n";
        }
        // long double, showpos, fixed positive
        ostringstream s25 {};
        long double ld25 = 123.123;
        s25 << fixed << showpos << ld25;
        if (s25.str() != "+123.123000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpos, fixed positive\n";
        }
        // long double, showpos, fixed negative
        ostringstream s26 {};
        long double ld26 = -123.123;
        s26 << fixed << showpos << ld26;
        if (s26.str() != "-123.123000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpos, fixed negative\n";
        }
        // long double, showpos, fixed zero
        ostringstream s27 {};
        long double ld27 = 0;
        s27 << fixed << showpos << ld27;
        if (s27.str() != "+0.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpos, fixed zero\n";
        }
        // long double, showpos, exp positive
        ostringstream s28 {};
        long double ld28 = 123.123;
        s28 << scientific << showpos << ld28;
        if (s28.str() != "+1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpos, scientific positive\n";
        }
        // long double, showpos, exp negative
        ostringstream s29 {};
        long double ld29 = -123.123;
        s29 << scientific << showpos << ld29;
        if (s29.str() != "-1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpos, scientific negative\n";
        }
        // long double, showpos, exp zero
        ostringstream s30 {};
        long double ld30 = 0;
        s30 << scientific << showpos << ld30;
        if (s30.str() != "+0.000000e+00")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpos, scientific zero\n";
        }
        // long double, uppercase, default positive
        ostringstream s31 {};
        long double ld31 = 123.123;
        s31 << uppercase << ld31;
        if (s31.str() != "123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, uppercase, default positive\n";
        }
        // long double, uppercase, fixed positive
        ostringstream s32 {};
        long double ld32 = 123.123;
        s32 << fixed << uppercase << ld32;
        if (s32.str() != "123.123000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, uppercase, fixed positive\n";
        }
        // long double, uppercase, exp positive
        ostringstream s33 {};
        long double ld33 = 123.123;
        s33 << scientific << uppercase << ld33;
        if (s33.str() != "1.231230E+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, uppercase, scientific positive\n";
        }
        // long double, showpoint, default positive, with point
        ostringstream s34 {};
        long double ld34 = 123.123;
        s34 << showpoint << ld34;
        if (s34.str() != "123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpoint, default positive, with point\n";
        }
        // long double, showpoint, fixed positive, with point
        ostringstream s35 {};
        long double ld35 = 123.123;
        s35 << fixed << showpoint << ld35;
        if (s35.str() != "123.123000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpoint, fixed positive, with point\n";
        }
        // long double, showpoint, exp positive, with point
        ostringstream s36 {};
        long double ld36 = 123.123;
        s36 << scientific << showpoint << ld36;
        if (s36.str() != "1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpoint, scientific positive, with point\n";
        }
        // long double, showpoint, default positive, without point
        ostringstream s37 {};
        long double ld37 = 123;
        s37 << showpoint << ld37;
        if (s37.str() != "123.000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpoint, default positive, without point\n";
        }
        // long double, showpoint, fixed positive, without point
        ostringstream s38 {};
        long double ld38 = 123;
        s38 << fixed << showpoint << ld38;
        if (s38.str() != "123.000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpoint, fixed positive, without point\n";
        }
        // long double, showpoint, exp positive, without point
        ostringstream s39 {};
        long double ld39 = 1;
        s39 << scientific << showpoint << ld39;
        if (s39.str() != "1.000000e+00")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << long double, showpoint, scientific positive, without point\n";
        }
    }
    // operator <<, bool
    {
        // bool true, as int
        ostringstream s1 {};
        bool b1 = true;
        s1 << b1;
        if (s1.str() != "1")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << bool, true, as int\n";
        }
        // bool false, as int
        ostringstream s2 {};
        bool b2 = false;
        s2 << b2;
        if (s2.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << bool, false, as int\n";
        }
        // bool true, as alpha
        ostringstream s3 {};
        bool b3 = true;
        s3 << boolalpha << b3;
        if (s3.str() != "true")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << bool, true, as alpha\n";
        }
        // bool false, as alpha
        ostringstream s4 {};
        bool b4 = false;
        s4 << boolalpha << b4;
        if (s4.str() != "false")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << bool, false, as alpha\n";
        }
        // bool true, as alpha, uppercase (uppercase has no effect)
        ostringstream s5 {};
        bool b5 = true;
        s5 << boolalpha << uppercase << b5;
        if (s5.str() != "true")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << bool, true, as alpha, uppercase\n";
        }
        // bool false, as alpha uppercase (uppercase has no effect)
        ostringstream s6 {};
        bool b6 = false;
        s6 << boolalpha << uppercase << b6;
        if (s6.str() != "false")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << bool, false, as alpha, uppercase\n";
        }
    }
    // operator <<, pointer
    {
        // pointer, valid
        ostringstream s1 {};
        const void* p1 = reinterpret_cast<void*>(0xCAFEBABE);
        s1 << p1;
        if (s1.str() != "0xcafebabe")
        {
            std::cout << "s1 == " << s1.str() << '\n';
            ++fail_count;
            std::cout << "FAILED ostringstream operator << pointer, valid\n";
        }
        // pointer, valid, uppercase
        ostringstream s2 {};
        const void* p2 = reinterpret_cast<void*>(0xCAFEBABE);
        s2 << uppercase << p2;
        if (s2.str() != "0XCAFEBABE")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << pointer, valid, uppercase\n";
        }
        // pointer, nullptr
        ostringstream s3 {};
        const void* p3 = nullptr;
        s3 << p3;
        if (s3.str() != "0")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << pointer, nullptr\n";
        }
    }
    // operator<< streambuf
    {
        // should work
        istringstream s1 {"Hello, world!"};
        ostringstream s2 {""};
        s2 << s1.rdbuf();
        if (!s1 || s1.eof() || s1.fail() || s1.bad() || !s2 || s2.eof() ||
            s2.fail() || s2.bad() || s2.str() != "Hello, world!")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << streambuf, should work\n";
        }
        // fail, no exception
        ostringstream s3 {"Hello, world!"};
        ostringstream s4 {""};
        s4 << s3.rdbuf();
        if (!s3 || s3.eof() || s3.fail() || s3.bad() || s4 || s4.eof() ||
            !s4.fail() || s4.bad() || s4.str() != "")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << streambuf, fail, no exception\n";
        }
        // fail, exception
        ostringstream s5 {"Hello, world!"};
        ostringstream s6 {""};
        s6.exceptions(ios_base::failbit);
        try {
            s6 << s5.rdbuf();
            ++fail_count;
            std::cout << "FAILED ostringstream operator << streambuf, fail, exception, no throw\n";
        }
        catch (ios_base::failure&)
        {
            if (!s5 || s5.eof() || s5.fail() || s5.bad() || s6 || s6.eof() ||
                !s6.fail() || s6.bad() || s6.str() != "")
            {
                ++fail_count;
                std::cout << "FAILED ostringstream operator << streambuf, fail, exception\n";
            }
        }
        catch (...)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator << streambuf, fail, exception, wrong exception type\n";
        }
    }
    // Unformatted output
    {
        // put
        ostringstream s1 {};
        s1.put('a');
        if (s1.str() != "a")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream put\n";
        }
        // write
        ostringstream s2 {};
        s2.write("Hello, world!", 12);
        if (s2.str() != "Hello, world")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream write\n";
        }
        // write, zero count
        ostringstream s3 {};
        s3.write("Hello, world!", 0);
        if (s3.str() != "")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream write, zero count\n";
        }
    }
    // Pointer positioning
    {
        // tellp, initial
        ostringstream s1 {};
        if (s1.tellp() != 0)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream tellp, initial\n";
        }
        // tellp, after output
        ostringstream s2 {};
        s2 << "Hello, world!";
        if (s2.tellp() != 13)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream tellp, after output\n";
        }
        // seekp, absolute, valid
        ostringstream s3 {};
        s3 << "Hello, world!";
        s3.seekp(3);
        if (s3.tellp() != 3)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream seekp, absolute, valid\n";
        }
        // seekp, absolute, valid, overwrite.
        ostringstream s4 {};
        s4 << "Hello, world!";
        s4.seekp(3);
        s4 << "ix";
        if (s4.tellp() != 5 || s4.str() != "Helix, world!")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream seekp, absolute, valid, overwrite\n";
        }
        // seekp, absolute, invalid
        ostringstream s5 {};
        s5 << "Hello, world!";
        s5.seekp(-1);
        if (!s5.fail())
        {
            ++fail_count;
            std::cout << "FAILED ostringstream seekp, absolute, invalid\n";
        }
        // seekp, relative, beg, valid
        ostringstream s6 {};
        s6 << "Hello, world!";
        s6.seekp(3, ios_base::beg);
        if (s6.tellp() != 3)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream seekp, relative, beg, valid\n";
        }
        // seekp, relative, beg, valid overwrite.
        ostringstream s7 {};
        s7 << "Hello, world!";
        s7.seekp(3, ios_base::beg);
        s7 << "ix";
        if (s7.tellp() != 5 || s7.str() != "Helix, world!")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream seekp, relative, beg, valid, overwrite\n";
        }
        // seekp, relative, beg, valid invalid
        ostringstream s8 {};
        s8 << "Hello, world!";
        s8.seekp(-1, ios_base::beg);
        if (!s8.fail())
        {
            ++fail_count;
            std::cout << "FAILED ostringstream seekp, relative, beg, invalid\n";
        }
        // seekp, relative, cur, valid
        ostringstream s9 {};
        s9 << "Hello, world!";
        s9.seekp(3);
        s9.seekp(2, ios_base::cur);
        if (s9.tellp() != 5)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream seekp, relative, cur, valid\n";
        }
        // seekp, relative, cur, valid overwrite.
        ostringstream s10 {};
        s10 << "Hello, world!";
        s10.seekp(5);
        s10.seekp(-2, ios_base::cur);
        s10 << "ix";
        if (s10.tellp() != 5 || s10.str() != "Helix, world!")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream seekp, relative, cur, valid, overwrite\n";
        }
        // seekp, relative, cur, valid invalid
        ostringstream s11 {};
        s11 << "Hello, world!";
        s11.seekp(3);
        s11.seekp(11, ios_base::cur);
        if (!s11.fail())
        {
            ++fail_count;
            std::cout << "FAILED ostringstream seekp, relative, cur, invalid\n";
        }
        // seekp, relative, end, valid
        ostringstream s12 {};
        s12 << "Hello, world!";
        s12.seekp(-8, ios_base::end);
        if (s12.tellp() != 5)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream seekp, relative, end, valid\n";
        }
        // seekp, relative, end, valid overwrite.
        ostringstream s13 {};
        s13 << "Hello, world!";
        s13.seekp(-10, ios_base::end);
        s13 << "ix";
        if (s13.tellp() != 5 || s13.str() != "Helix, world!")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream seekp, relative, end, valid, overwrite\n";
        }
        // seekp, relative, end, valid invalid
        ostringstream s14 {};
        s14 << "Hello, world!";
        s14.seekp(1, ios_base::end);
        if (!s14.fail())
        {
            ++fail_count;
            std::cout << "FAILED ostringstream seekp, relative, end, invalid\n";
        }
    }
    // Flush
    {
        // Flush does nothing for this stream.
        ostringstream s1 {};
        s1 << "Hello, world!";
        s1.flush();
        if (s1.str() != "Hello, world!")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream flush\n";
        }
    }
    // setstate
    {
        // Set state, good
        ostringstream s1 {};
        s1.setstate(ios_base::goodbit);
        if (!s1.good() || s1.fail() || s1.bad() || s1.eof() || !s1 ||
            static_cast<bool>(s1) == false || s1.rdstate() != ios_base::goodbit)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream setstate good\n";
        }
        // Set state, eof
        ostringstream s2 {};
        s2.setstate(ios_base::eofbit);
        if (s2.good() || s2.fail() || s2.bad() || !s2.eof() || !s2 ||
            static_cast<bool>(s2) == false || s2.rdstate() != ios_base::eofbit)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream setstate eof\n";
        }
        // Set state, fail
        ostringstream s3 {};
        s3.setstate(ios_base::failbit);
        if (s3.good() || !s3.fail() || s3.bad() || s3.eof() || s3 ||
            static_cast<bool>(s3) == true || s3.rdstate() != ios_base::failbit)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream setstate fail\n";
        }
        // Set state, bad
        ostringstream s4 {};
        s4.setstate(ios_base::badbit);
        if (s4.good() || !s4.fail() || !s4.bad() || s4.eof() || s4 ||
            static_cast<bool>(s4) == true || s4.rdstate() != ios_base::badbit)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream setstate bad\n";
        }
        // Set state, eof and fail
        ostringstream s5 {};
        s5.setstate(ios_base::eofbit | ios_base::failbit);
        if (s5.good() || !s5.fail() || s5.bad() || !s5.eof() || s5 ||
            static_cast<bool>(s5) == true ||
            s5.rdstate() != (ios_base::eofbit | ios_base::failbit))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream setstate eof and fail\n";
        }
        // Set state, fail and bad
        ostringstream s6 {};
        s6.setstate(ios_base::failbit | ios_base::badbit);
        if (s6.good() || !s6.fail() || !s6.bad() || s6.eof() || s6 ||
            static_cast<bool>(s6) == true ||
            s6.rdstate() != (ios_base::failbit | ios_base::badbit))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream setstate fail and bad\n";
        }
        // Set state, bad and eof
        ostringstream s7 {};
        s7.setstate(ios_base::badbit | ios_base::eofbit);
        if (s7.good() || !s7.fail() || !s7.bad() || !s7.eof() || s7 ||
            static_cast<bool>(s7) == true ||
            s7.rdstate() != (ios_base::badbit | ios_base::eofbit))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream setstate bad and eof\n";
        }
        // Set state, eof, fail and bad, sequential
        ostringstream s8 {};
        s8.setstate(ios_base::eofbit);
        s8.setstate(ios_base::failbit);
        s8.setstate(ios_base::badbit);
        if (s8.good() || !s8.fail() || !s8.bad() || !s8.eof() || s8 ||
            static_cast<bool>(s8) == true ||
            s8.rdstate() != (ios_base::eofbit | ios_base::failbit |
            ios_base::badbit))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream setstate eof, fail and bad, sequential\n";
        }
        // Set state, eof, fail and bad, together
        ostringstream s9 {};
        s9.setstate(ios_base::eofbit | ios_base::failbit | ios_base::badbit);
        if (s9.good() || !s9.fail() || !s9.bad() || !s9.eof() || s9 ||
            static_cast<bool>(s9) == true ||
            s9.rdstate() != (ios_base::eofbit | ios_base::failbit |
            ios_base::badbit))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream setstate eof, fail and bad, together\n";
        }
    }
    // clear
    {
        // Clear, good
        ostringstream s1 {};
        s1.clear(ios_base::eofbit | ios_base::failbit | ios_base::badbit);
        s1.clear(ios_base::goodbit);
        if (!s1.good() || s1.fail() || s1.bad() || s1.eof() || !s1 ||
            static_cast<bool>(s1) == false || s1.rdstate() != ios_base::goodbit)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream clear good\n";
        }
        // Clear, eof
        ostringstream s2 {};
        s2.clear(ios_base::eofbit);
        if (s2.good() || s2.fail() || s2.bad() || !s2.eof() || !s2 ||
            static_cast<bool>(s2) == false || s2.rdstate() != ios_base::eofbit)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream clear eof\n";
        }
        // Clear, fail
        ostringstream s3 {};
        s3.clear(ios_base::failbit);
        if (s3.good() || !s3.fail() || s3.bad() || s3.eof() || s3 ||
            static_cast<bool>(s3) == true || s3.rdstate() != ios_base::failbit)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream clear fail\n";
        }
        // Clear, bad
        ostringstream s4 {};
        s4.clear(ios_base::badbit);
        if (s4.good() || !s4.fail() || !s4.bad() || s4.eof() || s4 ||
            static_cast<bool>(s4) == true || s4.rdstate() != ios_base::badbit)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream clear bad\n";
        }
        // Clear, eof and fail
        ostringstream s5 {};
        s5.clear(ios_base::eofbit | ios_base::failbit);
        if (s5.good() || !s5.fail() || s5.bad() || !s5.eof() || s5 ||
            static_cast<bool>(s5) == true ||
            s5.rdstate() != (ios_base::eofbit | ios_base::failbit))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream clear eof and fail\n";
        }
        // Clear, fail and bad
        ostringstream s6 {};
        s6.clear(ios_base::failbit | ios_base::badbit);
        if (s6.good() || !s6.fail() || !s6.bad() || s6.eof() || s6 ||
            static_cast<bool>(s6) == true ||
            s6.rdstate() != (ios_base::failbit | ios_base::badbit))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream clear fail and bad\n";
        }
        // Clear, bad and eof
        ostringstream s7 {};
        s7.clear(ios_base::badbit | ios_base::eofbit);
        if (s7.good() || !s7.fail() || !s7.bad() || !s7.eof() || s7 ||
            static_cast<bool>(s7) == true ||
            s7.rdstate() != (ios_base::badbit | ios_base::eofbit))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream clear bad and eof\n";
        }
        // Clear, eof, fail and bad, sequential
        ostringstream s8 {};
        s8.clear(ios_base::eofbit);
        s8.clear(ios_base::failbit);
        s8.clear(ios_base::badbit);
        if (s8.good() || !s8.fail() || !s8.bad() || s8.eof() || s8 ||
            static_cast<bool>(s8) == true || s8.rdstate() != ios_base::badbit)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream clear eof, fail and bad, sequential\n";
        }
        // Clear, eof, fail and bad, together
        ostringstream s9 {};
        s9.clear(ios_base::eofbit | ios_base::failbit | ios_base::badbit);
        if (s9.good() || !s9.fail() || !s9.bad() || !s9.eof() || s9 ||
            static_cast<bool>(s9) == true ||
            s9.rdstate() != (ios_base::eofbit | ios_base::failbit |
            ios_base::badbit))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream clear eof, fail and bad, together\n";
        }
        // Clear, default
        ostringstream s10 {};
        s10.clear(ios_base::eofbit | ios_base::failbit | ios_base::badbit);
        s10.clear();
        if (!s10.good() || s10.fail() || s10.bad() || s10.eof() || !s10 ||
            static_cast<bool>(s10) == false ||
            s10.rdstate() != ios_base::goodbit)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream clear default\n";
        }
    }
    // copyfmt
    {
        // Set values to non-default and test the copy. Should copy everything
        // except the buffer and the iostate.
        ostringstream s1 {"hello"};
        s1.flags(ios_base::hex);
        s1.precision(5);
        s1.width(1);
        s1.setstate(ios_base::eofbit);
        s1.exceptions(ios_base::failbit);
        ostringstream s2 {};
        s1.tie(&s2);
        s1.fill('a');
        ostringstream s3 {"goodbye"};
        s3.copyfmt(s1);
        if (s3.flags() != ios_base::hex || s3.precision() != 5 ||
            s3.width() != 1 || s3.rdstate() != ios_base::goodbit ||
            s3.exceptions() != ios_base::failbit || s3.tie() != &s2 ||
            s3.fill() != 'a' || s3.str() != "goodbye")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream copyfmt\n";
        }
    }
    // fill
    {
        // default fill
        ostringstream s1 {};
        if (s1.fill() != ' ')
        {
            ++fail_count;
            std::cout << "FAILED ostringstream default fill\n";
        }
        // change fill
        ostringstream s2 {};
        s2.fill('a');
        if (s2.fill() != 'a')
        {
            ++fail_count;
            std::cout << "FAILED ostringstream change fill\n";
        }
    }
    // exceptions
    {
        // No exceptions
        ostringstream s1 {};
        s1.exceptions(ios_base::goodbit);
        try {
            s1.setstate(ios_base::badbit | ios_base::eofbit |
                ios_base::failbit);
        }
        catch (ios_base::failure&)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions none\n";
        }
        // fail
        ostringstream s2 {};
        s2.exceptions(ios_base::failbit);
        try {
            s2.setstate(ios_base::badbit | ios_base::eofbit);
        }
        catch (ios_base::failure&)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions fail false trigger\n";
        }
        try {
            s2.setstate(ios_base::failbit);
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions fail no trigger\n";
        }
        catch (ios_base::failure&) {}
        // bad
        ostringstream s3 {};
        s3.exceptions(ios_base::badbit);
        try {
            s3.setstate(ios_base::failbit | ios_base::eofbit);
        }
        catch (ios_base::failure&)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions bad false trigger\n";
        }
        try {
            s3.setstate(ios_base::badbit);
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions bad no trigger\n";
        }
        catch (ios_base::failure&) {}
        // eof
        ostringstream s4 {};
        s4.exceptions(ios_base::eofbit);
        try {
            s4.setstate(ios_base::failbit | ios_base::badbit);
        }
        catch (ios_base::failure&)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions bad false trigger\n";
        }
        try {
            s4.setstate(ios_base::eofbit);
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions bad no trigger\n";
        }
        catch (ios_base::failure&) {}
        // fail and bad
        ostringstream s5 {};
        s5.exceptions(ios_base::failbit | ios_base::badbit);
        try {
            s5.setstate(ios_base::eofbit);
        }
        catch (ios_base::failure&)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions fail and bad false trigger\n";
        }
        try {
            s5.setstate(ios_base::failbit);
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions fail and bad no trigger for fail\n";
        }
        catch (ios_base::failure&) {}
        try {
            s5.setstate(ios_base::badbit);
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions fail and bad no trigger for bad\n";
        }
        catch (ios_base::failure&) {}
        // fail and eof
        ostringstream s6 {};
        s6.exceptions(ios_base::failbit | ios_base::eofbit);
        try {
            s6.setstate(ios_base::badbit);
        }
        catch (ios_base::failure&)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions fail and eof false trigger\n";
        }
        try {
            s6.setstate(ios_base::failbit);
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions fail and eof no trigger for fail\n";
        }
        catch (ios_base::failure&) {}
        try {
            s6.setstate(ios_base::eofbit);
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions fail and eof no trigger for eof\n";
        }
        catch (ios_base::failure&) {}
        // bad and eof
        ostringstream s7 {};
        s7.exceptions(ios_base::badbit | ios_base::eofbit);
        try {
            s7.setstate(ios_base::failbit);
        }
        catch (ios_base::failure&)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions bad and eof false trigger\n";
        }
        try {
            s7.setstate(ios_base::badbit);
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions bad and eof no trigger for bad\n";
        }
        catch (ios_base::failure&) {}
        try {
            s7.setstate(ios_base::eofbit);
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions bad and eof no trigger for eof\n";
        }
        catch (ios_base::failure&) {}
        // all exceptions
        ostringstream s8 {};
        s8.exceptions(ios_base::failbit | ios_base::badbit | ios_base::eofbit);
        try {
            s8.setstate(ios_base::failbit);
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions all no trigger for fail\n";
        }
        catch (ios_base::failure&) {}
        try {
            s8.setstate(ios_base::badbit);
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions all no trigger for bad\n";
        }
        catch (ios_base::failure&) {}
        try {
            s8.setstate(ios_base::eofbit);
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions all no trigger for eof\n";
        }
        catch (ios_base::failure&) {}
        // Default get
        ostringstream s9 {};
        if (s9.exceptions() != ios_base::goodbit)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions default get\n";
        }
        // set and get
        ostringstream s10 {};
        ios_base::iostate st = s10.exceptions(ios_base::failbit);
        if (s10.exceptions() != ios_base::failbit || st != ios_base::goodbit)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream exceptions set and get\n";
        }
    }
    // TODO imbue (locales not implemented)
    // rdbuf
    {
        // Get
        ostringstream s1 {"Hello, world!"};
        if (!s1.rdbuf())
        {
            ++fail_count;
            std::cout << "FAILED ostringstream rdbuf get\n";
        }
        // Set is overriden and does not exist.
    }
    // tie
    {
        // Default get
        ostringstream s1 {"Hello, world!"};
        if (s1.tie())
        {
            ++fail_count;
            std::cout << "FAILED ostringstream tie default get\n";
        }
        // Default set
        ostringstream s2 {"Hello, world!"};
        ostringstream s3 {};
        ostream* o1 = s2.tie(&s3);
        if (o1 || s2.tie() != &s3)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream tie default set\n";
        }
        // Replace set
        ostringstream s4 {"Hello, world!"};
        ostringstream s5 {};
        ostringstream s6 {};
        s4.tie(&s5);
        ostream* o2 = s4.tie(&s6);
        if (o2 != &s5 || s4.tie() != &s6)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream tie replace set\n";
        }
    }
    // TODO narrow and widen (locales not implemented)
    // flags
    {
        // Default get
        ostringstream s1 {};
        if (s1.flags() != (ios_base::skipws | ios_base::dec))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream flags default get\n";
        }
        // Default set
        ostringstream s2 {};
        ios_base::fmtflags f1 = ios_base::oct | ios_base::internal |
            ios_base::fixed | ios_base::showbase | ios_base::uppercase;
        ios_base::fmtflags f2 = s2.flags(f1);
        if (f2 != (ios_base::skipws | ios_base::dec) || s2.flags() != f1)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream flags default set\n";
        }
        // Replace set
        ostringstream s3 {};
        ios_base::fmtflags f3 = ios_base::oct | ios_base::internal |
            ios_base::fixed | ios_base::showbase | ios_base::uppercase;
        ios_base::fmtflags f4 = ios_base::hex | ios_base::left |
            ios_base::scientific | ios_base::showpos | ios_base::unitbuf;
        s3.flags(f3);
        ios_base::fmtflags f5 = s3.flags(f4);
        if (f5 != f3 || s3.flags() != f4)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream flags replace set\n";
        }
    }
    // setf
    {
        // Set new flags
        ostringstream s1 {};
        s1.setf(ios_base::showbase);
        if (s1.flags() !=
            (ios_base::skipws | ios_base::dec | ios_base::showbase))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream setf set new\n";
        }
        // Clear and set
        ostringstream s2 {};
        s2.setf(ios_base::oct, ios_base::basefield);
        if (s2.flags() != (ios_base::skipws | ios_base::oct))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream setf clear and set\n";
        }
        // Clear
        ostringstream s3 {};
        s3.setf(static_cast<ios_base::fmtflags>(0), ios_base::basefield);
        if (s3.flags() != ios_base::skipws)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream setf clear\n";
        }
    }
    // unsetf
    {
        // Clear a specific flag
        ostringstream s1 {};
        s1.unsetf(ios_base::skipws);
        if (s1.flags() != ios_base::dec)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream unsetf clear specific\n";
        }
        // Clear an unset flag
        ostringstream s2 {};
        s2.unsetf(ios_base::showpos);
        if (s2.flags() != (ios_base::skipws | ios_base::dec))
        {
            ++fail_count;
            std::cout << "FAILED ostringstream unsetf clear unset\n";
        }
        // Clear by group
        ostringstream s3 {};
        s3.unsetf(ios_base::basefield);
        if (s3.flags() != ios_base::skipws)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream unsetf clear group\n";
        }
    }
    // precision (note, io manipulator for this is in iomanip)
    {
        // Default get
        ostringstream s1 {};
        if (s1.precision() != 6)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream precision default get\n";
        }
        // Function set
        ostringstream s2 {};
        s2.precision(10);
        if (s2.precision() != 10)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream precision function set\n";
        }
        // default, precision 10
        ostringstream s3 {};
        s3.precision(10);
        float f1 = 123;
        s3 << f1;
        if (s3.str() != "123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream precision default float\n";
        }
        // fixed, precision 10
        ostringstream s4 {};
        s4.precision(10);
        float f2 = 123;
        s4 << fixed << f2;
        if (s4.str() != "123.0000000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream precision fixed float\n";
        }
        // scientific, precision 10
        ostringstream s5 {};
        s5.precision(10);
        float f3 = 123;
        s5 << scientific << f3;
        if (s5.str() != "1.2300000000e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream precision scientific float\n";
        }
        // default, showpoint, precision 10
        ostringstream s6 {};
        s6.precision(10);
        float f4 = 123;
        s6 << showpoint << f4;
        if (s6.str() != "123.0000000")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream precision default float, showpoint\n";
        }
    }
    // width (note, io manipulator for this is in iomanip)
    {
        // Default get
        ostringstream s1 {};
        if (s1.width() != 0)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width default get\n";
        }
        // Function set
        ostringstream s2 {};
        s2.width(10);
        if (s2.width() != 10)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width function set\n";
        }
        // Signed, adjust left
        ostringstream s3 {};
        s3.width(10);
        int i1 = -123;
        s3 << left << i1;
        if (s3.str() != "-123      ")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width signed, adjust left\n";
        }
        // Signed, adjust right
        ostringstream s4 {};
        s4.width(10);
        int i2 = -123;
        s4 << right << i2;
        if (s4.str() != "      -123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width signed, adjust right\n";
        }
        // Signed, adjust internal
        ostringstream s5 {};
        s5.width(10);
        int i3 = -123;
        s5 << internal << i3;
        if (s5.str() != "-      123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width signed, adjust internal\n";
        }
        // Unsigned, adjust left, oct
        ostringstream s6 {};
        s6.width(10);
        unsigned int ui1 = 123;
        s6 << left << oct << showbase << ui1;
        if (s6.str() != "0173      ")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width unsigned, adjust left, oct\n";
        }
        // Unsigned, adjust right, oct
        ostringstream s7 {};
        s7.width(10);
        unsigned int ui2 = 123;
        s7 << right << oct << showbase << ui2;
        if (s7.str() != "      0173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width unsigned, adjust right, oct\n";
        }
        // Unsigned, adjust internal, oct
        ostringstream s8 {};
        s8.width(10);
        unsigned int ui3 = 123;
        s8 << internal << oct << showbase << ui3;
        if (s8.str() != "      0173")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width unsigned, adjust internal, oct\n";
        }
        // Unsigned, adjust left, hex
        ostringstream s9 {};
        s9.width(10);
        unsigned int ui4 = 123;
        s9 << left << hex << showbase << ui4;
        if (s9.str() != "0x7b      ")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width unsigned, adjust left, hex\n";
        }
        // Unsigned, adjust right, hex
        ostringstream s10 {};
        s10.width(10);
        unsigned int ui5 = 123;
        s10 << right << hex << showbase << ui5;
        if (s10.str() != "      0x7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width unsigned, adjust right, hex\n";
        }
        // Unsigned, adjust internal, hex
        ostringstream s11 {};
        s11.width(10);
        unsigned int ui6 = 123;
        s11 << internal << hex << showbase << ui6;
        if (s11.str() != "0x      7b")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width unsigned, adjust internal, hex\n";
        }
        // float, adjust left, default
        ostringstream s12 {};
        s12.width(10);
        float f1 = -123.123;
        s12 << left << f1;
        if (s12.str() != "-123.123  ")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width float, adjust left, default\n";
        }
        // float, adjust right, default
        ostringstream s13 {};
        s13.width(10);
        float f2 = -123.123;
        s13 << right << f2;
        if (s13.str() != "  -123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width float, adjust right, default\n";
        }
        // float, adjust internal, default
        ostringstream s14 {};
        s14.width(10);
        float f3 = -123.123;
        s14 << internal << f3;
        if (s14.str() != "-  123.123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width float, adjust internal, default\n";
        }
        // float, adjust left, fixed
        ostringstream s15 {};
        s15.width(15);
        float f4 = -123.123;
        s15 << left << fixed << f4;
        if (s15.str() != "-123.123001    ")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width float, adjust left, fixed\n";
        }
        // float, adjust right, fixed
        ostringstream s16 {};
        s16.width(15);
        float f5 = -123.123;
        s16 << right << fixed << f5;
        if (s16.str() != "    -123.123001")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width float, adjust right, fixed\n";
        }
        // float, adjust internal, fixed
        ostringstream s17 {};
        s17.width(15);
        float f6 = -123.123;
        s17 << internal << fixed << f6;
        if (s17.str() != "-    123.123001")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width float, adjust internal, fixed\n";
        }
        // float, adjust left, scientific
        ostringstream s18 {};
        s18.width(15);
        float f7 = -123.123;
        s18 << left << scientific << f7;
        if (s18.str() != "-1.231230e+02  ")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width float, adjust left, scientific\n";
        }
        // float, adjust right, scientific
        ostringstream s19 {};
        s19.width(15);
        float f8 = -123.123;
        s19 << right << scientific << f8;
        if (s19.str() != "  -1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width float, adjust right, scientific\n";
        }
        // float, adjust internal, scientific
        ostringstream s20 {};
        s20.width(15);
        float f9 = -123.123;
        s20 << internal << scientific << f9;
        if (s20.str() != "-  1.231230e+02")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width float, adjust internal, scientific\n";
        }
        // bool, adjust left
        ostringstream s21 {};
        s21.width(10);
        s21 << boolalpha << left << true;
        if (s21.str() != "true      ")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width bool, adjust left\n";
        }
        // bool, adjust right
        ostringstream s22 {};
        s22.width(10);
        s22 << boolalpha << right << true;
        if (s22.str() != "      true")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width bool, adjust right\n";
        }
        // bool, adjust internal
        ostringstream s23 {};
        s23.width(10);
        s23 << boolalpha << internal << true;
        if (s23.str() != "      true")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width bool, adjust internal\n";
        }
        // pointer, adjust left
        ostringstream s24 {};
        s24.width(15);
        s24 << left << reinterpret_cast<void*>(0xcafebabe);
        if (s24.str() != "0xcafebabe     ")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width pointer, adjust left\n";
        }
        // pointer, adjust right
        ostringstream s25 {};
        s25.width(15);
        s25 << right << reinterpret_cast<void*>(0xcafebabe);
        if (s25.str() != "     0xcafebabe")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width pointer, adjust right\n";
        }
        // pointer, adjust internal
        ostringstream s26 {};
        s26.width(15);
        s26 << internal << reinterpret_cast<void*>(0xcafebabe);
        if (s26.str() != "0x     cafebabe")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width pointer, adjust internal\n";
        }
        // signed, adjust left, change fill character
        ostringstream s27 {};
        s27.width(10);
        s27.fill('a');
        int i4 = -123;
        s27 << left << i4;
        if (s27.str() != "-123aaaaaa")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width signed, adjust left, change fill character\n";
        }
        // signed, adjust left, width reset
        ostringstream s28 {};
        s28.width(10);
        int i5 = -123;
        s28 << left << i5 << i5;
        if (s28.str() != "-123      -123")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width signed, adjust left, width reset\n";
        }
        // char, adjust left
        ostringstream s29 {};
        s29.width(10);
        s29 << left << 'a';
        if (s29.str() != "a         ")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width char, adjust left\n";
        }
        // char, adjust right
        ostringstream s30 {};
        s30.width(10);
        s30 << right << 'a';
        if (s30.str() != "         a")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width char, adjust right\n";
        }
        // char, adjust internal
        ostringstream s31 {};
        s31.width(10);
        s31 << internal << 'a';
        if (s31.str() != "         a")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width char, adjust internal\n";
        }
        // char*, adjust left
        ostringstream s32 {};
        s32.width(10);
        s32 << left << "aa";
        if (s32.str() != "aa        ")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width char*, adjust left\n";
        }
        // char*, adjust right
        ostringstream s33 {};
        s33.width(10);
        s33 << right << "aa";
        if (s33.str() != "        aa")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width char*, adjust right\n";
        }
        // char*, adjust internal
        ostringstream s34 {};
        s34.width(10);
        s34 << internal << "aa";
        if (s34.str() != "        aa")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream width char*, adjust internal\n";
        }
    }
    // TODO xalloc, iword and pword (omitted for now)
    // register_callback
    {
        int i1 = 0;
        int i2 = 0;
        cbp1 = &i1;
        cbp2 = &i2;
        // Register two callback and call them with copy and the destructor.
        {
            ostringstream s1 {};
            s1.register_callback(callback_erase, 1);
            s1.register_callback(callback_copy, 2);
            ostringstream s2 {};
            s2.copyfmt(s1);
            if (*cbp2 != 2)
            {
                ++fail_count;
                std::cout << "FAILED ostringstream register_callback copyfmt\n";
            }
        }
        if (*cbp1 != 1)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream register_callback erase\n";
        }
        cbp1 = nullptr;
        cbp2 = nullptr;
    }
    // failure exception
    {
        // what
        ios_base::failure ex1 {"testing"};
        if (ex1.what() != string{"testing: iostream error"})
        {
            ++fail_count;
            std::cout << "FAILED ostringstream failure what\n";
        }
        // code
        ios_base::failure ex2 {"testing"};
        if (ex2.code() != io_errc::stream)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream failure what\n";
        }
    }
    // non-member operator << char
    {
        // char
        ostringstream s1 {};
        char c1 = 'a';
        s1 << c1;
        if (s1.str() != "a")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator<< char\n";
        }
        // signed char
        ostringstream s2 {};
        signed char c2 = 'a';
        s2 << c2;
        if (s2.str() != "a")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator<< signed char\n";
        }
        // unsigned char
        ostringstream s3 {};
        unsigned char c3 = 'a';
        s3 << c3;
        if (s3.str() != "a")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator<< signed char\n";
        }
    }
    // non-member operator << char*
    {
        // char*
        ostringstream s1 {};
        const char* c1 = "aa";
        s1 << c1;
        if (s1.str() != "aa")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator<< char*\n";
        }
        // signed char*
        ostringstream s2 {};
        signed char c2[3] = {'a', 'a', '\0'};
        s2 << c2;
        if (s2.str() != "aa")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator<< signed char*\n";
        }
        // unsigned char*
        ostringstream s3 {};
        signed char c3[3] = {'a', 'a', '\0'};
        s3 << c3;
        if (s3.str() != "aa")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator<< signed char*\n";
        }
    }
    // non-member operator << rvalue reference
    {
        ostringstream s1 {};
        const char* c1 = "aa";
        klib::move(s1) << c1;
        if (s1.str() != "aa")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream operator<< rvalue reference\n";
        }
    }
    // io manipulators
    {
        // ends
        ostringstream s1 {};
        s1 << "hello" << ends << "world";
        if (strncmp(s1.str().c_str(), "hello\0world", 11) != 0)
        {
            ++fail_count;
            std::cout << "FAILED ostringstream ends\n";
        }
        // flush
        ostringstream s2 {};
        s2 << "hello" << flush;
        if (s2.str() != "hello")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream flush\n";
        }
        // endl
        ostringstream s3 {};
        s3 << "hello" << endl;
        if (s3.str() != "hello\n")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream endl\n";
        }
    }
    // Check that all the openmode values exist
    {
        static_assert(ios_base::app == ios_base::app);
        static_assert(ios_base::binary == ios_base::binary);
        static_assert(ios_base::in == ios_base::in);
        static_assert(ios_base::out == ios_base::out);
        static_assert(ios_base::trunc == ios_base::trunc);
        static_assert(ios_base::ate == ios_base::ate);
    }
    // Check that all the fmtflags values exist
    {
        static_assert(ios_base::dec == ios_base::dec);
        static_assert(ios_base::oct == ios_base::oct);
        static_assert(ios_base::hex == ios_base::hex);
        static_assert(ios_base::basefield == ios_base::basefield);
        static_assert(ios_base::left == ios_base::left);
        static_assert(ios_base::right == ios_base::right);
        static_assert(ios_base::internal == ios_base::internal);
        static_assert(ios_base::adjustfield == ios_base::adjustfield);
        static_assert(ios_base::scientific == ios_base::scientific);
        static_assert(ios_base::fixed == ios_base::fixed);
        static_assert(ios_base::floatfield == ios_base::floatfield);
        static_assert(ios_base::boolalpha == ios_base::boolalpha);
        static_assert(ios_base::showbase == ios_base::showbase);
        static_assert(ios_base::showpoint == ios_base::showpoint);
        static_assert(ios_base::showpos == ios_base::showpos);
        static_assert(ios_base::skipws == ios_base::skipws);
        static_assert(ios_base::unitbuf == ios_base::unitbuf);
        static_assert(ios_base::uppercase == ios_base::uppercase);
    }
    // Check that all the iostate values exist
    {
        static_assert(ios_base::goodbit == ios_base::goodbit);
        static_assert(ios_base::badbit == ios_base::badbit);
        static_assert(ios_base::failbit == ios_base::failbit);
        static_assert(ios_base::eofbit == ios_base::eofbit);
    }
    // Check that all the seekdir values exist
    {
        static_assert(ios_base::beg == ios_base::beg);
        static_assert(ios_base::end == ios_base::end);
        static_assert(ios_base::cur == ios_base::cur);
    }
    // Non-member operator << string (from klib_string.h)
    {
        // No padding
        string str1 = "Hello";
        ostringstream s1;
        s1 << str1;
        if (!s1 || s1.str() != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream non-member operator<< string, no padding\n";
        }
        // Left padding
        string str2 = "Hello";
        ostringstream s2;
        s2 << left;
        s2.width(10);
        s2 << str2;
        if (!s2 || s2.str() != "Hello     ")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream non-member operator<< string, left padding\n";
        }
        // Right padding
        string str3 = "Hello";
        ostringstream s3;
        s3 << right;
        s3.width(10);
        s3 << str3;
        if (!s3 || s3.str() != "     Hello")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream non-member operator<< string, right padding\n";
        }
        // Internal padding
        string str4 = "Hello";
        ostringstream s4;
        s4 << internal;
        s4.width(10);
        s4 << str4;
        if (!s4 || s4.str() != "     Hello")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream non-member operator<< string, internal padding\n";
        }
        // Default padding
        string str5 = "Hello";
        ostringstream s5;
        s5.width(10);
        s5 << str5;
        if (!s5 || s5.str() != "     Hello")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream non-member operator<< string, default padding\n";
        }
        // Empty string, no padding
        string str6 = "";
        ostringstream s6;
        s6 << str6;
        if (!s6 || s6.str() != "")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream non-member operator<< string, empty string, no padding\n";
        }
        // Empty string, padding
        string str7 = "";
        ostringstream s7;
        s7.width(5);
        s7 << str7;
        if (!s7 || s7.str() != "     ")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream non-member operator<< string, empty string, padding\n";
        }
        // Write after padding
        string str8 = "Hello";
        ostringstream s8;
        s8 << left;
        s8.width(10);
        s8 << str8;
        s8 << str8;
        if (!s8 || s8.str() != "Hello     Hello")
        {
            ++fail_count;
            std::cout << "FAILED ostringstream non-member operator<< string, write after padding\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
