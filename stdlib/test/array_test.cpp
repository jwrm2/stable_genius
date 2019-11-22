#include <iostream>

#include "../include/array"
#include "../include/type_traits"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // Types
    if (!is_same<typename array<int, 3>::value_type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED array::value_type\n";
    }
    if (!is_same<typename array<int, 3>::size_type, size_t>::value)
    {
        ++fail_count;
        std::cout << "FAILED array::size_type\n";
    }
    if (!is_same<typename array<int, 3>::difference_type, ptrdiff_t>::value)
    {
        ++fail_count;
        std::cout << "FAILED array::difference_type\n";
    }
    if (!is_same<typename array<int, 3>::reference, int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED array::reference\n";
    }
    if (!is_same<typename array<int, 3>::const_reference, const int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED array::const_reference\n";
    }
    if (!is_same<typename array<int, 3>::pointer, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED array::pointer\n";
    }
    if (!is_same<typename array<int, 3>::const_pointer, const int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED array::const_pointer\n";
    }
    if (!is_same<typename array<int, 3>::iterator,
        helper::normal_iterator<int*, array<int, 3>>>::value)
    {
        ++fail_count;
        std::cout << "FAILED array::iterator\n";
    }
    if (!is_same<typename array<int, 3>::const_iterator,
        helper::normal_iterator<const int*, array<int, 3>>>::value)
    {
        ++fail_count;
        std::cout << "FAILED array::iterator\n";
    }
    if (!is_same<typename array<int, 3>::reverse_iterator,
       reverse_iterator<helper::normal_iterator<int*, array<int, 3>>>>::value)
    {
        ++fail_count;
        std::cout << "FAILED array::reverse_iterator\n";
    }
    if (!is_same<typename array<int, 3>::const_reverse_iterator,
       reverse_iterator<helper::normal_iterator<const int*, array<int, 3>>>>::value)
    {
        ++fail_count;
        std::cout << "FAILED array::const_reverse_iterator\n";
    }
    // Constructors
    {
        // Implicit aggregate initialisation, right size.
        array<int, 3> a1 {1, 2, 3};
        if (a1.size() != 3 || a1[0] != 1 || a1[1] != 2 || a1[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED array aggregate initialisation same size\n";
        }
        // Implicit aggregate initialisation, too few elements.
        array<int, 3> a2 {1, 2};
        if (a2.size() != 3 || a2[0] != 1 || a2[1] != 2 || a2[2] != 0)
        {
            ++fail_count;
            std::cout << "FAILED array aggregate initialisation too few elements\n";
        }
        // Implicit aggregate initialisation, empty list.
        array<int, 3> a3 {};
        if (a3.size() != 3 || a3[0] != 0 || a3[1] != 0 || a3[2] != 0)
        {
            ++fail_count;
            std::cout << "FAILED array aggregate initialisation empty list\n";
        }
        // Implicit aggregate initialisation, empty array.
        array<int, 0> a4 {};
        if (a4.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED array aggregate initialisation empty array\n";
        }
        // Implicit default constructor, empty array.
        array<int, 0> a5;
        if (a5.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED array implicit default constructor empty array\n";
        }
    }
    // Assignment
    {
        // Copy assignment.
        array<int, 3> a1 {1, 2, 3};
        array<int, 3> a2 {3, 4, 5};
        a2 = a1;
        if (a2.size() != 3 || a2[0] != 1 || a2[1] != 2 || a2[2] != 3 ||
            a1.size() != 3 || a1[0] != 1 || a1[1] != 2 || a1[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED array::operator=(const array&)\n";
        }
        // Move assignment.
        array<int, 3> a3 {3, 4, 5};
        array<int, 3> a4 = move(a3);
        if (a4.size() != 3 || a4[0] != 3 || a4[1] != 4 || a4[2] != 5)
        {
            ++fail_count;
            std::cout << "FAILED array::operator=(array&&)\n";
        }
    }
    // operator[]
    {
        // Read
        array<int, 3> a1 {1, 2, 3};
        if (a1[0] != 1 || a1[1] != 2 || a1[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED array::operator[] read\n";
        }
        // const read
        const array<int, 3> a2 {1, 2, 3};
        if (a2[0] != 1|| a2[1] != 2 || a2[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED array::operator[] const read\n";
        }
        // Write
        array<int, 3> a3 {1, 2, 3};
        a3[0] = 4; a3[1] = 5; a3[2] = 6;
        if (a3.size() != 3 || a3[0] != 4 || a3[1] != 5 || a3[2] != 6)
        {
            ++fail_count;
            std::cout << "FAILED array::operator[] assign\n";
        }
    }
    // Front and back
    {
        // Front
        array<int, 3> a1 {1, 2, 3};
        if (a1.front() != 1)
        {
            ++fail_count;
            std::cout << "FAILED array::front\n";
        }
        // const front
        const array<int, 3> a2 {1, 2, 3};
        if (a2.front() != 1)
        {
            ++fail_count;
            std::cout << "FAILED const array::front\n";
        }
        // Back
        array<int, 3> a3 {1, 2, 3};
        if (a3.back() != 3)
        {
            ++fail_count;
            std::cout << "FAILED array::back\n";
        }
        // const back
        const array<int, 3> a4 {1, 2, 3};
        if (a4.back() != 3)
        {
            ++fail_count;
            std::cout << "FAILED const array::back\n";
        }
        // Data read
        const array<int, 3> a5 = {0, 1, 2};
        if (a5.data()[0] != 0 || a5.data()[1] != 1 || a5.data()[2] != 2)
        {
            ++fail_count;
            std::cout << "FAILED array::data() read\n";
        }
        // Data write
        array<int, 3> a6 = {0, 1, 2};
        a6.data()[0] = 3;
        a6.data()[1] = 4;
        a6.data()[2] = 5;
        if (a6.data()[0] != 3 || a6.data()[1] != 4 || a6.data()[2] != 5)
        {
            ++fail_count;
            std::cout << "FAILED array::data() write\n";
        }
        // Data empty
        array<int, 0> a7;
        if (a7.data() != nullptr)
        {
            ++fail_count;
            std::cout << "FAILED array::data() empty\n";
        }
    }
    // Iterators
    {
        // begin() read
        const array<int, 3> a1 {1, 2, 3};
        if (*a1.begin() != 1)
        {
            ++fail_count;
            std::cout << "FAILED array::begin() read\n";
        }
        // cbegin() read
        const array<int, 3> a2 {1, 2, 3};
        if (*a2.cbegin() != 1)
        {
            ++fail_count;
            std::cout << "FAILED array::cbegin() read\n";
        }
        // begin() write
        array<int, 3> a3 {1, 2, 3};
        *a3.begin() = 4;
        if (*a3.begin() != 4)
        {
            ++fail_count;
            std::cout << "FAILED array::begin() write\n";
        }
        // end() read
        const array<int, 3> a4 {1, 2, 3};
        if (*(a4.end() - 1) != 3)
        {
            ++fail_count;
            std::cout << "FAILED array::end() read\n";
        }
        // cend() read
        const array<int, 3> a5 {1, 2, 3};
        if (*(a5.cend() - 1) != 3)
        {
            ++fail_count;
            std::cout << "FAILED array::cend() read\n";
        }
        // end() write
        array<int, 3> a6 {1, 2, 3};
        *(a6.end() - 1) = 4;
        if (*(a6.end() - 1) != 4)
        {
            ++fail_count;
            std::cout << "FAILED array::end() write\n";
        }
        // rbegin() read
        const array<int, 3> a7 {1, 2, 3};
        if (*a7.rbegin() != 3)
        {
            ++fail_count;
            std::cout << "FAILED array::rbegin() read\n";
        }
        // crbegin() read
        const array<int, 3> a8 {1, 2, 3};
        if (*a8.crbegin() != 3)
        {
            ++fail_count;
            std::cout << "FAILED array::crbegin() read\n";
        }
        // rbegin() write
        array<int, 3> a9 {1, 2, 3};
        *a9.rbegin() = 4;
        if (*a9.rbegin() != 4)
        {
            ++fail_count;
            std::cout << "FAILED array::rbegin() write\n";
        }
        // rend() read
        const array<int, 3> a10 {1, 2, 3};
        if (*(a10.rend() - 1) != 1)
        {
            ++fail_count;
            std::cout << "FAILED array::rend() read\n";
        }
        // crend() read
        const array<int, 3> a11 {1, 2, 3};
        if (*(a11.crend() - 1) != 1)
        {
            ++fail_count;
            std::cout << "FAILED array::crend() read\n";
        }
        // rend() write
        array<int, 3> a12 {1, 2, 3};
        *(a12.rend() - 1) = 4;
        if (*(a12.rend() - 1) != 4)
        {
            ++fail_count;
            std::cout << "FAILED array::rend() write\n";
        }
    }
    // Capacity
    {
        // Size, not empty.
        array<int, 3> a1 {1, 2, 3};
        if (a1.size() != 3)
        {
            ++fail_count;
            std::cout << "FAILED array::size() not empty\n";
        }
        // Size, empty.
        array<int, 0> a2;
        if (a2.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED array::size() empty\n";
        }
        // Max Size, not empty.
        array<int, 3> a3 {1, 2, 3};
        if (a3.max_size() != 3)
        {
            ++fail_count;
            std::cout << "FAILED array::max_size() not empty\n";
        }
        // Max Size, empty.
        array<int, 0> a4;
        if (a4.max_size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED array::max_size() empty\n";
        }
        // Empty, not empty.
        array<int, 3> a5 {1, 2, 3};
        if (a5.empty())
        {
            ++fail_count;
            std::cout << "FAILED array::empty() not empty\n";
        }
        // Empty, empty.
        array<int, 0> a6;
        if (!a6.empty())
        {
            ++fail_count;
            std::cout << "FAILED array::empty() empty\n";
        }
    }
    // Algorithms
    {
        // Fill, non-zero size
        array<int, 3> a1 {1, 2, 3};
        a1.fill(3);
        if (a1.size() != 3 || a1[0] != 3 || a1[1] != 3 || a1[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED array::fill() non-zero size\n";
        }
        // Fill, zero size
        array<int, 0> a2;
        a2.fill(3);
        if (!a2.empty())
        {
            ++fail_count;
            std::cout << "FAILED array::fill() zero size\n";
        }
        // Swap, non-zero size
        array<int, 3> a3 {1, 2, 3};
        array<int, 3> a4 {4, 5, 6};
        a3.swap(a4);
        if (a3.size() != 3 || a3[0] != 4 || a3[1] != 5 || a3[2] != 6 ||
            a4.size() != 3 || a4[0] != 1 || a4[1] != 2 || a4[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED array::swap() non-zero size\n";
        }
        // Swap, zero size
        array<int, 0> a5;
        array<int, 0> a6;
        a5.swap(a6);
        if (!a5.empty() || !a6.empty())
        {
            ++fail_count;
            std::cout << "FAILED array::swap() zero size\n";
        }
    }
    // Comparisons
    {
        // operator==
        array<int, 3> a1 {1, 2, 3};
        array<int, 3> a2 {1, 2, 3};
        array<int, 3> a3 {1, 2, 4};
        if (!(a1 == a2))
        {
            ++fail_count;
            std::cout << "FAILED array == array, equal\n";
        }
        if (a1 == a3)
        {
            ++fail_count;
            std::cout << "FAILED array == array, not equal\n";
        }
        // operator!=
        if (a1 != a2)
        {
            ++fail_count;
            std::cout << "FAILED array != array, equal\n";
        }
        if (!(a1 != a3))
        {
            ++fail_count;
            std::cout << "FAILED array != array, not equal\n";
        }
        // operator<
        array<int, 3> a4 {1, 2, 3};
        array<int, 3> a5 {1, 2, 3};
        array<int, 3> a6 {1, 2, 4};
        array<int, 3> a7 {2, 2, 3};
        if (a4 < a5)
        {
            ++fail_count;
            std::cout << "FAILED array < array, equal\n";
        }
        if (!(a4 < a6))
        {
            ++fail_count;
            std::cout << "FAILED array < array, second higher at end\n";
        }
        if (a6 < a4)
        {
            ++fail_count;
            std::cout << "FAILED array < array, first higher at end\n";
        }
        if (!(a4 < a7))
        {
            ++fail_count;
            std::cout << "FAILED array < array, second higher at start\n";
        }
        if (a7 < a4)
        {
            ++fail_count;
            std::cout << "FAILED array < array, first higher at start\n";
        }
        // operator <=
        if (!(a4 <= a5))
        {
            ++fail_count;
            std::cout << "FAILED array <= array, equal\n";
        }
        if (!(a4 <= a6))
        {
            ++fail_count;
            std::cout << "FAILED array <= array, second higher at end\n";
        }
        if (a6 <= a4)
        {
            ++fail_count;
            std::cout << "FAILED array <= array, first higher at end\n";
        }
        if (!(a4 <= a7))
        {
            ++fail_count;
            std::cout << "FAILED array <= array, second higher at start\n";
        }
        if (a7 <= a4)
        {
            ++fail_count;
            std::cout << "FAILED array <= array, first higher at start\n";
        }
        // operator >
        if (a4 > a5)
        {
            ++fail_count;
            std::cout << "FAILED array > array, equal\n";
        }
        if (a4 > a6)
        {
            ++fail_count;
            std::cout << "FAILED array > array, second higher at end\n";
        }
        if (!(a6 > a4))
        {
            ++fail_count;
            std::cout << "FAILED array > array, first higher at end\n";
        }
        if (a4 > a7)
        {
            ++fail_count;
            std::cout << "FAILED array > array, second higher at start\n";
        }
        if (!(a7 > a4))
        {
            ++fail_count;
            std::cout << "FAILED array > array, first higher at start\n";
        }
        // operator >=
        if (!(a4 >= a5))
        {
            ++fail_count;
            std::cout << "FAILED array >= array, equal\n";
        }
        if (a4 >= a6)
        {
            ++fail_count;
            std::cout << "FAILED array >= array, second higher at end\n";
        }
        if (!(a6 >= a4))
        {
            ++fail_count;
            std::cout << "FAILED array >= array, first higher at end\n";
        }
        if (a4 >= a7)
        {
            ++fail_count;
            std::cout << "FAILED array >= array, second higher at start\n";
        }
        if (!(a7 >= a4))
        {
            ++fail_count;
            std::cout << "FAILED array >= array, first higher at start\n";
        }
    }
    // Specialisations
    {
        // ref, read
        array<int, 3> a1 {1, 2, 3};
        if (get<0>(a1) != 1 || get<1>(a1) != 2 || get<2>(a1) != 3)
        {
            ++fail_count;
            std::cout << "FAILED get(array) ref read\n";
        }
        // ref, write
        array<int, 3> a2 {1, 2, 3};
        get<0>(a2) = 4; get<1>(a2) = 5; get<2>(a2) = 6;
        if (get<0>(a2) != 4 || get<1>(a2) != 5 || get<2>(a2) != 6)
        {
            ++fail_count;
            std::cout << "FAILED get(array) ref write\n";
        }
        // const ref, read
        const array<int, 3> a3 {1, 2, 3};
        if (get<0>(a3) != 1 || get<1>(a3) != 2 || get<2>(a3) != 3)
        {
            ++fail_count;
            std::cout << "FAILED get(array) const ref read\n";
        }
        // rvalue ref, read
        if (get<0>(array<int, 3> {1, 2, 3}) != 1 ||
            get<1>(array<int, 3> {1, 2, 3}) != 2 ||
            get<2>(array<int, 3> {1, 2, 3}) != 3)
        {
            ++fail_count;
            std::cout << "FAILED get(array) rvalue ref read\n";
        }
        // Swap, non-zero size
        array<int, 3> a4 {1, 2, 3};
        array<int, 3> a5 {4, 5, 6};
        swap(a4, a5);
        if (a4.size() != 3 || a4[0] != 4 || a4[1] != 5 || a4[2] != 6 ||
            a5.size() != 3 || a5[0] != 1 || a5[1] != 2 || a5[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED swap(array, array) non-zero size\n";
        }
        // Swap, zero size
        array<int, 0> a6;
        array<int, 0> a7;
        swap(a6, a7);
        if (!a6.empty() || !a7.empty())
        {
            ++fail_count;
            std::cout << "FAILED swap(array, array) zero size\n";
        }
        // tuple_size, non-zero
        if (tuple_size<array<int, 3>>::value != 3)
        {
            ++fail_count;
            std::cout << "FAILED tuple_size<array> non-zero size\n";
        }
        // tuple_size, zero
        if (tuple_size<array<int, 0>>::value != 0)
        {
            ++fail_count;
            std::cout << "FAILED tuple_size<array> zero size\n";
        }
        // tuple_element
        if (!is_same<tuple_element<0, array<int, 3>>::type, int>::value ||
            !is_same<tuple_element<1, array<int, 3>>::type, int>::value ||
            !is_same<tuple_element<2, array<int, 3>>::type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED tuple_element<array>\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
