#include <iostream>
#include <iomanip>

#include "../include/string"

#include "../include/cmath"
#include "../include/cwchar"
#include "../include/ios"
#include "../include/limits"
#include "../include/type_traits"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // char_traits<char>
    {
        // Types
        if (!is_same<typename char_traits<char>::char_type, char>::value)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::char_type\n";
        }
        if (!is_same<typename char_traits<char>::int_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::int_type\n";
        }
        if (!is_same<typename char_traits<char>::off_type, klib::streamoff>::value)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::off_type\n";
        }
        if (!is_same<typename char_traits<char>::pos_type, klib::streampos>::value)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::pos_type\n";
        }
        if (!is_same<typename char_traits<char>::state_type, klib::mbstate_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::state_type\n";
        }
        // assign
        char c1;
        char_traits<char>::assign(c1, 'a');
        if (c1 != 'a')
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::assign char literal\n";
        }
        char c2;
        char_traits<char>::assign(c2, c1);
        if (c2 != 'a')
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::assign char\n";
        }
        char c3[4] = "abc";
        char_traits<char>::assign(c3, 2, 'd');
        if (c3[0] != 'd' || c3[1] != 'd' || c3[2] != 'c')
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::assign char*\n";
        }
        // eq
        char c4 = 'a';
        char c5 = 'a';
        char c6 = 'b';
        if (!char_traits<char>::eq(c4, c5))
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::eq equal\n";
        }
        if (char_traits<char>::eq(c4, c6))
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::eq not equal\n";
        }
        // lt
        char c7 = 'a';
        char c8 = 'a';
        char c9 = 'b';
        if (!char_traits<char>::lt(c7, c9))
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::lt less than\n";
        }
        if (char_traits<char>::lt(c7, c8))
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::lt equal\n";
        }
        if (char_traits<char>::lt(c9, c7))
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::lt greater than\n";
        }
        // move
        char c10[4] = "abc";
        char c11[4] = "def";
        // No overlap
        char_traits<char>::move(c10 + 1, c11, 2);
        if (c10[0] != 'a' || c10[1] != 'd' || c10[2] != 'e' || c10[3] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::move no overlap\n";
        }
        // Overlap
        char_traits<char>::move(c11, c11 + 1, 2);
        if (c11[0] != 'e' || c11[1] != 'f' || c11[2] != 'f' || c11[3] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::move overlap\n";
        }
        // copy
        char c12[4] = "abc";
        char c13[4] = "def";
        char_traits<char>::copy(c12, c13, 2);
        if (c12[0] != 'd' || c12[1] != 'e' || c12[2] != 'c' || c12[3] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::copy\n";
        }
        // compare
        char c14[4] = "abc";
        char c15[4] = "abc";
        char c16[4] = "def";
        if (char_traits<char>::compare(c14, c16, 3) >= 0)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::compare less than\n";
        }
        if (char_traits<char>::compare(c14, c15, 3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::compare equal\n";
        }
        if (char_traits<char>::compare(c16, c14, 3) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::compare greater than\n";
        }
        // length
        char c17[1] = "";
        char c18[4] = "abc";
        char c19[4] = "a\0b";
        if (char_traits<char>::length(c17) != 0)
        {
            ++fail_count;
            std::cout << "length(c17) == " << char_traits<char>::length(c17) << '\n';
            std::cout << "FAILED char_traits<char>::length empty\n";
        }
        if (char_traits<char>::length(c18) != 3)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::length normal\n";
        }
        if (char_traits<char>::length(c19) != 1)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::length with nulls\n";
        }
        // find
        char c20[4] = "abc";
        if (char_traits<char>::find(c20, 4, 'b') != c20 + 1)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::find success\n";
        }
        if (char_traits<char>::find(c20, 2, 'c') != NULL)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::find failed out of range\n";
        }
        if (char_traits<char>::find(c20, 4, 'd') != NULL)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::find failed not present\n";
        }
        // to_char_type
        int i1 = 97;
        if (char_traits<char>::to_char_type(i1) != 'a')
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::to_char_type\n";
        }
        // to_int_type
        char c21 = 'a';
        if (char_traits<char>::to_int_type(c21) != 97)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::to_int_type\n";
        }
        // eq_int_type
        int i2 = 97;
        int i3 = 97;
        int i4 = 98;
        if (!char_traits<char>::eq_int_type(i2, i3))
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::eq_int_type equal\n";
        }
        if (char_traits<char>::eq_int_type(i2, i4))
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::eq_int_type not equal\n";
        }
        // eof
        if (char_traits<char>::eof() != EOF)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::eof\n";
        }
        // not_eof
        if (char_traits<char>::not_eof(97) == 0)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::not_eof not EOF\n";
        }
        if (char_traits<char>::not_eof(EOF) != 0)
        {
            ++fail_count;
            std::cout << "FAILED char_traits<char>::not_eof EOF\n";
        }
    }
    // string (basic_string<char>)
    constexpr size_t default_space = 7;
    {
        // Types
        if (!is_same<string::traits_type, char_traits<char>>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::traits_type\n";
        }
        if (!is_same<string::value_type, char>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::value_type\n";
        }
        if (!is_same<string::allocator_type, allocator<char>>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::allocator_type\n";
        }
        if (!is_same<string::size_type,
            typename allocator_traits<string::allocator_type>::size_type>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::size_type\n";
        }
        if (!is_same<string::difference_type,
            typename allocator_traits<string::allocator_type>::difference_type>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::difference_type\n";
        }
        if (!is_same<string::reference, char&>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::reference\n";
        }
        if (!is_same<string::const_reference, const char&>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::const_reference\n";
        }
        if (!is_same<string::pointer,
            typename allocator_traits<string::allocator_type>::pointer>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::pointer\n";
        }
        if (!is_same<string::const_pointer,
            typename allocator_traits<string::allocator_type>::const_pointer>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::const_pointer\n";
        }
        if (!is_same<string::iterator,
            helper::normal_iterator<string::pointer, string>>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::iterator\n";
        }
        if (!is_same<string::const_iterator,
            helper::normal_iterator<string::const_pointer, string>>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::const_iterator\n";
        }
        if (!is_same<string::reverse_iterator,
            reverse_iterator<string::iterator>>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::reverse_iterator\n";
        }
        if (!is_same<string::const_reverse_iterator,
            reverse_iterator<string::const_iterator>>::value)
        {
            ++fail_count;
            std::cout << "FAILED string::const_reverse_iterator\n";
        }
        if (string::npos != static_cast<string::size_type>(-1))
        {
            ++fail_count;
            std::cout << "FAILED string::npos value\n";
        }
    }
        // Constructors
    {
        string s1;
        if (!s1.empty() || s1.size() != 0 || s1[0] != '\0' ||
            s1.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string()\n";
        }
        string s2 {allocator<char>()};
        if (!s2.empty() || s2.size() != 0 || s2[0] != '\0' ||
            s1.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string(allocator)\n";
        }
        string s3 (3, 'a');
        if (s3.size() != 3 || s3[0] != 'a' || s3[1] != 'a' || s3[2] != 'a' ||
            s3[3] != '\0' || s3.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string(count, char)\n";
        }
        string s4 {"hello"};
        string s5 {s4, 1, 2};
        string s6 {s4, 3, 5};
        if (s5.size() != 2 || s5[0] != 'e' || s5[1] != 'l' || s5[2] != '\0' ||
            s5.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string(string, pos, count) valid count\n";
        }
        if (s6.size() != 2 || s6[0] != 'l' || s6[1] != 'o' || s6[2] != '\0' ||
            s6.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string(string, pos, count) too large count\n";
        }
        string s7 {"hello", 2};
        if (s7.size() != 2 || s7[0] != 'h' || s7[1] != 'e' || s7[2] != '\0' ||
            s7.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string(C string, count)\n";
        }
        string s8 {"he"};
        if (s8.size() != 2 || s8[0] != 'h' || s8[1] != 'e' || s8[2] != '\0' ||
            s8.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string(C string)\n";
        }
        string s9 {"hello"};
        string s10 {s9.begin() + 1, s9.begin() + 3};
        if (s10.size() != 2 || s10[0] != 'e' || s10[1] != 'l' ||
            s10[2] != '\0' || s10.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string(Iter, Iter)\n";
        }
        string s11 {"he"};
        string s12 {s11};
        if (s12.size() != 2 || s12[0] != 'h' || s12[1] != 'e' ||
            s12[2] != '\0' || s12.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string(string)\n";
        }
        string s13 {"he"};
        string s14 {s13, allocator<char>()};
        if (s14.size() != 2 || s14[0] != 'h' || s14[1] != 'e' ||
            s14[2] != '\0' || s14.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string(string, allocator)\n";
        }
        string s15 {"he"};
        string s16 {move(s15)};
        if (s16.size() != 2 || s16[0] != 'h' || s16[1] != 'e' ||
            s16[2] != '\0' || s16.capacity() != default_space || !s15.empty() ||
            s15.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string(string&&)\n";
        }
        string s17 {"he"};
        string s18 {move(s17), allocator<char>()};
        if (s18.size() != 2 || s18[0] != 'h' || s18[1] != 'e' ||
            s18[2] != '\0' || s18.capacity() != default_space || !s17.empty() ||
            s17.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string(string&&, allocator)\n";
        }
        string s19 {'h', 'e'};
        if (s19.size() != 2 || s19[0] != 'h' || s19[1] != 'e' ||
            s19[2] != '\0' || s19.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED string(initializer_list)\n";
        }
        string s20 {"Hello, world!"};
        if (s20.size() != 13 || s20 != "Hello, world!" || s20.capacity() != 13)
        {
            ++fail_count;
            std::cout << "FAILED string(C string) above default space\n";
        }
        // Note: check destructor by running test/memcheck.sh on the string_test
        // executable and check for memory leaks.
    }
    {
        // operator=
        string s1 {"he"};
        string s2 {"llo"};
        s2 = s1;
        if (s2.size() != 2 || s2.capacity() != default_space || s2[0] != 'h' ||
            s2[1] != 'e' || s2[3] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED operator=(string)\n";
        }
        string s3 {"he"};
        string s4 {"llo"};
        s4 = move(s3);
        if (s4.size() != 2 || s4.capacity() != default_space || s4[0] != 'h' ||
            s4[1] != 'e' || s4[2] != '\0' || !s3.empty() ||
            s3.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED operator=(string&&)\n";
        }
        string s5 {"llo"};
        s5 = "he";
        if (s5.size() != 2 || s5.capacity() != default_space || s5[0] != 'h' ||
            s5[1] != 'e' || s5[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED operator=(C string)\n";
        }
        string s6 {"llo"};
        s6 = 'h';
        if (s6.size() != 1 || s6.capacity() != default_space || s6[0] != 'h' ||
            s6[1] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED operator=(char)\n";
        }
        string s7 {"llo"};
        s7 = {'h', 'e'};
        if (s7.size() != 2 || s7.capacity() != default_space || s7[0] != 'h' ||
            s7[1] != 'e' || s7[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED operator=(initializer_list)\n";
        }
    }
    {
        // assign
        string s1 {"llo"};
        s1.assign(2, 'h');
        if (s1.size() != 2 || s1.capacity() != default_space || s1[0] != 'h' ||
            s1[1] != 'h' || s1[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED assign(count, char)\n";
        }
        string s2 {"he"};
        string s3 {"llo"};
        s3.assign(s2);
        if (s3.size() != 2 || s3.capacity() != default_space || s3[0] != 'h' ||
            s3[1] != 'e' || s3[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED assign(string)\n";
        }
        string s4 {"hello"};
        string s5 {"he"};
        string s6 {"he"};
        s5.assign(s4, 1, 2);
        if (s5.size() != 2 || s5.capacity() != default_space || s5[0] != 'e' ||
            s5[1] != 'l' || s5[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED assign(string, pos, count) valid count\n";
        }
        s6.assign(s4, 3, 5);
        if (s6.size() != 2 || s6.capacity() != default_space || s6[0] != 'l' ||
            s6[1] != 'o' || s6[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED assign(string, pos, count) count too large\n";
        }
        string s7 {"he"};
        string s8 {"llo"};
        s8.assign(move(s7));
        if (s8.size() != 2 || s8.capacity() != default_space || s8[0] != 'h' ||
            s8[1] != 'e' || s8[2] != '\0' || !s7.empty() ||
            s7.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED assign(string&&)\n";
        }
        string s9 {"llo"};
        s9.assign("hello", 2);
        if (s9.size() != 2 || s9.capacity() != default_space || s9[0] != 'h' ||
            s9[1] != 'e' || s9[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED assign(C string, count)\n";
        }
        string s10 {"llo"};
        s10.assign("he");
        if (s10.size() != 2 || s10.capacity() != default_space ||
            s10[0] != 'h' || s10[1] != 'e' || s10[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED assign(C string)\n";
        }
        string s11 {"hello"};
        string s12 {"llo"};
        s12.assign(s11.begin(), s11.begin() + 2);
        if (s12.size() != 2 || s12.capacity() != default_space ||
            s12[0] != 'h' || s12[1] != 'e' || s12[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED assign(Iter, Iter)\n";
        }
        string s13 {"llo"};
        s13.assign({'h', 'e'});
        if (s13.size() != 2 || s13.capacity() != default_space ||
            s13[0] != 'h' || s13[1] != 'e' || s13[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED assign(initializer_list)\n";
        }
    }
    {
        // get_allocator
        string s1 {"hello"};
        if (s1.get_allocator() != allocator<char>{})
        {
            ++fail_count;
            std::cout << "FAILED get_allocator()\n";
        }
        // operator[]
        string s2 {"he"};
        if (s2[0] != 'h' || s2[1] != 'e' || s2[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED operator[]\n";
        }
        const string s3 {"he"};
        if (s3[0] != 'h' || s3[1] != 'e' || s3[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED operator[] const\n";
        }
        // front
        string s4 {"hello"};
        if (s4.front() != 'h')
        {
            ++fail_count;
            std::cout << "FAILED front\n";
        }
        const string s5 {"hello"};
        if (s5.front() != 'h')
        {
            ++fail_count;
            std::cout << "FAILED front const\n";
        }
        // back
        string s6 {"hello"};
        if (s6.back() != 'o')
        {
            ++fail_count;
            std::cout << "FAILED back\n";
        }
        const string s7 {"hello"};
        if (s5.back() != 'o')
        {
            ++fail_count;
            std::cout << "FAILED back const\n";
        }
        // data
        string s8 {"he"};
        if (s8.data()[0] != 'h' || s8.data()[1] != 'e' || s8.data()[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED data\n";
        }
        // c_str
        string s9 {"he"};
        if (s9.c_str()[0] != 'h' || s9.c_str()[1] != 'e' ||
            s9.c_str()[2] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED c_str\n";
        }
    }
    {
        // begin
        string s1 {"hello"};
        if (*s1.begin() != 'h')
        {
            ++fail_count;
            std::cout << "FAILED begin\n";
        }
        const string s2 {"hello"};
        if (*s2.begin() != 'h')
        {
            ++fail_count;
            std::cout << "FAILED begin const\n";
        }
        string s3 {"hello"};
        if (*s3.cbegin() != 'h')
        {
            ++fail_count;
            std::cout << "FAILED cbegin\n";
        }
        // end
        string s4 {"hello"};
        if (*(s4.end() - 1) != 'o')
        {
            ++fail_count;
            std::cout << "FAILED end\n";
        }
        const string s5 {"hello"};
        if (*(s5.end() - 1) != 'o')
        {
            ++fail_count;
            std::cout << "FAILED end const\n";
        }
        string s6 {"hello"};
        if (*(s6.cend() - 1) != 'o')
        {
            ++fail_count;
            std::cout << "FAILED cend\n";
        }
        // rbegin
        string s7 {"hello"};
        if (*s7.rbegin() != 'o')
        {
            ++fail_count;
            std::cout << "FAILED rbegin\n";
        }
        const string s8 {"hello"};
        if (*s8.rbegin() != 'o')
        {
            ++fail_count;
            std::cout << "FAILED rbegin const\n";
        }
        string s9 {"hello"};
        if (*s9.crbegin() != 'o')
        {
            ++fail_count;
            std::cout << "FAILED crbegin\n";
        }
        // rend
        string s10 {"hello"};
        if (*(s10.rend() - 1) != 'h')
        {
            ++fail_count;
            std::cout << "FAILED rend\n";
        }
        const string s11 {"hello"};
        if (*(s11.rend() - 1) != 'h')
        {
            ++fail_count;
            std::cout << "FAILED rend const\n";
        }
        string s12 {"hello"};
        if (*(s12.crend() - 1) != 'h')
        {
            ++fail_count;
            std::cout << "FAILED crend\n";
        }
    }
    {
        // empty
        string s1 {"hello"};
        string s2 {""};
        string s3;
        if (s1.empty() || !s2.empty() || !s3.empty())
        {
            ++fail_count;
            std::cout << "FAILED empty\n";
        }
        // size
        string s4 {"hello"};
        string s5;
        if (s4.size() != 5 || s5.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED size\n";
        }
        // length
        string s6 {"hello"};
        string s7;
        if (s6.length() != 5 || s7.length() != 0)
        {
            ++fail_count;
            std::cout << "FAILED length\n";
        }
        // max_size
        string s8;
        if (s8.max_size() !=
            allocator_traits<allocator<char>>::max_size(allocator<char>{}))
        {
            ++fail_count;
            std::cout << "FAILED max_size\n";
        }
        // reserve
        string s9 {"he"};
        s9.reserve(2);
        if (s9.capacity() != default_space || s9[0] != 'h' || s9[1] != 'e' ||
            s9[2] != '\0' || s9.size() != 2)
        {
            ++fail_count;
            std::cout << "FAILED reserve decrease\n";
        }
        string s10 {"he"};
        s10.reserve(22);
        if (s10.capacity() != 22 || s10[0] != 'h' || s10[1] != 'e' ||
            s10[2] != '\0' || s10.size() != 2)
        {
            ++fail_count;
            std::cout << "FAILED reserve increase\n";
        }
        // capacity
        string s11 {"hello"};
        if (s11.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED capacity default\n";
        }
        string s12 {"Hello, world!"};
        if (s12.capacity() != 13)
        {
            ++fail_count;
            std::cout << "FAILED capacity long\n";
        }
        // shrink_to_fit
        string s13 {"he"};
        s13.shrink_to_fit();
        if (s13.capacity() != 2)
        {
            ++fail_count;
            std::cout << "FAILED shrink_to_fit from default\n";
        }
        string s14 {"Hello, world!"};
        s14.shrink_to_fit();
        if (s14.capacity() != 13)
        {
            ++fail_count;
            std::cout << "FAILED shrink_to_fit exact\n";
        }
        string s15 {"Hello, world!"};
        s15.reserve(50);
        s15.shrink_to_fit();
        if (s15.capacity() != 13)
        {
            ++fail_count;
            std::cout << "FAILED shrink_to_fit long\n";
        }
        // clear
        string s16 {"hello"};
        s16.clear();
        if (!s16.empty() || s16[0] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED clear not empty\n";
        }
        string s17 {""};
        s17.clear();
        if (!s17.empty() || s17[0] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED clear empty\n";
        }
    }
    {
        // insert
        string s1 {"heo"};
        s1.insert(2, 2, 'l');
        if (s1.size() != 5 || s1.capacity() != default_space || s1 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, count, value) capacity ok\n";
        }
        string s2 {"heo"};
        s2.insert(2, 6, 'l');
        if (s2.size() != 9 || s2.capacity() != 9 || s2 != "hellllllo")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, count, value) capacity increase\n";
        }
        string s3 {"heo"};
        s3.insert(0, 2, 'l');
        if (s3.size() != 5 || s3.capacity() != default_space || s3 != "llheo")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, count, value) beginning\n";
        }
        string s4 {"heo"};
        s4.insert(3, 2, 'l');
        if (s4.size() != 5 || s4.capacity() != default_space || s4 != "heoll")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, count, value) end\n";
        }
        string s5 {"heo"};
        s5.insert(4, 2, 'l');
        if (s5.size() != 3 || s5.capacity() != default_space || s5 != "heo")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, count, value) past end\n";
        }
        string s6 {"heo"};
        s6.insert(2, 0, 'l');
        if (s6.size() != 3 || s6.capacity() != default_space || s6 != "heo")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, count, value) no characters\n";
        }
        string s7 {"heo"};
        s7.insert(2, "llo", 2);
        if (s7.size() != 5 || s7.capacity() != default_space || s7 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, const_pointer, count) capacity ok\n";
        }
        string s8 {"heo"};
        s8.insert(2, "llllll", 6);
        if (s8.size() != 9 || s8.capacity() != 9 || s8 != "hellllllo")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, const_pointer, count) capacity increase\n";
        }
        string s9 {"heo"};
        s9.insert(0, "ll", 2);
        if (s9.size() != 5 || s9.capacity() != default_space || s9 != "llheo")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, const_pointer, count) beginning\n";
        }
        string s10 {"heo"};
        s10.insert(3, "ll", 2);
        if (s10.size() != 5 || s10.capacity() != default_space ||
            s10 != "heoll")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, const_pointer, count) end\n";
        }
        string s11 {"heo"};
        s11.insert(4, "ll", 2);
        if (s11.size() != 3 || s11.capacity() != default_space || s11 != "heo")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, const_pointer, count) past end\n";
        }
        string s12 {"heo"};
        s12.insert(2, "ll", 0);
        if (s12.size() != 3 || s12.capacity() != default_space || s12 != "heo")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, const_pointer, count) no characters\n";
        }
        string s13 {"heo"};
        s13.insert(2, "ll");
        if (s13.size() != 5 || s13.capacity() != default_space ||
            s13 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, const_pointer) characters\n";
        }
        string s14 {"heo"};
        s14.insert(2, "");
        if (s14.size() != 3 || s14.capacity() != default_space || s14 != "heo")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, const_pointer) no characters\n";
        }
        string s15 {"heo"};
        string s16 {"ll"};
        s15.insert(2, s16);
        if (s15.size() != 5 || s15.capacity() != default_space ||
            s15 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, string)\n";
        }
        string s17 {"heo"};
        string s18 {"ollo"};
        s17.insert(2, s18, 1, 2);
        if (s17.size() != 5 || s17.capacity() != default_space ||
            s17 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, string, pos, count) in range\n";
        }
        string s19 {"heo"};
        string s20 {"ollo"};
        s19.insert(2, s20, 1);
        if (s19.size() != 6 || s19.capacity() != default_space ||
            s19 != "helloo")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, string, pos, count) out of range\n";
        }
        string s21 {"heo"};
        string s22 {"ollo"};
        s21.insert(2, s22, 1);
        if (s21.size() != 6 || s21.capacity() != default_space ||
            s21 != "helloo")
        {
            ++fail_count;
            std::cout << "FAILED insert(index, string, pos, count) default range\n";
        }
        string s23 {"heo"};
        auto p1 = s23.insert(s23.begin() + 2, 'l');
        if (s23.size() != 4 || s23.capacity() != default_space ||
            s23 != "helo" || p1 != s23.begin() + 2 || *p1 != 'l')
        {
            ++fail_count;
            std::cout << "FAILED insert(const_iterator, value)\n";
        }
        string s24 {"heo"};
        auto p2 = s24.insert(s24.begin() + 2, 2, 'l');
        if (s24.size() != 5 || s24.capacity() != default_space ||
            s24 != "hello" || p2 != s24.begin() + 2 || *p2 != 'l')
        {
            ++fail_count;
            std::cout << "FAILED insert(const_iterator, count, value)\n";
        }
        string s25 {"heo"};
        string s26 {"ollo"};
        auto p3 = s25.insert(s25.begin() + 2, s26.begin() + 1, s26.end() - 1);
        if (s25.size() != 5 || s25.capacity() != default_space ||
            s25 != "hello" || p3 != s25.begin() + 2 || *p3 != 'l')
        {
            ++fail_count;
            std::cout << "FAILED insert(const_iterator, iter, iter)\n";
        }
        string s27 {"heo"};
        auto p4 = s27.insert(s27.begin() + 2, {'l', 'l'});
        if (s27.size() != 5 || s27.capacity() != default_space ||
            s27 != "hello" || p4 != s27.begin() + 2 || *p4 != 'l')
        {
            ++fail_count;
            std::cout << "FAILED insert(const_iterator, initializer_list)\n";
        }
    }
    {
        // erase
        string s1 {"hello"};
        s1.erase(2, 2);
        if (s1.size() != 3 || s1.capacity() != default_space || s1 != "heo")
        {
            ++fail_count;
            std::cout << "FAILED erase(index, count) valid\n";
        }
        string s2 {"hello"};
        s2.erase(2, 10);
        if (s2.size() != 2 || s2.capacity() != default_space || s2 != "he")
        {
            ++fail_count;
            std::cout << "FAILED erase(index, count) out of range\n";
        }
        string s3 {"hello"};
        s3.erase(2);
        if (s3.size() != 2 || s3.capacity() != default_space || s3 != "he")
        {
            ++fail_count;
            std::cout << "FAILED erase(index, count) default end\n";
        }
        string s4 {"hello"};
        s4.erase();
        if (!s4.empty() || s4[0] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED erase(index, count) default start and end\n";
        }
        string s5 {"hello"};
        auto p1 = s5.erase(s5.begin() + 1);
        if (s5.size() != 4 || s5.capacity() != default_space || s5 != "hllo" ||
            p1 != s5.begin() + 1 || *p1 != 'l')
        {
            ++fail_count;
            std::cout << "FAILED erase(const_iterator)\n";
        }
        string s6 {"hello"};
        auto p2 = s6.erase(s6.begin() + 2, s6.begin() + 4);
        if (s6.size() != 3 || s6.capacity() != default_space || s6 != "heo" ||
            p2 != s6.begin() + 2 || *p2 != 'o')
        {
            ++fail_count;
            std::cout << "FAILED erase(const_iterator, const_iterator)\n";
        }
        // push_back
        string s7 {"hell"};
        s7.push_back('o');
        if (s7.size() != 5 || s7.capacity() != default_space || s7 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED push_back(value) capacity ok\n";
        }
        string s8 {"helllll"};
        s8.push_back('o');
        if (s8.size() != 8 || s8.capacity() != 8 || s8 != "helllllo")
        {
            ++fail_count;
            std::cout << "FAILED push_back(value) capacity increase\n";
        }
        // pop_back
        string s9 {"helloo"};
        s9.pop_back();
        if (s9.size() != 5 || s9.capacity() != default_space || s9 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED pop_back()\n";
        }
    }
    {
        // append
        string s1 {"hello"};
        s1.append(2, 'o');
        if (s1.size() != 7 || s1.capacity() != default_space || s1 != "hellooo")
        {
            ++fail_count;
            std::cout << "FAILED append(count, value) capacity ok\n";
        }
        string s2 {"hello"};
        s2.append(3, 'o');
        if (s2.size() != 8 || s2.capacity() != 8 || s2 != "helloooo")
        {
            ++fail_count;
            std::cout << "FAILED append(count, value) capacity increase\n";
        }
        string s3 {"he"};
        string s4 {"llo"};
        s3.append(s4);
        if (s3.size() != 5 || s3.capacity() != default_space || s3 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED append(string)\n";
        }
        string s5 {"he"};
        string s6 {"olloo"};
        s5.append(s6, 1, 3);
        if (s5.size() != 5 || s5.capacity() != default_space || s5 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED append(string, pos, count) valid\n";
        }
        string s7 {"he"};
        string s8 {"olloo"};
        s7.append(s8, 1, 10);
        if (s7.size() != 6 || s7.capacity() != default_space || s7 != "helloo")
        {
            ++fail_count;
            std::cout << "FAILED append(string, pos, count) out of range\n";
        }
        string s9 {"he"};
        string s10 {"olloo"};
        s9.append(s10, 1);
        if (s9.size() != 6 || s9.capacity() != default_space || s9 != "helloo")
        {
            ++fail_count;
            std::cout << "FAILED append(string, pos, count) default range\n";
        }
        string s11 {"he"};
        s11.append("llooo", 3);
        if (s11.size() != 5 || s11.capacity() != default_space ||
            s11 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED append(const_pointer, count)\n";
        }
        string s12 {"he"};
        s12.append("llo");
        if (s12.size() != 5 || s12.capacity() != default_space ||
            s12 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED append(const_pointer)\n";
        }
        string s13 {"he"};
        string s14 {"olloo"};
        s13.append(s14.begin() + 1, s14.end() - 1);
        if (s13.size() != 5 || s13.capacity() != default_space ||
            s13 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED append(iter, iter)\n";
        }
        string s15 {"he"};
        s15.append({'l', 'l', 'o'});
        if (s15.size() != 5 || s15.capacity() != default_space ||
            s15 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED append(initializer_list)\n";
        }
    }
    {
        // operator+=
        string s1 {"he"};
        string s2 {"llo"};
        s1 += s2;
        if (s1.size() != 5 || s1.capacity() != default_space || s1 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+=(string)\n";
        }
        string s3 {"hello"};
        s3 += 'o';
        if (s3.size() != 6 || s3.capacity() != default_space || s3 != "helloo")
        {
            ++fail_count;
            std::cout << "FAILED operator+=(char)\n";
        }
        string s4 {"he"};
        s4 += "llo";
        if (s4.size() != 5 || s4.capacity() != default_space || s4 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+=(const_pointer)\n";
        }
        string s5 {"he"};
        s5 += {'l', 'l', 'o'};
        if (s5.size() != 5 || s5.capacity() != default_space || s5 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+=(initializer_list)\n";
        }
    }
    {
        // compare
        string s1 {"abc"};
        string s2 {"abc"};
        string s3 {"abcd"};
        string s4 {"def"};
        if (s1.compare(s2) != 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(string) equal\n";
        }
        if (s1.compare(s3) >= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(string) less than\n";
        }
        if (s4.compare(s1) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(string) greater than\n";
        }
        string s5 {"xabcy"};
        string s6 {"abc"};
        string s7 {"abcd"};
        string s8 {"def"};
        if (s5.compare(1, 3, s6) != 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, string) equal\n";
        }
        if (s5.compare(1, 3, s7) >= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, string) less than\n";
        }
        if (s5.compare(0, 3, s8) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, string) greater than\n";
        }
        string s9 {"xabcy"};
        string s10 {"xabcy"};
        string s11 {"xabcdy"};
        string s12 {"xdefy"};
        if (s9.compare(1, 3, s10, 1, 3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, string, pos, count) equal\n";
        }
        if (s9.compare(1, 3, s11, 1, 4) >= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, string, pos, count) less than\n";
        }
        if (s9.compare(0, 3, s12, 1, 3) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, string, pos, count) greater than\n";
        }
        if (s9.compare(0, 3, s12, 1) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, string, pos, count) greater than default range\n";
        }
        string s13 ("bcd");
        if (s13.compare("bcd") != 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(const_pointer) equal\n";
        }
        if (s13.compare("bcde") >= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(const_pointer) less than\n";
        }
        if (s13.compare("abc") <= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(const_pointer) greater than\n";
        }
        string s14 ("xbcdy");
        if (s14.compare(1, 3, "bcd") != 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, const_pointer) equal\n";
        }
        if (s14.compare(1, 3, "bcde") >= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, const_pointer) less than\n";
        }
        if (s14.compare(1, 3, "abc") <= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, const_pointer) greater than\n";
        }
        string s15 ("xbcdy");
        if (s15.compare(1, 3, "bcdyy", 3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, const_pointer, count) equal\n";
        }
        if (s15.compare(1, 3, "bcdeyy", 4) >= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, const_pointer, count) less than\n";
        }
        if (s15.compare(1, 3, "abcyy", 3) <= 0)
        {
            ++fail_count;
            std::cout << "FAILED compare(pos, count, const_pointer, count) greater than\n";
        }
    }
    {
        // replace
        string s1 {"hello"};
        string s2 {"mno"};
        s1.replace(2, 2, s2);
        if (s1.size() != 6 || s1.capacity() != default_space || s1 != "hemnoo")
        {
            ++fail_count;
            std::cout << "FAILED replace(pos, count, string)\n";
        }
        string s3 {"hello"};
        string s4 {"mno"};
        s3.replace(s3.begin() + 2, s3.begin() + 4, s4);
        if (s3.size() != 6 || s3.capacity() != default_space || s3 != "hemnoo")
        {
            ++fail_count;
            std::cout << "FAILED replace(iter, iter, string)\n";
        }
        string s5 {"hello"};
        string s6 {"mnopq"};
        s5.replace(2, 2, s6, 1, 3);
        if (s5.size() != 6 || s5.capacity() != default_space || s5 != "henopo")
        {
            ++fail_count;
            std::cout << "FAILED replace(pos, count, string, pos, count)\n";
        }
        string s7 {"hello"};
        string s8 {"mnopq"};
        s7.replace(s7.begin() + 2, s7.begin() + 4, s8.begin() + 1,
            s8.begin() + 4);
        if (s7.size() != 6 || s7.capacity() != default_space || s7 != "henopo")
        {
            ++fail_count;
            std::cout << "FAILED replace(iter, iter, iter, iter)\n";
        }
        string s9 {"hello"};
        s9.replace(2, 2, "mnopq", 3);
        if (s9.size() != 6 || s9.capacity() != default_space || s9 != "hemnoo")
        {
            ++fail_count;
            std::cout << "FAILED replace(pos, count, char*, count)\n";
        }
        string s10 {"hello"};
        s10.replace(s10.begin(), s10.begin() + 2, "mnopq", 3);
        if (s10.size() != 6 || s10.capacity() != default_space ||
            s10 != "mnollo")
        {
            ++fail_count;
            std::cout << "FAILED replace(iter, iter, char*, count)\n";
        }
        string s11 {"hello"};
        s11.replace(2, 2, "mno");
        if (s11.size() != 6 || s11.capacity() != default_space ||
            s11 != "hemnoo")
        {
            ++fail_count;
            std::cout << "FAILED replace(pos, count, char*)\n";
        }
        string s12 {"hello"};
        s12.replace(s12.end() - 2, s12.end(), "mno");
        if (s12.size() != 6 || s12.capacity() != default_space ||
            s12 != "helmno")
        {
            ++fail_count;
            std::cout << "FAILED replace(iter, iter, char*)\n";
        }
        string s13 {"hello"};
        s13.replace(3, 2, 3, 'x');
        if (s13.size() != 6 || s13.capacity() != default_space ||
            s13 != "helxxx")
        {
            ++fail_count;
            std::cout << "FAILED replace(pos, count, count, char)\n";
        }
        string s14 {"hello"};
        s14.replace(s14.begin(), s14.begin() + 2, 3, 'x');
        if (s14.size() != 6 || s14.capacity() != default_space ||
            s14 != "xxxllo")
        {
            ++fail_count;
            std::cout << "FAILED replace(iter, iter, count, char)\n";
        }
        string s15 {"hello"};
        s15.replace(s15.begin() + 2, s15.begin() + 4, {'m', 'n', 'o'});
        if (s15.size() != 6 || s15.capacity() != default_space ||
            s15 != "hemnoo")
        {
            ++fail_count;
            std::cout << "FAILED replace(iter, iter, initializer_list)\n";
        }
    }
    // substr
    {
        string s1 {"hello"};
        string s2 = s1.substr(1, 3);
        if (s2.size() != 3 || s2.capacity() != default_space || s2 != "ell")
        {
            ++fail_count;
            std::cout << "FAILED substr(pos, count)\n";
        }
        string s3 {"hello"};
        string s4 = s3.substr(1);
        if (s4.size() != 4 || s4.capacity() != default_space || s4 != "ello")
        {
            ++fail_count;
            std::cout << "FAILED substr(pos)\n";
        }
        string s5 {"hello"};
        string s6 = s5.substr();
        if (s6.size() != 5 || s6.capacity() != default_space || s6 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED substr()\n";
        }
    }
    // copy
    {
        string s1 {"hello"};
        char* c1 = new char[6];
        c1[0] = 'a'; c1[1] = 'a'; c1[2] = 'a'; c1[3] = 'a'; c1[4] = 'a';
        s1.copy(c1, 3, 1);
        if (c1[0] != 'e' || c1[1] != 'l' || c1[2] != 'l' || c1[3] != 'a' ||
            c1[4] != 'a')
        {
            ++fail_count;
            std::cout << "FAILED copy(char*, count, pos)\n";
        }
        delete[] c1;
        string s2 {"hello"};
        char* c2 = new char[5];
        c2[0] = 'a'; c2[1] = 'a'; c2[2] = 'a'; c2[3] = 'a'; c2[4] = 'a';
        s2.copy(c2, 10, 1);
        if (c2[0] != 'e' || c2[1] != 'l' || c2[2] != 'l' || c2[3] != 'o' ||
            c2[4] != 'a')
        {
            ++fail_count;
            std::cout << "FAILED copy(char*, count, pos) excessive count\n";
        }
        delete[] c2;
        string s3 {"hello"};
        char* c3 = new char[5];
        c3[0] = 'a'; c3[1] = 'a'; c3[2] = 'a'; c3[3] = 'a'; c3[4] = 'a';
        s3.copy(c3, 4);
        if (c3[0] != 'h' || c3[1] != 'e' || c3[2] != 'l' || c3[3] != 'l' ||
            c3[4] != 'a')
        {
            ++fail_count;
            std::cout << "FAILED copy(char*, count)\n";
        }
        delete[] c3;
        string s4 {"hello"};
        char* c4 = new char[6];
        c4[0] = 'a'; c4[1] = 'a'; c4[2] = 'a'; c4[3] = 'a'; c4[4] = 'a';
        c4[5] = 'a';
        s4.copy(c4, 10);
        if (c4[0] != 'h' || c4[1] != 'e' || c4[2] != 'l' || c4[3] != 'l' ||
            c4[4] != 'o' || c4[5] != 'a')
        {
            ++fail_count;
            std::cout << "FAILED copy(char*, count) excessive count\n";
        }
        delete[] c4;
    }
    // resize
    {
        string s1 {"hello"};
        s1.resize(7);
        if (s1.size() != 7 || s1.capacity() != default_space ||
            s1[0] != 'h' || s1[1] != 'e' || s1[2] != 'l' || s1[3] != 'l' ||
            s1[4] != 'o' || s1[5] != '\0' || s1[6] != '\0')
        {
            ++fail_count;
            std::cout << "FAILED resize(size) increase\n";
        }
        string s2 {"hello"};
        s2.resize(2);
        if (s2.size() != 2 || s2.capacity() != default_space ||
            s2 != "he")
        {
            ++fail_count;
            std::cout << "FAILED resize(size) decrease\n";
        }
        string s3 {"hello"};
        s3.resize(7, 'a');
        if (s3.size() != 7 || s3.capacity() != default_space ||
            s3 != "helloaa")
        {
            ++fail_count;
            std::cout << "FAILED resize(size, char) increase\n";
        }
        string s4 {"hello"};
        s4.resize(2, 'a');
        if (s4.size() != 2 || s4.capacity() != default_space ||
            s4 != "he")
        {
            ++fail_count;
            std::cout << "FAILED resize(size, char) decrease\n";
        }
    }
    // swap
    {
        // Member
        string s1 {"hello"};
        string s2 {"farewell"};
        s1.swap(s2);
        if (s1.size() != 8 || s1.capacity() != 8 || s1 != "farewell" ||
            s2.size() != 5 || s2.capacity() != default_space || s2 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED swap(string)\n";
        }
        // Non-member
        string s3 {"hello"};
        string s4 {"farewell"};
        swap(s3, s4);
        if (s3.size() != 8 || s3.capacity() != 8 || s3 != "farewell" ||
            s4.size() != 5 || s4.capacity() != default_space || s4 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED swap(string, string)\n";
        }
    }
    // find
    {
        string s1 {""};
        if (s1.find("a") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find(string) empty\n";
        }
        string s2 {""};
        if (s2.find("") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find(empty) empty\n";
        }
        string s3 {"a"};
        if (s3.find("") != 0)
        {
            ++fail_count;
            std::cout << "FAILED find(empty)\n";
        }
        string s4 {"hello"};
        if (s4.find(string{"el"}) != 1)
        {
            ++fail_count;
            std::cout << "FAILED find(string) present\n";
        }
        string s5 {"hello"};
        if (s5.find(string{"ab"}) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find(string) not present\n";
        }
        string s6 {"hello"};
        if (s6.find(string{"lo"}, 2) != 3)
        {
            ++fail_count;
            std::cout << "FAILED find(string, pos) present\n";
        }
        string s7 {"hello"};
        if (s7.find(string{"el"}, 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find(string, pos) not present\n";
        }
        string s8 {"hello"};
        if (s8.find("looooo", 2, 2) != 3)
        {
            ++fail_count;
            std::cout << "FAILED find(char*, pos, count) present\n";
        }
        string s9 {"hello"};
        if (s9.find("lloooo", 1, 4) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find(char*, pos, count) not present\n";
        }
        string s10 {"hello"};
        if (s10.find("lo", 2) != 3)
        {
            ++fail_count;
            std::cout << "FAILED find(char*, pos) present\n";
        }
        string s11 {"hello"};
        if (s11.find("el", 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find(char*, pos) not present\n";
        }
        string s12 {"hello"};
        if (s12.find("lo") != 3)
        {
            ++fail_count;
            std::cout << "FAILED find(char*) present\n";
        }
        string s13 {"hello"};
        if (s13.find("elo") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find(char*) not present\n";
        }
        string s14 {"hello"};
        if (s14.find('e') != 1)
        {
            ++fail_count;
            std::cout << "FAILED find(char) present\n";
        }
        string s15 {"hello"};
        if (s15.find('a') != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find(char) not present\n";
        }
        string s16 {"hello"};
        if (s16.find('l', 1) != 2)
        {
            ++fail_count;
            std::cout << "FAILED find(char, pos) present\n";
        }
        string s17 {"hello"};
        if (s17.find('e', 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find(char, pos) not present\n";
        }
    }
    // rfind
    {
        string s1 {""};
        if (s1.rfind("a") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED rfind(string) empty\n";
        }
        string s2 {""};
        if (s2.rfind("") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED rfind(empty) empty\n";
        }
        string s3 {"ab"};
        if (s3.rfind("") != 1)
        {
            ++fail_count;
            std::cout << "FAILED rfind(empty)\n";
        }
        string s4 {"hello"};
        if (s4.rfind(string{"el"}) != 1)
        {
            ++fail_count;
            std::cout << "FAILED rfind(string) present\n";
        }
        string s5 {"hello"};
        if (s5.rfind(string{"ab"}) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED rfind(string) not present\n";
        }
        string s6 {"hello"};
        if (s6.rfind(string{"el"}, 2) != 1)
        {
            ++fail_count;
            std::cout << "FAILED rfind(string, pos) present\n";
        }
        string s7 {"hello"};
        if (s7.rfind(string{"lo"}, 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED rfind(string, pos) not present\n";
        }
        string s8 {"hello"};
        if (s8.rfind("elllll", 2, 2) != 1)
        {
            ++fail_count;
            std::cout << "FAILED rfind(char*, pos, count) present\n";
        }
        string s9 {"hello"};
        if (s9.rfind("elllll", 3, 4) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED rfind(char*, pos, count) not present\n";
        }
        string s10 {"hello"};
        if (s10.rfind("el", 1) != 1)
        {
            ++fail_count;
            std::cout << "FAILED rfind(char*, pos) present\n";
        }
        string s11 {"hello"};
        if (s11.rfind("lo", 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED rfind(char*, pos) not present\n";
        }
        string s12 {"hello"};
        if (s12.rfind("lo") != 3)
        {
            ++fail_count;
            std::cout << "FAILED rfind(char*) present\n";
        }
        string s13 {"hello"};
        if (s13.rfind("elo") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED rfind(char*) not present\n";
        }
        string s14 {"hello"};
        if (s14.rfind('l') != 3)
        {
            ++fail_count;
            std::cout << "FAILED rfind(char) present\n";
        }
        string s15 {"hello"};
        if (s15.rfind('a') != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED rfind(char) not present\n";
        }
        string s16 {"hello"};
        if (s16.rfind('l', 2) != 2)
        {
            ++fail_count;
            std::cout << "FAILED rfind(char, pos) present\n";
        }
        string s17 {"hello"};
        if (s17.rfind('l', 1) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED rfind(char, pos) not present\n";
        }
    }
    // find_first_of
    {
        string s1 {""};
        if (s1.find_first_of("a") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(string) empty\n";
        }
        string s2 {""};
        if (s2.find_first_of("") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(empty) empty\n";
        }
        string s3 {"ab"};
        if (s3.find_first_of("") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(empty)\n";
        }
        string s4 {"hello"};
        if (s4.find_first_of(string{"leo"}) != 1)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(string) present\n";
        }
        string s5 {"hello"};
        if (s5.find_first_of(string{"abc"}) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(string) not present\n";
        }
        string s6 {"hello"};
        if (s6.find_first_of(string{"leo"}, 2) != 2)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(string, pos) present\n";
        }
        string s7 {"hello"};
        if (s7.find_first_of(string{"he"}, 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(string, pos) not present\n";
        }
        string s8 {"hello"};
        if (s8.find_first_of("loe", 1, 2) != 2)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(char*, pos, count) present\n";
        }
        string s9 {"hello"};
        if (s9.find_first_of("eho", 2, 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(char*, pos, count) not present\n";
        }
        string s10 {"hello"};
        if (s10.find_first_of("ol") != 2)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(char*) present\n";
        }
        string s11 {"hello"};
        if (s11.find_first_of("ab") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(char*) not present\n";
        }
        string s12 {"hello"};
        if (s12.find_first_of("ol", 1) != 2)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(char*, pos) present\n";
        }
        string s13 {"hello"};
        if (s13.find_first_of("eh", 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(char*, pos) not present\n";
        }
        string s14 {"hello"};
        if (s14.find_first_of('e') != 1)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(char) present\n";
        }
        string s15 {"hello"};
        if (s15.find_first_of('a') != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(char) not present\n";
        }
        string s16 {"hello"};
        if (s16.find_first_of('l', 2) != 2)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(char, pos) present\n";
        }
        string s17 {"hello"};
        if (s17.find_first_of('e', 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_of(char, pos) not present\n";
        }
    }
    // find_first_not_of
    {
        string s1 {""};
        if (s1.find_first_not_of("a") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(string) empty\n";
        }
        string s2 {""};
        if (s2.find_first_not_of("") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(empty) empty\n";
        }
        string s3 {"ab"};
        if (s3.find_first_not_of("") != 0)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(empty)\n";
        }
        string s4 {"hello"};
        if (s4.find_first_not_of(string{"hab"}) != 1)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_f(string) present\n";
        }
        string s5 {"hello"};
        if (s5.find_first_not_of(string{"ehlo"}) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(string) not present\n";
        }
        string s6 {"hello"};
        if (s6.find_first_not_of(string{"el"}, 2) != 4)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(string, pos) present\n";
        }
        string s7 {"hello"};
        if (s7.find_first_not_of(string{"ol"}, 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(string, pos) not present\n";
        }
        string s8 {"hello"};
        if (s8.find_first_not_of("eol", 1, 2) != 2)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(char*, pos, count) present\n";
        }
        string s9 {"hello"};
        if (s9.find_first_not_of("loab", 2, 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(char*, pos, count) not present\n";
        }
        string s10 {"hello"};
        if (s10.find_first_not_of("heo") != 2)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(char*) present\n";
        }
        string s11 {"hello"};
        if (s11.find_first_not_of("ehol") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(char*) not present\n";
        }
        string s12 {"hello"};
        if (s12.find_first_not_of("eo", 1) != 2)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(char*, pos) present\n";
        }
        string s13 {"hello"};
        if (s13.find_first_not_of("ol", 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(char*, pos) not present\n";
        }
        string s14 {"hello"};
        if (s14.find_first_not_of('e') != 0)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(char) present\n";
        }
        string s16 {"hello"};
        if (s16.find_first_not_of('l', 2) != 4)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(char, pos) present\n";
        }
        string s17 {"hello"};
        if (s17.find_first_not_of('o', 4) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_first_not_of(char, pos) not present\n";
        }
    }
    // find_last_of
    {
        string s1 {""};
        if (s1.find_last_of("a") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(string) empty\n";
        }
        string s2 {""};
        if (s2.find_last_of("") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(empty) empty\n";
        }
        string s3 {"ab"};
        if (s3.find_last_of("") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(empty)\n";
        }
        string s4 {"hello"};
        if (s4.find_last_of(string{"leo"}) != 4)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(string) present\n";
        }
        string s5 {"hello"};
        if (s5.find_last_of(string{"abc"}) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(string) not present\n";
        }
        string s6 {"hello"};
        if (s6.find_last_of(string{"leo"}, 2) != 2)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(string, pos) present\n";
        }
        string s7 {"hello"};
        if (s7.find_last_of(string{"lo"}, 1) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(string, pos) not present\n";
        }
        string s8 {"hello"};
        if (s8.find_last_of("hel", 2, 2) != 1)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(char*, pos, count) present\n";
        }
        string s9 {"hello"};
        if (s9.find_last_of("loh", 1, 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(char*, pos, count) not present\n";
        }
        string s10 {"hello"};
        if (s10.find_last_of("hl") != 3)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(char*) present\n";
        }
        string s11 {"hello"};
        if (s11.find_last_of("ab") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(char*) not present\n";
        }
        string s12 {"hello"};
        if (s12.find_last_of("eh", 3) != 1)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(char*, pos) present\n";
        }
        string s13 {"hello"};
        if (s13.find_last_of("lo", 1) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(char*, pos) not present\n";
        }
        string s14 {"hello"};
        if (s14.find_last_of('l') != 3)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(char) present\n";
        }
        string s15 {"hello"};
        if (s15.find_last_of('a') != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(char) not present\n";
        }
        string s16 {"hello"};
        if (s16.find_last_of('e', 1) != 1)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(char, pos) present\n";
        }
        string s17 {"hello"};
        if (s17.find_last_of('o', 3) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_of(char, pos) not present\n";
        }
    }
    // find_last_not_of
    {
        string s1 {""};
        if (s1.find_last_not_of("a") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(string) empty\n";
        }
        string s2 {""};
        if (s2.find_last_of("") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(empty) empty\n";
        }
        string s3 {"ab"};
        if (s3.find_last_not_of("") != 1)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(empty)\n";
        }
        string s4 {"hello"};
        if (s4.find_last_not_of(string{"oab"}) != 3)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_f(string) present\n";
        }
        string s5 {"hello"};
        if (s5.find_last_not_of(string{"ehlo"}) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(string) not present\n";
        }
        string s6 {"hello"};
        if (s6.find_last_not_of(string{"el"}, 2) != 0)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(string, pos) present\n";
        }
        string s7 {"hello"};
        if (s7.find_last_not_of(string{"ehl"}, 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(string, pos) not present\n";
        }
        string s8 {"hello"};
        if (s8.find_last_not_of("lhe", 3, 2) != 1)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(char*, pos, count) present\n";
        }
        string s9 {"hello"};
        if (s9.find_last_not_of("heab", 1, 2) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(char*, pos, count) not present\n";
        }
        string s10 {"hello"};
        if (s10.find_last_not_of("heo") != 3)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(char*) present\n";
        }
        string s11 {"hello"};
        if (s11.find_last_not_of("ehol") != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(char*) not present\n";
        }
        string s12 {"hello"};
        if (s12.find_last_not_of("eo", 3) != 3)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(char*, pos) present\n";
        }
        string s13 {"hello"};
        if (s13.find_last_not_of("eh", 1) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(char*, pos) not present\n";
        }
        string s14 {"hello"};
        if (s14.find_last_not_of('e') != 4)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(char) present\n";
        }
        string s16 {"hello"};
        if (s16.find_last_not_of('l', 3) != 1)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(char, pos) present\n";
        }
        string s17 {"hello"};
        if (s17.find_last_not_of('h', 0) != string::npos)
        {
            ++fail_count;
            std::cout << "FAILED find_last_not_of(char, pos) not present\n";
        }
    }
    // npos
    if (string::npos != static_cast<decltype(string::npos)>(-1))
    {
        ++fail_count;
        std::cout << "FAILED npos\n";
    }
    // Concatenation
    {
        string s1 {"he"};
        string s2 {"llo"};
        string s3 {s1 + s2};
        if (s3.size() != 5 || s3.capacity() != default_space || s3 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+(const string&, const string&)\n";
        }
        string s4 {"llo"};
        string s5 {"he" + s4};
        if (s5.size() != 5 || s5.capacity() != default_space || s5 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+(const char*, const string&)\n";
        }
        string s6 {"ello"};
        string s7 {'h' + s6};
        if (s7.size() != 5 || s7.capacity() != default_space || s7 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+(char, const string&)\n";
        }
        string s8 {"he"};
        string s9 {s8 + "llo"};
        if (s9.size() != 5 || s9.capacity() != default_space || s9 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+(const string&, const char*)\n";
        }
        string s10 {"hell"};
        string s11 {s10 + 'o'};
        if (s11.size() != 5 || s11.capacity() != default_space ||
            s11 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+(const string&, char)\n";
        }
        string s12 {"llo"};
        string s13 {move(string{"he"}) + s12};
        if (s13.size() != 5 || s13.capacity() != default_space ||
            s13 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+(string&&, const string&)\n";
        }
        string s14 {"he"};
        string s15 {s14 + move(string{"llo"})};
        if (s13.size() != 5 || s13.capacity() != default_space ||
            s13 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+(const string&, string&&)\n";
        }
        string s16 {move(string{"he"}) + move(string{"llo"})};
        if (s16.size() != 5 || s16.capacity() != default_space ||
            s16 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+(string&&, string&&)\n";
        }
        string s17 {"he" + move(string{"llo"})};
        if (s17.size() != 5 || s17.capacity() != default_space ||
            s17 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+(const char*, string&&)\n";
        }
        string s18 {'h' + move(string{"ello"})};
        if (s18.size() != 5 || s18.capacity() != default_space ||
            s18 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+(char, string&&)\n";
        }
        string s19 {move(string{"he"}) + "llo"};
        if (s19.size() != 5 || s19.capacity() != default_space ||
            s19 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+(string&&, const char*)\n";
        }
        string s20 {move(string{"hell"}) + 'o'};
        if (s20.size() != 5 || s20.capacity() != default_space ||
            s20 != "hello")
        {
            ++fail_count;
            std::cout << "FAILED operator+(string&&, char)\n";
        }
    }
    // Comparisons
    {
        // operator==
        string s1 {"abc"};
        string s2 {"abc"};
        string s3 {"abcd"};
        string s4 {"def"};
        string s5 {""};
        string s6 {""};
        if (!(s1 == s2))
        {
            ++fail_count;
            std::cout << "FAILED operator== equal\n";
        }
        if (s1 == s3)
        {
            ++fail_count;
            std::cout << "FAILED operator== shorter\n";
        }
        if (s1 == s4)
        {
            ++fail_count;
            std::cout << "FAILED operator== earlier\n";
        }
        if (s1 == s5)
        {
            ++fail_count;
            std::cout << "FAILED operator== empty\n";
        }
        if (!(s5 == s6))
        {
            ++fail_count;
            std::cout << "FAILED operator== both empty\n";
        }
        if (!(s1 == "abc"))
        {
            ++fail_count;
            std::cout << "FAILED operator== equal rhs char*\n";
        }
        if (!("abc" == s1))
        {
            ++fail_count;
            std::cout << "FAILED operator== equal lhs char*\n";
        }
        if (s1 == "abcd")
        {
            ++fail_count;
            std::cout << "FAILED operator== not equal rhs char*\n";
        }
        if ("abcd" == s1)
        {
            ++fail_count;
            std::cout << "FAILED operator== not equal lhs char*\n";
        }
        // operator!=
        string s7 {"abc"};
        string s8 {"abc"};
        string s9 {"abcd"};
        string s10 {"def"};
        string s11 {""};
        string s12 {""};
        if (s7 != s8)
        {
            ++fail_count;
            std::cout << "FAILED operator!= equal\n";
        }
        if (!(s7 != s9))
        {
            ++fail_count;
            std::cout << "FAILED operator!= shorter\n";
        }
        if (!(s7 != s10))
        {
            ++fail_count;
            std::cout << "FAILED operator!= earlier\n";
        }
        if (!(s7 != s11))
        {
            ++fail_count;
            std::cout << "FAILED operator!= empty\n";
        }
        if (s11 != s12)
        {
            ++fail_count;
            std::cout << "FAILED operator!= both empty\n";
        }
        if (s7 != "abc")
        {
            ++fail_count;
            std::cout << "FAILED operator!= equal rhs char*\n";
        }
        if ("abc" != s7)
        {
            ++fail_count;
            std::cout << "FAILED operator!= equal lhs char*\n";
        }
        if (!(s7 != "abcd"))
        {
            ++fail_count;
            std::cout << "FAILED operator!= not equal rhs char*\n";
        }
        if (!("abcd" != s7))
        {
            ++fail_count;
            std::cout << "FAILED operator!= not equal lhs char*\n";
        }
        // operator<
        string s13 {"abc"};
        string s14 {"abc"};
        string s15 {"abcd"};
        string s16 {"def"};
        string s17 {""};
        string s18 {""};
        if (s13 < s14)
        {
            ++fail_count;
            std::cout << "FAILED operator< equal\n";
        }
        if (!(s13 < s15))
        {
            ++fail_count;
            std::cout << "FAILED operator< shorter\n";
        }
        if (!(s13 < s16))
        {
            ++fail_count;
            std::cout << "FAILED operator< earlier\n";
        }
        if (s13 < s17)
        {
            ++fail_count;
            std::cout << "FAILED operator< empty\n";
        }
        if (s17 < s18)
        {
            ++fail_count;
            std::cout << "FAILED operator< both empty\n";
        }
        if (s13 < "abc")
        {
            ++fail_count;
            std::cout << "FAILED operator< equal rhs char*\n";
        }
        if ("abc" < s13)
        {
            ++fail_count;
            std::cout << "FAILED operator< equal lhs char*\n";
        }
        if (!(s13 < "abcd"))
        {
            ++fail_count;
            std::cout << "FAILED operator< not equal rhs char*\n";
        }
        if ("abcd" < s13)
        {
            ++fail_count;
            std::cout << "FAILED operator< not equal lhs char*\n";
        }
        // operator>
        string s19 {"abc"};
        string s20 {"abc"};
        string s21 {"abcd"};
        string s22 {"def"};
        string s23 {""};
        string s24 {""};
        if (s19 > s20)
        {
            ++fail_count;
            std::cout << "FAILED operator> equal\n";
        }
        if (s19 > s21)
        {
            ++fail_count;
            std::cout << "FAILED operator> shorter\n";
        }
        if (s19 > s22)
        {
            ++fail_count;
            std::cout << "FAILED operator> earlier\n";
        }
        if (!(s19 > s23))
        {
            ++fail_count;
            std::cout << "FAILED operator> empty\n";
        }
        if (s23 > s24)
        {
            ++fail_count;
            std::cout << "FAILED operator> both empty\n";
        }
        if (s19 > "abc")
        {
            ++fail_count;
            std::cout << "FAILED operator> equal rhs char*\n";
        }
        if ("abc" > s19)
        {
            ++fail_count;
            std::cout << "FAILED operator> equal lhs char*\n";
        }
        if (s19 > "abcd")
        {
            ++fail_count;
            std::cout << "FAILED operator> not equal rhs char*\n";
        }
        if (!("abcd" > s19))
        {
            ++fail_count;
            std::cout << "FAILED operator> not equal lhs char*\n";
        }
        // operator<=
        string s25 {"abc"};
        string s26 {"abc"};
        string s27 {"abcd"};
        string s28 {"def"};
        string s29 {""};
        string s30 {""};
        if (!(s25 <= s26))
        {
            ++fail_count;
            std::cout << "FAILED operator<= equal\n";
        }
        if (!(s25 <= s27))
        {
            ++fail_count;
            std::cout << "FAILED operator<= shorter\n";
        }
        if (!(s25 <= s28))
        {
            ++fail_count;
            std::cout << "FAILED operator<= earlier\n";
        }
        if (s25 <= s29)
        {
            ++fail_count;
            std::cout << "FAILED operator<= empty\n";
        }
        if (!(s29 <= s30))
        {
            ++fail_count;
            std::cout << "FAILED operator<= both empty\n";
        }
        if (!(s25 <= "abc"))
        {
            ++fail_count;
            std::cout << "FAILED operator<= equal rhs char*\n";
        }
        if (!("abc" <= s25))
        {
            ++fail_count;
            std::cout << "FAILED operator<= equal lhs char*\n";
        }
        if (!(s25 <= "abcd"))
        {
            ++fail_count;
            std::cout << "FAILED operator<= not equal rhs char*\n";
        }
        if ("abcd" <= s25)
        {
            ++fail_count;
            std::cout << "FAILED operator<= not equal lhs char*\n";
        }
        // operator<=
        string s31 {"abc"};
        string s32 {"abc"};
        string s33 {"abcd"};
        string s34 {"def"};
        string s35 {""};
        string s36 {""};
        if (!(s31 >= s32))
        {
            ++fail_count;
            std::cout << "FAILED operator>= equal\n";
        }
        if (s31 >= s33)
        {
            ++fail_count;
            std::cout << "FAILED operator>= shorter\n";
        }
        if (s31 >= s34)
        {
            ++fail_count;
            std::cout << "FAILED operator>= earlier\n";
        }
        if (!(s31 >= s35))
        {
            ++fail_count;
            std::cout << "FAILED operator>= empty\n";
        }
        if (!(s31 >= s36))
        {
            ++fail_count;
            std::cout << "FAILED operator>= both empty\n";
        }
        if (!(s31 >= "abc"))
        {
            ++fail_count;
            std::cout << "FAILED operator>= equal rhs char*\n";
        }
        if (!("abc" >= s31))
        {
            ++fail_count;
            std::cout << "FAILED operator>= equal lhs char*\n";
        }
        if (s31 >= "abcd")
        {
            ++fail_count;
            std::cout << "FAILED operator>= not equal rhs char*\n";
        }
        if (!("abcd" >= s31))
        {
            ++fail_count;
            std::cout << "FAILED operator>= not equal lhs char*\n";
        }
    }
    // Conversion to signed integer
    {
        string s1 {"123"};
        int i1 = stoi(s1);
        if (i1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stoi(string) positive no spaces\n";
        }
        string s2 {"-123"};
        int i2 = stoi(s2);
        if (i2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED stoi(string) negative no spaces\n";
        }
        string s3 {"   123 "};
        int i3 = stoi(s3);
        if (i3 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stoi(string) positive with spaces\n";
        }
        string s4 {"   -123 "};
        int i4 = stoi(s4);
        if (i4 != -123)
        {
            ++fail_count;
            std::cout << "FAILED stoi(string) negative with spaces\n";
        }
        string s5 {"0xA6"};
        int i5 = stoi(s5, nullptr, 0);
        if (i5 != 166)
        {
            ++fail_count;
            std::cout << "FAILED stoi(string, nullptr, 0) assumed hex\n";
        }
        string s6 {"076"};
        int i6 = stoi(s6, nullptr, 0);
        if (i6 != 62)
        {
            ++fail_count;
            std::cout << "FAILED stoi(string, nullptr, 0) assumed octal\n";
        }
        string s7 {"0xA6"};
        int i7 = stoi(s7, nullptr, 16);
        if (i7 != 166)
        {
            ++fail_count;
            std::cout << "FAILED stoi(string, nullptr, 16) explicit hex\n";
        }
        string s8 {"076"};
        int i8 = stoi(s8, nullptr, 8);
        if (i8 != 62)
        {
            ++fail_count;
            std::cout << "FAILED stoi(string, nullptr, 8) explicit octal\n";
        }
        string s9 {"07697"};
        size_t st1;
        int i9 = stoi(s9, &st1, 8);
        if (i9 != 62 || st1 != 3)
        {
            ++fail_count;
            std::cout << "FAILED stoi(string, size_t*, 8) octal with bad digit\n";
        }
        string s10 {"123"};
        size_t st2;
        int i10 = stoi(s10, &st2);
        if (i10 != 123 || st2 != 3)
        {
            ++fail_count;
            std::cout << "FAILED stoi(string, size_t*) simple digit count\n";
        }
        string s11 {"   123a  "};
        size_t st3;
        int i11 = stoi(s11, &st3);
        if (i11 != 123 || st3 != 6)
        {
            ++fail_count;
            std::cout << "FAILED stoi(string, size_t*) digit count with spaces\n";
        }
    }
    // Conversion to signed long
    {
        string s1 {"123"};
        long l1 = stol(s1);
        if (l1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stol(string) positive no spaces\n";
        }
        string s2 {"-123"};
        long l2 = stol(s2);
        if (l2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED stol(string) negative no spaces\n";
        }
        string s3 {"   123 "};
        long l3 = stol(s3);
        if (l3 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stol(string) positive with spaces\n";
        }
        string s4 {"   -123 "};
        long l4 = stol(s4);
        if (l4 != -123)
        {
            ++fail_count;
            std::cout << "FAILED stol(string) negative with spaces\n";
        }
        string s5 {"0xA6"};
        long l5 = stol(s5, nullptr, 0);
        if (l5 != 166)
        {
            ++fail_count;
            std::cout << "FAILED stol(string, nullptr, 0) assumed hex\n";
        }
        string s6 {"076"};
        long l6 = stol(s6, nullptr, 0);
        if (l6 != 62)
        {
            ++fail_count;
            std::cout << "FAILED stol(string, nullptr, 0) assumed octal\n";
        }
        string s7 {"0xA6"};
        long l7 = stol(s7, nullptr, 16);
        if (l7 != 166)
        {
            ++fail_count;
            std::cout << "FAILED stol(string, nullptr, 16) explicit hex\n";
        }
        string s8 {"076"};
        long l8 = stol(s8, nullptr, 8);
        if (l8 != 62)
        {
            ++fail_count;
            std::cout << "FAILED stol(string, nullptr, 8) explicit octal\n";
        }
        string s9 {"07697"};
        size_t st1;
        long l9 = stol(s9, &st1, 8);
        if (l9 != 62 || st1 != 3)
        {
            ++fail_count;
            std::cout << "FAILED stol(string, size_t*, 8) octal with bad digit\n";
        }
        string s10 {"123"};
        size_t st2;
        long l10 = stol(s10, &st2);
        if (l10 != 123 || st2 != 3)
        {
            ++fail_count;
            std::cout << "FAILED stol(string, size_t*) simple digit count\n";
        }
        string s11 {"   123a  "};
        size_t st3;
        long l11 = stol(s11, &st3);
        if (l11 != 123 || st3 != 6)
        {
            ++fail_count;
            std::cout << "FAILED stol(string, size_t*) digit count with spaces\n";
        }
    }
    // Conversion to signed long long
    {
        string s1 {"123"};
        long long ll1 = stoll(s1);
        if (ll1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stoll(string) positive no spaces\n";
        }
        string s2 {"-123"};
        long long ll2 = stoll(s2);
        if (ll2 != -123)
        {
            ++fail_count;
            std::cout << "FAILED stoll(string) negative no spaces\n";
        }
        string s3 {"   123 "};
        long long ll3 = stoll(s3);
        if (ll3 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stoll(string) positive with spaces\n";
        }
        string s4 {"   -123 "};
        long long ll4 = stoll(s4);
        if (ll4 != -123)
        {
            ++fail_count;
            std::cout << "FAILED stoll(string) negative with spaces\n";
        }
        string s5 {"0xA6"};
        long long ll5 = stoll(s5, nullptr, 0);
        if (ll5 != 166)
        {
            ++fail_count;
            std::cout << "FAILED stoll(string, nullptr, 0) assumed hex\n";
        }
        string s6 {"076"};
        long long ll6 = stoll(s6, nullptr, 0);
        if (ll6 != 62)
        {
            ++fail_count;
            std::cout << "FAILED stoll(string, nullptr, 0) assumed octal\n";
        }
        string s7 {"0xA6"};
        long long ll7 = stoll(s7, nullptr, 16);
        if (ll7 != 166)
        {
            ++fail_count;
            std::cout << "FAILED stoll(string, nullptr, 16) explicit hex\n";
        }
        string s8 {"076"};
        long long ll8 = stoll(s8, nullptr, 8);
        if (ll8 != 62)
        {
            ++fail_count;
            std::cout << "FAILED stoll(string, nullptr, 8) explicit octal\n";
        }
        string s9 {"07697"};
        size_t st1;
        long long ll9 = stoll(s9, &st1, 8);
        if (ll9 != 62 || st1 != 3)
        {
            ++fail_count;
            std::cout << "FAILED stoll(string, size_t*, 8) octal with bad digit\n";
        }
        string s10 {"123"};
        size_t st2;
        long long ll10 = stoll(s10, &st2);
        if (ll10 != 123 || st2 != 3)
        {
            ++fail_count;
            std::cout << "FAILED stoll(string, size_t*) simple digit count\n";
        }
        string s11 {"   123a  "};
        size_t st3;
        long long ll11 = stoll(s11, &st3);
        if (ll11 != 123 || st3 != 6)
        {
            ++fail_count;
            std::cout << "FAILED stoll(string, size_t*) digit count with spaces\n";
        }
    }
    // Conversion to unsigned long
    {
        string s1 {"123"};
        unsigned long ul1 = stoul(s1);
        if (ul1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stoul(string) no spaces\n";
        }
        string s2 {"   123 "};
        unsigned long ul2 = stoul(s2);
        if (ul2 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stoul(string) with spaces\n";
        }
        string s3 {"0xA6"};
        unsigned long ul3 = stoul(s3, nullptr, 0);
        if (ul3 != 166)
        {
            ++fail_count;
            std::cout << "FAILED stoul(string, nullptr, 0) assumed hex\n";
        }
        string s4 {"076"};
        unsigned long ul4 = stoul(s4, nullptr, 0);
        if (ul4 != 62)
        {
            ++fail_count;
            std::cout << "FAILED stoul(string, nullptr, 0) assumed octal\n";
        }
        string s5 {"0xA6"};
        unsigned long ul5 = stoul(s5, nullptr, 16);
        if (ul5 != 166)
        {
            ++fail_count;
            std::cout << "FAILED stoul(string, nullptr, 16) explicit hex\n";
        }
        string s6 {"076"};
        unsigned long ul6 = stoul(s6, nullptr, 8);
        if (ul6 != 62)
        {
            ++fail_count;
            std::cout << "FAILED stoul(string, nullptr, 8) explicit octal\n";
        }
        string s7 {"07697"};
        size_t st1;
        unsigned long ul7 = stoul(s7, &st1, 8);
        if (ul7 != 62 || st1 != 3)
        {
            ++fail_count;
            std::cout << "FAILED stoul(string, size_t*, 8) octal with bad digit\n";
        }
        string s8 {"123"};
        size_t st2;
        unsigned long ul8 = stoul(s8, &st2);
        if (ul8 != 123 || st2 != 3)
        {
            ++fail_count;
            std::cout << "FAILED stoul(string, size_t*) simple digit count\n";
        }
        string s9 {"   123a  "};
        size_t st3;
        unsigned long ul9 = stoul(s9, &st3);
        if (ul9 != 123 || st3 != 6)
        {
            ++fail_count;
            std::cout << "FAILED stoul(string, size_t*) digit count with spaces\n";
        }
    }
    // Conversion to unsigned long long
    {
        string s1 {"123"};
        unsigned long long ull1 = stoull(s1);
        if (ull1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stoull(string) no spaces\n";
        }
        string s2 {"   123 "};
        unsigned long long ull2 = stoull(s2);
        if (ull2 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stoull(string) with spaces\n";
        }
        string s3 {"0xA6"};
        unsigned long long ull3 = stoull(s3, nullptr, 0);
        if (ull3 != 166)
        {
            ++fail_count;
            std::cout << "FAILED stoull(string, nullptr, 0) assumed hex\n";
        }
        string s4 {"076"};
        unsigned long long ull4 = stoull(s4, nullptr, 0);
        if (ull4 != 62)
        {
            ++fail_count;
            std::cout << "FAILED stoull(string, nullptr, 0) assumed octal\n";
        }
        string s5 {"0xA6"};
        unsigned long long ull5 = stoull(s5, nullptr, 16);
        if (ull5 != 166)
        {
            ++fail_count;
            std::cout << "FAILED stoull(string, nullptr, 16) explicit hex\n";
        }
        string s6 {"076"};
        unsigned long long ull6 = stoull(s6, nullptr, 8);
        if (ull6 != 62)
        {
            ++fail_count;
            std::cout << "FAILED stoull(string, nullptr, 8) explicit octal\n";
        }
        string s7 {"07697"};
        size_t st1;
        unsigned long long ull7 = stoull(s7, &st1, 8);
        if (ull7 != 62 || st1 != 3)
        {
            ++fail_count;
            std::cout << "FAILED stoull(string, size_t*, 8) octal with bad digit\n";
        }
        string s8 {"123"};
        size_t st2;
        unsigned long long ull8 = stoull(s8, &st2);
        if (ull8 != 123 || st2 != 3)
        {
            ++fail_count;
            std::cout << "FAILED stoull(string, size_t*) simple digit count\n";
        }
        string s9 {"   123a  "};
        size_t st3;
        unsigned long long ull9 = stoull(s9, &st3);
        if (ull9 != 123 || st3 != 6)
        {
            ++fail_count;
            std::cout << "FAILED stoull(string, size_t*) digit count with spaces\n";
        }
    }
    // Conversion to float
    {
        // Straight integer
        string s1 {"123"};
        float f1 = stof(s1);
        if (f1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stof(string) integer\n";
        }
        // Simple decimal
        string s2 {"1.23"};
        float f2 = stof(s2);
        if (f2 != 1.23f)
        {
            ++fail_count;
            std::cout << "FAILED stof(string) simple decimal\n";
        }
        // Negative decimal
        string s3 {"-1.23"};
        float f3 = stof(s3);
        if (f3 != -1.23f)
        {
            ++fail_count;
            std::cout << "FAILED stof(string) negative decimal\n";
        }
        // Decimal with spaces
        string s4 {"    111.23"};
        float f4 = stof(s4);
        if (f4 != 111.23f)
        {
            ++fail_count;
            std::cout << "FAILED stof(string) decimal with spaces\n";
        }
        // Decimal exponential
        string s5 {"1.23456e3"};
        float f5 = stof(s5);
        if (f5 != 1234.56f)
        {
            ++fail_count;
            std::cout << "FAILED stof(string) exponential decimal\n";
        }
        // Straight hex integer
        string s6 {"0x1C3"};
        float f6 = stof(s6);
        if (f6 != 451)
        {
            ++fail_count;
            std::cout << "FAILED stof(string) hex integer\n";
        }
        // Simple hexadecimal
        string s7 {"0x1.C3"};
        float f7 = stof(s7);
        if (f7 != 1.76171875f)
        {
            ++fail_count;
            std::cout << "FAILED stof(string) simple hexadecimal\n";
        }
        // Negative hexadecimal
        string s8 {"-0x1.C3"};
        float f8 = stof(s8);
        if (f8 != -1.76171875f)
        {
            ++fail_count;
            std::cout << "FAILED stof(string) negative hexadecimal\n";
        }
        // Hexadecimal with spaces
        string s9 {"    0xa11.2e"};
        float f9 = stof(s9);
        if (f9 != 2577.1796875f)
        {
            ++fail_count;
            std::cout << "FAILED stof(string) hexadecimal with spaces\n";
        }
        // Hexadecimal exponential
        string s10 {"0x1.C3p3"};
        float f10 = stof(s10);
        if (f10 != 14.09375f)
        {
            ++fail_count;
            std::cout << "FAILED stof(string) exponential hexadecimal\n";
        }
        // INF
        string s11 {"iNf"};
        float f11 = stof(s11);
        if (f11 != numeric_limits<float>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED stof(string) inf\n";
        }
        string s12 {"  -0xiNfinITy"};
        float f12 = stof(s12);
        if (f12 != -numeric_limits<float>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED stof(string) negative infinity\n";
        }
        // NAN
        string s13 {"  nAn   "};
        float f13 = stof(s13);
        if (!isnan(f13))
        {
            ++fail_count;
            std::cout << "FAILED stof(string) NaN\n";
        }
    }
    // Conversion to double
    {
        // Straight integer
        string s1 {"123"};
        double d1 = stod(s1);
        if (d1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stod(string) integer\n";
        }
        // Simple decimal
        string s2 {"1.23"};
        double d2 = stod(s2);
        if (d2 != 1.23)
        {
            ++fail_count;
            std::cout << "FAILED stod(string) simple decimal\n";
        }
        // Negative decimal
        string s3 {"-1.23"};
        double d3 = stod(s3);
        if (d3 != -1.23)
        {
            ++fail_count;
            std::cout << "FAILED stod(string) negative decimal\n";
        }
        // Decimal with spaces
        string s4 {"    111.23"};
        double d4 = stod(s4);
        if (d4 != 111.23)
        {
            ++fail_count;
            std::cout << "FAILED stod(string) decimal with spaces\n";
        }
        // Decimal exponential
        string s5 {"1.23456e3"};
        double d5 = stod(s5);
        if (d5 != 1234.56)
        {
            ++fail_count;
            std::cout << "FAILED stod(string) exponential decimal\n";
        }
        // Straight hex integer
        string s6 {"0x1C3"};
        double d6 = stod(s6);
        if (d6 != 451)
        {
            ++fail_count;
            std::cout << "FAILED stod(string) hex integer\n";
        }
        // Simple hexadecimal
        string s7 {"0x1.C3"};
        double d7 = stod(s7);
        if (d7 != 1.76171875)
        {
            ++fail_count;
            std::cout << "FAILED stod(string) simple hexadecimal\n";
        }
        // Negative hexadecimal
        string s8 {"-0x1.C3"};
        double d8 = stod(s8);
        if (d8 != -1.76171875)
        {
            ++fail_count;
            std::cout << "FAILED stod(string) negative hexadecimal\n";
        }
        // Hexadecimal with spaces
        string s9 {"    0xa11.2e"};
        double d9 = stod(s9);
        if (d9 != 2577.1796875)
        {
            ++fail_count;
            std::cout << "FAILED stod(string) hexadecimal with spaces\n";
        }
        // Hexadecimal exponential
        string s10 {"0x1.C3p3"};
        double d10 = stod(s10);
        if (d10 != 14.09375)
        {
            ++fail_count;
            std::cout << "FAILED stod(string) exponential hexadecimal\n";
        }
        // INF
        string s11 {"iNf"};
        double d11 = stod(s11);
        if (d11 != numeric_limits<double>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED stod(string) inf\n";
        }
        string s12 {"  -0xiNfinITy"};
        double d12 = stod(s12);
        if (d12 != -numeric_limits<double>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED stod(string) negative infinity\n";
        }
        // NAN
        string s13 {"  nAn   "};
        double d13 = stod(s13);
        if (!isnan(d13))
        {
            ++fail_count;
            std::cout << "FAILED stod(string) NaN\n";
        }
    }
    // Conversion to long double
    {
        // Straight integer
        string s1 {"123"};
        long double ld1 = stold(s1);
        if (ld1 != 123)
        {
            ++fail_count;
            std::cout << "FAILED stold(string) integer\n";
        }
        // Simple decimal
        string s2 {"1.23"};
        long double ld2 = stold(s2);
        if (ld2 != 1.23l)
        {
            ++fail_count;
            std::cout << "FAILED stold(string) simple decimal\n";
        }
        // Negative decimal
        string s3 {"-1.23"};
        long double ld3 = stold(s3);
        if (ld3 != -1.23l)
        {
            ++fail_count;
            std::cout << "FAILED stold(string) negative decimal\n";
        }
        // Decimal with spaces
        string s4 {"    111.23"};
        long double ld4 = stold(s4);
        if (ld4 != 111.23l)
        {
            ++fail_count;
            std::cout << "FAILED stold(string) decimal with spaces\n";
        }
        // Decimal exponential
        string s5 {"1.23456e3"};
        long double ld5 = stold(s5);
        if (ld5 != 1234.56l)
        {
            ++fail_count;
            std::cout << "FAILED stold(string) exponential decimal\n";
        }
        // Straight hex integer
        string s6 {"0x1C3"};
        long double ld6 = stold(s6);
        if (ld6 != 451)
        {
            ++fail_count;
            std::cout << "FAILED stold(string) hex integer\n";
        }
        // Simple hexadecimal
        string s7 {"0x1.C3"};
        long double ld7 = stold(s7);
        if (ld7 != 1.76171875l)
        {
            ++fail_count;
            std::cout << "FAILED stold(string) simple hexadecimal\n";
        }
        // Negative hexadecimal
        string s8 {"-0x1.C3"};
        long double ld8 = stold(s8);
        if (ld8 != -1.76171875l)
        {
            ++fail_count;
            std::cout << "FAILED stold(string) negative hexadecimal\n";
        }
        // Hexadecimal with spaces
        string s9 {"    0xa11.2e"};
        long double ld9 = stold(s9);
        if (ld9 != 2577.1796875l)
        {
            ++fail_count;
            std::cout << "FAILED stold(string) hexadecimal with spaces\n";
        }
        // Hexadecimal exponential
        string s10 {"0x1.C3p3"};
        long double ld10 = stold(s10);
        if (ld10 != 14.09375l)
        {
            ++fail_count;
            std::cout << "FAILED stold(string) exponential hexadecimal\n";
        }
        // INF
        string s11 {"iNf"};
        long double ld11 = stold(s11);
        if (ld11 != numeric_limits<long double>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED stold(string) inf\n";
        }
        string s12 {"  -0xiNfinITy"};
        long double ld12 = stold(s12);
        if (ld12 != -numeric_limits<long double>::infinity())
        {
            ++fail_count;
            std::cout << "FAILED stold(string) negative infinity\n";
        }
        // NAN
        string s13 {"  nAn   "};
        long double ld13 = stold(s13);
        if (!isnan(ld13))
        {
            ++fail_count;
            std::cout << "FAILED stold(string) NaN\n";
        }
    }
    // Conversion to string
    {
        // Simple int
        int i1 = 123;
        string s1 = to_string(i1);
        if (s1.size() != 3 || s1.capacity() != default_space || s1 != "123")
        {
            ++fail_count;
            std::cout << "FAILED to_string(int) simple int\n";
        }
        // Negative int
        int i2 = -123;
        string s2 = to_string(i2);
        if (s2.size() != 4 || s2.capacity() != default_space || s2 != "-123")
        {
            ++fail_count;
            std::cout << "FAILED to_string(int) negative int\n";
        }
        // Simple long
        int l1 = 123;
        string s3 = to_string(l1);
        if (s3.size() != 3 || s3.capacity() != default_space || s3 != "123")
        {
            ++fail_count;
            std::cout << "FAILED to_string(long) simple long\n";
        }
        // Negative long
        int l2 = -123;
        string s4 = to_string(l2);
        if (s4.size() != 4 || s4.capacity() != default_space || s4 != "-123")
        {
            ++fail_count;
            std::cout << "FAILED to_string(long) negative long\n";
        }
        // unsigned int
        unsigned int ui1 = 123;
        string s5 = to_string(ui1);
        if (s5.size() != 3 || s5.capacity() != default_space || s5 != "123")
        {
            ++fail_count;
            std::cout << "FAILED to_string(unsigned int)\n";
        }
        // unsigned long
        unsigned long ul1 = 123;
        string s6 = to_string(ul1);
        if (s6.size() != 3 || s6.capacity() != default_space || s6 != "123")
        {
            ++fail_count;
            std::cout << "FAILED to_string(unsigned long)\n";
        }
        // unsigned long long
        unsigned long long ull1 = 123;
        string s7 = to_string(ull1);
        if (s7.size() != 3 || s7.capacity() != default_space || s7 != "123")
        {
            ++fail_count;
            std::cout << "FAILED to_string(unsigned long long)\n";
        }
        // Simple float
        float f1 = 1.23;
        string s8 = to_string(f1);
        if (s8.size() != 16 || s8.capacity() != 16 || s8 != "1.23000001907349")
        {
            ++fail_count;
            std::cout << "FAILED to_string(float) simple float\n";
        }
        // Negative float
        float f2 = -1.23;
        string s9 = to_string(f2);
        if (s9.size() != 17 || s9.capacity() != 17 || s9 != "-1.23000001907349")
        {
            ++fail_count;
            std::cout << "FAILED to_string(float) negative float\n";
        }
        // Exponential float
        float f3 = 1.23456e3;
        string s10 = to_string(f3);
        if (s10.size() != 16 || s10.capacity() != 16 ||
            s10 != "1234.56005859375")
        {
            ++fail_count;
            std::cout << "FAILED to_string(float) exponential float\n";
        }
        // INF float
        float f4 = numeric_limits<float>::infinity();
        string s11 = to_string(f4);
        if (s11.size() != 3 || s11.capacity() != default_space || s11 != "INF")
        {
            ++fail_count;
            std::cout << "FAILED to_string(float) INF float\n";
        }
        // Negative INF float
        float f5 = -numeric_limits<float>::infinity();
        string s12 = to_string(f5);
        if (s12.size() != 4 || s12.capacity() != default_space || s12 != "-INF")
        {
            ++fail_count;
            std::cout << "FAILED to_string(float) negative INF float\n";
        }
        // NaN float
        float f6 = numeric_limits<float>::quiet_NaN();
        string s13 = to_string(f6);
        if (s13.size() != 3 || s13.capacity() != default_space || s13 != "NaN")
        {
            ++fail_count;
            std::cout << "FAILED to_string(float) NaN float\n";
        }
        // Simple double
        double d1 = 1.23;
        string s14 = to_string(d1);
        if (s14.size() != 14 || s14.capacity() != 14 ||
            s14 != "1.230000000000")
        {
            ++fail_count;
            std::cout << "FAILED to_string(double) simple double\n";
        }
        // Negative double
        double d2 = -1.23;
        string s15 = to_string(d2);
        if (s15.size() != 15 || s15.capacity() != 15 ||
            s15 != "-1.230000000000")
        {
            ++fail_count;
            std::cout << "FAILED to_string(double) negative double\n";
        }
        // Exponential double
        double d3 = 1.23456e3;
        string s16 = to_string(d3);
        if (s16.size() != 19 || s16.capacity() != 19 ||
            s16 != "1234.55999999999995")
        {
            ++fail_count;
            std::cout << "FAILED to_string(double) exponential double\n";
        }
        // INF double
        double d4 = numeric_limits<double>::infinity();
        string s17 = to_string(d4);
        if (s17.size() != 3 || s17.capacity() != default_space || s17 != "INF")
        {
            ++fail_count;
            std::cout << "FAILED to_string(double) INF double\n";
        }
        // Negative INF double
        double d5 = -numeric_limits<double>::infinity();
        string s18 = to_string(d5);
        if (s18.size() != 4 || s18.capacity() != default_space || s18 != "-INF")
        {
            ++fail_count;
            std::cout << "FAILED to_string(double) negative INF double\n";
        }
        // NaN double
        double d6 = numeric_limits<double>::quiet_NaN();
        string s19 = to_string(d6);
        if (s19.size() != 3 || s19.capacity() != default_space || s19 != "NaN")
        {
            ++fail_count;
            std::cout << "FAILED to_string(double) NaN double\n";
        }
        // Simple long double
        long double ld1 = 1.23;
        string s20 = to_string(ld1);
        if (s20.size() != 14 || s20.capacity() != 14 ||
            s20 != "1.230000000000")
        {
            ++fail_count;
            std::cout << "FAILED to_string(long double) simple long double\n";
        }
        // Negative double
        long double ld2 = -1.23;
        string s21 = to_string(ld2);
        if (s21.size() != 15 || s21.capacity() != 15 ||
            s21 != "-1.230000000000")
        {
            ++fail_count;
            std::cout << "FAILED to_string(long double) negative long double\n";
        }
        // Exponential double
        long double ld3 = 1.23456e3;
        string s22 = to_string(ld3);
        if (s22.size() != 19 || s22.capacity() != 19 ||
            s22 != "1234.55999999999995")
        {
            ++fail_count;
            std::cout << "FAILED to_string(long double) exponential long double\n";
        }
        // INF double
        long double ld4 = numeric_limits<long double>::infinity();
        string s23 = to_string(ld4);
        if (s23.size() != 3 || s23.capacity() != default_space || s23 != "INF")
        {
            ++fail_count;
            std::cout << "FAILED to_string(long double) INF long double\n";
        }
        // Negative INF double
        long double ld5 = -numeric_limits<long double>::infinity();
        string s24 = to_string(ld5);
        if (s24.size() != 4 || s24.capacity() != default_space || s24 != "-INF")
        {
            ++fail_count;
            std::cout << "FAILED to_string(long double) negative INF long double\n";
        }
        // NaN double
        long double ld6 = numeric_limits<long double>::quiet_NaN();
        string s25 = to_string(ld6);
        if (s25.size() != 3 || s25.capacity() != default_space || s25 != "NaN")
        {
            ++fail_count;
            std::cout << "FAILED to_string(long double) NaN long double\n";
        }
        // Simple char
        char c1 = 'a';
        string s26 = to_string(c1);
        if (s26.size() != 1 || s26.capacity() != default_space || s26 != "a")
        {
            ++fail_count;
            std::cout << "FAILED to_string(char) simple char\n";
        }
        // unsigned char
        unsigned char c2 = 'a';
        string s27 = to_string(c2);
        if (s27.size() != 1 || s27.capacity() != default_space || s27 != "a")
        {
            ++fail_count;
            std::cout << "FAILED to_string(char) unsigned char\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
