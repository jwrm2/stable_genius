#include <iostream>

#include "../include/iterator"

#include "../include/vector"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // tags
    if (!is_class<output_iterator_tag>::value)
    {
        ++fail_count;
        std::cout << "FAILED output_iterator_tag is not a class\n";
    }
    if (!is_class<input_iterator_tag>::value)
    {
        ++fail_count;
        std::cout << "FAILED input_iterator_tag is not a class\n";
    }
    if (!is_class<forward_iterator_tag>::value)
    {
        ++fail_count;
        std::cout << "FAILED forward_iterator_tag is not a class\n";
    }
    if (!is_base_of<input_iterator_tag, forward_iterator_tag>::value)
    {
        ++fail_count;
        std::cout << "FAILED forward_iterator_tag is not an input_iterator_tag\n";
    }
    if (!is_class<bidirectional_iterator_tag>::value)
    {
        ++fail_count;
        std::cout << "FAILED bidirectional_iterator_tag is not a class\n";
    }
    if (!is_base_of<forward_iterator_tag, bidirectional_iterator_tag>::value)
    {
        ++fail_count;
        std::cout << "FAILED bidirectional_iterator_tag is not a forward_iterator_tag\n";
    }
    if (!is_class<random_access_iterator_tag>::value)
    {
        ++fail_count;
        std::cout << "FAILED random_access_iterator_tag is not a class\n";
    }
    if (!is_base_of<bidirectional_iterator_tag,
        random_access_iterator_tag>::value)
    {
        ++fail_count;
        std::cout << "FAILED random_access_iterator_tag is not a bidirectional_iterator_tag\n";
    }
    // iterator
    {
        using iterator_type = iterator<input_iterator_tag, int>;
        if (!is_same<typename iterator_type::iterator_category,
            input_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED iterator_type::iterator_category\n";
        }
        if (!is_same<typename iterator_type::value_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED iterator_type::value_type\n";
        }
        if (!is_same<typename iterator_type::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED iterator_type::difference_type\n";
        }
        if (!is_same<typename iterator_type::pointer, int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED iterator_type::pointer\n";
        }
        if (!is_same<typename iterator_type::reference, int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED iterator_type::reference\n";
        }
    }
    // iterator_traits
    {
        // using base iterator
        using it1 = iterator_traits<iterator<input_iterator_tag, int>>;
        if (!is_same<typename it1::iterator_category,
            input_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED it1::iterator_category\n";
        }
        if (!is_same<typename it1::value_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED it1::value_type\n";
        }
        if (!is_same<typename it1::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED it1::difference_type\n";
        }
        if (!is_same<typename it1::pointer, int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED it1::pointer\n";
        }
        if (!is_same<typename it1::reference, int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED it1::reference\n";
        }
        // using vector
        using it2 = vector<int>::iterator;
        if (!is_same<typename it2::iterator_category,
            random_access_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED it2::iterator_category\n";
        }
        if (!is_same<typename it2::value_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED it2::value_type\n";
        }
        if (!is_same<typename it2::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED it2::difference_type\n";
        }
        if (!is_same<typename it2::pointer, int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED it2::pointer\n";
        }
        if (!is_same<typename it2::reference, int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED it2::reference\n";
        }
        // using pointer
        using it3 = iterator_traits<int*>;
        if (!is_same<typename it3::iterator_category,
            random_access_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED it3::iterator_category\n";
        }
        if (!is_same<typename it3::value_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED it3::value_type\n";
        }
        if (!is_same<typename it3::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED it3::difference_type\n";
        }
        if (!is_same<typename it3::pointer, int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED it3::pointer\n";
        }
        if (!is_same<typename it3::reference, int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED it3::reference\n";
        }
        // using const pointer
        using it4 = iterator_traits<const int*>;
        if (!is_same<typename it4::iterator_category,
            random_access_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED it4::iterator_category\n";
        }
        if (!is_same<typename it4::value_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED it4::value_type\n";
        }
        if (!is_same<typename it4::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED it4::difference_type\n";
        }
        if (!is_same<typename it4::pointer, const int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED it4::pointer\n";
        }
        if (!is_same<typename it4::reference, const int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED it4::reference\n";
        }
        // using pointer to const
        using it5 = iterator_traits<int* const>;
        if (!is_same<typename it5::iterator_category,
            random_access_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED it5::iterator_category\n";
        }
        if (!is_same<typename it5::value_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED it5::value_type\n";
        }
        if (!is_same<typename it5::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED it5::difference_type\n";
        }
        if (!is_same<typename it5::pointer, int* const>::value)
        {
            ++fail_count;
            std::cout << "FAILED it5::pointer\n";
        }
        if (!is_same<typename it5::reference, const int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED it5::reference\n";
        }
        // using const pointer to const
        using it6 = iterator_traits<const int* const>;
        if (!is_same<typename it6::iterator_category,
            random_access_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED it6::iterator_category\n";
        }
        if (!is_same<typename it6::value_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED it6::value_type\n";
        }
        if (!is_same<typename it6::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED it6::difference_type\n";
        }
        if (!is_same<typename it6::pointer, const int* const>::value)
        {
            ++fail_count;
            std::cout << "FAILED it6::pointer\n";
        }
        if (!is_same<typename it6::reference, const int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED it6::reference\n";
        }
    }
    // reverse_iterator
    {
        // traits
        using it_type = reverse_iterator<typename vector<int>::iterator>;
        if (!is_same<typename it_type::iterator_category,
            random_access_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator::iterator_category\n";
        }
        if (!is_same<typename it_type::value_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator::value_type\n";
        }
        if (!is_same<typename it_type::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator::difference_type\n";
        }
        if (!is_same<typename it_type::pointer, int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator::pointer\n";
        }
        if (!is_same<typename it_type::reference, int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator::reference\n";
        }
        // default constructor
        it_type it1;
        if (it1.base() != vector<int>::iterator())
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator default constructor\n";
        }
        // construct from iterator
        vector<int> v1 {3, 4, 5};
        it_type it2 {v1.begin() + 1};
        if (*it2 != 3)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator construct from iterator\n";
        }
        // copy constructor
        vector<int> v2 {3, 4, 5};
        it_type it3 {v2.begin() + 1};
        it_type it4 {it3};
        if (*it4 != 3)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator copy constructor\n";
        }
        // base
        vector<int> v3 {3, 4, 5};
        it_type it5 {v3.begin() + 1};
        if (it5.base() != v3.begin() + 1)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator base()\n";
        }
        // operator*
        vector<int> v4 {3, 4, 5};
        it_type it6 {v4.end()};
        if (*it6 != 5)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator*\n";
        }
        // operator->
        struct dereference_test_type { int i; };
        dereference_test_type dtt1{3}, dtt2{4}, dtt3{5};
        vector<dereference_test_type> v5 {dtt1, dtt2, dtt3};
        reverse_iterator<typename vector<dereference_test_type>::iterator> it7
            {v5.end()};
        if (it7->i != 5)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator->\n";
        }
        // operator++
        vector<int> v6 {3, 4, 5};
        it_type it8 {v6.end()};
        it_type it9 = ++it8;
        if (*it8 != 4 || *it9 != 4)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator pre operator++\n";
        }
        vector<int> v7 {3, 4, 5};
        it_type it10 {v7.end()};
        it_type it11 = it10++;
        if (*it10 != 4 || *it11 != 5)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator post operator++\n";
        }
        // operator--
        vector<int> v8 {3, 4, 5};
        it_type it12 {v8.begin() + 1};
        it_type it13 = --it12;
        if (*it12 != 4 || *it13 != 4)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator pre operator--\n";
        }
        vector<int> v9 {3, 4, 5};
        it_type it14 {v9.begin() + 1};
        it_type it15 = it14--;
        if (*it14 != 4 || *it15 != 3)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator post operator--\n";
        }
        // operator+
        vector<int> v10 {3, 4, 5};
        it_type it16 {v10.end()};
        it_type it17 = it16 + 2;
        if (*it17 != 3)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator + offset\n";
        }
        vector<int> v11 {3, 4, 5};
        it_type it18 {v11.end()};
        it_type it19 = 2 + it18;
        if (*it19 != 3)
        {
            ++fail_count;
            std::cout << "FAILED offset + reverse_iterator\n";
        }
        // operator+=
        vector<int> v12 {3, 4, 5};
        it_type it20 {v12.end()};
        it20 += 2;
        if (*it20 != 3)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator+=\n";
        }
        // operator-
        vector<int> v13 {3, 4, 5};
        it_type it21 {v13.begin() + 1};
        it_type it22 = it21 - 2;
        if (*it22 != 5)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator - offset\n";
        }
        if (it21 - it22 != 2)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator - reverse_iterator\n";
        }
        // operator-=
        vector<int> v14 {3, 4, 5};
        it_type it23 {v14.begin() + 1};
        it23 -= 2;
        if (*it23 != 5)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator-=\n";
        }
        // operator[]
        vector<int> v15 {3, 4, 5};
        it_type it24 {v15.end()};
        if (it24[0] != 5 || it24[1] != 4 || it24[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator[]\n";
        }
        // operator==
        vector<int> v16 {3, 4, 5};
        it_type it25 {v16.end()};
        it_type it26 {v16.end()};
        it_type it27 {v16.begin() + 1};
        if (!(it25 == it26))
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator== equal\n";
        }
        if (it25 == it27)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator== not equal\n";
        }
        // operator!=
        vector<int> v17 {3, 4, 5};
        it_type it28 {v17.end()};
        it_type it29 {v17.end()};
        it_type it30 {v17.begin() + 1};
        if (it28 != it29)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator!= equal\n";
        }
        if (!(it28 != it30))
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator!= not equal\n";
        }
        // operator<
        vector<int> v18 {3, 4, 5};
        it_type it31 {v18.end()};
        it_type it32 {v18.end() - 1};
        it_type it33 {v18.end() - 1};
        if (!(it31 < it32))
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator< less than\n";
        }
        if (it32 < it31)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator< greater than\n";
        }
        if (it32 < it33)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator< equal\n";
        }
        // operator<=
        vector<int> v19 {3, 4, 5};
        it_type it34 {v19.end()};
        it_type it35 {v19.end() - 1};
        it_type it36 {v19.end() - 1};
        if (!(it34 <= it35))
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator<= less than\n";
        }
        if (it35 <= it34)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator<= greater than\n";
        }
        if (!(it35 <= it36))
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator<= equal\n";
        }
        // operator>
        vector<int> v20 {3, 4, 5};
        it_type it37 {v20.end()};
        it_type it38 {v20.end() - 1};
        it_type it39 {v20.end() - 1};
        if (it37 > it38)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator> less than\n";
        }
        if (!(it38 > it37))
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator> greater than\n";
        }
        if (it38 > it39)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator> equal\n";
        }
        // operator>=
        vector<int> v21 {3, 4, 5};
        it_type it40 {v21.end()};
        it_type it41 {v21.end() - 1};
        it_type it42 {v21.end() - 1};
        if (it40 >= it41)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator>= less than\n";
        }
        if (!(it41 >= it40))
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator>= greater than\n";
        }
        if (!(it41 >= it42))
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator operator>= equal\n";
        }
        // initializer_list
        reverse_iterator<std::initializer_list<int>::iterator> it43 = 
            rbegin({3, 4, 5});
        if (*it43 != 5)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator initializer_list rbegin\n";
        }
        // initializer_list
        reverse_iterator<std::initializer_list<int>::iterator> it44 = 
            rend({3, 4, 5});
        if (*(--it44) != 3)
        {
            ++fail_count;
            std::cout << "FAILED reverse_iterator initializer_list rend\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
