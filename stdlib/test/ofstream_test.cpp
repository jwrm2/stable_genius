#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>

#include "../include/cstdio"
#include "../include/fstream"
#include "../include/string"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // Make sure test file does not exist.
    klib::string test_file = "junk";
    std::remove(test_file.c_str());

    // Types
    if (!is_same<ofstream::char_type, char>::value)
    {
        ++fail_count;
        std::cout << "FAILED ofstream::char_type\n";
    }
    if (!is_same<ofstream::traits_type, char_traits<char>>::value)
    {
        ++fail_count;
        std::cout << "FAILED ofstream::traits_type\n";
    }
    if (!is_same<ofstream::int_type, char_traits<char>::int_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED ofstream::int_type\n";
    }
    if (!is_same<ofstream::pos_type, char_traits<char>::pos_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED ofstream::pos_type\n";
    }
    if (!is_same<ofstream::off_type, char_traits<char>::off_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED ofstream::off_type\n";
    }

    // Constructor
    {
        // Default constructor.
        ofstream s1 {};
        if (s1.flags() != (ios_base::skipws | ios_base::dec) ||
            s1.precision() != 6 || s1.width() != 0 ||
            s1.rdstate() != ios_base::goodbit ||
            s1.exceptions() != ios_base::goodbit || s1.tie() != nullptr ||
            s1.rdbuf() == nullptr || s1.fill() != ' ' || s1.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ofstream default constructor\n";
        }
        // Constructor with char* file name.
        ofstream s2 {test_file.c_str()};
        struct stat st1;
        if (s2.flags() != (ios_base::skipws | ios_base::dec) ||
            s2.precision() != 6 || s2.width() != 0 ||
            s2.rdstate() != ios_base::goodbit ||
            s2.exceptions() != ios_base::goodbit || s2.tie() != nullptr ||
            s2.rdbuf() == nullptr || s2.fill() != ' ' || !s2.is_open() ||
            stat(test_file.c_str(), &st1) != 0 || st1.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream constructor with char* file name\n";
        }
        std::remove(test_file.c_str());
        // Constructor with char* file name and mode.
        ofstream s3 {test_file.c_str(), ios_base::app};
        struct stat st2;
        if (s3.flags() != (ios_base::skipws | ios_base::dec) ||
            s3.precision() != 6 || s3.width() != 0 ||
            s3.rdstate() != ios_base::goodbit ||
            s3.exceptions() != ios_base::goodbit || s3.tie() != nullptr ||
            s3.rdbuf() == nullptr || s3.fill() != ' ' || !s3.is_open() ||
            stat(test_file.c_str(), &st2) != 0 || st2.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream constructor with char* file name and mode\n";
        }
        std::remove(test_file.c_str());
        // Constructor with string file name.
        ofstream s4 {test_file};
        struct stat st3;
        if (s4.flags() != (ios_base::skipws | ios_base::dec) ||
            s4.precision() != 6 || s4.width() != 0 ||
            s4.rdstate() != ios_base::goodbit ||
            s4.exceptions() != ios_base::goodbit || s4.tie() != nullptr ||
            s4.rdbuf() == nullptr || s4.fill() != ' ' || !s4.is_open() ||
            stat(test_file.c_str(), &st3) != 0 || st3.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream constructor with string file name\n";
        }
        std::remove(test_file.c_str());
        // Constructor with string file name and mode.
        ofstream s5 {test_file, ios_base::app};
        struct stat st4;
        if (s5.flags() != (ios_base::skipws | ios_base::dec) ||
            s5.precision() != 6 || s5.width() != 0 ||
            s5.rdstate() != ios_base::goodbit ||
            s5.exceptions() != ios_base::goodbit || s5.tie() != nullptr ||
            s5.rdbuf() == nullptr || s3.fill() != ' ' || !s5.is_open() ||
            stat(test_file.c_str(), &st4) != 0 || st4.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream constructor with string file name and mode\n";
        }
        std::remove(test_file.c_str());
        // Move constructor.
        ofstream s6 {test_file};
        s6.flags(ios_base::hex);
        s6.precision(5);
        s6.width(1);
        s6.setstate(ios_base::eofbit);
        s6.exceptions(ios_base::failbit);
        ofstream s7 {};
        s6.tie(&s7);
        s6.fill('a');
        ofstream s8 {move(s6)};
        struct stat st5;
        if (s8.flags() != ios_base::hex || s8.precision() != 5 ||
            s8.width() != 1 || s8.rdstate() != ios_base::eofbit ||
            s8.exceptions() != ios_base::failbit || s8.tie() != &s7 ||
            s8.rdbuf() == nullptr || s8.fill() != 'a' || !s8.is_open() ||
            stat(test_file.c_str(), &st5) != 0 || st5.st_size != 0 ||
            s6.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ofstream move constructor\n";
        }
        std::remove(test_file.c_str());
    }
    // Assignment
    {
        // Move assignment.
        ofstream s1 {test_file};
        s1.flags(ios_base::hex);
        s1.precision(5);
        s1.width(1);
        s1.setstate(ios_base::eofbit);
        s1.exceptions(ios_base::failbit);
        ofstream s2 {};
        s1.tie(&s2);
        s1.fill('a');
        ofstream s3 {};
        s3 = move(s1);
        struct stat st1;
        if (s3.flags() != ios_base::hex || s3.precision() != 5 ||
            s3.width() != 1 || s3.rdstate() != ios_base::eofbit ||
            s3.exceptions() != ios_base::failbit || s3.tie() != &s2 ||
            s3.rdbuf() == nullptr || s3.fill() != 'a' || !s3.is_open() ||
            stat(test_file.c_str(), &st1) != 0 || st1.st_size != 0 ||
            s1.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ofstream move assignment\n";
        }
        std::remove(test_file.c_str());
    }
    // swap
    {
        // swap member
        ofstream s1 {test_file};
        s1.flags(ios_base::hex);
        s1.precision(5);
        s1.width(1);
        s1.setstate(ios_base::eofbit);
        s1.exceptions(ios_base::failbit);
        ofstream s2 {};
        s1.tie(&s2);
        s1.fill('a');
        ofstream s3 {};
        s3.swap(s1);
        struct stat st1;
        if (s3.flags() != ios_base::hex || s3.precision() != 5 ||
            s3.width() != 1 || s3.rdstate() != ios_base::eofbit ||
            s3.exceptions() != ios_base::failbit || s3.tie() != &s2 ||
            s3.rdbuf() == nullptr || s3.fill() != 'a' ||
            stat(test_file.c_str(), &st1) != 0 || st1.st_size != 0 ||
            !s3.is_open() || s1.flags() != (ios_base::skipws | ios_base::dec) ||
            s1.precision() != 6 || s1.width() != 0 ||
            s1.rdstate() != ios_base::goodbit ||
            s1.exceptions() != ios_base::goodbit || s1.tie() != nullptr ||
            s1.rdbuf() == nullptr || s1.fill() != ' ' || s1.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ofstream swap member\n";
        }
        std::remove(test_file.c_str());
        // swap non-member
        ofstream s4 {test_file};
        s4.flags(ios_base::hex);
        s4.precision(5);
        s4.width(1);
        s4.setstate(ios_base::eofbit);
        s4.exceptions(ios_base::failbit);
        ofstream s5 {};
        s4.tie(&s5);
        s4.fill('a');
        ofstream s6 {};
        swap(s4, s6);
        struct stat st2;
        if (s6.flags() != ios_base::hex || s6.precision() != 5 ||
            s6.width() != 1 || s6.rdstate() != ios_base::eofbit ||
            s6.exceptions() != ios_base::failbit || s6.tie() != &s5 ||
            s6.rdbuf() == nullptr || s6.fill() != 'a' ||
            stat(test_file.c_str(), &st2) != 0 || st2.st_size != 0 ||
            !s6.is_open() || s4.flags() != (ios_base::skipws | ios_base::dec) ||
            s4.precision() != 6 || s4.width() != 0 ||
            s4.rdstate() != ios_base::goodbit ||
            s4.exceptions() != ios_base::goodbit || s4.tie() != nullptr ||
            s4.rdbuf() == nullptr || s4.fill() != ' ' || s4.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ofstream swap non-member\n";
        }
        std::remove(test_file.c_str());
    }
    // rdbuf
    {
        ofstream s1 {test_file};
        if (!s1.rdbuf()->is_open())
        {
            ++fail_count;
            std::cout << "FAILED ofstream rdbuf\n";
        }
        std::remove(test_file.c_str());
    }
    // is_open
    {
        // not open
        ofstream s1 {};
        if (s1.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ofstream is_open, not open\n";
        }
        // open
        ofstream s2 {test_file};
        if (!s2.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ofstream is_open, open\n";
        }
        std::remove(test_file.c_str());
    }
    // open
    {
        // out, new file
        ofstream s1 {};
        s1.open(test_file);
        struct stat st1;
        if (!s1 || !s1.is_open() || stat(test_file.c_str(), &st1) != 0 ||
            st1.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out, new file\n";
        }
        s1.close();
        std::remove(test_file.c_str());
        // out|trunc, new file
        ofstream s2 {};
        s2.open(test_file, ios_base::trunc);
        struct stat st2;
        if (!s2 || !s2.is_open() || stat(test_file.c_str(), &st2) != 0 ||
            st2.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|trunc, new file\n";
        }
        s2.close();
        std::remove(test_file.c_str());
        // out|app, new file
        ofstream s3 {};
        s3.open(test_file, ios_base::app);
        struct stat st3;
        if (!s3 || !s3.is_open() || stat(test_file.c_str(), &st3) != 0 ||
            st3.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|app, new file\n";
        }
        s3.close();
        std::remove(test_file.c_str());
        // out|in, new file
        ofstream s4 {};
        s4.open(test_file, ios_base::in);
        struct stat st4;
        if (s4 || s4.is_open() || stat(test_file.c_str(), &st4) == 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|in, new file\n";
        }
        s4.close();
        std::remove(test_file.c_str());
        // out|in|trunc, new file
        ofstream s5 {};
        s5.open(test_file, ios_base::in | ios_base::trunc);
        struct stat st5;
        if (!s5 || !s5.is_open() || stat(test_file.c_str(), &st5) != 0 ||
            st5.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|in|trunc, new file\n";
        }
        s5.close();
        std::remove(test_file.c_str());
        // out|in|app, new file
        ofstream s6 {};
        s6.open(test_file, ios_base::in | ios_base::app);
        struct stat st6;
        if (!s6 || !s6.is_open() || stat(test_file.c_str(), &st6) != 0 ||
            st6.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|in|app, new file\n";
        }
        s6.close();
        std::remove(test_file.c_str());
        // binary|out, new file
        ofstream s7 {};
        s7.open(test_file, ios_base::binary);
        struct stat st7;
        if (!s7 || !s7.is_open() || stat(test_file.c_str(), &st7) != 0 ||
            st7.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out, new file\n";
        }
        s7.close();
        std::remove(test_file.c_str());
        // binary|out|trunc, new file
        ofstream s8 {};
        s8.open(test_file, ios_base::binary | ios_base::trunc);
        struct stat st8;
        if (!s8 || !s8.is_open() || stat(test_file.c_str(), &st8) != 0 ||
            st8.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out|trunc, new file\n";
        }
        s8.close();
        std::remove(test_file.c_str());
        // binary|out|app, new file
        ofstream s9 {};
        s9.open(test_file, ios_base::binary | ios_base::app);
        struct stat st9;
        if (!s9 || !s9.is_open() || stat(test_file.c_str(), &st9) != 0 ||
            st9.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out|app, new file\n";
        }
        s9.close();
        std::remove(test_file.c_str());
        // binary|out|in, new file
        ofstream s10 {};
        s10.open(test_file, ios_base::binary | ios_base::in);
        struct stat st10;
        if (s10 || s10.is_open() || stat(test_file.c_str(), &st10) == 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out|in, new file\n";
        }
        s10.close();
        std::remove(test_file.c_str());
        // binary|out|in|trunc, new file
        ofstream s11 {};
        s11.open(test_file, ios_base::binary | ios_base::in | ios_base::trunc);
        struct stat st11;
        if (!s11 || !s11.is_open() || stat(test_file.c_str(), &st11) != 0 ||
            st11.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out|in|trunc, new file\n";
        }
        s11.close();
        std::remove(test_file.c_str());
        // binary|out|in|app, new file
        ofstream s12 {};
        s12.open(test_file, ios_base::binary | ios_base::in | ios_base::app);
        struct stat st12;
        if (!s12 || !s12.is_open() || stat(test_file.c_str(), &st12) != 0 ||
            st12.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out|in|app, new file\n";
        }
        s12.close();
        std::remove(test_file.c_str());
        // out, existing file
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello";
        stds1.close();
        ofstream s13 {};
        s13.open(test_file);
        struct stat st13;
        if (!s13 || !s13.is_open() || stat(test_file.c_str(), &st13) != 0 ||
            st13.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out, existing file\n";
        }
        s13.close();
        std::remove(test_file.c_str());
        // out|trunc, existing file
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Hello";
        stds2.close();
        ofstream s14 {};
        s14.open(test_file, ios_base::trunc);
        struct stat st14;
        if (!s14 || !s14.is_open() || stat(test_file.c_str(), &st14) != 0 ||
            st14.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|trunc, existing file\n";
        }
        s14.close();
        std::remove(test_file.c_str());
        // out|app, existing file
        std::ofstream stds3 {test_file.c_str()};
        stds3 << "Hello";
        stds3.close();
        ofstream s15 {};
        s15.open(test_file, ios_base::app);
        struct stat st15;
        if (!s15 || !s15.is_open() || stat(test_file.c_str(), &st15) != 0 ||
            st15.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|trunc, existing file\n";
        }
        s15 << 'a';
        s15.close();
        std::ifstream stds4 {test_file.c_str()};
        std::string str1;
        stds4 >> str1;
        stds4.close();
        if (str1 != "Helloa")
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|app, existing file, content test\n";
        }
        std::remove(test_file.c_str());
        // out|in, existing file
        std::ofstream stds5 {test_file.c_str()};
        stds5 << "Hello";
        stds5.close();
        ofstream s16 {};
        s16.open(test_file, ios_base::in);
        struct stat st16;
        if (!s16 || !s16.is_open() || stat(test_file.c_str(), &st16) != 0 ||
            st16.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|in, existing file\n";
        }
        s16 << 'a';
        s16.close();
        std::ifstream stds6 {test_file.c_str()};
        std::string str2;
        stds6 >> str2;
        stds6.close();
        if (str2 != "aello")
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|in, existing file, content test\n";
        }
        std::remove(test_file.c_str());
        // out|in|trunc, existing file
        std::ofstream stds7 {test_file.c_str()};
        stds7 << "Hello";
        stds7.close();
        ofstream s17 {};
        s17.open(test_file, ios_base::in | ios_base::trunc);
        struct stat st17;
        if (!s17 || !s17.is_open() || stat(test_file.c_str(), &st17) != 0 ||
            st17.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|in|trunc, existing file\n";
        }
        s17.close();
        std::remove(test_file.c_str());
        // out|in|app, existing file
        std::ofstream stds8 {test_file.c_str()};
        stds8 << "Hello";
        stds8.close();
        ofstream s18 {};
        s18.open(test_file, ios_base::in | ios_base::app);
        struct stat st18;
        if (!s18 || !s18.is_open() || stat(test_file.c_str(), &st18) != 0 ||
            st18.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|in|app, existing file\n";
        }
        s18 << 'a';
        s18.close();
        std::ifstream stds9 {test_file.c_str()};
        std::string str3;
        stds9 >> str3;
        stds9.close();
        if (str3 != "Helloa")
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|in|app, existing file, content test\n";
        }
        std::remove(test_file.c_str());
        // binary|out, existing file
        std::ofstream stds10 {test_file.c_str()};
        stds10 << "Hello";
        stds10.close();
        ofstream s19 {};
        s19.open(test_file, ios_base::binary);
        struct stat st19;
        if (!s19 || !s19.is_open() || stat(test_file.c_str(), &st19) != 0 ||
            st19.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out, existing file\n";
        }
        s19.close();
        std::remove(test_file.c_str());
        // binary|out|trunc, existing file
        std::ofstream stds11 {test_file.c_str()};
        stds11 << "Hello";
        stds11.close();
        ofstream s20 {};
        s20.open(test_file, ios_base::binary | ios_base::trunc);
        struct stat st20;
        if (!s20 || !s20.is_open() || stat(test_file.c_str(), &st20) != 0 ||
            st20.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out|trunc, existing file\n";
        }
        s20.close();
        std::remove(test_file.c_str());
        // binary|out|app, existing file
        std::ofstream stds12 {test_file.c_str()};
        stds12 << "Hello";
        stds12.close();
        ofstream s21 {};
        s21.open(test_file, ios_base::binary | ios_base::app);
        struct stat st21;
        if (!s21 || !s21.is_open() || stat(test_file.c_str(), &st21) != 0 ||
            st21.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out|trunc, existing file\n";
        }
        s21 << 'a';
        s21.close();
        std::ifstream stds13 {test_file.c_str()};
        std::string str4;
        stds13 >> str4;
        stds13.close();
        if (str4 != "Helloa")
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out|app, existing file, content test\n";
        }
        std::remove(test_file.c_str());
        // binary|out|in, existing file
        std::ofstream stds14 {test_file.c_str()};
        stds14 << "Hello";
        stds14.close();
        ofstream s22 {};
        s22.open(test_file, ios_base::binary | ios_base::in);
        struct stat st22;
        if (!s22 || !s22.is_open() || stat(test_file.c_str(), &st22) != 0 ||
            st22.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out|in, existing file\n";
        }
        s22 << 'a';
        s22.close();
        std::ifstream stds15 {test_file.c_str()};
        std::string str5;
        stds15 >> str5;
        stds15.close();
        if (str5 != "aello")
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out|in, existing file, content test\n";
        }
        std::remove(test_file.c_str());
        // binary|out|in|trunc, existing file
        std::ofstream stds16 {test_file.c_str()};
        stds16 << "Hello";
        stds16.close();
        ofstream s23 {};
        s23.open(test_file, ios_base::binary | ios_base::in | ios_base::trunc);
        struct stat st23;
        if (!s23 || !s23.is_open() || stat(test_file.c_str(), &st23) != 0 ||
            st23.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out|in|trunc, existing file\n";
        }
        s23.close();
        std::remove(test_file.c_str());
        // binary|out|in|app, existing file
        std::ofstream stds17 {test_file.c_str()};
        stds17 << "Hello";
        stds17.close();
        ofstream s24 {};
        s24.open(test_file, ios_base::binary | ios_base::in | ios_base::app);
        struct stat st24;
        if (!s24 || !s24.is_open() || stat(test_file.c_str(), &st24) != 0 ||
            st24.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, binary|out|in|app, existing file\n";
        }
        s24 << 'a';
        s24.close();
        std::ifstream stds18 {test_file.c_str()};
        std::string str6;
        stds18 >> str6;
        stds18.close();
        if (str6 != "Helloa")
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|in|app, existing file, content test\n";
        }
        std::remove(test_file.c_str());
        // out|in|ate, existing file
        std::ofstream stds19 {test_file.c_str()};
        stds19 << "Hello";
        stds19.close();
        ofstream s25 {};
        s25.open(test_file, ios_base::in | ios_base::ate);
        struct stat st25;
        if (!s25 || !s25.is_open() || stat(test_file.c_str(), &st25) != 0 ||
            st25.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|in|ate, existing file\n";
        }
        s25 << 'a';
        s25.close();
        std::ifstream stds20 {test_file.c_str()};
        std::string str7;
        stds20 >> str7;
        stds20.close();
        if (str7 != "Helloa")
        {
            ++fail_count;
            std::cout << "FAILED ofstream open, out|in|ate, existing file, content test\n";
        }
        std::remove(test_file.c_str());
    }
    // close
    {
        // After opening new file.
        ofstream s1 {test_file};
        s1 << "Hello";
        s1.close();
        struct stat st1;
        if (!s1 || s1.is_open() || stat(test_file.c_str(), &st1) != 0 ||
            st1.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream close, after opening new file\n";
        }
        std::remove(test_file.c_str());
        // After truncating existing file.
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Goodbye";
        stds1.close();
        ofstream s2 {test_file};
        s2 << "Hello";
        s2.close();
        struct stat st2;
        if (!s2 || s2.is_open() || stat(test_file.c_str(), &st2) != 0 ||
            st2.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream close, after truncating existing file\n";
        }
        std::remove(test_file.c_str());
        // After appending to existing file.
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Goodbye";
        stds2.close();
        ofstream s3 {test_file, ios_base::app};
        s3 << "Hello";
        s3.close();
        struct stat st3;
        if (!s3 || s3.is_open() || stat(test_file.c_str(), &st3) != 0 ||
            st3.st_size != 12)
        {
            ++fail_count;
            std::cout << "FAILED ofstream close, after appending to existing file\n";
        }
        std::remove(test_file.c_str());
        // Without opening.
        ofstream s4 {};
        s4.close();
        if (s4 || s4.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ofstream close, without opening\n";
        }
        // After already closing.
        ofstream s5 {test_file};
        s5 << "Hello";
        s5.close();
        s5.close();
        struct stat st5;
        if (s5 || s5.is_open() || stat(test_file.c_str(), &st5) != 0 ||
            st5.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream close, after already closing\n";
        }
        std::remove(test_file.c_str());
    }
    // writing, out only
    {
        // To new file.
        ofstream s1 {test_file};
        s1 << "Hello";
        s1.close();
        std::ifstream stds1 {test_file.c_str()};
        std::string str1;
        stds1 >> str1;
        if (str1 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED ofstream writing, to new file\n";
        }
        std::remove(test_file.c_str());
        // To existing file, truncate
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Goodbye";
        stds2.close();
        ofstream s2 {test_file};
        s2 << "Hello";
        s2.close();
        std::ifstream stds3 {test_file.c_str()};
        std::string str2;
        stds3 >> str2;
        if (str2 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED ofstream writing, to existing file, truncate\n";
        }
        std::remove(test_file.c_str());
        // To existing file, append
        std::ofstream stds4 {test_file.c_str()};
        stds4 << "Goodbye";
        stds4.close();
        ofstream s3 {test_file, ios_base::app};
        s3 << "Hello";
        s3.close();
        std::ifstream stds5 {test_file.c_str()};
        std::string str3;
        stds5 >> str3;
        if (str3 != "GoodbyeHello")
        {
            ++fail_count;
            std::cout << "FAILED ofstream writing, to existing file, append\n";
        }
        std::remove(test_file.c_str());
        // To new file, overflow
        ofstream s4 {test_file};
        for (size_t i = 0; i < BUFSIZ + 10; ++i)
            s4 << 'a';
        s4.close();
        struct stat st1;
        if (stat(test_file.c_str(), &st1) != 0 || st1.st_size != BUFSIZ + 10)
        {
            ++fail_count;
            std::cout << "FAILED ofstream writing, to new file, overflow\n";
        }
        std::remove(test_file.c_str());
        // To existing file, append, overflow
        std::ofstream stds6 {test_file.c_str()};
        stds6 << "Goodbye";
        stds6.close();
        ofstream s5 {test_file, ios_base::app};
        for (size_t i = 0; i < BUFSIZ + 10; ++i)
            s5 << 'a';
        s5.close();
        struct stat st2;
        if (stat(test_file.c_str(), &st2) != 0 || st2.st_size != BUFSIZ + 17)
        {
            ++fail_count;
            std::cout << "FAILED ofstream writing, to exisitng file, append, overflow\n";
        }
        std::remove(test_file.c_str());
    }
    // writing, out|in
    {
        // To existing file, overwrite
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Goodbye";
        stds1.close();
        ofstream s1 {test_file, ios_base::in};
        s1 << "Hello";
        s1.close();
        std::ifstream stds2 {test_file.c_str()};
        std::string str1;
        stds2 >> str1;
        if (str1 != "Helloye")
        {
            ++fail_count;
            std::cout << "FAILED ofstream writing out|in, to existing file, overwrite\n";
        }
        std::remove(test_file.c_str());
        // To existing file, append
        std::ofstream stds4 {test_file.c_str()};
        stds4 << "Goodbye";
        stds4.close();
        ofstream s3 {test_file, ios_base::in | ios_base::app};
        s3 << "Hello";
        s3.close();
        std::ifstream stds5 {test_file.c_str()};
        std::string str3;
        stds5 >> str3;
        if (str3 != "GoodbyeHello")
        {
            ++fail_count;
            std::cout << "FAILED ofstream writing, to existing file, append\n";
        }
        std::remove(test_file.c_str());
        // To new file, overflow
        ofstream s4 {test_file};
        for (size_t i = 0; i < BUFSIZ + 10; ++i)
            s4 << 'a';
        s4.close();
        struct stat st1;
        if (stat(test_file.c_str(), &st1) != 0 || st1.st_size != BUFSIZ + 10)
        {
            ++fail_count;
            std::cout << "FAILED ofstream writing, to new file, overflow\n";
        }
        std::remove(test_file.c_str());
        // To existing file, append, overflow
        std::ofstream stds6 {test_file.c_str()};
        stds6 << "Goodbye";
        stds6.close();
        ofstream s5 {test_file, ios_base::in | ios_base::app};
        for (size_t i = 0; i < BUFSIZ + 10; ++i)
            s5 << 'a';
        s5.close();
        struct stat st2;
        if (stat(test_file.c_str(), &st2) != 0 || st2.st_size != BUFSIZ + 17)
        {
            ++fail_count;
            std::cout << "FAILED ofstream writing, to existing file, append, overflow\n";
        }
        std::remove(test_file.c_str());
    }
    // tellp, write only
    {
        // New file
        ofstream s1 {test_file};
        ofstream::pos_type p1 = s1.tellp();
        s1.close();
        if (static_cast<ofstream::off_type>(p1) != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, new file\n";
        }
        std::remove(test_file.c_str());
        // New file, after writing.
        ofstream s2 {test_file};
        s2 << "Hello";
        ofstream::pos_type p2 = s2.tellp();
        s2.close();
        if (static_cast<ofstream::off_type>(p2) != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, new file, after writing\n";
        }
        std::remove(test_file.c_str());
        // Existing file, truncate.
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Goodbye";
        stds1.close();
        ofstream s3 {test_file};
        ofstream::pos_type p3 = s3.tellp();
        s3.close();
        if (static_cast<ofstream::off_type>(p3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, existing file, truncate\n";
        }
        std::remove(test_file.c_str());
        // Existing file, truncate, after writing.
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Goodbye";
        stds2.close();
        ofstream s4 {test_file};
        s4 << "Hello";
        ofstream::pos_type p4 = s4.tellp();
        s4.close();
        if (static_cast<ofstream::off_type>(p4) != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, existing file, truncate, after writing\n";
        }
        std::remove(test_file.c_str());
        // Existing file, append.
        std::ofstream stds3 {test_file.c_str()};
        stds3 << "Goodbye";
        stds3.close();
        ofstream s5 {test_file, ios_base::app};
        ofstream::pos_type p5 = s5.tellp();
        s5.close();
        if (static_cast<ofstream::off_type>(p5) != 7)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, existing file, append\n";
        }
        std::remove(test_file.c_str());
        // Existing file, append, after writing.
        std::ofstream stds4 {test_file.c_str()};
        stds4 << "Goodbye";
        stds4.close();
        ofstream s6 {test_file, ios_base::app};
        s6 << "Hello";
        ofstream::pos_type p6 = s6.tellp();
        s6.close();
        if (static_cast<ofstream::off_type>(p6) != 12)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, existing file, append, after writing\n";
        }
        std::remove(test_file.c_str());
    }
    // tellp, read and write
    {
        // Existing file
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Goodbye";
        stds1.close();
        ofstream s1 {test_file, ios_base::in};
        ofstream::pos_type p1 = s1.tellp();
        s1.close();
        if (static_cast<ofstream::off_type>(p1) != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, with reading, exiting file\n";
        }
        std::remove(test_file.c_str());
        // New file, after writing.
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Goodbye";
        stds2.close();
        ofstream s2 {test_file, ios_base::in};
        s2 << "Hello";
        ofstream::pos_type p2 = s2.tellp();
        s2.close();
        if (static_cast<ofstream::off_type>(p2) != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, with reading, existing file, after writing\n";
        }
        std::remove(test_file.c_str());
        // New file, truncate
        ofstream s3 {test_file, ios_base::in | ios_base::trunc};
        ofstream::pos_type p3 = s3.tellp();
        s3.close();
        if (static_cast<ofstream::off_type>(p3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, with reading, truncate, new file\n";
        }
        std::remove(test_file.c_str());
        // New file, truncate, after writing.
        ofstream s4 {test_file, ios_base::in | ios_base::trunc};
        s4 << "Hello";
        ofstream::pos_type p4 = s4.tellp();
        s4.close();
        if (static_cast<ofstream::off_type>(p4) != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, with reading, truncate, new file, after writing\n";
        }
        std::remove(test_file.c_str());
        // Existing file, truncate.
        std::ofstream stds3 {test_file.c_str()};
        stds3 << "Goodbye";
        stds3.close();
        ofstream s5 {test_file, ios_base::in | ios_base::trunc};
        ofstream::pos_type p5 = s5.tellp();
        s5.close();
        if (static_cast<ofstream::off_type>(p5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, with reading, existing file, truncate\n";
        }
        std::remove(test_file.c_str());
        // Existing file, truncate, after writing.
        std::ofstream stds4 {test_file.c_str()};
        stds4 << "Goodbye";
        stds4.close();
        ofstream s6 {test_file, ios_base::in | ios_base::trunc};
        s6 << "Hello";
        ofstream::pos_type p6 = s6.tellp();
        s6.close();
        if (static_cast<ofstream::off_type>(p6) != 5)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, with reading, existing file, truncate, after writing\n";
        }
        std::remove(test_file.c_str());
        // Existing file, append. Note in this case, the file pointer is
        // initially at the start of the file, but writing moves it to the end.
        std::ofstream stds5 {test_file.c_str()};
        stds5 << "Goodbye";
        stds5.close();
        ofstream s7 {test_file, ios_base::in | ios_base::app};
        ofstream::pos_type p7 = s7.tellp();
        s7.close();
        if (static_cast<ofstream::off_type>(p7) != 0)
        {
            std::cout << "p7 = " << static_cast<ofstream::off_type>(p7) << '\n';
            ++fail_count;
            std::cout << "FAILED ofstream tellp, with reading, existing file, append\n";
        }
        std::remove(test_file.c_str());
        // Existing file, append, after writing. Note in this case, the file
        // pointer is initially at the start of the file, but writing moves it
        // to the end.
        std::ofstream stds6 {test_file.c_str()};
        stds6 << "Goodbye";
        stds6.close();
        ofstream s8 {test_file, ios_base::in | ios_base::app};
        s8 << "Hello";
        ofstream::pos_type p8 = s8.tellp();
        s8.close();
        if (static_cast<ofstream::off_type>(p8) != 12)
        {
            ++fail_count;
            std::cout << "FAILED ofstream tellp, with reading, existing file, append, after writing\n";
        }
        std::remove(test_file.c_str());
    }
    // seekp, beg
    {
        // New file, out
        ofstream s1 {test_file};
        s1 << "Hello";
        s1.seekp(1, ios_base::beg);
        s1 << "ab";
        s1.close();
        std::ifstream stds1 {test_file.c_str()};
        std::string str1;
        stds1 >> str1;
        stds1.close();
        if (str1 != "Hablo")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, beg, new file, out\n";
        }
        std::remove(test_file.c_str());
        // New file, out|app (app sets the pointer to the end at each write)
        ofstream s2 {test_file, ios_base::app};
        s2 << "Hello";
        s2.seekp(1, ios_base::beg);
        s2 << "ab";
        s2.close();
        std::ifstream stds2 {test_file.c_str()};
        std::string str2;
        stds2 >> str2;
        stds2.close();
        if (str2 != "Helloab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, beg, new file, out|app\n";
        }
        std::remove(test_file.c_str());
        // New file, out|in|trunc
        ofstream s3 {test_file, ios_base::in | ios_base::trunc};
        s3 << "Hello";
        s3.seekp(1, ios_base::beg);
        s3 << "ab";
        s3.close();
        std::ifstream stds3 {test_file.c_str()};
        std::string str3;
        stds3 >> str3;
        stds3.close();
        if (str3 != "Hablo")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, beg, new file, out|in|trunc\n";
        }
        std::remove(test_file.c_str());
        // New file, out|in|app (app sets the pointer to the end at each write)
        ofstream s4 {test_file, ios_base::in | ios_base::app};
        s4 << "Hello";
        s4.seekp(1, ios_base::beg);
        s4 << "ab";
        s4.close();
        std::ifstream stds4 {test_file.c_str()};
        std::string str4;
        stds4 >> str4;
        stds4.close();
        if (str4 != "Helloab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, beg, new file, out|in|app\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out
        std::ofstream stds5 {test_file.c_str()};
        stds5 << "Hello";
        stds5.close();
        ofstream s5 {test_file};
        s5 << "Goodbye";
        s5.seekp(1, ios_base::beg);
        s5 << "ab";
        s5.close();
        std::ifstream stds6 {test_file.c_str()};
        std::string str5;
        stds6 >> str5;
        stds6.close();
        if (str5 != "Gabdbye")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, beg, existing file, out\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|app (app sets the pointer to the end at each write)
        std::ofstream stds7 {test_file.c_str()};
        stds7 << "Hello";
        stds7.close();
        ofstream s6 {test_file, ios_base::app};
        s6 << "Goodbye";
        s6.seekp(1, ios_base::beg);
        s6 << "ab";
        s6.close();
        std::ifstream stds8 {test_file.c_str()};
        std::string str6;
        stds8 >> str6;
        stds8.close();
        if (str6 != "HelloGoodbyeab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, beg, existing file, out|app\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|in
        std::ofstream stds9 {test_file.c_str()};
        stds9 << "Hello";
        stds9.close();
        ofstream s7 {test_file, ios_base::in};
        s7 << "Good";
        s7.seekp(1, ios_base::beg);
        s7 << "ab";
        s7.close();
        std::ifstream stds10 {test_file.c_str()};
        std::string str7;
        stds10 >> str7;
        stds10.close();
        if (str7 != "Gabdo")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, beg, existing file, out|in\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|in|trunc
        std::ofstream stds11 {test_file.c_str()};
        stds11 << "Hello";
        stds11.close();
        ofstream s8 {test_file, ios_base::out | ios_base::trunc};
        s8 << "Goodbye";
        s8.seekp(1, ios_base::beg);
        s8 << "ab";
        s8.close();
        std::ifstream stds12 {test_file.c_str()};
        std::string str8;
        stds12 >> str8;
        stds12.close();
        if (str8 != "Gabdbye")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, beg, existing file, out|in|trunc\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|in|app (app sets the pointer to the end at each write)
        std::ofstream stds13 {test_file.c_str()};
        stds13 << "Hello";
        stds13.close();
        ofstream s9 {test_file, ios_base::in | ios_base::app};
        s9 << "Goodbye";
        s6.seekp(1, ios_base::beg);
        s9 << "ab";
        s9.close();
        std::ifstream stds14 {test_file.c_str()};
        std::string str9;
        stds14 >> str9;
        stds14.close();
        if (str9 != "HelloGoodbyeab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, beg, existing file, out|in|app\n";
        }
        std::remove(test_file.c_str());
    }
    // seekp, cur
    {
        // New file, out
        ofstream s1 {test_file};
        s1 << "Hello";
        s1.seekp(-2, ios_base::cur);
        s1 << "ab";
        s1.close();
        std::ifstream stds1 {test_file.c_str()};
        std::string str1;
        stds1 >> str1;
        stds1.close();
        if (str1 != "Helab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, cur, new file, out\n";
        }
        std::remove(test_file.c_str());
        // New file, out|app (app sets the pointer to the end at each write)
        ofstream s2 {test_file, ios_base::app};
        s2 << "Hello";
        s2.seekp(-2, ios_base::cur);
        s2 << "ab";
        s2.close();
        std::ifstream stds2 {test_file.c_str()};
        std::string str2;
        stds2 >> str2;
        stds2.close();
        if (str2 != "Helloab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, cur, new file, out|app\n";
        }
        std::remove(test_file.c_str());
        // New file, out|in|trunc
        ofstream s3 {test_file, ios_base::in | ios_base::trunc};
        s3 << "Hello";
        s3.seekp(-2, ios_base::cur);
        s3 << "ab";
        s3.close();
        std::ifstream stds3 {test_file.c_str()};
        std::string str3;
        stds3 >> str3;
        stds3.close();
        if (str3 != "Helab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, cur, new file, out|in|trunc\n";
        }
        std::remove(test_file.c_str());
        // New file, out|in|app (app sets the pointer to the end at each write)
        ofstream s4 {test_file, ios_base::in | ios_base::app};
        s4 << "Hello";
        s4.seekp(-2, ios_base::cur);
        s4 << "ab";
        s4.close();
        std::ifstream stds4 {test_file.c_str()};
        std::string str4;
        stds4 >> str4;
        stds4.close();
        if (str4 != "Helloab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, cur, new file, out|in|app\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out
        std::ofstream stds5 {test_file.c_str()};
        stds5 << "Hello";
        stds5.close();
        ofstream s5 {test_file};
        s5 << "Goodbye";
        s5.seekp(-2, ios_base::cur);
        s5 << "ab";
        s5.close();
        std::ifstream stds6 {test_file.c_str()};
        std::string str5;
        stds6 >> str5;
        stds6.close();
        if (str5 != "Goodbab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, cur, existing file, out\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|app (app sets the pointer to the end at each write)
        std::ofstream stds7 {test_file.c_str()};
        stds7 << "Hello";
        stds7.close();
        ofstream s6 {test_file, ios_base::app};
        s6 << "Goodbye";
        s6.seekp(-2, ios_base::cur);
        s6 << "ab";
        s6.close();
        std::ifstream stds8 {test_file.c_str()};
        std::string str6;
        stds8 >> str6;
        stds8.close();
        if (str6 != "HelloGoodbyeab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, cur, existing file, out|app\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|in
        std::ofstream stds9 {test_file.c_str()};
        stds9 << "Hello";
        stds9.close();
        ofstream s7 {test_file, ios_base::in};
        s7 << "Good";
        s7.seekp(-2, ios_base::cur);
        s7 << "ab";
        s7.close();
        std::ifstream stds10 {test_file.c_str()};
        std::string str7;
        stds10 >> str7;
        stds10.close();
        if (str7 != "Goabo")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, cur, existing file, out|in\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|in|trunc
        std::ofstream stds11 {test_file.c_str()};
        stds11 << "Hello";
        stds11.close();
        ofstream s8 {test_file, ios_base::out | ios_base::trunc};
        s8 << "Goodbye";
        s8.seekp(-2, ios_base::cur);
        s8 << "ab";
        s8.close();
        std::ifstream stds12 {test_file.c_str()};
        std::string str8;
        stds12 >> str8;
        stds12.close();
        if (str8 != "Goodbab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, cur, existing file, out|in|trunc\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|in|app (app sets the pointer to the end at each write)
        std::ofstream stds13 {test_file.c_str()};
        stds13 << "Hello";
        stds13.close();
        ofstream s9 {test_file, ios_base::in | ios_base::app};
        s9 << "Goodbye";
        s6.seekp(-2, ios_base::cur);
        s9 << "ab";
        s9.close();
        std::ifstream stds14 {test_file.c_str()};
        std::string str9;
        stds14 >> str9;
        stds14.close();
        if (str9 != "HelloGoodbyeab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, cur, existing file, out|in|app\n";
        }
        std::remove(test_file.c_str());
    }
    // seekp, end
    {
        // New file, out
        ofstream s1 {test_file};
        s1 << "Hello";
        s1.seekp(-2, ios_base::end);
        s1 << "ab";
        s1.close();
        std::ifstream stds1 {test_file.c_str()};
        std::string str1;
        stds1 >> str1;
        stds1.close();
        if (str1 != "Helab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, end, new file, out\n";
        }
        std::remove(test_file.c_str());
        // New file, out|app (app sets the pointer to the end at each write)
        ofstream s2 {test_file, ios_base::app};
        s2 << "Hello";
        s2.seekp(-2, ios_base::end);
        s2 << "ab";
        s2.close();
        std::ifstream stds2 {test_file.c_str()};
        std::string str2;
        stds2 >> str2;
        stds2.close();
        if (str2 != "Helloab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, end, new file, out|app\n";
        }
        std::remove(test_file.c_str());
        // New file, out|in|trunc
        ofstream s3 {test_file, ios_base::in | ios_base::trunc};
        s3 << "Hello";
        s3.seekp(-2, ios_base::end);
        s3 << "ab";
        s3.close();
        std::ifstream stds3 {test_file.c_str()};
        std::string str3;
        stds3 >> str3;
        stds3.close();
        if (str3 != "Helab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, end, new file, out|in|trunc\n";
        }
        std::remove(test_file.c_str());
        // New file, out|in|app (app sets the pointer to the end at each write)
        ofstream s4 {test_file, ios_base::in | ios_base::app};
        s4 << "Hello";
        s4.seekp(-2, ios_base::end);
        s4 << "ab";
        s4.close();
        std::ifstream stds4 {test_file.c_str()};
        std::string str4;
        stds4 >> str4;
        stds4.close();
        if (str4 != "Helloab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, end, new file, out|in|app\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out
        std::ofstream stds5 {test_file.c_str()};
        stds5 << "Hello";
        stds5.close();
        ofstream s5 {test_file};
        s5 << "Goodbye";
        s5.seekp(-2, ios_base::end);
        s5 << "ab";
        s5.close();
        std::ifstream stds6 {test_file.c_str()};
        std::string str5;
        stds6 >> str5;
        stds6.close();
        if (str5 != "Goodbab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, end, existing file, out\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|app (app sets the pointer to the end at each write)
        std::ofstream stds7 {test_file.c_str()};
        stds7 << "Hello";
        stds7.close();
        ofstream s6 {test_file, ios_base::app};
        s6 << "Goodbye";
        s6.seekp(-2, ios_base::end);
        s6 << "ab";
        s6.close();
        std::ifstream stds8 {test_file.c_str()};
        std::string str6;
        stds8 >> str6;
        stds8.close();
        if (str6 != "HelloGoodbyeab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, end, existing file, out|app\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|in
        std::ofstream stds9 {test_file.c_str()};
        stds9 << "Hello";
        stds9.close();
        ofstream s7 {test_file, ios_base::in};
        s7 << "Good";
        s7.seekp(-2, ios_base::end);
        s7 << "ab";
        s7.close();
        std::ifstream stds10 {test_file.c_str()};
        std::string str7;
        stds10 >> str7;
        stds10.close();
        if (str7 != "Gooab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, end, existing file, out|in\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|in|trunc
        std::ofstream stds11 {test_file.c_str()};
        stds11 << "Hello";
        stds11.close();
        ofstream s8 {test_file, ios_base::out | ios_base::trunc};
        s8 << "Goodbye";
        s8.seekp(-2, ios_base::end);
        s8 << "ab";
        s8.close();
        std::ifstream stds12 {test_file.c_str()};
        std::string str8;
        stds12 >> str8;
        stds12.close();
        if (str8 != "Goodbab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, end, existing file, out|in|trunc\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|in|app (app sets the pointer to the end at each write)
        std::ofstream stds13 {test_file.c_str()};
        stds13 << "Hello";
        stds13.close();
        ofstream s9 {test_file, ios_base::in | ios_base::app};
        s9 << "Goodbye";
        s6.seekp(-2, ios_base::end);
        s9 << "ab";
        s9.close();
        std::ifstream stds14 {test_file.c_str()};
        std::string str9;
        stds14 >> str9;
        stds14.close();
        if (str9 != "HelloGoodbyeab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, end, existing file, out|in|app\n";
        }
        std::remove(test_file.c_str());
    }
    // seekp, absolute
    {
        // New file, out
        ofstream s1 {test_file};
        s1 << "Hello";
        s1.seekp(2);
        s1 << "ab";
        s1.close();
        std::ifstream stds1 {test_file.c_str()};
        std::string str1;
        stds1 >> str1;
        stds1.close();
        if (str1 != "Heabo")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, absolute, new file, out\n";
        }
        std::remove(test_file.c_str());
        // New file, out|app (app sets the pointer to the end at each write)
        ofstream s2 {test_file, ios_base::app};
        s2 << "Hello";
        s2.seekp(2);
        s2 << "ab";
        s2.close();
        std::ifstream stds2 {test_file.c_str()};
        std::string str2;
        stds2 >> str2;
        stds2.close();
        if (str2 != "Helloab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, absolute, new file, out|app\n";
        }
        std::remove(test_file.c_str());
        // New file, out|in|trunc
        ofstream s3 {test_file, ios_base::in | ios_base::trunc};
        s3 << "Hello";
        s3.seekp(2);
        s3 << "ab";
        s3.close();
        std::ifstream stds3 {test_file.c_str()};
        std::string str3;
        stds3 >> str3;
        stds3.close();
        if (str3 != "Heabo")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, absolute, new file, out|in|trunc\n";
        }
        std::remove(test_file.c_str());
        // New file, out|in|app (app sets the pointer to the end at each write)
        ofstream s4 {test_file, ios_base::in | ios_base::app};
        s4 << "Hello";
        s4.seekp(2);
        s4 << "ab";
        s4.close();
        std::ifstream stds4 {test_file.c_str()};
        std::string str4;
        stds4 >> str4;
        stds4.close();
        if (str4 != "Helloab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, absolute, new file, out|in|app\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out
        std::ofstream stds5 {test_file.c_str()};
        stds5 << "Hello";
        stds5.close();
        ofstream s5 {test_file};
        s5 << "Goodbye";
        s5.seekp(2);
        s5 << "ab";
        s5.close();
        std::ifstream stds6 {test_file.c_str()};
        std::string str5;
        stds6 >> str5;
        stds6.close();
        if (str5 != "Goabbye")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, absolute, existing file, out\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|app (app sets the pointer to the end at each write)
        std::ofstream stds7 {test_file.c_str()};
        stds7 << "Hello";
        stds7.close();
        ofstream s6 {test_file, ios_base::app};
        s6 << "Goodbye";
        s6.seekp(2);
        s6 << "ab";
        s6.close();
        std::ifstream stds8 {test_file.c_str()};
        std::string str6;
        stds8 >> str6;
        stds8.close();
        if (str6 != "HelloGoodbyeab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, absolute, existing file, out|app\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|in
        std::ofstream stds9 {test_file.c_str()};
        stds9 << "Hello";
        stds9.close();
        ofstream s7 {test_file, ios_base::in};
        s7 << "Goodbye";
        s7.seekp(2);
        s7 << "ab";
        s7.close();
        std::ifstream stds10 {test_file.c_str()};
        std::string str7;
        stds10 >> str7;
        stds10.close();
        if (str7 != "Goabbye")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, absolute, existing file, out|in\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|in|trunc
        std::ofstream stds11 {test_file.c_str()};
        stds11 << "Hello";
        stds11.close();
        ofstream s8 {test_file, ios_base::out | ios_base::trunc};
        s8 << "Goodbye";
        s8.seekp(2);
        s8 << "ab";
        s8.close();
        std::ifstream stds12 {test_file.c_str()};
        std::string str8;
        stds12 >> str8;
        stds12.close();
        if (str8 != "Goabbye")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, absolute, existing file, out|in|trunc\n";
        }
        std::remove(test_file.c_str());
        // Existing file, out|in|app (app sets the pointer to the end at each write)
        std::ofstream stds13 {test_file.c_str()};
        stds13 << "Hello";
        stds13.close();
        ofstream s9 {test_file, ios_base::in | ios_base::app};
        s9 << "Goodbye";
        s6.seekp(2);
        s9 << "ab";
        s9.close();
        std::ifstream stds14 {test_file.c_str()};
        std::string str9;
        stds14 >> str9;
        stds14.close();
        if (str9 != "HelloGoodbyeab")
        {
            ++fail_count;
            std::cout << "FAILED ofstream seekp, absolute, existing file, out|in|app\n";
        }
        std::remove(test_file.c_str());
    }
    // flush
    {
        // New file
        ofstream s1 {test_file};
        s1 << "Hello";
        s1.flush();
        std::ifstream stds1 {test_file.c_str()};
        std::string str1;
        stds1 >> str1;
        stds1.close();
        s1.close();
        if (str1 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED ofstream flush, new file\n";
        }
        std::remove(test_file.c_str());
        // Exisiting file truncation
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Hello";
        stds2.close();
        ofstream s2 {test_file};
        s2 << "Goodbye";
        s2.flush();
        std::ifstream stds3 {test_file.c_str()};
        std::string str2;
        stds3 >> str2;
        stds3.close();
        s2.close();
        if (str2 != "Goodbye")
        {
            ++fail_count;
            std::cout << "FAILED ofstream flush, exisiting file truncation\n";
        }
        std::remove(test_file.c_str());
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
