#include <algorithm>
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
    if (!is_same<ifstream::char_type, char>::value)
    {
        ++fail_count;
        std::cout << "FAILED ifstream::char_type\n";
    }
    if (!is_same<ifstream::traits_type, char_traits<char>>::value)
    {
        ++fail_count;
        std::cout << "FAILED ifstream::traits_type\n";
    }
    if (!is_same<ifstream::int_type, char_traits<char>::int_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED ifstream::int_type\n";
    }
    if (!is_same<ifstream::pos_type, char_traits<char>::pos_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED ifstream::pos_type\n";
    }
    if (!is_same<ifstream::off_type, char_traits<char>::off_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED ifstream::off_type\n";
    }

    // Constructor
    {
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello";
        stds1.close();
        // Default constructor.
        ifstream s1 {};
        if (s1.flags() != (ios_base::skipws | ios_base::dec) ||
            s1.precision() != 6 || s1.width() != 0 ||
            s1.rdstate() != ios_base::goodbit ||
            s1.exceptions() != ios_base::goodbit || s1.tie() != nullptr ||
            s1.rdbuf() == nullptr || s1.fill() != ' ' || s1.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream default constructor\n";
        }
        // Constructor with char* file name.
        ifstream s2 {test_file.c_str()};
        struct stat st1;
        if (s2.flags() != (ios_base::skipws | ios_base::dec) ||
            s2.precision() != 6 || s2.width() != 0 ||
            s2.rdstate() != ios_base::goodbit ||
            s2.exceptions() != ios_base::goodbit || s2.tie() != nullptr ||
            s2.rdbuf() == nullptr || s2.fill() != ' ' || !s2.is_open() ||
            stat(test_file.c_str(), &st1) != 0 || st1.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream constructor with char* file name\n";
        }
        // Constructor with char* file name and mode.
        ifstream s3 {test_file.c_str(), ios_base::app};
        struct stat st2;
        if (s3.flags() != (ios_base::skipws | ios_base::dec) ||
            s3.precision() != 6 || s3.width() != 0 ||
            s3.rdstate() != ios_base::goodbit ||
            s3.exceptions() != ios_base::goodbit || s3.tie() != nullptr ||
            s3.rdbuf() == nullptr || s3.fill() != ' ' || !s3.is_open() ||
            stat(test_file.c_str(), &st2) != 0 || st2.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream constructor with char* file name and mode\n";
        }
        // Constructor with string file name.
        ifstream s4 {test_file};
        struct stat st3;
        if (s4.flags() != (ios_base::skipws | ios_base::dec) ||
            s4.precision() != 6 || s4.width() != 0 ||
            s4.rdstate() != ios_base::goodbit ||
            s4.exceptions() != ios_base::goodbit || s4.tie() != nullptr ||
            s4.rdbuf() == nullptr || s4.fill() != ' ' || !s4.is_open() ||
            stat(test_file.c_str(), &st3) != 0 || st3.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream constructor with string file name\n";
        }
        // Constructor with string file name and mode.
        ifstream s5 {test_file, ios_base::app};
        struct stat st4;
        if (s5.flags() != (ios_base::skipws | ios_base::dec) ||
            s5.precision() != 6 || s5.width() != 0 ||
            s5.rdstate() != ios_base::goodbit ||
            s5.exceptions() != ios_base::goodbit || s5.tie() != nullptr ||
            s5.rdbuf() == nullptr || s3.fill() != ' ' || !s5.is_open() ||
            stat(test_file.c_str(), &st4) != 0 || st4.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream constructor with string file name and mode\n";
        }
        // Move constructor.
        ifstream s6 {test_file};
        s6.flags(ios_base::hex);
        s6.precision(5);
        s6.width(1);
        s6.setstate(ios_base::eofbit);
        s6.exceptions(ios_base::failbit);
        ofstream s7 {};
        s6.tie(&s7);
        s6.fill('a');
        ifstream s8 {move(s6)};
        struct stat st5;
        if (s8.flags() != ios_base::hex || s8.precision() != 5 ||
            s8.width() != 1 || s8.rdstate() != ios_base::eofbit ||
            s8.exceptions() != ios_base::failbit || s8.tie() != &s7 ||
            s8.rdbuf() == nullptr || s8.fill() != 'a' || !s8.is_open() ||
            stat(test_file.c_str(), &st5) != 0 || st5.st_size != 5 || s6.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream move constructor\n";
        }
        std::remove(test_file.c_str());
    }
    // Assignment
    {
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello";
        stds1.close();
        // Move assignment.
        ifstream s1 {test_file};
        s1.flags(ios_base::hex);
        s1.precision(5);
        s1.width(1);
        s1.setstate(ios_base::eofbit);
        s1.exceptions(ios_base::failbit);
        ofstream s2 {};
        s1.tie(&s2);
        s1.fill('a');
        ifstream s3 {};
        s3 = move(s1);
        struct stat st1;
        if (s3.flags() != ios_base::hex || s3.precision() != 5 ||
            s3.width() != 1 || s3.rdstate() != ios_base::eofbit ||
            s3.exceptions() != ios_base::failbit || s3.tie() != &s2 ||
            s3.rdbuf() == nullptr || s3.fill() != 'a' || !s3.is_open() ||
            stat(test_file.c_str(), &st1) != 0 || st1.st_size != 5 || s1.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream move assignment\n";
        }
        std::remove(test_file.c_str());
    }
    // swap
    {
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello";
        stds1.close();
        // swap member
        ifstream s1 {test_file};
        s1.flags(ios_base::hex);
        s1.precision(5);
        s1.width(1);
        s1.setstate(ios_base::eofbit);
        s1.exceptions(ios_base::failbit);
        ofstream s2 {};
        s1.tie(&s2);
        s1.fill('a');
        ifstream s3 {};
        s3.swap(s1);
        struct stat st1;
        if (s3.flags() != ios_base::hex || s3.precision() != 5 ||
            s3.width() != 1 || s3.rdstate() != ios_base::eofbit ||
            s3.exceptions() != ios_base::failbit || s3.tie() != &s2 ||
            s3.rdbuf() == nullptr || s3.fill() != 'a' ||
            stat(test_file.c_str(), &st1) != 0 || st1.st_size != 5 || !s3.is_open() ||
            s1.flags() != (ios_base::skipws | ios_base::dec) ||
            s1.precision() != 6 || s1.width() != 0 ||
            s1.rdstate() != ios_base::goodbit ||
            s1.exceptions() != ios_base::goodbit || s1.tie() != nullptr ||
            s1.rdbuf() == nullptr || s1.fill() != ' ' || s1.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream swap member\n";
        }
        // swap non-member
        ifstream s4 {test_file};
        s4.flags(ios_base::hex);
        s4.precision(5);
        s4.width(1);
        s4.setstate(ios_base::eofbit);
        s4.exceptions(ios_base::failbit);
        ofstream s5 {};
        s4.tie(&s5);
        s4.fill('a');
        ifstream s6 {};
        swap(s4, s6);
        struct stat st2;
        if (s6.flags() != ios_base::hex || s6.precision() != 5 ||
            s6.width() != 1 || s6.rdstate() != ios_base::eofbit ||
            s6.exceptions() != ios_base::failbit || s6.tie() != &s5 ||
            s6.rdbuf() == nullptr || s6.fill() != 'a' ||
            stat(test_file.c_str(), &st2) != 0 || st2.st_size != 5 || !s6.is_open() ||
            s4.flags() != (ios_base::skipws | ios_base::dec) ||
            s4.precision() != 6 || s4.width() != 0 ||
            s4.rdstate() != ios_base::goodbit ||
            s4.exceptions() != ios_base::goodbit || s4.tie() != nullptr ||
            s4.rdbuf() == nullptr || s4.fill() != ' ' || s4.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream swap non-member\n";
        }
        std::remove(test_file.c_str());
    }
    // rdbuf
    {
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello";
        stds1.close();
        ifstream s1 {test_file};
        if (!s1.rdbuf()->is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream rdbuf\n";
        }
        std::remove(test_file.c_str());
    }
    // is_open
    {
        // not open
        ifstream s1 {};
        if (s1.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream is_open, not open\n";
        }
        // open
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello";
        stds1.close();
        ifstream s2 {test_file};
        if (!s2.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream is_open, open\n";
        }
        std::remove(test_file.c_str());
    }
    // open
    {
        // in, new file
        ifstream s1 {test_file};
        if (s1 || !s1.fail() || s1.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, new file, in\n";
        }
        s1.close();
        std::remove(test_file.c_str());
        // out|in, new file
        ifstream s2 {test_file, ios_base::out};
        if (s2 || !s2.fail() || s2.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, new file, out|in\n";
        }
        s2.close();
        std::remove(test_file.c_str());
        // out|in|trunc, new file
        ifstream s3 {test_file, ios_base::out | ios_base::trunc};
        struct stat st1;
        if (!s3 || !s3.is_open() || stat(test_file.c_str(), &st1) != 0 ||
            st1.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, new file, out|in|trunc\n";
        }
        s3.close();
        std::remove(test_file.c_str());
        // out|in|app, new file
        ifstream s4 {test_file, ios_base::out | ios_base::app};
        struct stat st2;
        if (!s4 || !s4.is_open() || stat(test_file.c_str(), &st2) != 0 ||
            st2.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, new file, out|in|app\n";
        }
        s4.close();
        std::remove(test_file.c_str());
        // in|app, new file
        ifstream s5 {test_file, ios_base::app};
        struct stat st3;
        if (!s5 || !s5.is_open() || stat(test_file.c_str(), &st3) != 0 ||
            st3.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, new file, in|app\n";
        }
        s5.close();
        std::remove(test_file.c_str());
        // binary|in, new file
        ifstream s6 {test_file, ios_base::binary};
        if (s6 || !s6.fail() || s6.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, new file, binary|in\n";
        }
        s6.close();
        std::remove(test_file.c_str());
        // binary|out|in, new file
        ifstream s7 {test_file, ios_base::binary | ios_base::out};
        if (s7 || !s7.fail() || s7.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, new file, binary|out|in\n";
        }
        s7.close();
        std::remove(test_file.c_str());
        // binary|out|in|trunc, new file
        ifstream s8 {test_file, ios_base::binary | ios_base::out |
                                ios_base::trunc};
        struct stat st4;
        if (!s8 || !s8.is_open() || stat(test_file.c_str(), &st4) != 0 ||
            st4.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, new file, binary|out|in|trunc\n";
        }
        s8.close();
        std::remove(test_file.c_str());
        // binary|out|in|app, new file
        ifstream s9 {test_file, ios_base::binary | ios_base::out |
                                ios_base::app};
        struct stat st5;
        if (!s9 || !s9.is_open() || stat(test_file.c_str(), &st5) != 0 ||
            st5.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, new file, binary|out|in|app\n";
        }
        s9.close();
        std::remove(test_file.c_str());
        // binary|in|app, new file
        ifstream s10 {test_file, ios_base::binary | ios_base::app};
        struct stat st6;
        if (!s10 || !s10.is_open() || stat(test_file.c_str(), &st6) != 0 ||
            st6.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, new file, binary|in|app\n";
        }
        s10.close();
        std::remove(test_file.c_str());
        // in, existing file
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello";
        stds1.close();
        ifstream s11 {test_file};
        struct stat st7;
        if (!s11 || !s11.is_open()|| stat(test_file.c_str(), &st7) != 0 ||
            st7.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, existing file, in\n";
        }
        s11.close();
        std::remove(test_file.c_str());
        // out|in, existing file
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Hello";
        stds2.close();
        ifstream s12 {test_file, ios_base::out};
        struct stat st8;
        if (!s12 || !s12.is_open()|| stat(test_file.c_str(), &st8) != 0 ||
            st8.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, existing file, out|in\n";
        }
        s12.close();
        std::remove(test_file.c_str());
        // out|in|trunc, existing file
        std::ofstream stds3 {test_file.c_str()};
        stds3 << "Hello";
        stds3.close();
        ifstream s13 {test_file, ios_base::out | ios_base::trunc};
        struct stat st9;
        if (!s13 || !s13.is_open() || stat(test_file.c_str(), &st9) != 0 ||
            st9.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, existing file, out|in|trunc\n";
        }
        s13.close();
        std::remove(test_file.c_str());
        // out|in|app, existing file
        std::ofstream stds4 {test_file.c_str()};
        stds4 << "Hello";
        stds4.close();
        ifstream s14 {test_file, ios_base::out | ios_base::app};
        struct stat st10;
        if (!s14 || !s14.is_open() || stat(test_file.c_str(), &st10) != 0 ||
            st10.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, existing file, out|in|app\n";
        }
        s14.close();
        std::remove(test_file.c_str());
        // in|app, existing file
        std::ofstream stds5 {test_file.c_str()};
        stds5 << "Hello";
        stds5.close();
        ifstream s15 {test_file, ios_base::app};
        struct stat st11;
        if (!s15 || !s15.is_open() || stat(test_file.c_str(), &st11) != 0 ||
            st11.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, existing file, in|app\n";
        }
        s15.close();
        std::remove(test_file.c_str());
        // binary|in, existing file
        std::ofstream stds6 {test_file.c_str()};
        stds6 << "Hello";
        stds6.close();
        ifstream s16 {test_file, ios_base::binary};
        struct stat st12;
        if (!s16 || !s16.is_open()|| stat(test_file.c_str(), &st12) != 0 ||
            st12.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, existing file, binary|in\n";
        }
        s16.close();
        std::remove(test_file.c_str());
        // binary|out|in, existing file
        std::ofstream stds7 {test_file.c_str()};
        stds7 << "Hello";
        stds7.close();
        ifstream s17 {test_file, ios_base::binary | ios_base::out};
        struct stat st13;
        if (!s17 || !s17.is_open()|| stat(test_file.c_str(), &st13) != 0 ||
            st13.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, existing file, binary|out|in\n";
        }
        s17.close();
        std::remove(test_file.c_str());
        // binary|out|in|trunc, existing file
        std::ofstream stds8 {test_file.c_str()};
        stds8 << "Hello";
        stds8.close();
        ifstream s18 {test_file, ios_base::binary | ios_base::out |
                                 ios_base::trunc};
        struct stat st14;
        if (!s18 || !s18.is_open() || stat(test_file.c_str(), &st14) != 0 ||
            st14.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, existing file, binary|out|in|trunc\n";
        }
        s18.close();
        std::remove(test_file.c_str());
        // binary|out|in|app, existing file
        std::ofstream stds9 {test_file.c_str()};
        stds9 << "Hello";
        stds9.close();
        ifstream s19 {test_file, ios_base::binary | ios_base::out |
                                 ios_base::app};
        struct stat st15;
        if (!s19 || !s19.is_open() || stat(test_file.c_str(), &st15) != 0 ||
            st15.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, existing file, binary|out|in|app\n";
        }
        s19.close();
        std::remove(test_file.c_str());
        // binary|in|app, existing file
        std::ofstream stds10 {test_file.c_str()};
        stds10 << "Hello";
        stds10.close();
        ifstream s20 {test_file, ios_base::binary | ios_base::app};
        struct stat st16;
        if (!s20 || !s20.is_open() || stat(test_file.c_str(), &st16) != 0 ||
            st16.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream open, existing file, binary|in|app\n";
        }
        s20.close();
        std::remove(test_file.c_str());
    }
    // close
    {
        // After opening existing file.
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello";
        stds1.close();
        ifstream s1 {test_file};
        s1.close();
        struct stat st1;
        if (!s1 || s1.is_open() || stat(test_file.c_str(), &st1) != 0 ||
            st1.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream close, existing file\n";
        }
        std::remove(test_file.c_str());
        // After opening and reading existing file.
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Hello";
        stds2.close();
        ifstream s2 {test_file};
        klib::string str1;
        s2 >> str1;
        s2.close();
        struct stat st2;
        if (!s2 || s2.is_open() || stat(test_file.c_str(), &st2) != 0 ||
            st2.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream close, existing file, after read\n";
        }
        std::remove(test_file.c_str());
        // After truncating existing file.
        std::ofstream stds3 {test_file.c_str()};
        stds3 << "Hello";
        stds3.close();
        ifstream s3 {test_file, ios_base::out | ios_base::trunc};
        s3.close();
        struct stat st3;
        if (!s3 || s3.is_open() || stat(test_file.c_str(), &st3) != 0 ||
            st3.st_size != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream close, existing file, after truncation\n";
        }
        std::remove(test_file.c_str());
        // Without opening
        ifstream s4;
        s4.close();
        if (s4 || s4.is_open())
        {
            ++fail_count;
            std::cout << "FAILED ifstream close, without opening\n";
        }
        // After already closing
        std::ofstream stds4 {test_file.c_str()};
        stds4 << "Hello";
        stds4.close();
        ifstream s5 {test_file};
        s5.close();
        s5.close();
        struct stat st4;
        if (s5 || s5.is_open() || stat(test_file.c_str(), &st4) != 0 ||
            st4.st_size != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream close, after already closing\n";
        }
        std::remove(test_file.c_str());
    }
    // Reading, in only
    {
        // Exisiting file
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello";
        stds1.close();
        ifstream s1 {test_file};
        string str1;
        s1 >> str1;
        struct stat st1;
        if (!s1 || !s1.eof() || stat(test_file.c_str(), &st1) != 0 ||
            st1.st_size != 5 || str1 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream read, in only, existing file\n";
        }
        s1.close();
        std::remove(test_file.c_str());
        // Exisiting file, underflow
        std::ofstream stds2 {test_file.c_str()};
        for (int i = 0; i < BUFSIZ + 10; ++i)
            stds2 << 'a';
        stds2.close();
        ifstream s2 {test_file};
        string str2;
        s2 >> str2;
        struct stat st2;
        std::string stdstr1 {str2.c_str()};
        if (!s2 || !s2.eof() || stat(test_file.c_str(), &st2) != 0 ||
            st2.st_size != BUFSIZ + 10 || str2.size() != BUFSIZ + 10 || 
            !std::all_of(stdstr1.begin(), stdstr1.end(),
            [](char c){ return c == 'a'; }))
        {
            ++fail_count;
            std::cout << "FAILED ifstream read, in only, existing file, underflow\n";
        }
        s2.close();
        std::remove(test_file.c_str());
    }
    // Reading, open for reading and writing
    {
        // Exisiting file, out|in
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello";
        stds1.close();
        ifstream s1 {test_file, ios_base::out};
        string str1;
        s1 >> str1;
        struct stat st1;
        if (!s1 || !s1.eof() || stat(test_file.c_str(), &st1) != 0 ||
            st1.st_size != 5 || str1 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream read, out|in, existing file\n";
        }
        s1.close();
        std::remove(test_file.c_str());
        // Exisiting file, out|in, underflow
        std::ofstream stds2 {test_file.c_str()};
        for (int i = 0; i < BUFSIZ + 10; ++i)
            stds2 << 'a';
        stds2.close();
        ifstream s2 {test_file, ios_base::out};
        string str2;
        s2 >> str2;
        struct stat st2;
        std::string stdstr1 {str2.c_str()};
        if (!s2 || !s2.eof() || stat(test_file.c_str(), &st2) != 0 ||
            st2.st_size != BUFSIZ + 10 || str2.size() != BUFSIZ + 10 || 
            !std::all_of(stdstr1.begin(), stdstr1.end(),
            [](char c){ return c == 'a'; }))
        {
            ++fail_count;
            std::cout << "FAILED ifstream read, out|in, existing file, underflow\n";
        }
        s2.close();
        std::remove(test_file.c_str());
        // Exisiting file, out|in|trunc
        std::ofstream stds3 {test_file.c_str()};
        stds3 << "Hello";
        stds3.close();
        ifstream s3 {test_file, ios_base::out | ios_base::trunc};
        string str3;
        s3 >> str3;
        struct stat st3;
        if (s3 || !s3.eof() || stat(test_file.c_str(), &st3) != 0 ||
            st3.st_size != 0 || str3 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream read, out|in|trunc, existing file\n";
        }
        s3.close();
        std::remove(test_file.c_str());
        // Exisiting file, out|in|trunc, underflow
        std::ofstream stds4 {test_file.c_str()};
        for (int i = 0; i < BUFSIZ + 10; ++i)
            stds4 << 'a';
        stds4.close();
        ifstream s4 {test_file, ios_base::out | ios_base::trunc};
        string str4;
        s4 >> str4;
        struct stat st4;
        if (s4 || !s4.eof() || stat(test_file.c_str(), &st4) != 0 ||
            st4.st_size != 0 || str4 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream read, out|in|trunc, existing file, underflow\n";
        }
        s4.close();
        std::remove(test_file.c_str());
        // New file, out|in|trunc
        ifstream s5 {test_file, ios_base::out | ios_base::trunc};
        string str5;
        s5 >> str5;
        struct stat st5;
        if (s5 || !s5.eof() || stat(test_file.c_str(), &st5) != 0 ||
            st5.st_size != 0 || str5 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream read, out|in|trunc, new file\n";
        }
        s5.close();
        std::remove(test_file.c_str());
        // Exisiting file, in|app
        std::ofstream stds6 {test_file.c_str()};
        stds6 << "Hello";
        stds6.close();
        ifstream s6 {test_file, ios_base::app};
        string str6;
        s6 >> str6;
        struct stat st6;
        if (!s6 || !s6.eof() || stat(test_file.c_str(), &st6) != 0 ||
            st6.st_size != 5 || str6 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream read, in|app, existing file\n";
        }
        s6.close();
        std::remove(test_file.c_str());
        // Exisiting file, in|app, underflow
        std::ofstream stds7 {test_file.c_str()};
        for (int i = 0; i < BUFSIZ + 10; ++i)
            stds7 << 'a';
        stds7.close();
        ifstream s7 {test_file, ios_base::app};
        string str7;
        s7 >> str7;
        struct stat st7;
        std::string stdstr2 {str7.c_str()};
        if (!s7 || !s7.eof() || stat(test_file.c_str(), &st7) != 0 ||
            st7.st_size != BUFSIZ + 10 || str7.size() != BUFSIZ + 10 || 
            !std::all_of(stdstr2.begin(), stdstr2.end(),
            [](char c){ return c == 'a'; }))
        {
            ++fail_count;
            std::cout << "FAILED ifstream read, in|app, existing file, underflow\n";
        }
        s7.close();
        std::remove(test_file.c_str());
        // New file, in|app
        ifstream s8 {test_file, ios_base::app};
        string str8;
        s8 >> str8;
        struct stat st8;
        if (s8 || !s8.eof() || stat(test_file.c_str(), &st8) != 0 ||
            st8.st_size != 0 || str8 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream read, in|app, new file\n";
        }
        s8.close();
        std::remove(test_file.c_str());
    }
    // tellg
    {
        // in, before read
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello Goodbye";
        stds1.close();
        ifstream s1 {test_file};
        ifstream::pos_type g1 = s1.tellg();
        if (static_cast<ifstream::off_type>(g1) != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream tellg, in, before read\n";
        }
        s1.close();
        std::remove(test_file.c_str());
        // in, after read
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Hello Goodbye";
        stds2.close();
        ifstream s2 {test_file};
        string str1;
        s2 >> str1;
        ifstream::pos_type g2 = s2.tellg();
        if (static_cast<ifstream::off_type>(g2) != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream tellg, in, after read\n";
        }
        s2.close();
        std::remove(test_file.c_str());
        // out|in, before read
        std::ofstream stds3 {test_file.c_str()};
        stds3 << "Hello Goodbye";
        stds3.close();
        ifstream s3 {test_file, ios_base::out};
        ifstream::pos_type g3 = s3.tellg();
        if (static_cast<ifstream::off_type>(g3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream tellg, out|in, before read\n";
        }
        s3.close();
        std::remove(test_file.c_str());
        // out|in, after read
        std::ofstream stds4 {test_file.c_str()};
        stds4 << "Hello Goodbye";
        stds4.close();
        ifstream s4 {test_file, ios_base::out};
        string str2;
        s4 >> str2;
        ifstream::pos_type g4 = s4.tellg();
        if (static_cast<ifstream::off_type>(g4) != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream tellg, out|in, after read\n";
        }
        s4.close();
        std::remove(test_file.c_str());
        // out|in|trunc, before read, existing file
        std::ofstream stds5 {test_file.c_str()};
        stds5 << "Hello Goodbye";
        stds5.close();
        ifstream s5 {test_file, ios_base::out | ios_base::trunc};
        ifstream::pos_type g5 = s5.tellg();
        if (static_cast<ifstream::off_type>(g5) != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream tellg, out|in|trunc, existing file, before read\n";
        }
        s5.close();
        std::remove(test_file.c_str());
        // out|in|trunc, after read, existing file
        std::ofstream stds6 {test_file.c_str()};
        stds6 << "Hello Goodbye";
        stds6.close();
        ifstream s6 {test_file, ios_base::out | ios_base::trunc};
        string str3;
        s6 >> str3;
        ifstream::pos_type g6 = s6.tellg();
        if (static_cast<ifstream::off_type>(g6) != -1)
        {
            ++fail_count;
            std::cout << "FAILED ifstream tellg, out|in|trunc, existing file, after read\n";
        }
        s6.close();
        std::remove(test_file.c_str());
        // out|in|trunc, before read, new file
        ifstream s7 {test_file, ios_base::out | ios_base::trunc};
        ifstream::pos_type g7 = s7.tellg();
        if (static_cast<ifstream::off_type>(g7) != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream tellg, out|in|trunc, new file, before read\n";
        }
        s7.close();
        std::remove(test_file.c_str());
        // out|in|trunc, after read, new file
        ifstream s8 {test_file, ios_base::out | ios_base::trunc};
        string str4;
        s8 >> str4;
        ifstream::pos_type g8 = s8.tellg();
        if (static_cast<ifstream::off_type>(g8) != -1)
        {
            ++fail_count;
            std::cout << "FAILED ifstream tellg, out|in|trunc, new file, after read\n";
        }
        s8.close();
        std::remove(test_file.c_str());
        // in|app, before read, existing file
        std::ofstream stds7 {test_file.c_str()};
        stds7 << "Hello Goodbye";
        stds7.close();
        ifstream s9 {test_file, ios_base::app};
        ifstream::pos_type g9 = s9.tellg();
        if (static_cast<ifstream::off_type>(g9) != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream tellg, in|app, existing file, before read\n";
        }
        s9.close();
        std::remove(test_file.c_str());
        // in|app, after read, existing file
        std::ofstream stds8 {test_file.c_str()};
        stds8 << "Hello Goodbye";
        stds8.close();
        ifstream s10 {test_file, ios_base::app};
        string str5;
        s10 >> str5;
        ifstream::pos_type g10 = s10.tellg();
        if (static_cast<ifstream::off_type>(g10) != 5)
        {
            ++fail_count;
            std::cout << "FAILED ifstream tellg, in|app, existing file, after read\n";
        }
        s10.close();
        std::remove(test_file.c_str());
        // in|app, before read, new file
        ifstream s11 {test_file, ios_base::app};
        ifstream::pos_type g11 = s11.tellg();
        if (static_cast<ifstream::off_type>(g11) != 0)
        {
            ++fail_count;
            std::cout << "FAILED ifstream tellg, in|app, new file, before read\n";
        }
        s11.close();
        std::remove(test_file.c_str());
        // in|app, after read, new file
        ifstream s12 {test_file, ios_base::app};
        string str6;
        s12 >> str6;
        ifstream::pos_type g12 = s12.tellg();
        if (static_cast<ifstream::off_type>(g12) != -1)
        {
            ++fail_count;
            std::cout << "FAILED ifstream tellg, in|app, new file, after read\n";
        }
        s12.close();
        std::remove(test_file.c_str());
    }
    // seekp, beg
    {
        // in
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello Goodbye";
        stds1.close();
        ifstream s1 {test_file};
        s1.seekg(1, ios_base::beg);
        string str1;
        s1 >> str1;
        if (str1 != "ello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, beg, in\n";
        }
        s1.close();
        std::remove(test_file.c_str());
        // out|in
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Hello Goodbye";
        stds2.close();
        ifstream s2 {test_file, ios_base::out};
        s2.seekg(1, ios_base::beg);
        string str2;
        s2 >> str2;
        if (str2 != "ello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, beg, out|in\n";
        }
        s2.close();
        std::remove(test_file.c_str());
        // out|in|trunc, existing file
        std::ofstream stds3 {test_file.c_str()};
        stds3 << "Hello Goodbye";
        stds3.close();
        ifstream s3 {test_file, ios_base::out | ios_base::trunc};
        s3.seekg(1, ios_base::beg);
        string str3;
        s3 >> str3;
        if (s3 || str3 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, beg, out|in|trunc, existing file\n";
        }
        s3.close();
        std::remove(test_file.c_str());
        // out|in|trunc, new file
        ifstream s4 {test_file, ios_base::out | ios_base::trunc};
        s4.seekg(1, ios_base::beg);
        string str4;
        s4 >> str4;
        if (s4 || str4 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, beg, out|in|trunc, new file\n";
        }
        s4.close();
        std::remove(test_file.c_str());
        // in|app, existing file
        std::ofstream stds4 {test_file.c_str()};
        stds4 << "Hello Goodbye";
        stds4.close();
        ifstream s5 {test_file, ios_base::app};
        s5.seekg(1, ios_base::beg);
        string str5;
        s5 >> str5;
        if (str5 != "ello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, beg, in|app, existing file\n";
        }
        s5.close();
        std::remove(test_file.c_str());
        // in|app, new file
        ifstream s6 {test_file, ios_base::app};
        s6.seekg(1, ios_base::beg);
        string str6;
        s6 >> str6;
        if (s6 || str6 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, beg, in|app, new file\n";
        }
        s6.close();
        std::remove(test_file.c_str());
    }
    // seekp, cur
    {
        // in
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello Goodbye";
        stds1.close();
        ifstream s1 {test_file};
        s1.seekg(1, ios_base::cur);
        string str1;
        s1 >> str1;
        if (str1 != "ello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, cur, in\n";
        }
        s1.close();
        std::remove(test_file.c_str());
        // out|in
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Hello Goodbye";
        stds2.close();
        ifstream s2 {test_file, ios_base::out};
        s2.seekg(1, ios_base::cur);
        string str2;
        s2 >> str2;
        if (str2 != "ello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, cur, out|in\n";
        }
        s2.close();
        std::remove(test_file.c_str());
        // out|in|trunc, existing file
        std::ofstream stds3 {test_file.c_str()};
        stds3 << "Hello Goodbye";
        stds3.close();
        ifstream s3 {test_file, ios_base::out | ios_base::trunc};
        s3.seekg(1, ios_base::cur);
        string str3;
        s3 >> str3;
        if (s3 || str3 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, cur, out|in|trunc, existing file\n";
        }
        s3.close();
        std::remove(test_file.c_str());
        // out|in|trunc, new file
        ifstream s4 {test_file, ios_base::out | ios_base::trunc};
        s4.seekg(1, ios_base::cur);
        string str4;
        s4 >> str4;
        if (s4 || str4 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, cur, out|in|trunc, new file\n";
        }
        s4.close();
        std::remove(test_file.c_str());
        // in|app, existing file
        std::ofstream stds4 {test_file.c_str()};
        stds4 << "Hello Goodbye";
        stds4.close();
        ifstream s5 {test_file, ios_base::app};
        s5.seekg(1, ios_base::cur);
        string str5;
        s5 >> str5;
        if (str5 != "ello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, cur, in|app, existing file\n";
        }
        s5.close();
        std::remove(test_file.c_str());
        // in|app, new file
        ifstream s6 {test_file, ios_base::app};
        s6.seekg(1, ios_base::cur);
        string str6;
        s6 >> str6;
        if (s6 || str6 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, cur, in|app, new file\n";
        }
        s6.close();
        std::remove(test_file.c_str());
    }
    // seekp, end
    {
        // in
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello Goodbye";
        stds1.close();
        ifstream s1 {test_file};
        s1.seekg(-2, ios_base::end);
        string str1;
        s1 >> str1;
        if (str1 != "ye")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, end, in\n";
        }
        s1.close();
        std::remove(test_file.c_str());
        // out|in
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Hello Goodbye";
        stds2.close();
        ifstream s2 {test_file, ios_base::out};
        s2.seekg(-2, ios_base::end);
        string str2;
        s2 >> str2;
        if (str2 != "ye")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, end, out|in\n";
        }
        s2.close();
        std::remove(test_file.c_str());
        // out|in|trunc, existing file
        std::ofstream stds3 {test_file.c_str()};
        stds3 << "Hello Goodbye";
        stds3.close();
        ifstream s3 {test_file, ios_base::out | ios_base::trunc};
        s3.seekg(-2, ios_base::end);
        string str3;
        s3 >> str3;
        if (s3 || str3 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, end, out|in|trunc, existing file\n";
        }
        s3.close();
        std::remove(test_file.c_str());
        // out|in|trunc, new file
        ifstream s4 {test_file, ios_base::out | ios_base::trunc};
        s4.seekg(-2, ios_base::end);
        string str4;
        s4 >> str4;
        if (s4 || str4 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, end, out|in|trunc, new file\n";
        }
        s4.close();
        std::remove(test_file.c_str());
        // in|app, existing file
        std::ofstream stds4 {test_file.c_str()};
        stds4 << "Hello Goodbye";
        stds4.close();
        ifstream s5 {test_file, ios_base::app};
        s5.seekg(-2, ios_base::end);
        string str5;
        s5 >> str5;
        if (str5 != "ye")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, end, in|app, existing file\n";
        }
        s5.close();
        std::remove(test_file.c_str());
        // in|app, new file
        ifstream s6 {test_file, ios_base::app};
        s6.seekg(-2, ios_base::end);
        string str6;
        s6 >> str6;
        if (s6 || str6 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, end, in|app, new file\n";
        }
        s6.close();
        std::remove(test_file.c_str());
    }
    // seekp, absolute
    {
        // in
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello Goodbye";
        stds1.close();
        ifstream s1 {test_file};
        s1.seekg(1);
        string str1;
        s1 >> str1;
        if (str1 != "ello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, absolute, in\n";
        }
        s1.close();
        std::remove(test_file.c_str());
        // out|in
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Hello Goodbye";
        stds2.close();
        ifstream s2 {test_file, ios_base::out};
        s2.seekg(1);
        string str2;
        s2 >> str2;
        if (str2 != "ello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, absolute, out|in\n";
        }
        s2.close();
        std::remove(test_file.c_str());
        // out|in|trunc, existing file
        std::ofstream stds3 {test_file.c_str()};
        stds3 << "Hello Goodbye";
        stds3.close();
        ifstream s3 {test_file, ios_base::out | ios_base::trunc};
        s3.seekg(1);
        string str3;
        s3 >> str3;
        if (s3 || str3 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, absolute, out|in|trunc, existing file\n";
        }
        s3.close();
        std::remove(test_file.c_str());
        // out|in|trunc, new file
        ifstream s4 {test_file, ios_base::out | ios_base::trunc};
        s4.seekg(1);
        string str4;
        s4 >> str4;
        if (s4 || str4 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, absolute, out|in|trunc, new file\n";
        }
        s4.close();
        std::remove(test_file.c_str());
        // in|app, existing file
        std::ofstream stds4 {test_file.c_str()};
        stds4 << "Hello Goodbye";
        stds4.close();
        ifstream s5 {test_file, ios_base::app};
        s5.seekg(1);
        string str5;
        s5 >> str5;
        if (str5 != "ello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, absolute, in|app, existing file\n";
        }
        s5.close();
        std::remove(test_file.c_str());
        // in|app, new file
        ifstream s6 {test_file, ios_base::app};
        s6.seekg(1);
        string str6;
        s6 >> str6;
        if (s6 || str6 != "")
        {
            ++fail_count;
            std::cout << "FAILED ifstream seekg, absolute, in|app, new file\n";
        }
        s6.close();
        std::remove(test_file.c_str());
    }
    // sync
    {
        // No change
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello Goodbye";
        stds1.close();
        ifstream s1 {test_file};
        string str1;
        s1 >> str1;
        s1.sync();
        s1 >> str1;
        if (str1 != "Goodbye")
        {
            ++fail_count;
            std::cout << "FAILED ifstream, sync, no change\n";
        }
        s1.close();
        std::remove(test_file.c_str());
        // Underlying change
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Hello ";
        stds2.flush();
        ifstream s2 {test_file};
        string str2;
        s2 >> str2;
        stds2 << "Goodbye";
        s2.sync();
        s2 >> str2;
        if (str1 != "Goodbye")
        {
            ++fail_count;
            std::cout << "FAILED ifstream, sync, underlying change\n";
        }
        s2.close();
        std::remove(test_file.c_str());
    }
    // putback
    {
        // Non-modifying
        std::ofstream stds1 {test_file.c_str()};
        stds1 << "Hello";
        stds1.close();
        ifstream s1 {test_file};
        s1.get();
        s1.putback('H');
        string str1;
        s1 >> str1;
        if (s1.gcount() != 0 || !s1 || !s1.eof() || s1.fail() || s1.bad() ||
            str1 != "Hello")
        {
            ++fail_count;
            std::cout << "FAILED ifstream putback, non-modifying\n";
        }
        s1.close();
        std::remove(test_file.c_str());
        // Modifying, not allowed
        std::ofstream stds2 {test_file.c_str()};
        stds2 << "Hello";
        stds2.close();
        ifstream s2 {test_file};
        s2.get();
        s2.putback('Y');
        if (s2.gcount() != 0 || s2 || s2.eof() || !s2.fail() || !s2.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream putback, modifying, not allowed\n";
        }
        s2.close();
        std::remove(test_file.c_str());
        // At start
        std::ofstream stds3 {test_file.c_str()};
        stds3 << "Hello";
        stds3.close();
        ifstream s3 {test_file};
        s3.putback('Y');
        if (s3.gcount() != 0 || s3 || s3.eof() || !s3.fail() || !s3.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream putback, at start\n";
        }
        s3.close();
        std::remove(test_file.c_str());
        // Empty
        std::ofstream stds4 {test_file.c_str()};
        stds4.close();
        ifstream s4 {test_file};
        s4.putback('Y');
        if (s4.gcount() != 0 || s4 || s4.eof() || !s4.fail() || !s4.bad())
        {
            ++fail_count;
            std::cout << "FAILED istringstream putback, empty\n";
        }
        s4.close();
        std::remove(test_file.c_str());
        // Modifying, allowed
        std::ofstream stds5 {test_file.c_str()};
        stds5 << "Hello";
        stds5.close();
        ifstream s5 {test_file, ios_base::out};
        s5.get();
        s5.putback('Y');
        string str2;
        s5 >> str2;
        if (s5.gcount() != 0 || !s5 || !s5.eof() || s5.fail() || s5.bad() ||
            str2 != "Yello")
        {
            ++fail_count;
            std::cout << "FAILED istringstream putback, modifying, allowed\n";
        }
        s5.close();
        std::remove(test_file.c_str());
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
