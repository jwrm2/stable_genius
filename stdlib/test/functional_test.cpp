#include <iostream>

#include "../include/functional"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // less
    {
        // types
        if (!is_same<typename less<int>::first_argument_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED less::first_argument_type\n";
        }
        if (!is_same<typename less<int>::second_argument_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED less::second_argument_type\n";
        }
        if (!is_same<typename less<int>::result_type, bool>::value)
        {
            ++fail_count;
            std::cout << "FAILED less::result_type\n";
        }
        less<int> l1;
        int i1 = 5;
        int i2 = 5;
        int i3 = 10;
        if (l1(i1, i2) || !l1(i1, i3) || l1(i3, i1))
        {
            ++fail_count;
            std::cout << "FAILED less::operator ()\n";
        }
        // void specialisation
        less<> l2;
        int i4 = 5;
        int i5 = 5;
        int i6 = 10;
        if (l2(i4, i5) || !l2(i4, i6) || l2(i6, i4))
        {
            ++fail_count;
            std::cout << "FAILED less<void>::operator ()\n";
        }
    }
    // greater
    {
        // types
        if (!is_same<typename greater<int>::first_argument_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED greater::first_argument_type\n";
        }
        if (!is_same<typename greater<int>::second_argument_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED greater::second_argument_type\n";
        }
        if (!is_same<typename greater<int>::result_type, bool>::value)
        {
            ++fail_count;
            std::cout << "FAILED greater::result_type\n";
        }
        greater<int> g1;
        int i1 = 5;
        int i2 = 5;
        int i3 = 10;
        if (g1(i1, i2) || !g1(i3, i1) || g1(i1, i3))
        {
            ++fail_count;
            std::cout << "FAILED greater::operator ()\n";
        }
        // void specialisation
        greater<> g2;
        int i4 = 5;
        int i5 = 5;
        int i6 = 10;
        if (g2(i4, i5) || !g2(i6, i4) || g2(i4, i6))
        {
            ++fail_count;
            std::cout << "FAILED greater<void>::operator ()\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
