#include <iostream>

#include <stddef.h>

#include "../include/limits"
#include "../include/memory"
#include "../include/type_traits"
#include "../include/vector"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // Types
    if (!is_same<typename vector<int>::value_type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED vector::value_type\n";
    }
    if (!is_same<typename vector<int>::allocator_type, allocator<int>>::value)
    {
        ++fail_count;
        std::cout << "FAILED vector::allocator_type\n";
    }
    if (!is_same<typename vector<int>::size_type, size_t>::value)
    {
        ++fail_count;
        std::cout << "FAILED vector::size_type\n";
    }
    if (!is_same<typename vector<int>::difference_type, ptrdiff_t>::value)
    {
        ++fail_count;
        std::cout << "FAILED vector::difference_type\n";
    }
    if (!is_same<typename vector<int>::reference, int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED vector::reference\n";
    }
    if (!is_same<typename vector<int>::const_reference, const int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED vector::const_reference\n";
    }
    if (!is_same<typename vector<int>::pointer, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED vector::pointer\n";
    }
    if (!is_same<typename vector<int>::const_pointer, const int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED vector::const_pointer\n";
    }
    if (!is_same<typename vector<int>::iterator,
        helper::normal_iterator<int*, vector<int>>>::value)
    {
        ++fail_count;
        std::cout << "FAILED vector::iterator\n";
    }
    if (!is_same<typename vector<int>::const_iterator,
        helper::normal_iterator<const int*, vector<int>>>::value)
    {
        ++fail_count;
        std::cout << "FAILED vector::iterator\n";
    }
    if (!is_same<typename vector<int>::reverse_iterator,
       reverse_iterator<helper::normal_iterator<int*, vector<int>>>>::value)
    {
        ++fail_count;
        std::cout << "FAILED vector::reverse_iterator\n";
    }
    if (!is_same<typename vector<int>::const_reverse_iterator,
       reverse_iterator<helper::normal_iterator<const int*, vector<int>>>>::value)
    {
        ++fail_count;
        std::cout << "FAILED vector::const_reverse_iterator\n";
    }

    constexpr int default_space = 8;
    constexpr int test_value = 5;
    constexpr int test_size = 3;
    constexpr int test_size_2 = 10;
    { // Constructor testing block.
        // Default constructor
        vector<int> v1;
        if (v1.size() != 0 || v1.capacity() != default_space ||
            v1.get_allocator() != allocator<int>{})
        {
            ++fail_count;
            std::cout << "FAILED vector()\n";
        }
        // Constructor with allocator
        vector<int> v2 {allocator<int>{}};
        if (v2.size() != 0 || v2.capacity() != default_space ||
            v2.get_allocator() != allocator<int>{})
        {
            ++fail_count;
            std::cout << "FAILED vector(allocator())\n";
        }
        // Constructor with allocator for different type
        vector<int> v3 {allocator<char>{}};
        if (v3.size() != 0 || v3.capacity() != default_space ||
            v3.get_allocator() != allocator<int>{})
        {
            ++fail_count;
            std::cout << "FAILED vector<int>(allocator<char>())\n";
        }
        // Construct with number of elements and value.
        vector<int> v4 (test_size, test_value);
        bool all_equal = true;
        for (const int& i : v4)
            if (i != test_value)
            {
                all_equal = false;
                break;
            }
        if (v4.size() != test_size || v4.capacity() != default_space ||
            v4.get_allocator() != allocator<int>{} || !all_equal )
        {
            ++fail_count;
            std::cout << "FAILED vector(size_t, int)\n";
        }
        // Construct with number of elements exceeding default capacity and
        // value.
        vector<int> v5 (test_size_2, test_value);
        all_equal = true;
        for (const int& i : v5)
            if (i != test_value)
            {
                all_equal = false;
                break;
            }
        if (v5.size() != test_size_2 || v5.capacity() != v5.size() ||
            v5.get_allocator() != allocator<int>{} || !all_equal )
        {
            ++fail_count;
            std::cout << "FAILED vector(size_t, int) larger than default capacity\n";
        }
        // Construct with number of elements, using default value.
        vector<int> v6 (test_size);
        all_equal = true;
        for (const int& i : v6)
            if (i != int{})
            {
                all_equal = false;
                break;
            }
        if (v6.size() != test_size || v6.capacity() != default_space ||
            v6.get_allocator() != allocator<int>{} || !all_equal )
        {
            ++fail_count;
            std::cout << "FAILED vector(size_t)\n";
        }
        // Construct from initializer_list.
        vector<int> v7 {1, 2, 3};
        if (v7.size() != 3 || v7.capacity() != default_space ||
            v7.get_allocator() != allocator<int>() || v7[0] != 1 ||
            v7[1] != 2 || v7[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector(initializer_list)\n";
        }
        // Construct from iterator range.
        vector<int> v8 {1, 2, 3};
        vector<int> v9 {++v8.begin(), v8.end()};
        if (v9.size() != 2 || v9.capacity() != default_space ||
            v9.get_allocator() != allocator<int>() || v9[0] != 2 ||
            v9[1] != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector(iterator, iterator)\n";
        }
        // Copy constructor.
        vector<int> v10 {1, 2};
        vector<int> v11 {v10};
        if (v11.size() != 2 || v11.capacity() != default_space ||
            v11.get_allocator() != allocator<int>() || v11[0] != 1 ||
            v11[1] != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector(vector)\n";
        }
        // Copy constructor with allocator.
        vector<int> v12 {1, 2};
        vector<int> v13 {v12, allocator<int>{}};
        if (v13.size() != 2 || v13.capacity() != default_space ||
            v13.get_allocator() != allocator<int>() || v13[0] != 1 ||
            v13[1] != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector(const vector&, allocator)\n";
        }
        // Move constructor.
        vector<int> v14 {move(vector<int>{1, 2})};
        if (v14.size() != 2 || v14.capacity() != default_space ||
            v14.get_allocator() != allocator<int>() || v14[0] != 1 ||
            v14[1] != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector(vector&&)\n";
        }
        // Move constructor with allocator.
        vector<int> v15 {move(vector<int>{1, 2}), allocator<int>{}};
        if (v15.size() != 2 || v15.capacity() != default_space ||
            v15.get_allocator() != allocator<int>() || v15[0] != 1 ||
            v15[1] != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector(vector&&, allocator)\n";
        }
    } // End constructor testing block.
    { // Assignment block
        // Copy assignment.
        vector<int> v1 {1, 2};
        vector<int> v2 {3, 4, 5};
        v2 = v1;
        if (v2.size() != 2 || v2.capacity() != default_space ||
            v2.get_allocator() != allocator<int>() || v2[0] != 1 ||
            v2[1] != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector::operator=(const vector&)\n";
        }
        // Move assignment.
        vector<int> v3 {3, 4, 5};
        v3 = vector<int>{1, 2};
        if (v3.size() != 2 || v3.capacity() != default_space ||
            v3.get_allocator() != allocator<int>() || v3[0] != 1 ||
            v3[1] != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector::operator=(vector&&)\n";
        }
        // Initializer list assignment.
        vector<int> v4 {3, 4, 5};
        v4 = {1, 2};
        if (v4.size() != 2 || v4.capacity() != default_space ||
            v4.get_allocator() != allocator<int>() || v4[0] != 1 ||
            v4[1] != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector::operator=(initializer_list)\n";
        }
        // Assign value and number.
        vector<int> v5 {1, 2, 3};
        v5.assign(2, 4);
        if (v5.size() != 2 || v5.capacity() != default_space ||
            v5.get_allocator() != allocator<int>() || v5[0] != 4 ||
            v5[1] != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::assign(count, value)\n";
        }
        // Assign range.
        vector<int> v6 {1, 2, 3};
        vector<int> v7 {4, 5, 6};
        v6.assign(++v7.begin(), v7.end());
        if (v6.size() != 2 || v6.capacity() != default_space ||
            v6.get_allocator() != allocator<int>() || v6[0] != 5 ||
            v6[1] != 6)
        {
            ++fail_count;
            std::cout << "FAILED vector::assign(iterator, iterator)\n";
        }
        // Assign initializer_list.
        vector<int> v8 {1, 2, 3};
        v8.assign({5, 6});
        if (v8.size() != 2 || v8.capacity() != default_space ||
            v8.get_allocator() != allocator<int>() || v8[0] != 5 ||
            v8[1] != 6)
        {
            ++fail_count;
            std::cout << "FAILED vector::assign(initializer_list)\n";
        }
        // get_allocator
        vector<int> v9;
        allocator<int> a1 = v9.get_allocator();
        if (a1 != allocator<int>{})
        {
            ++fail_count;
            std::cout << "FAILED vector::assign(initializer_list)\n";
        }
    } // End assignment block.
    { // Data access block.
        // operator[] read
        const vector<int> v1 = {0, 1, 2};
        if (v1[1] != 1)
        {
            ++fail_count;
            std::cout << "FAILED vector::operator[] read\n";
        }
        // operator[] write
        vector<int> v2 = {0, 1, 2};
        v2[1] = 3;
        if (v2[1] != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::operator[] write\n";
        }
        // Front read
        const vector<int> v3 = {0, 1, 2};
        if (v3.front() != 0)
        {
            ++fail_count;
            std::cout << "FAILED vector::front() read\n";
        }
        // Front write
        vector<int> v4 = {0, 1, 2};
        v4.front() = 3;
        if (v4.front() != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::front() write\n";
        }
        // Back read
        const vector<int> v5 = {0, 1, 2};
        if (v5.back() != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector::back() read\n";
        }
        // Back write
        vector<int> v6 = {0, 1, 2};
        v6.back() = 3;
        if (v6.back() != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::back() write\n";
        }
        // Data read
        const vector<int> v7 = {0, 1, 2};
        if (v7.data()[0] != 0 || v7.data()[1] != 1 || v7.data()[2] != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector::data() read\n";
        }
        // Data write
        vector<int> v8 = {0, 1, 2};
        v8.data()[0] = 3;
        v8.data()[1] = 4;
        v8.data()[2] = 5;
        if (v8.data()[0] != 3 || v8.data()[1] != 4 || v8.data()[2] != 5)
        {
            ++fail_count;
            std::cout << "FAILED vector::data() write\n";
        }
        // Data empty
        vector<int> v9;
        if (v9.data() != nullptr)
        {
            ++fail_count;
            std::cout << "FAILED vector::data() empty\n";
        }
    } // End data access block
    { // Iterator block
        // begin() read
        const vector<int> v1 {1, 2, 3};
        if (*v1.begin() != 1)
        {
            ++fail_count;
            std::cout << "FAILED vector::begin() read\n";
        }
        // cbegin() read
        const vector<int> v2 {1, 2, 3};
        if (*v2.cbegin() != 1)
        {
            ++fail_count;
            std::cout << "FAILED vector::cbegin() read\n";
        }
        // begin() write
        vector<int> v3 {1, 2, 3};
        *v3.begin() = 4;
        if (*v3.begin() != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::begin() write\n";
        }
        // end() read
        const vector<int> v4 {1, 2, 3};
        if (*(v4.end() - 1) != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::end() read\n";
        }
        // cend() read
        const vector<int> v5 {1, 2, 3};
        if (*(v5.cend() - 1) != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::cend() read\n";
        }
        // end() write
        vector<int> v6 {1, 2, 3};
        *(v6.end() - 1) = 4;
        if (*(v6.end() - 1) != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::end() write\n";
        }
        // rbegin() read
        const vector<int> v7 {1, 2, 3};
        if (*v7.rbegin() != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::rbegin() read\n";
        }
        // crbegin() read
        const vector<int> v8 {1, 2, 3};
        if (*v8.crbegin() != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::crbegin() read\n";
        }
        // rbegin() write
        vector<int> v9 {1, 2, 3};
        *v9.rbegin() = 4;
        if (*v9.rbegin() != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::rbegin() write\n";
        }
        // rend() read
        const vector<int> v10 {1, 2, 3};
        if (*(v10.rend() - 1) != 1)
        {
            ++fail_count;
            std::cout << "FAILED vector::rend() read\n";
        }
        // crend() read
        const vector<int> v11 {1, 2, 3};
        if (*(v11.crend() - 1) != 1)
        {
            ++fail_count;
            std::cout << "FAILED vector::crend() read\n";
        }
        // rend() write
        vector<int> v12 {1, 2, 3};
        *(v12.rend() - 1) = 4;
        if (*(v12.rend() - 1) != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::rend() write\n";
        }
    } // End iterator block
    { // Capacity block
        // empty
        vector<int> v1 {};
        if (!v1.empty())
        {
            ++fail_count;
            std::cout << "FAILED vector::empty() when empty\n";
        }
        vector<int> v2 {1, 2, 3};
        if (v2.empty())
        {
            ++fail_count;
            std::cout << "FAILED vector::empty() when not empty\n";
        }
        // size
        vector<int> v3 {};
        if (v3.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED vector::size() when 0\n";
        }
        vector<int> v4 {1, 2, 3};
        if (v4.size() != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::size() when 3\n";
        }
        // max_size
        vector<int> v5 {};
        if(v5.max_size() != numeric_limits<size_t>::max() / sizeof(int))
        {
            ++fail_count;
            std::cout << "FAILED vector::max_size()\n";
        }
        // capacity
        vector<int> v6 = {1, 2, 3};
        if (v6.capacity() != default_space)
        {
            ++fail_count;
            std::cout << "FAILED vector::capacity() with default\n";
        }
        vector<int> v7 = (10);
        if (v7.capacity() != 10)
        {
            ++fail_count;
            std::cout << "FAILED vector::capacity() with 10\n";
        }
        // reserve
        vector<int> v8 {};
        v8.reserve(10);
        if (v8.capacity() != 10)
        {
            ++fail_count;
            std::cout << "FAILED vector::reserve()\n";
        }
        // shrink_to_fit
        vector<int> v9 {1, 2, 3};
        v9.shrink_to_fit();
        if (v9.capacity() != 3 || v9[0] != 1 || v9[1] != 2 || v9[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::shrink_to_fit() with 3\n";
        }
        vector<int> v10 {};
        v10.shrink_to_fit();
        if (v10.capacity() != 0 || !v10.empty())
        {
            ++fail_count;
            std::cout << "FAILED vector::shrink_to_fit() empty\n";
        }
    } // End capacity block
    { // Modifier block
        // clear
        vector<int> v1 {1, 2, 3};
        v1.clear();
        if (!v1.empty())
        {
            ++fail_count;
            std::cout << "FAILED vector::clear() when not empty\n";
        }
        vector<int> v2 {};
        v2.clear();
        if (!v2.empty())
        {
            ++fail_count;
            std::cout << "FAILED vector::clear() when empty\n";
        }
        // insert, single element, beginning, copy
        vector<int> v3 {1, 2};
        int i1 = 4;
        auto it1 = v3.insert(v3.begin(), i1);
        if (v3.size() != 3 || v3[0] != 4 || v3[1] != 1 || v3[2] != 2 ||
            *it1 != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::insert(iterator, const int&) beginning\n";
        }
        // insert, single element, middle, copy
        vector<int> v4 {1, 2};
        int i2 = 4;
        auto it2 = v4.insert(v4.begin() + 1, i2);
        if (v4.size() != 3 || v4[0] != 1 || v4[1] != 4 || v4[2] != 2 ||
            *it2 != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::insert(iterator, const int&) middle\n";
        }
        // insert, single element, end, copy
        vector<int> v5 {1, 2};
        int i3 = 4;
        auto it3 = v5.insert(v5.end(), i3);
        if (v5.size() != 3 || v5[0] != 1 || v5[1] != 2 || v5[2] != 4 ||
            *it3 != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::insert(iterator, const int&) end\n";
        }
        // insert, single element, end, copy, causing a reserve
        vector<int> v11 {1, 2, 3, 4, 5, 6, 7, 8};
        int i4 = 9;
        auto it4 = v11.insert(v11.end(), i4);
        if (v11.size() != 9 || v11[0] != 1 || v11[1] != 2 || v11[2] != 3 ||
            v11[3] != 4 || v11[4] != 5 || v11[5] != 6 || v11[6] != 7 ||
            v11[7] != 8 || v11[8] != 9 || *it4 != 9)
        {
            ++fail_count;
            std::cout << "FAILED vector::insert(iterator, const int&) end reserve\n";
        }
        // insert, single element, beginning, move
        vector<int> v6 {1, 2};
        auto it5 = v6.insert(v6.begin(), 4);
        if (v6.size() != 3 || v6[0] != 4 || v6[1] != 1 || v6[2] != 2 ||
            *it5 != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::insert(iterator, int&&) beginning\n";
        }
        // insert, single element, middle, move
        vector<int> v7 {1, 2};
        auto it6 = v7.insert(v7.begin() + 1, 4);
        if (v7.size() != 3 || v7[0] != 1 || v7[1] != 4 || v7[2] != 2 ||
            *it6 != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::insert(iterator, int&&) middle\n";
        }
        // insert, single element, end, move
        vector<int> v8 {1, 2};
        auto it7 = v8.insert(v8.end(), 4);
        if (v8.size() != 3 || v8[0] != 1 || v8[1] != 2 || v8[2] != 4 ||
            *it7 != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::insert(iterator, int&&) end\n";
        }
        // insert, multiple copies, small number
        vector<int> v9 {1, 2};
        auto it8 = v9.insert(v9.begin() + 1, 3, 4);
        if (v9.size() != 5 || v9[0] != 1 || v9[1] != 4 || v9[2] != 4 ||
            v9[3] != 4 || v9[4] != 2 || *it8 != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::insert(iterator, size_t, const int&)\n";
        }
        // insert, multiple copies, enough to cause a reserve
        vector<int> v10 {1, 2};
        auto it9 = v10.insert(v10.begin() + 1, 7, 4);
        if (v10.size() != 9 || v10[0] != 1 || v10[1] != 4 || v10[2] != 4 ||
            v10[3] != 4 || v10[4] != 4 || v10[5] != 4 || v10[6] != 4 ||
            v10[7] != 4 || v10[8] != 2 || *it9 != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::insert(iterator, size_t, const int&) reserve\n";
        }
        // insert a small range
        vector<int> v12 {1, 2};
        vector<int> v13 {3, 4};
        auto it10 = v12.insert(v12.begin() + 1, v13.begin(), v13.end());
        if (v12.size() != 4 || v12[0] != 1 || v12[1] != 3 || v12[2] != 4 ||
            v12[3] != 2 || *it10 != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::insert(iterator, iterator, iterator)\n";
        }
        // insert a range, long enough to cause a reserve
        vector<int> v14 {1, 2};
        vector<int> v15 {3, 4, 5, 6, 7, 8, 9};
        auto it11 = v14.insert(v14.begin() + 1, v15.begin(), v15.end());
        if (v14.size() != 9 || v14[0] != 1 || v14[1] != 3 || v14[2] != 4 ||
            v14[3] != 5 || v14[4] != 6 || v14[5] != 7 || v14[6] != 8 ||
            v14[7] != 9 || v14[8] != 2 || *it11 != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::insert(iterator, iterator, iterator) reserve\n";
        }
        // emplace
        vector<int> v16 {1, 2};
        auto it12 = v16.emplace(v16.begin() + 1, 4);
        if (v16.size() != 3 || v16[0] != 1 || v16[1] != 4 || v16[2] != 2 ||
            *it12 != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::emplace(iterator, int)\n";
        }
        // erase, single, start
        vector<int> v17 {1, 2, 3};
        auto it13 = v17.erase(v17.begin());
        if (v17.size() != 2 || v17[0] != 2 || v17[1] != 3 || *it13 != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector::erase(iterator) start\n";
        }
        // erase, single, middle
        vector<int> v18 {1, 2, 3};
        auto it14 = v18.erase(v18.begin() + 1);
        if (v18.size() != 2 || v18[0] != 1 || v18[1] != 3 || *it14 != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::erase(iterator) middle\n";
        }
        // erase, single, end
        vector<int> v19 {1, 2, 3};
        auto it15 = v19.erase(v19.begin() + 2);
        if (v19.size() != 2 || v19[0] != 1 || v19[1] != 2 || it15 != v19.end())
        {
            ++fail_count;
            std::cout << "FAILED vector::erase(iterator) end\n";
        }
        // erase, range, start
        vector<int> v20 {1, 2, 3, 4};
        auto it16 = v20.erase(v20.begin(), v20.begin() + 2);
        if (v20.size() != 2 || v20[0] != 3 || v20[1] != 4 || *it16 != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::erase(iterator, iterator) start\n";
        }
        // erase, range, middle
        vector<int> v21 {1, 2, 3, 4};
        auto it17 = v21.erase(v21.begin() + 1, v21.begin() + 3);
        if (v21.size() != 2 || v21[0] != 1 || v21[1] != 4 || *it17 != 4)
        {
            ++fail_count;
            std::cout << "FAILED vector::erase(iterator, iterator) middle\n";
        }
        // erase, range, end
        vector<int> v22 {1, 2, 3, 4};
        auto it18 = v22.erase(v22.begin() + 2, v22.begin() + 4);
        if (v22.size() != 2 || v22[0] != 1 || v22[1] != 2 || it18 != v22.end())
        {
            ++fail_count;
            std::cout << "FAILED vector::erase(iterator, iterator) end\n";
        }
        // push_back, empty, move
        vector<int> v23 {};
        v23.push_back(1);
        if (v23.size() != 1 || v23[0] != 1)
        {
            ++fail_count;
            std::cout << "FAILED vector::push_back(int) empty move\n";
        }
        // push_back, small, move
        vector<int> v24 {1, 2};
        v24.push_back(3);
        if (v24.size() != 3 || v24[0] != 1 || v24[1] != 2 || v24[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::push_back(int) small move\n";
        }
        // push_back, causing a reserve, move
        vector<int> v25 {1, 2, 3, 4, 5, 6, 7, 8};
        v25.push_back(9);
        if (v25.size() != 9 || v25[0] != 1 || v25[1] != 2 || v25[2] != 3 ||
            v25[3] != 4 || v25[4] != 5 || v25[5] != 6 || v25[6] != 7 ||
            v25[7] != 8 || v25[8] != 9)
        {
            ++fail_count;
            std::cout << "FAILED vector::push_back(int) reserve move\n";
        }
        // push_back, empty, copy
        vector<int> v26 {};
        int i5 = 1;
        v26.push_back(i5);
        if (v26.size() != 1 || v26[0] != 1)
        {
            ++fail_count;
            std::cout << "FAILED vector::push_back(int) empty copy\n";
        }
        // push_back, small, copy
        vector<int> v27 {1, 2};
        int i6 = 3;
        v27.push_back(i6);
        if (v27.size() != 3 || v27[0] != 1 || v27[1] != 2 || v27[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::push_back(int) small copy\n";
        }
        // push_back, causing a reserve, copy
        vector<int> v28 {1, 2, 3, 4, 5, 6, 7, 8};
        int i7 = 9;
        v28.push_back(i7);
        if (v28.size() != 9 || v28[0] != 1 || v28[1] != 2 || v28[2] != 3 ||
            v28[3] != 4 || v28[4] != 5 || v28[5] != 6 || v28[6] != 7 ||
            v28[7] != 8 || v28[8] != 9)
        {
            ++fail_count;
            std::cout << "FAILED vector::push_back(int) reserve copy\n";
        }
        // emplace_back
        vector<int> v29 {1, 2};
        v29.emplace_back(3);
        if (v29.size() != 3 || v29[0] != 1 || v29[1] != 2 || v29[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::emplace_back(int)\n";
        }
        // pop_back
        vector<int> v30 {1, 2, 3};
        v30.pop_back();
        if (v30.size() != 2 || v30[0] != 1 || v30[1] != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector::pop_back()\n";
        }
        // resize, reduce
        vector<int> v31 {1, 2, 3};
        v31.resize(2);
        if (v31.size() != 2 || v31[0] != 1 || v31[1] != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector::resize() reduce\n";
        }
        // resize, reduce to zero
        vector<int> v32 {1, 2, 3};
        v32.resize(0);
        if (v32.size() != 0 || !v32.empty())
        {
            ++fail_count;
            std::cout << "FAILED vector::resize() reduce to zero\n";
        }
        // resize, increase default value
        vector<int> v33 {1, 2};
        v33.resize(3);
        if (v33.size() != 3 || v33[0] != 1 || v33[1] != 2 || v33[2] != int{})
        {
            ++fail_count;
            std::cout << "FAILED vector::resize() increase default\n";
        }
        // resize, increase custom value
        vector<int> v34 {1, 2};
        v34.resize(4, 3);
        if (v34.size() != 4 || v34[0] != 1 || v34[1] != 2 || v34[2] != 3 ||
            v34[3] != 3)
        {
            ++fail_count;
            std::cout << "FAILED vector::resize() increase custom\n";
        }
        // resize, increase default value, cause a reserve
        vector<int> v35 {1, 2};
        v35.resize(9);
        if (v35.size() != 9 || v35[0] != 1 || v35[1] != 2 || v35[2] != int{} ||
            v35[3] != int{} || v35[4] != int{} || v35[5] != int{} ||
            v35[6] != int{} || v35[7] != int{} || v35[8] != int{})
        {
            ++fail_count;
            std::cout << "FAILED vector::resize() increase default reserve\n";
        }
        // swap
        vector<int> v36 {1, 2};
        vector<int> v37 {3, 4, 5};
        v36.swap(v37);
        if (v36.size() != 3 || v36[0] != 3 || v36[1] != 4 || v36[2] != 5 ||
            v37.size() != 2 || v37[0] != 1 || v37[1] != 2)
        {
            ++fail_count;
            std::cout << "FAILED vector::swap()\n";
        }
        // non-member swap
        vector<int> v38 {1, 2};
        vector<int> v39 {3, 4, 5};
        swap(v38, v39);
        if (v38.size() != 3 || v38[0] != 3 || v38[1] != 4 || v38[2] != 5 ||
            v39.size() != 2 || v39[0] != 1 || v39[1] != 2)
        {
            ++fail_count;
            std::cout << "FAILED swap() vectors\n";
        }
    } // End modifier block
    { // operator block
        // operator==
        vector<int> v1 {1, 2, 3};
        vector<int> v2 {1, 2, 3};
        vector<int> v3 {1, 2};
        if (!(v1 == v2))
        {
            ++fail_count;
            std::cout << "FAILED vector == vector, equal\n";
        }
        if (v1 == v3)
        {
            ++fail_count;
            std::cout << "FAILED vector == vector, not equal\n";
        }
        // operator!=
        if (v1 != v2)
        {
            ++fail_count;
            std::cout << "FAILED vector != vector, equal\n";
        }
        if (!(v1 != v3))
        {
            ++fail_count;
            std::cout << "FAILED vector != vector, not equal\n";
        }
        // operator<
        vector<int> v4 {1, 2, 3};
        vector<int> v5 {1, 2, 3};
        vector<int> v6 {1, 2};
        vector<int> v7 {};
        vector<int> v8 {2, 3, 4};
        if (v4 < v5)
        {
            ++fail_count;
            std::cout << "FAILED vector < vector, equal\n";
        }
        if (v4 < v6)
        {
            ++fail_count;
            std::cout << "FAILED vector < vector, second prequel\n";
        }
        if (!(v6 < v4))
        {
            ++fail_count;
            std::cout << "FAILED vector < vector, first prequel\n";
        }
        if (v4 < v7)
        {
            ++fail_count;
            std::cout << "FAILED vector < vector, second empty\n";
        }
        if (!(v7 < v4))
        {
            ++fail_count;
            std::cout << "FAILED vector < vector, first empty\n";
        }
        if (v7 < v7)
        {
            ++fail_count;
            std::cout << "FAILED vector < vector, both empty\n";
        }
        if (!(v4 < v8))
        {
            ++fail_count;
            std::cout << "FAILED vector < vector, first element-wise less\n";
        }
        if (v8 < v4)
        {
            ++fail_count;
            std::cout << "FAILED vector < vector, second element-wise less\n";
        }
        // operator <=
        if (!(v4 <= v5))
        {
            ++fail_count;
            std::cout << "FAILED vector <= vector, equal\n";
        }
        if (v4 <= v6)
        {
            ++fail_count;
            std::cout << "FAILED vector <= vector, second prequel\n";
        }
        if (!(v6 <= v4))
        {
            ++fail_count;
            std::cout << "FAILED vector <= vector, first prequel\n";
        }
        if (v4 <= v7)
        {
            ++fail_count;
            std::cout << "FAILED vector <= vector, second empty\n";
        }
        if (!(v7 <= v4))
        {
            ++fail_count;
            std::cout << "FAILED vector <= vector, first empty\n";
        }
        if (!(v7 <= v7))
        {
            ++fail_count;
            std::cout << "FAILED vector <= vector, both empty\n";
        }
        if (!(v4 <= v8))
        {
            ++fail_count;
            std::cout << "FAILED vector <= vector, first element-wise less\n";
        }
        if (v8 <= v4)
        {
            ++fail_count;
            std::cout << "FAILED vector <= vector, second element-wise less\n";
        }
        // operator >
        if (v4 > v5)
        {
            ++fail_count;
            std::cout << "FAILED vector > vector, equal\n";
        }
        if (!(v4 > v6))
        {
            ++fail_count;
            std::cout << "FAILED vector > vector, second prequel\n";
        }
        if (v6 > v4)
        {
            ++fail_count;
            std::cout << "FAILED vector > vector, first prequel\n";
        }
        if (!(v4 > v7))
        {
            ++fail_count;
            std::cout << "FAILED vector > vector, second empty\n";
        }
        if (v7 > v4)
        {
            ++fail_count;
            std::cout << "FAILED vector > vector, first empty\n";
        }
        if (v7 < v7)
        {
            ++fail_count;
            std::cout << "FAILED vector > vector, both empty\n";
        }
        if (v4 > v8)
        {
            ++fail_count;
            std::cout << "FAILED vector > vector, first element-wise less\n";
        }
        if (!(v8 > v4))
        {
            ++fail_count;
            std::cout << "FAILED vector > vector, second element-wise less\n";
        }
        // operator >=
        if (!(v4 >= v5))
        {
            ++fail_count;
            std::cout << "FAILED vector >= vector, equal\n";
        }
        if (!(v4 >= v6))
        {
            ++fail_count;
            std::cout << "FAILED vector >= vector, second prequel\n";
        }
        if (v6 >= v4)
        {
            ++fail_count;
            std::cout << "FAILED vector >= vector, first prequel\n";
        }
        if (!(v4 >= v7))
        {
            ++fail_count;
            std::cout << "FAILED vector >= vector, second empty\n";
        }
        if (v7 >= v4)
        {
            ++fail_count;
            std::cout << "FAILED vector >= vector, first empty\n";
        }
        if (!(v7 >= v7))
        {
            ++fail_count;
            std::cout << "FAILED vector >= vector, both empty\n";
        }
        if (v4 >= v8)
        {
            ++fail_count;
            std::cout << "FAILED vector >= vector, first element-wise less\n";
        }
        if (!(v8 >= v4))
        {
            ++fail_count;
            std::cout << "FAILED vector >= vector, second element-wise less\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
