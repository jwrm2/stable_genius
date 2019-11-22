#include <iostream>

#include "../include/utility"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // declval
    if (!is_same<decltype(declval<int>()), int&&>::value)
    {
        ++fail_count;
        std::cout << "FAILED declval<int>()\n";
    }

    // forward
    {
        int i1;
        if (!is_same<decltype(forward<int>(i1)), int&&>::value)
        {
            ++fail_count;
            std::cout << "FAILED forward(int&)\n";
        }
        if (!is_same<decltype(forward<int>(int{})), int&&>::value)
        {
            ++fail_count;
            std::cout << "FAILED forward(int&&)\n";
        }
    }

    // move
    {
        int i1;
        if (!is_same<decltype(move(i1)), int&&>::value)
        {
            ++fail_count;
            std::cout << "FAILED move(int&)\n";
        }
        if (!is_same<decltype(move(int{})), int&&>::value)
        {
            ++fail_count;
            std::cout << "FAILED move(int&&)\n";
        }
    }

    // swap
    {
        int i1 = 5;
        int i2 = 10;
        swap(i1, i2);
        if (i1 != 10 || i2 != 5)
        {
            ++fail_count;
            std::cout << "FAILED swap(int, int)\n";
        }
        int i3[] = {1, 2, 3};
        int i4[] = {4, 5, 6};
        swap(i3, i4);
        if (i3[2] != 6 || i4[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED swap(int[], int[])\n";
        }
    }
    // pair
    // member functions
    {
        if (!is_same<pair<int, char>::first_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED pair<int, char> first_type\n";
        }
        if (!is_same<pair<int, char>::second_type, char>::value)
        {
            ++fail_count;
            std::cout << "FAILED pair<int, char> second_type\n";
        }
        pair<int, char> p1;
        if (p1.first != 0 || p1.second != '\0')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, char> default constructor\n";
        }
        pair<int, char> p2 {1, 'a'};
        if (p2.first != 1 || p2.second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, char> lvalue reference constructor\n";
        }
        pair<int, char> p3 {move('b'), move('a')};
        if (p3.first != 'b' || p3.second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, char> rvalue reference constructor\n";
        }
        pair<int, char> p4 {move('b'), 'a'};
        if (p4.first != 'b' || p4.second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, char> mixed reference constructor, rvalue first\n";
        }
        pair<char, int> p5 {'a', move('b')};
        if (p5.first != 'a' || p5.second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED pair<char, int> mixed reference constructor, rvalue second\n";
        }
        pair<char, char> p6 {'a', 'b'};
        pair<int, int> p7 {p6};
        if (p7.first != 'a' || p7.second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, int> transforming copy constructor\n";
        }
        pair<int, int> p8 {1, 2};
        pair<int, int> p9 {p8};
        if (p9.first != 1 || p9.second != 2)
        {
            ++fail_count;
            std::cout << "FAILED pair<int, int> normal copy constructor\n";
        }
        pair<char, char> p10 {'a', 'b'};
        pair<int, int> p11 {move(p10)};
        if (p11.first != 'a' || p11.second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, int> transforming move constructor\n";
        }
        pair<int, int> p12 {1, 2};
        pair<int, int> p13 {move(p12)};
        if (p13.first != 1 || p13.second != 2)
        {
            ++fail_count;
            std::cout << "FAILED pair<int, int> normal move constructor\n";
        }
        pair<char, char> p14 {'a', 'b'};
        pair<int, int> p15;
        p15 = p14;
        if (p15.first != 'a' || p15.second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, int> transforming copy assignment\n";
        }
        pair<int, int> p16 {1, 2};
        pair<int, int> p17;
        p17 = p16;
        if (p17.first != 1 || p17.second != 2)
        {
            ++fail_count;
            std::cout << "FAILED pair<int, int> normal copy assignment\n";
        }
        pair<char, char> p18 {'a', 'b'};
        pair<int, int> p19;
        p19 = move(p18);
        if (p19.first != 'a' || p19.second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, int> transforming move assignment\n";
        }
        pair<int, int> p20 {1, 2};
        pair<int, int> p21;
        p21 = move(p20);
        if (p21.first != 1 || p21.second != 2)
        {
            ++fail_count;
            std::cout << "FAILED pair<int, int> normal move assignment\n";
        }
        pair<int, char> p22 {1, 'a'};
        pair<int, char> p23 {2, 'b'};
        p22.swap(p23);
        if (p22.first != 2 || p22.second != 'b' ||
            p23.first != 1 || p23.second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, char>::swap\n";
        }
    }
    // pair
    // comparison functions
    {
        pair<int, char> p1 {1, 'a'};
        pair<int, char> p2 {2, 'a'};
        pair<int, char> p3 {1, 'b'};
        pair<int, char> p4 {2, 'b'};
        pair<int, char> p5 {1, 'a'};
        if (!(p1 == p5))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator= equal\n";
        }
        if (p1 == p2)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator= first different\n";
        }
        if (p1 == p3)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator= second different\n";
        }
        if (p1 == p4)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator= both different\n";
        }
        if (p1 != p5)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator!= equal\n";
        }
        if (!(p1 != p2))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator!= first different\n";
        }
        if (!(p1 != p3))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator!= second different\n";
        }
        if (!(p1 != p4))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator!= both different\n";
        }
        if (p1 < p5)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator< equal\n";
        }
        if (!(p1 < p2))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator< first greater\n";
        }
        if (!(p1 < p3))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator< second greater\n";
        }
        if (!(p1 < p4))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator< both greater\n";
        }
        if (p2 < p1)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator< first less\n";
        }
        if (p3 < p1)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator< second less\n";
        }
        if (p4 < p1)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator< both less\n";
        }
        if (!(p1 <= p5))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator<= equal\n";
        }
        if (!(p1 <= p2))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator<= first greater\n";
        }
        if (!(p1 <= p3))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator<= second greater\n";
        }
        if (!(p1 <= p4))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator<= both greater\n";
        }
        if (p2 <= p1)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator<= first less\n";
        }
        if (p3 <= p1)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator<= second less\n";
        }
        if (p4 <= p1)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator<= both less\n";
        }
        if (p1 > p5)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator> equal\n";
        }
        if (p1 > p2)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator> first greater\n";
        }
        if (p1 > p3)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator> second greater\n";
        }
        if (p1 > p4)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator> both greater\n";
        }
        if (!(p2 > p1))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator> first less\n";
        }
        if (!(p3 > p1))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator> second less\n";
        }
        if (!(p4 > p1))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator> both less\n";
        }
        if (!(p1 >= p5))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator>= equal\n";
        }
        if (p1 >= p2)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator>= first greater\n";
        }
        if (p1 >= p3)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator>= second greater\n";
        }
        if (p1 >= p4)
        {
            ++fail_count;
            std::cout << "FAILED pair::operator>= both greater\n";
        }
        if (!(p2 >= p1))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator>= first less\n";
        }
        if (!(p3 >= p1))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator>= second less\n";
        }
        if (!(p4 >= p1))
        {
            ++fail_count;
            std::cout << "FAILED pair::operator>= both less\n";
        }
    }
    // pair
    // helper functions
    {
        pair<int, char> p1 = make_pair(1, 'a');
        if (p1.first != 1 || p1.second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, char> make_pair(int, char)\n";
        }
        pair<int, char> p2 = make_pair(static_cast<const int>(1),
            static_cast<const char>('a'));
        if (p2.first != 1 || p2.second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, char> make_pair(const int, const char)\n";
        }
        pair<int, char> p3 = make_pair(static_cast<const int&>(1),
            static_cast<const char&>('a'));
        if (p3.first != 1 || p3.second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, char> make_pair(const int&, const char&)\n";
        }
        pair<int, char> p4 = make_pair(move(1), move('a'));
        if (p4.first != 1 || p4.second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED pair<int, char> make_pair(int&&, char&&)\n";
        }
        pair<int, char> p5 {1, 'a'};
        pair<int, char> p6 {2, 'b'};
        swap(p5, p6);
        if (p5.first != 2 || p5.second != 'b' ||
            p6.first != 1 || p6.second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED swap(pair<int, char>, pair<int, char>)\n";
        }
        if (tuple_size<pair<int, char>>::value != 2)
        {
            ++fail_count;
            std::cout << "FAILED tuple_size<pair<int, char>>\n";
        }
        if (!is_same<tuple_element<0, pair<int, char>>::type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED tuple_element<0, pair<int, char>>\n";
        }
        if (!is_same<tuple_element<1, pair<int, char>>::type, char>::value)
        {
            ++fail_count;
            std::cout << "FAILED tuple_element<0, pair<int, char>>\n";
        }
        pair<int, char> p7 {1, 'a'};
        const pair<int, char> p8 {1, 'a'};
        if (get<0>(p7) != 1)
        {
            ++fail_count;
            std::cout << "FAILED get_pair<0>(pair<int, char>&)\n";
        }
        if (get<0>(move(p7)) != 1)
        {
            ++fail_count;
            std::cout << "FAILED get_pair<0>(pair<int, char>&&)\n";
        }
        if (get<0>(p8) != 1)
        {
            ++fail_count;
            std::cout << "FAILED get_pair<0>(const pair<int, char>&)\n";
        }
        if (get<1>(p7) != 'a')
        {
            ++fail_count;
            std::cout << "FAILED get_pair<1>(pair<int, char>&)\n";
        }
        if (get<1>(move(p7)) != 'a')
        {
            ++fail_count;
            std::cout << "FAILED get_pair<1>(pair<int, char>&&)\n";
        }
        if (get<1>(p8) != 'a')
        {
            ++fail_count;
            std::cout << "FAILED get_pair<1>(const pair<int, char>&)\n";
        }
        if (get<int>(p7) != 1)
        {
            ++fail_count;
            std::cout << "FAILED get_pair<int>(pair<int, char>&)\n";
        }
        if (get<int>(move(p7)) != 1)
        {
            ++fail_count;
            std::cout << "FAILED get_pair<int>(pair<int, char>&&)\n";
        }
        if (get<int>(p8) != 1)
        {
            ++fail_count;
            std::cout << "FAILED get_pair<int>(const pair<int, char>&)\n";
        }
        if (get<char>(p7) != 'a')
        {
            ++fail_count;
            std::cout << "FAILED get_pair<char>(pair<int, char>&)\n";
        }
        if (get<char>(move(p7)) != 'a')
        {
            ++fail_count;
            std::cout << "FAILED get_pair<char>(pair<int, char>&&)\n";
        }
        if (get<char>(p8) != 'a')
        {
            ++fail_count;
            std::cout << "FAILED get_pair<char>(const pair<int, char>&)\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
