#include <chrono>
#include <iostream>
#include <random>

#include "../include/functional"
#include "../include/memory"
#include "../include/rbtree"
#include "../include/type_traits"

using namespace klib;
using namespace klib::helper;

// Make a test tree with n random int values between +/- x. Uses the move
// constructor, so check that without using this function.
rb_tree<int> make_test(size_t n, int x)
{
    x = (x < 0 ? -x : x);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine gen {seed};
    std::uniform_int_distribution<int> dist(-x, x);
    rb_tree<int> t {};

    while (n-- != 0)
        t.insert(dist(gen));

    return t;
}

// Test type that's a little more complicated than just an int.
class test_type {
public:
    int val;

    explicit test_type(int v) : val{v} {}

    // Provide a less than so the default comparator works.
    bool operator<(const test_type& other) const { return val < other.val; }
};

// Allow printing.
/*std::ostream& operator<<(std::ostream& os, const test_type& t)
{
    os << t.val;
    return os;
}*/

// Comparator for test_type, a little more complicated than using the default.
class test_comparator {
public:
    bool operator()(const test_type& lhs, const test_type& rhs) const
    {
        return lhs < rhs;
    }

    // Make this 'transparent' and allow direct comparison of ints and
    // test_types.
    struct is_transparent;

    bool operator()(const test_type& lhs, int rhs) const
    {
        return lhs.val < rhs;
    }

    bool operator()(int lhs, const test_type& rhs) const
    {
        return lhs < rhs.val;
    }
};

// Test type that does a heap allocation.
class test_type_2 {
public:
    int* val;

    explicit test_type_2(int v) : val{new int {v}} {}

    test_type_2(const test_type_2& other) : val{new int {*other.val}} {}

    test_type_2(test_type_2&& other) : val {other.val} { other.val = nullptr; }

    ~test_type_2() { delete val; }

    test_type_2& operator=(const test_type_2& other)
    {
        delete val;
        val = new int {*other.val};
        return *this;
    }

    test_type_2& operator=(test_type_2&& other)
    {
        delete val;
        val = other.val;
        other.val = nullptr;
        return *this;
    }

    // Proved a less than so the default comparator works.
    bool operator<(const test_type_2& other) const { return *val < *other.val; }
};

// Allow printing.
/*std::ostream& operator<<(std::ostream& os, const test_type_2& t)
{
    os << *t.val;
    return os;
}*/

int main()
{
    size_t fail_count = 0;

    // Types
    {
        if (!is_same<rb_tree<int>::value_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::value_type\n";
        }
        if (!is_same<rb_tree<int>::size_type, size_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::size_type\n";
        }
        if (!is_same<rb_tree<int>::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::difference_type\n";
        }
        if (!is_same<rb_tree<int>::reference, int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reference\n";
        }
        if (!is_same<rb_tree<int>::const_reference, const int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_reference\n";
        }
        if (!is_same<rb_tree<int>::pointer, int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::pointer\n";
        }
        if (!is_same<rb_tree<int>::const_pointer, const int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_pointer\n";
        }
        if (!is_same<rb_tree<int>::iterator,
            rb_tree<int>::rb_iterator<true, int, int*, int&>>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator\n";
        }
        if (!is_same<rb_tree<int>::const_iterator,
            rb_tree<int>::rb_iterator<false, const int, const int*,
            const int&>>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_iterator\n";
        }
        if (!is_same<rb_tree<int>::reverse_iterator, reverse_iterator<
            rb_tree<int>::rb_iterator<true, int, int*, int&>>>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reverse_iterator\n";
        }
        if (!is_same<rb_tree<int>::const_reverse_iterator, reverse_iterator<
            rb_tree<int>::rb_iterator<false, const int, const int*,
            const int&>>>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_reverse_iterator\n";
        }
        if (!is_same<rb_tree<int>::allocator_type, allocator<int>>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::allocator\n";
        }
        if (!is_same<rb_tree<int>::value_compare, less<int>>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::value_compare\n";
        }
    }
    // Constructors.
    {
        // Default constructor.
        rb_tree<int> t1;
        if (!t1.empty() || t1.get_allocator() != allocator<int> {} ||
            t1.value_comp()(4, 2) || t1.check() == 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree()\n";
        }
        // Constructor with allocator.
        rb_tree<int> t2 {allocator<int>{}};
        if (!t2.empty() || t2.get_allocator() != allocator<int> {} ||
            t2.value_comp()(4, 2) || t2.check() == 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree(allocator<int>{})\n";
        }
        // Constructor with different allocator.
        rb_tree<int> t3 {allocator<char>{}};
        if (!t3.empty() || t3.get_allocator() != allocator<int> {} ||
            t3.value_comp()(4, 2) || t3.check() == 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree(allocator<char>{})\n";
        }
        // Constructor with comparator
        rb_tree<int, greater<int>> t4 {greater<int>{}};
        if (!t4.empty() || t4.get_allocator() != allocator<int> {} ||
            t4.value_comp()(2, 4) || t4.check() == 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree(greater<int>{})\n";
        }
        // Constructor with comparator and allocator
        rb_tree<int, greater<int>> t5 {greater<int>{}, allocator<int>{}};
        if (!t5.empty() || t5.get_allocator() != allocator<int> {} ||
            t5.value_comp()(2, 4) || t5.check() == 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree(greater<int>{}, allocator<int>{})\n";
        }
        // Copy constructor when empty.
        rb_tree<int> t6 {};
        rb_tree<int> t7 {t6};
        if (!t6.empty() || t6.get_allocator() != allocator<int> {} ||
            t6.value_comp()(4, 2) || t6.check() == 0 ||
            !t7.empty() || t7.get_allocator() != allocator<int> {} ||
            t7.value_comp()(4, 2) || t7.check() == 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree(const rb_tree&) empty\n";
        }
        // Move constructor when empty.
        rb_tree<int> t8 {};
        rb_tree<int> t9 {klib::move(t8)};
        if (!t9.empty() || t9.get_allocator() != allocator<int> {} ||
            t9.value_comp()(4, 2) || t9.check() == 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree(rb_tree&&) empty\n";
        }
        // Copy constructor when not empty.
        rb_tree<int> t10 {};
        t10.insert(10); t10.insert(19); t10.insert(10); t10.insert(13);
        t10.insert(29); t10.insert(9); t10.insert(12); t10.insert(18);
        rb_tree<int> t11 {t10};
        if (t11.empty() || t11.size() != 8 || t11.check() == 0 || 
            *t11.begin() != 9 || *(--t11.end()) != 29 ||
            t10.empty() || t10.size() != 8 || t10.check() == 0 || 
            *t10.begin() != 9 || *(--t10.end()) != 29)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree(const rb_tree&) not empty\n";
        }
        // Move constructor when not empty.
        rb_tree<int> t12 {};
        t12.insert(15); t12.insert(18); t12.insert(18); t12.insert(2);
        t12.insert(10); t12.insert(13); t12.insert(9); t12.insert(1);
        rb_tree<int> t13 {t12};
        if (t13.empty() || t13.size() != 8 || t13.check() == 0 || 
            *t13.begin() != 1 || *(--t13.end()) != 18)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree(rb_tree&&) not empty\n";
        }
        // Copy constructor with heap data type.
        rb_tree<test_type_2> t14 {};
        t14.emplace(10); t14.emplace(19); t14.emplace(10); t14.emplace(13);
        t14.emplace(29); t14.emplace(9); t14.emplace(12); t14.emplace(18);
        rb_tree<test_type_2> t15 {t14};
        if (t15.empty() || t15.size() != 8 || t15.check() == 0 || 
            *(*t15.begin()).val != 9 || *(*--t15.end()).val != 29 ||
            t14.empty() || t14.size() != 8 || t14.check() == 0 || 
            *(*t14.begin()).val != 9 || *(*--t14.end()).val != 29)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree(const rb_tree&) heap type\n";
        }
        // Move constructor when not empty.
        rb_tree<test_type_2> t16 {};
        t16.emplace(15); t16.emplace(18); t16.emplace(18); t16.emplace(2);
        t16.emplace(10); t16.emplace(13); t16.emplace(9); t16.emplace(1);
        rb_tree<test_type_2> t17 {klib::move(t16)};
        if (t17.empty() || t17.size() != 8 || t17.check() == 0 || 
            *(*t17.begin()).val != 1 || *(*--t17.end()).val != 18)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree(rb_tree&&) heap type\n";
        }
    }
    // Assignment.
    {
        rb_tree<int> t1;
        t1.insert(15); t1.insert(18); t1.insert(18); t1.insert(2);
        t1.insert(10); t1.insert(13); t1.insert(9); t1.insert(1);
        rb_tree<int> t2;
        t2 = t1;
        if (t1.empty() || t1.size() != 8 || t1.check() == 0 || 
            *t1.begin() != 1 || *(--t1.end()) != 18 ||
            t2.empty() || t2.size() != 8 || t2.check() == 0 || 
            *t2.begin() != 1 || *(--t2.end()) != 18)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree copy assignment not empty\n";
        }
        rb_tree<int> t3;
        rb_tree<int> t4;
        t4.insert(15); t4.insert(18); t4.insert(18); t4.insert(2);
        t4.insert(10); t4.insert(13); t4.insert(9); t4.insert(1);
        t4 = t3;
        if (!t3.empty() || t3.size() != 0 || t3.check() == 0 ||
            !t4.empty() || t4.size() != 0 || t4.check() == 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree copy assignment empty\n";
        }
        rb_tree<int> t5;
        t5.insert(15); t5.insert(18); t5.insert(18); t5.insert(2);
        t5.insert(10); t5.insert(13); t5.insert(9); t5.insert(1);
        rb_tree<int> t6;
        t6 = klib::move(t5);
        if (t6.empty() || t6.size() != 8 || t6.check() == 0 || 
            *t6.begin() != 1 || *(--t6.end()) != 18)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree move assignment not empty\n";
        }
        rb_tree<int> t7;
        rb_tree<int> t8;
        t8.insert(15); t8.insert(18); t8.insert(18); t8.insert(2);
        t8.insert(10); t8.insert(13); t8.insert(9); t8.insert(1);
        t8 = klib::move(t7);
        if (!t8.empty() || t8.size() != 0 || t8.check() == 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree move assignment empty\n";
        }
        rb_tree<test_type_2> t9;
        t9.emplace(15); t9.emplace(18); t9.emplace(18); t9.emplace(2);
        t9.emplace(10); t9.emplace(13); t9.emplace(9); t9.emplace(1);
        rb_tree<test_type_2> t10;
        t10 = t9;
        if (t9.empty() || t9.size() != 8 || t9.check() == 0 || 
            *(*t9.begin()).val != 1 || *(*--t9.end()).val != 18 ||
            t10.empty() || t10.size() != 8 || t10.check() == 0 || 
            *(*t10.begin()).val != 1 || *(*--t10.end()).val != 18)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree copy assignment not empty heap type\n";
        }
        rb_tree<test_type_2> t11;
        rb_tree<test_type_2> t12;
        t12.emplace(15); t12.emplace(18); t12.emplace(18); t12.emplace(2);
        t12.emplace(10); t12.emplace(13); t12.emplace(9); t12.emplace(1);
        t12 = t11;
        if (!t11.empty() || t11.size() != 0 || t11.check() == 0 ||
            !t12.empty() || t12.size() != 0 || t12.check() == 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree copy assignment empty heap type\n";
        }
        rb_tree<test_type_2> t13;
        t13.emplace(15); t13.emplace(18); t13.emplace(18); t13.emplace(2);
        t13.emplace(10); t13.emplace(13); t13.emplace(9); t13.emplace(1);
        rb_tree<test_type_2> t14;
        t14 = klib::move(t13);
        if (t14.empty() || t14.size() != 8 || t14.check() == 0 || 
            *(*t14.begin()).val != 1 || *(*--t14.end()).val != 18)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree move assignment not empty heap type\n";
        }
        rb_tree<test_type_2> t15;
        rb_tree<test_type_2> t16;
        t16.emplace(15); t16.emplace(18); t16.emplace(18); t16.emplace(2);
        t16.emplace(10); t16.emplace(13); t16.emplace(9); t16.emplace(1);
        t16 = klib::move(t15);
        if (!t16.empty() || t16.size() != 0 || t16.check() == 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree move assignment empty heap type\n";
        }
    }
    // get_allocator
    {
        rb_tree<int> t1;
        if (t1.get_allocator() != allocator<int> {})
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::get_allocator default int\n";
        }
        rb_tree<int> t2 {allocator<char>{}};
        if (t2.get_allocator() != allocator<int> {})
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::get_allocator default rebind int\n";
        }
        rb_tree<test_type> t3;
        if (t3.get_allocator() != allocator<test_type> {})
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::get_allocator default cutsom\n";
        }
        rb_tree<test_type> t4 {allocator<int>{}};
        if (t4.get_allocator() != allocator<test_type> {})
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::get_allocator default rebind custom\n";
        }
    }
    // iterator types
    {
        if (!is_same<rb_tree<int>::iterator::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator::iterator_category\n";
        }
        if (!is_same<rb_tree<int>::iterator::value_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator::value_type\n";
        }
        if (!is_same<rb_tree<int>::iterator::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator::difference_type\n";
        }
        if (!is_same<rb_tree<int>::iterator::pointer, int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator::pointer\n";
        }
        if (!is_same<rb_tree<int>::iterator::reference, int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator::reference\n";
        }
        if (!is_same<iterator_traits<rb_tree<int>::iterator>::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::iterator_category\n";
        }
        if (!is_same<iterator_traits<rb_tree<int>::iterator>::value_type,
            int>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::value_type\n";
        }
        if (!is_same<iterator_traits<rb_tree<int>::iterator>::difference_type,
            ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::difference_type\n";
        }
        if (!is_same<iterator_traits<rb_tree<int>::iterator>::pointer,
            int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::pointer\n";
        }
        if (!is_same<iterator_traits<rb_tree<int>::iterator>::reference,
            int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::reference\n";
        }
    }
    // const_iterator types
    {
        if (!is_same<rb_tree<int>::const_iterator::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_iterator::iterator_category const\n";
        }
        if (!is_same<rb_tree<int>::const_iterator::value_type,
            const int>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_iterator::value_type const\n";
        }
        if (!is_same<rb_tree<int>::const_iterator::difference_type,
            ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_iterator::difference_type const\n";
        }
        if (!is_same<rb_tree<int>::const_iterator::pointer, const int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_iterator::pointer const\n";
        }
        if (!is_same<rb_tree<int>::const_iterator::reference,
            const int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_iterator::reference const\n";
        }
        if (!is_same<
            iterator_traits<rb_tree<int>::const_iterator>::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::iterator_category const\n";
        }
        if (!is_same<iterator_traits<rb_tree<int>::const_iterator>::value_type,
            const int>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::value_type const\n";
        }
        if (!is_same<
            iterator_traits<rb_tree<int>::const_iterator>::difference_type,
            ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::difference_type const\n";
        }
        if (!is_same<iterator_traits<rb_tree<int>::const_iterator>::pointer,
            const int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::pointer const\n";
        }
        if (!is_same<iterator_traits<rb_tree<int>::const_iterator>::reference,
            const int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::reference const\n";
        }
    }
    // reverse_iterator types
    {
        if (!is_same<rb_tree<int>::reverse_iterator::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reverse_iterator::iterator_category\n";
        }
        if (!is_same<rb_tree<int>::reverse_iterator::value_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reverse_iterator::value_type\n";
        }
        if (!is_same<rb_tree<int>::reverse_iterator::difference_type,
            ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reverse_iterator::difference_type\n";
        }
        if (!is_same<rb_tree<int>::reverse_iterator::pointer, int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reverse_iterator::pointer\n";
        }
        if (!is_same<rb_tree<int>::reverse_iterator::reference, int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reverse_iterator::reference\n";
        }
        if (!is_same<
            iterator_traits<rb_tree<int>::reverse_iterator>::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::iterator_category reverse\n";
        }
        if (!is_same<
            iterator_traits<rb_tree<int>::reverse_iterator>::value_type,
            int>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::value_type reverse\n";
        }
        if (!is_same<
            iterator_traits<rb_tree<int>::reverse_iterator>::difference_type,
            ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::difference_type reverse\n";
        }
        if (!is_same<iterator_traits<rb_tree<int>::reverse_iterator>::pointer,
            int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::pointer reverse\n";
        }
        if (!is_same<iterator_traits<rb_tree<int>::reverse_iterator>::reference,
            int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::reference reverse\n";
        }
    }
    // const_reverse_iterator types
    {
        if (!is_same<rb_tree<int>::const_reverse_iterator::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_reverse_iterator::iterator_category const\n";
        }
        if (!is_same<rb_tree<int>::const_reverse_iterator::value_type,
            const int>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_reverse_iterator::value_type const\n";
        }
        if (!is_same<rb_tree<int>::const_reverse_iterator::difference_type,
            ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_reverse_iterator::difference_type const\n";
        }
        if (!is_same<rb_tree<int>::const_reverse_iterator::pointer,
            const int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_reverse_iterator::pointer const\n";
        }
        if (!is_same<rb_tree<int>::const_reverse_iterator::reference,
            const int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::const_reverse_iterator::reference const\n";
        }
        if (!is_same< iterator_traits<rb_tree<int>::const_reverse_iterator>
            ::iterator_category, bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::iterator_category const reverse\n";
        }
        if (!is_same<
            iterator_traits<rb_tree<int>::const_reverse_iterator>::value_type,
            const int>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::value_type const reverse\n";
        }
        if (!is_same<iterator_traits<rb_tree<int>::const_reverse_iterator>
            ::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::difference_type const reverse\n";
        }
        if (!is_same<
            iterator_traits<rb_tree<int>::const_reverse_iterator>::pointer,
            const int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::pointer const reverse\n";
        }
        if (!is_same<
            iterator_traits<rb_tree<int>::const_reverse_iterator>::reference,
            const int&>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator_traits::reference const reverse\n";
        }
    }
    // begin/end
    {
        rb_tree<int> t1;
        if (t1.begin() != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::begin empty\n";
        }
        rb_tree<int> t2;
        t2.insert(10);
        if (*t2.begin() != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::begin 1 element\n";
        }
        t2.insert(15);
        if (*t2.begin() != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::begin insert after\n";
        }
        t2.insert(5);
        if (*t2.begin() != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::begin insert before\n";
        }
        t2.erase(10);
        if (*t2.begin() != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::begin erase middle\n";
        }
        t2.erase(5);
        if (*t2.begin() != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::begin erase start\n";
        }
        t2.erase(15);
        if (t2.begin() != t2.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::begin erase empty\n";
        }
        rb_tree<int> t3;
        t3.insert(10);
        if (*(--t3.end()) != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::end 1 element\n";
        }
        t3.insert(15);
        if (*(--t3.end()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::end insert after\n";
        }
        t3.insert(5);
        if (*(--t3.end()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::end insert before\n";
        }
        t3.erase(10);
        if (*(--t3.end()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::end erase middle\n";
        }
        t3.erase(15);
        if (*(--t3.end()) != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::end erase end\n";
        }
        t3.erase(5);
        if (t3.begin() != t3.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::end erase empty\n";
        }
    }
    // cbegin/cend
    {
        rb_tree<int> t1;
        if (t1.cbegin() != t1.cend())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cbegin empty\n";
        }
        rb_tree<int> t2;
        t2.insert(10);
        if (*t2.cbegin() != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cbegin 1 element\n";
        }
        t2.insert(15);
        if (*t2.cbegin() != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cbegin insert after\n";
        }
        t2.insert(5);
        if (*t2.cbegin() != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cbegin insert before\n";
        }
        t2.erase(10);
        if (*t2.cbegin() != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cbegin erase middle\n";
        }
        t2.erase(5);
        if (*t2.cbegin() != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cbegin erase start\n";
        }
        t2.erase(15);
        if (t2.cbegin() != t2.cend())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cbegin erase empty\n";
        }
        rb_tree<int> t3;
        t3.insert(10);
        if (*(--t3.cend()) != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cend 1 element\n";
        }
        t3.insert(15);
        if (*(--t3.cend()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cend insert after\n";
        }
        t3.insert(5);
        if (*(--t3.cend()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cend insert before\n";
        }
        t3.erase(10);
        if (*(--t3.cend()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cend erase middle\n";
        }
        t3.erase(15);
        if (*(--t3.cend()) != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cend erase cend\n";
        }
        t3.erase(5);
        if (t3.cbegin() != t3.cend())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::cend erase empty\n";
        }
    }
    // iterator increments/decrements
    {
        rb_tree<int> t1;
        t1.insert(1); t1.insert(2); t1.insert(3);
        auto it1 = t1.begin();
        auto it2 = ++it1;
        if (*it1 != 2 || *it2 != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator pre operator++\n";
        }
        auto it3 = it1++;
        if (*it1 != 3 || *it3 != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator post operator++\n";
        }
        auto it4 = --it1;
        if (*it1 != 2 || *it4 != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator pre operator--\n";
        }
        auto it5 = it1--;
        if (*it1 != 1 || *it5 != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator post operator--\n";
        }
        it1 = t1.end();
        --it1;
        if (*it1 != 3)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator operator-- on end\n";
        }
        ++it1;
        if (it1 != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator operator++ to end\n";
        }
    }
    // rbegin/rend
    {
        rb_tree<int> t1;
        if (t1.rbegin() != t1.rend())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rbegin empty\n";
        }
        rb_tree<int> t2;
        t2.insert(10);
        if (*t2.rbegin() != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rbegin 1 element\n";
        }
        t2.insert(15);
        if (*t2.rbegin() != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rbegin insert after\n";
        }
        t2.insert(5);
        if (*t2.rbegin() != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rbegin insert before\n";
        }
        t2.erase(10);
        if (*t2.rbegin() != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rbegin erase middle\n";
        }
        t2.erase(15);
        if (*t2.rbegin() != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rbegin erase end\n";
        }
        t2.erase(5);
        if (t2.rbegin() != t2.rend())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rbegin erase empty\n";
        }
        rb_tree<int> t3;
        t3.insert(10);
        if (*(--t3.rend()) != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rend 1 element\n";
        }
        t3.insert(15);
        if (*(--t3.rend()) != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rend insert after\n";
        }
        t3.insert(5);
        if (*(--t3.rend()) != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rend insert before\n";
        }
        t3.erase(10);
        if (*(--t3.rend()) != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rend erase middle\n";
        }
        t3.erase(5);
        if (*(--t3.rend()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rend erase start\n";
        }
        t3.erase(15);
        if (t3.rbegin() != t3.rend())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::rend erase empty\n";
        }
    }
    // crbegin/crend
    {
        rb_tree<int> t1;
        if (t1.crbegin() != t1.crend())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crbegin empty\n";
        }
        rb_tree<int> t2;
        t2.insert(10);
        if (*t2.crbegin() != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crbegin 1 element\n";
        }
        t2.insert(15);
        if (*t2.crbegin() != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crbegin insert after\n";
        }
        t2.insert(5);
        if (*t2.crbegin() != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crbegin insert before\n";
        }
        t2.erase(10);
        if (*t2.crbegin() != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crbegin erase middle\n";
        }
        t2.erase(15);
        if (*t2.crbegin() != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crbegin erase end\n";
        }
        t2.erase(5);
        if (t2.crbegin() != t2.crend())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crbegin erase empty\n";
        }
        rb_tree<int> t3;
        t3.insert(10);
        if (*(--t3.crend()) != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crend 1 element\n";
        }
        t3.insert(15);
        if (*(--t3.crend()) != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crend insert after\n";
        }
        t3.insert(5);
        if (*(--t3.crend()) != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crend insert before\n";
        }
        t3.erase(10);
        if (*(--t3.crend()) != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crend erase middle\n";
        }
        t3.erase(5);
        if (*(--t3.crend()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crend erase start\n";
        }
        t3.erase(15);
        if (t3.crbegin() != t3.crend())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::crend erase empty\n";
        }
    }
    // reverse_iterator increments/decrements
    {
        rb_tree<int> t1;
        t1.insert(1); t1.insert(2); t1.insert(3);
        auto it1 = t1.rbegin();
        auto it2 = ++it1;
        if (*it1 != 2 || *it2 != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reverse_iterator pre operator++\n";
        }
        auto it3 = it1++;
        if (*it1 != 1 || *it3 != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reverse_iterator post operator++\n";
        }
        auto it4 = --it1;
        if (*it1 != 2 || *it4 != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reverse_iterator pre operator--\n";
        }
        auto it5 = it1--;
        if (*it1 != 3 || *it5 != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reverse_iterator post operator--\n";
        }
        it1 = t1.rend();
        --it1;
        if (*it1 != 1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reverse_iterator operator-- on rend\n";
        }
        ++it1;
        if (it1 != t1.rend())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::reverse_iterator operator++ to rend\n";
        }
    }
    // Iterator invalidation.
    {
        rb_tree<int> t1;
        auto it1 = t1.begin();
        auto it2 = t1.end();
        t1.insert(1); t1.insert(3); t1.insert(5);
        if (it1 != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator start, insert on empty, invalidated\n";
        }
        if (it2 != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator end, insert on empty, invalidated\n";
        }
        auto it3 = t1.begin();
        auto it4 = ++t1.begin();
        auto it5 = --t1.end();
        auto it6 = t1.end();
        t1.insert(2);
        t1.insert(4);
        t1.insert(6);
        if (*it3 != 1 || it3 != t1.begin())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator start, insert, invalidated\n";
        }
        if (*it4 != 3)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator middle, insert, invalidated\n";
        }
        if (*it5 != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator last, insert, invalidated\n";
        }
        if (it6 != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator end, insert, invalidated\n";
        }
        ++it3;
        ++it4;
        ++it5;
        if (*it3 != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator start, increment after insert, invalidated\n";
        }
        if (*it4 != 4)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator middle, increment after insert, invalidated\n";
        }
        if (*it5 != 6)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator last, increment after insert, invalidated\n";
        }
        t1.erase(1);
        t1.erase(3);
        t1.erase(5);
        if (*it3 != 2 || it3 != t1.begin())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator start, erase, invalidated\n";
        }
        if (*it4 != 4)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator middle, erase, invalidated\n";
        }
        if (*it5 != 6)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator last, erase, invalidated\n";
        }
        ++it3;
        ++it4;
        ++it5;
        if (*it3 != 4)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator start, increment after erase, invalidated\n";
        }
        if (*it4 != 6)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator middle, increment after erase, invalidated\n";
        }
        if (it5 != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::iterator last, increment after erase, invalidated\n";
        }  
    }
    // empty
    {
        rb_tree<int> t1;
        if (!t1.empty())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::empty on construction\n";
        }
        t1.insert(1); t1.insert(3); t1.insert(3); t1.insert(5);
        if (t1.empty())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::empty after insert\n";
        }
        for (size_t i = 0; i < 4; ++i)
            t1.erase(t1.begin());
        if (!t1.empty())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::empty after erase by iterator\n";
        }
        t1.insert(1); t1.insert(3); t1.insert(3); t1.insert(5);
        if (t1.empty())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::empty after reinsert\n";
        }
        t1.erase(1); t1.erase(3); t1.erase(5);
        if (!t1.empty())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::empty after erase by value\n";
        }
    }
    // size
    {
        rb_tree<int> t1;
        if (t1.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::size on construction\n";
        }
        t1.insert(1); t1.insert(3); t1.insert(3); t1.insert(5);
        if (t1.size() != 4)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::size after insert\n";
        }
        for (size_t i = 0; i < 3; ++i)
            t1.erase(t1.begin());
        if (t1.size() != 1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::size after erase by iterator\n";
        }
        t1.erase(t1.begin());
        if (t1.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::size after erase to empty by iterator\n";
        }
        t1.insert(1); t1.insert(3); t1.insert(3); t1.insert(5);
        if (t1.size() != 4)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::size after reinsert\n";
        }
        t1.erase(3);
        if (t1.size() != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::size after double erase by value\n";
        }
        t1.erase(1); t1.erase(5);
        if (t1.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::size after erase to empty by value\n";
        }
    }
    // max_size
    {
        rb_tree<int> t1;
        if (t1.max_size() != allocator_traits<rb_tree<int>::allocator_type>
            ::max_size(t1.get_allocator()))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::max_size default int allocator\n";
        }
        rb_tree<test_type> t2;
        if (t2.max_size() !=
            allocator_traits<rb_tree<test_type>::allocator_type>
            ::max_size(t2.get_allocator()))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::max_size default custom type allocator\n";
        }
    }
    // clear
    {
        rb_tree<int> t1;
        t1.clear();
        if (!t1.empty() || t1.size() != 0 || t1.begin() != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::clear from empty\n";
        }
        rb_tree<int> t2;
        t2.insert(1); t2.insert(2); t2.insert(3);
        t2.clear();
        if (!t2.empty() || t2.size() != 0 || t2.begin() != t2.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::clear after insert\n";
        }
        rb_tree<int> t3;
        t3.insert(1); t3.insert(2); t3.insert(3);
        t3.erase(2);
        t3.clear();
        if (!t3.empty() || t3.size() != 0 || t3.begin() != t3.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::clear after erase\n";
        }
        t3.insert(2);
        if (t3.empty() || t3.size() != 1 || t3.check() == 0 ||
            *t3.begin() != 2 || *--t3.end() != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert after clear\n";
        }
    }
    // insert
    {
        rb_tree<int> t1 {};
        t1.insert(5);
        if (t1.empty() || t1.size() != 1 || t1.check() == 0 || *t1.begin() != 5
            || *(--t1.end()) != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert first time\n";
        }
        t1.insert(10);
        if (t1.empty() || t1.size() != 2 || t1.check() == 0 || *t1.begin() != 5
            || *(--t1.end()) != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert second time\n";
        }
        t1.insert(15);
        if (t1.empty() || t1.size() != 3 || t1.check() == 0 || *t1.begin() != 5
            || *(--t1.end()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert third time\n";
        }
        t1.insert(5);
        if (t1.empty() || t1.size() != 4 || t1.check() == 0 || *t1.begin() != 5
            || *(--t1.end()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert fourth time\n";
        }
        t1.insert(1);
        if (t1.empty() || t1.size() != 5 || t1.check() == 0 || *t1.begin() != 1
            || *(--t1.end()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert fifth time\n";
        }
        t1.insert(-2); t1.insert(3); t1.insert(22); t1.insert(15);
        t1.insert(12); t1.insert(9); t1.insert(17); t1.insert(0);
        if (t1.empty() || t1.size() != 13 || t1.check() == 0 || 
            *t1.begin() != -2 || *(--t1.end()) != 22)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert sixth time\n";
        }
        rb_tree<int> t2 {};
        t2.insert(t2.begin(), 5);
        if (t2.empty() || t2.size() != 1 || t2.check() == 0 || *t2.begin() != 5
            || *(--t2.end()) != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert first time with irrelevant hint\n";
        }
        t2.insert(t2.end(), 10);
        if (t2.empty() || t2.size() != 2 || t2.check() == 0 || *t2.begin() != 5
            || *(--t2.end()) != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert second time with correct hint at end\n";
        }
        t2.insert(t2.begin(), 15);
        if (t2.empty() || t2.size() != 3 || t2.check() == 0 || *t2.begin() != 5
            || *(--t2.end()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert third time with incorrect hint at end\n";
        }
        t2.insert(t2.begin(), 3);
        if (t2.empty() || t2.size() != 4 || t2.check() == 0 || *t2.begin() != 3
            || *(--t2.end()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert fourth time with correct hint at beginning\n";
        }
        t2.insert(t2.end(), 1);
        if (t2.empty() || t2.size() != 5 || t2.check() == 0 || *t2.begin() != 1
            || *(--t2.end()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert fifth time with incorrect hint at beginning\n";
        }
        t2.insert(++++t2.begin(), 4);
        if (t2.empty() || t2.size() != 6 || t2.check() == 0 || *t2.begin() != 1
            || *(--t2.end()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert sixth time with correct hint in middle\n";
        }
        t2.insert(++++t2.begin(), 12);
        if (t2.empty() || t2.size() != 7 || t2.check() == 0 || *t2.begin() != 1
            || *(--t2.end()) != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::insert seventh time with incorrect hint in middle\n";
        }
    }
    // emplace and emplace_hint, using test_type
    {
        rb_tree<test_type> t1 {};
        t1.emplace(5);
        if (t1.empty() || t1.size() != 1 || t1.check() == 0 ||
            (*t1.begin()).val != 5 || (*--t1.end()).val != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace first time\n";
        }
        t1.emplace(10);
        if (t1.empty() || t1.size() != 2 || t1.check() == 0 ||
            (*t1.begin()).val != 5 || (*--t1.end()).val != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace second time\n";
        }
        t1.emplace(15);
        if (t1.empty() || t1.size() != 3 || t1.check() == 0 ||
            (*t1.begin()).val != 5 || (*--t1.end()).val != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace third time\n";
        }
        t1.emplace(5);
        if (t1.empty() || t1.size() != 4 || t1.check() == 0 ||
            (*t1.begin()).val != 5 || (*--t1.end()).val != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace fourth time\n";
        }
        t1.emplace(1);
        if (t1.empty() || t1.size() != 5 || t1.check() == 0 ||
            (*t1.begin()).val != 1 || (*--t1.end()).val != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace fifth time\n";
        }
        t1.emplace(-2); t1.emplace(3); t1.emplace(22); t1.emplace(15);
        t1.emplace(12); t1.emplace(9); t1.emplace(17); t1.emplace(0);
        if (t1.empty() || t1.size() != 13 || t1.check() == 0 || 
            (*t1.begin()).val != -2 || (*--t1.end()).val != 22)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace sixth time\n";
        }
        rb_tree<test_type> t2 {};
        t2.emplace_hint(t2.begin(), 5);
        if (t2.empty() || t2.size() != 1 || t2.check() == 0 ||
            (*t2.begin()).val != 5 || (*--t2.end()).val != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace_hint first time with irrelevant hint\n";
        }
        t2.emplace_hint(t2.end(), 10);
        if (t2.empty() || t2.size() != 2 || t2.check() == 0 ||
            (*t2.begin()).val != 5 || (*--t2.end()).val != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace_hint second time with correct hint at end\n";
        }
        t2.emplace_hint(t2.begin(), 15);
        if (t2.empty() || t2.size() != 3 || t2.check() == 0 ||
            (*t2.begin()).val != 5 || (*--t2.end()).val != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace_hint third time with incorrect hint at end\n";
        }
        t2.emplace_hint(t2.begin(), 3);
        if (t2.empty() || t2.size() != 4 || t2.check() == 0 ||
            (*t2.begin()).val != 3 || (*--t2.end()).val != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace_hint fourth time with correct hint at beginning\n";
        }
        t2.emplace_hint(t2.end(), 1);
        if (t2.empty() || t2.size() != 5 || t2.check() == 0 ||
            (*t2.begin()).val != 1 || (*--t2.end()).val != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace_hint fifth time with incorrect hint at beginning\n";
        }
        t2.emplace_hint(++++t2.begin(), 4);
        if (t2.empty() || t2.size() != 6 || t2.check() == 0 ||
            (*t2.begin()).val != 1 || (*--t2.end()).val != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace_hint sixth time with correct hint in middle\n";
        }
        t2.emplace_hint(++++t2.begin(), 12);
        if (t2.empty() || t2.size() != 7 || t2.check() == 0 ||
            (*t2.begin()).val != 1 || (*--t2.end()).val != 15)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::emplace_hint seventh time with incorrect hint in middle\n";
        }
    }
    // erase
    {
        constexpr int no_nodes = 500;
        constexpr int range = 30;
        rb_tree<int> t1 = make_test(no_nodes, range);
        for (size_t i = 0; i < t1.size(); ++i)
        {
            rb_tree<int> t2 {t1};
            auto iter = t2.begin();
            for (size_t j = 0; j < i; ++iter, ++j);
            t2.erase(iter);
            if (t2.check() == 0)
            {
                ++fail_count;
                std::cout << "FAILED rb_tree::erase by iterator, constant size\n";
            }       
        }
        rb_tree<int> t2 = make_test(no_nodes, range);
        while (t2.size() > 0)
        {
            t2.erase(t2.begin());
            if (t2.check() == 0)
            {
                ++fail_count;
                std::cout << "FAILED rb_tree::erase by beginning iterator, reducing size\n";
            }
        }
        if (!t2.empty())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::erase by beginning iterator, reducing to empty\n";
        }
        rb_tree<int> t3 = make_test(no_nodes, range);
        while (t3.size() > 0)
        {
            t3.erase(--t3.end());
            if (t3.check() == 0)
            {
                ++fail_count;
                std::cout << "FAILED rb_tree::erase by end iterator, reducing size\n";
            }
        }
        if (!t3.empty())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::erase by end iterator, reducing to empty\n";
        }
        rb_tree<int> t4 = make_test(no_nodes, range);
        while (t4.size() > 0)
        {
            auto it = t4.begin();
            for (size_t i = 0; i < t4.size() / 2; ++i)
                ++it;
            t4.erase(it);
            if (t4.check() == 0)
            {
                ++fail_count;
                std::cout << "FAILED rb_tree::erase by middle iterator, reducing size\n";
            }
        }
        if (!t4.empty())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::erase by middle iterator, reducing to empty\n";
        }
        rb_tree<int> t5 = make_test(no_nodes, range);
        for (int i = -range; i <= range; ++i)
        {
            rb_tree<int> t6 {t5};
            t6.erase(i);
            if (t6.check() == 0)
            {
                ++fail_count;
                std::cout << "FAILED rb_tree::erase by value, constant size\n";
            }       
        }
        rb_tree<int> t6 = make_test(no_nodes, range);
        for (int i = -range; i <= range; ++i)
        {
            t6.erase(i);
            if (t6.check() == 0)
            {
                ++fail_count;
                std::cout << "FAILED rb_tree::erase by value, reducing size\n";
            }
        }
        if (!t6.empty())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::erase by value, reducing to empty\n";
        }
        rb_tree<int> t7 = make_test(no_nodes, range);
        auto it1 = t7.erase(++t7.begin(), --t7.end());
        if (t7.empty() || t7.size() != 2 || t7.check() == 0 ||
            it1 != --t7.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::erase by iterator range\n";
        }
        rb_tree<int> t8 = make_test(no_nodes, range);
        auto it2 = t8.erase(t8.begin(), t8.end());
        if (!t8.empty() || t8.size() != 0 || t8.check() == 0 || it2 != t8.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::erase to empty by iterator range\n";
        }
    }
    // swap
    {
        rb_tree<int> t1;
        rb_tree<int> t2;
        t2.insert(3); t2.insert(4); t2.insert(5);
        t1.swap(t2);
        if (t1.empty() || t1.size() != 3 || t1.check() == 0 ||
            *t1.begin() != 3 || *--t1.end() != 5 ||
            !t2.empty() || t2.size() != 0 || t2.begin() != t2.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::swap int one empty\n";
        }
        rb_tree<int> t3;
        t3.insert(6); t3.insert(7); t3.insert(8); t3.insert(9);
        rb_tree<int> t4;
        t4.insert(3); t4.insert(4); t4.insert(5);
        t3.swap(t4);
        if (t3.empty() || t3.size() != 3 || t3.check() == 0 ||
            *t3.begin() != 3 || *--t3.end() != 5 ||
            t4.empty() || t4.size() != 4 || t4.check() == 0 ||
            *t4.begin() != 6 || *--t4.end() != 9)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::swap int none empty\n";
        }
        rb_tree<int, greater<int>> t5;
        t5.insert(6); t5.insert(7); t5.insert(8); t5.insert(9);
        rb_tree<int, greater<int>> t6;
        t6.insert(3); t6.insert(4); t6.insert(5);
        t5.swap(t6);
        if (t5.empty() || t5.size() != 3 || t5.check() == 0 ||
            *t5.begin() != 5 || *--t5.end() != 3 || t5.value_comp()(2, 4) ||
            t6.empty() || t6.size() != 4 || t6.check() == 0 ||
            *t6.begin() != 9 || *--t6.end() != 6 || t6.value_comp()(2, 4))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::swap int none empty non-default comparator\n";
        }
        rb_tree<test_type_2> t7;
        t7.emplace(6); t7.emplace(7); t7.emplace(8); t7.emplace(9);
        rb_tree<test_type_2> t8;
        t8.emplace(3); t8.emplace(4); t8.emplace(5);
        t8.swap(t7);
        if (t7.empty() || t7.size() != 3 || t7.check() == 0 ||
            *(*t7.begin()).val != 3 || *(*--t7.end()).val != 5 ||
            t8.empty() || t8.size() != 4 || t8.check() == 0 ||
            *(*t8.begin()).val != 6 || *(*--t8.end()).val != 9)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::swap heap type none empty\n";
        }
    }
    // Non-member swap
    {
        rb_tree<int> t1;
        rb_tree<int> t2;
        t2.insert(3); t2.insert(4); t2.insert(5);
        swap(t1, t2);
        if (t1.empty() || t1.size() != 3 || t1.check() == 0 ||
            *t1.begin() != 3 || *--t1.end() != 5 ||
            !t2.empty() || t2.size() != 0 || t2.begin() != t2.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::swap int one empty\n";
        }
        rb_tree<int> t3;
        t3.insert(6); t3.insert(7); t3.insert(8); t3.insert(9);
        rb_tree<int> t4;
        t4.insert(3); t4.insert(4); t4.insert(5);
        swap(t3, t4);
        if (t3.empty() || t3.size() != 3 || t3.check() == 0 ||
            *t3.begin() != 3 || *--t3.end() != 5 ||
            t4.empty() || t4.size() != 4 || t4.check() == 0 ||
            *t4.begin() != 6 || *--t4.end() != 9)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::swap int none empty\n";
        }
        rb_tree<int, greater<int>> t5;
        t5.insert(6); t5.insert(7); t5.insert(8); t5.insert(9);
        rb_tree<int, greater<int>> t6;
        t6.insert(3); t6.insert(4); t6.insert(5);
        swap(t5, t6);
        if (t5.empty() || t5.size() != 3 || t5.check() == 0 ||
            *t5.begin() != 5 || *--t5.end() != 3 || t5.value_comp()(2, 4) ||
            t6.empty() || t6.size() != 4 || t6.check() == 0 ||
            *t6.begin() != 9 || *--t6.end() != 6 || t6.value_comp()(2, 4))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::swap int none empty non-default comparator\n";
        }
        rb_tree<test_type_2> t7;
        t7.emplace(6); t7.emplace(7); t7.emplace(8); t7.emplace(9);
        rb_tree<test_type_2> t8;
        t8.emplace(3); t8.emplace(4); t8.emplace(5);
        swap(t7, t8);
        if (t7.empty() || t7.size() != 3 || t7.check() == 0 ||
            *(*t7.begin()).val != 3 || *(*--t7.end()).val != 5 ||
            t8.empty() || t8.size() != 4 || t8.check() == 0 ||
            *(*t8.begin()).val != 6 || *(*--t8.end()).val != 9)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::swap heap type none empty\n";
        }
    }
    // count
    {
        rb_tree<int> t1;
        if (t1.count(1) != 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::count empty\n";
        }
        t1.insert(15); t1.insert(18); t1.insert(13); t1.insert(2);
        t1.insert(10); t1.insert(13); t1.insert(9); t1.insert(1);
        if (t1.count(3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::count not present\n";
        }
        if (t1.count(1) != 1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::count singly present start\n";
        }
        if (t1.count(10) != 1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::count singly present middle\n";
        }
        if (t1.count(18) != 1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::count singly present end\n";
        }
        t1.insert(1); t1.insert(18);
        if (t1.count(1) != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::count doubly present start\n";
        }
        if (t1.count(13) != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::count double present middle\n";
        }
        if (t1.count(18) != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::count double present end\n";
        }
        rb_tree<test_type, test_comparator> t2;
        if (t2.count(1) != 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::count transparent empty\n";
        }
        t2.emplace(15); t2.emplace(18); t2.emplace(13); t2.emplace(2);
        t2.emplace(10); t2.emplace(13); t2.emplace(9); t2.emplace(1);
        if (t2.count(3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::count transparent not present\n";
        }
        if (t2.count(9) != 1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::count transparent present\n";
        }
        if (t2.count(13) != 2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::count transparent doubly present\n";
        }
    }
    // find
    {
        rb_tree<int> t1;
        if (t1.find(1) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::find empty\n";
        }
        t1.insert(15); t1.insert(18); t1.insert(13); t1.insert(2);
        t1.insert(10); t1.insert(13); t1.insert(9); t1.insert(1);
        if (t1.find(3) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::find not present\n";
        }
        if (*t1.find(1) != 1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::find singly present start\n";
        }
        if (*t1.find(10) != 10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::find singly present middle\n";
        }
        if (*t1.find(18) != 18)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::find singly present end\n";
        }
        t1.insert(1); t1.insert(18);
        if (*t1.find(1) != 1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::find doubly present start\n";
        }
        if (*t1.find(13) != 13)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::find double present middle\n";
        }
        if (*t1.find(18) != 18)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::find double present end\n";
        }
        rb_tree<test_type, test_comparator> t2;
        if (t2.find(1) != t2.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::find transparent empty\n";
        }
        t2.emplace(15); t2.emplace(18); t2.emplace(13); t2.emplace(2);
        t2.emplace(10); t2.emplace(13); t2.emplace(9); t2.emplace(1);
        if (t2.find(3) != t2.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::find transparent not present\n";
        }
        if ((*t2.find(9)).val != 9)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::find transparent present\n";
        }
        if ((*t2.find(13)).val != 13)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::find transparent doubly present\n";
        }
    }
    // lower_bound
    {
        rb_tree<int> t1;
        if (t1.lower_bound(1) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::lower_bound empty\n";
        }
        t1.insert(1); t1.insert(3); t1.insert(3); t1.insert(5);
        if (*t1.lower_bound(2) != 3 || *++t1.lower_bound(2) != 3)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::lower_bound non-existent middle\n";
        }
        if (t1.lower_bound(0) != t1.begin())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::lower_bound non-existent left\n";
        }
        if (t1.lower_bound(6) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::lower_bound non-existent right\n";
        }
        if (t1.lower_bound(1) != t1.begin())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::lower_bound exists left\n";
        }
        if (*t1.lower_bound(3) != 3 || *++t1.lower_bound(3) != 3)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::lower_bound exists middle\n";
        }
        if (*t1.lower_bound(5) != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::lower_bound exists right\n";
        }
    }
    // upper_bound
    {
        rb_tree<int> t1;
        if (t1.upper_bound(1) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::upper_bound empty\n";
        }
        t1.insert(1); t1.insert(3); t1.insert(3); t1.insert(5);
        if (t1.upper_bound(0) != t1.begin())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::upper_bound non-existent left\n";
        }
        if (*t1.upper_bound(2) != 3 || *++t1.upper_bound(2) != 3)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::upper_bound non-existent middle\n";
        }
        if (t1.upper_bound(6) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::upper_bound non-existent right\n";
        }
        if (*t1.upper_bound(1) != 3 || *++t1.upper_bound(1) != 3)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::upper_bound exists left\n";
        }
        if (*t1.upper_bound(3) != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::upper_bound exists middle\n";
        }
        if (t1.upper_bound(5) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::upper_bound exists right\n";
        }
    }
    // equal_range
    {
        rb_tree<int> t1;
        if (t1.equal_range(1).first != t1.end() ||
            t1.equal_range(1).second != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::equal_range empty\n";
        }
        t1.insert(1); t1.insert(3); t1.insert(3); t1.insert(5);
        if (t1.equal_range(0).first != t1.begin() ||
            t1.equal_range(0).first != t1.equal_range(0).second)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::equal_range non-existent left\n";
        }
        if (*t1.equal_range(2).first != 3 || *++t1.equal_range(2).first != 3 ||
            t1.equal_range(2).first != t1.equal_range(2).second)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::equal_range non-existent middle\n";
        }
        if (t1.equal_range(6).first != t1.end() ||
            t1.equal_range(6).first != t1.equal_range(6).second)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::equal_range non-existent right\n";
        }
        if (*t1.equal_range(1).first != 1 || *t1.equal_range(1).second != 3 ||
            *++t1.equal_range(1).second != 3)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::equal_range exists left\n";
        }
        if (*t1.equal_range(3).first != 3 || *++t1.equal_range(3).first != 3 ||
            *t1.equal_range(3).second != 5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::equal_range exists middle\n";
        }
        if (*t1.equal_range(5).first != 5 ||
            t1.equal_range(5).second != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::equal_range exists right\n";
        }
    }
    // value_comp
    {
        rb_tree<int> t1;
        if (!is_same<decltype(t1.value_comp()), less<int>>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::value_comp less<int>\n";
        }
        rb_tree<int, greater<int>> t2;
        if (!is_same<decltype(t2.value_comp()), greater<int>>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::value_comp greater<int>\n";
        }
        rb_tree<test_type, test_comparator> t3;
        if (!is_same<decltype(t3.value_comp()), test_comparator>::value)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::value_comp test_comparator\n";
        }
    }
    // operator==
    {
        rb_tree<int> t1;
        rb_tree<int> t2;
        t2.insert(1);
        t2.insert(2);
        t2.insert(3);
        rb_tree<int> t3;
        t3.insert(1);
        t3.insert(2);
        t3.insert(3);
        rb_tree<int> t4;
        t4.insert(1);
        t4.insert(2);
        rb_tree<int> t5;
        if (t1 == t2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator== int one empty\n";
        }
        if (!(t1 == t5))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator== int both empty\n";
        }
        if (!(t2 == t3))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator== int both full equal\n";
        }
        if (t2 == t4)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator== int different sizes\n";
        }
        rb_tree<test_type, test_comparator> t6;
        rb_tree<test_type, test_comparator> t7;
        t7.emplace(1);
        t7.emplace(2);
        t7.emplace(3);
        rb_tree<test_type, test_comparator> t8;
        t8.emplace(1);
        t8.emplace(2);
        t8.emplace(3);
        rb_tree<test_type, test_comparator> t9;
        t9.emplace(1);
        t9.emplace(2);
        rb_tree<test_type, test_comparator> t10;
        if (t6 == t7)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator== custom type one empty\n";
        }
        if (!(t6 == t10))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator== custom type both empty\n";
        }
        if (!(t7 == t8))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator== custom type both full equal\n";
        }
        if (t7 == t9)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator== custom type different sizes\n";
        }
    }
    // operator!=
    {
        rb_tree<int> t1;
        rb_tree<int> t2;
        t2.insert(1); t2.insert(2); t2.insert(3);
        rb_tree<int> t3;
        t3.insert(1); t3.insert(2); t3.insert(3);
        rb_tree<int> t4;
        t4.insert(1); t4.insert(2);
        rb_tree<int> t5;
        if (!(t1 != t2))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator!= int one empty\n";
        }
        if (t1 != t5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator!= int both empty\n";
        }
        if (t2 != t3)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator!= int both full equal\n";
        }
        if (!(t2 != t4))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator!= int different sizes\n";
        }
        rb_tree<test_type, test_comparator> t6;
        rb_tree<test_type, test_comparator> t7;
        t7.emplace(1); t7.emplace(2); t7.emplace(3);
        rb_tree<test_type, test_comparator> t8;
        t8.emplace(1); t8.emplace(2); t8.emplace(3);
        rb_tree<test_type, test_comparator> t9;
        t9.emplace(1); t9.emplace(2);
        rb_tree<test_type, test_comparator> t10;
        if (!(t6 != t7))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator!= custom type one empty\n";
        }
        if (t6 != t10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator!= custom type both empty\n";
        }
        if (t7 != t8)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator!= custom type both full equal\n";
        }
        if (!(t7 != t9))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator!= custom type different sizes\n";
        }
    }
    // operator<
    {
        rb_tree<int> t1;
        t1.insert(1); t1.insert(2); t1.insert(3);
        rb_tree<int> t2;
        t2.insert(1); t2.insert(2); t2.insert(3);
        rb_tree<int> t3;
        t3.insert(1); t3.insert(2);
        rb_tree<int> t4;
        rb_tree<int> t5;
        t5.insert(2); t5.insert(3); t5.insert(4);
        if (t1 < t2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< int equal\n";
        }
        if (t1 < t3)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< int second prequel\n";
        }
        if (!(t3 < t1))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< int first prequel\n";
        }
        if (t1 < t4)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< int second empty\n";
        }
        if (!(t4 < t1))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< int first empty\n";
        }
        if (t4 < t4)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< int both empty\n";
        }
        if (!(t1 < t5))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< int first element-wise less\n";
        }
        if (t5 < t1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< int second element-wise less\n";
        }
        rb_tree<test_type, test_comparator> t6;
        t6.insert(1); t6.insert(2); t6.insert(3);
        rb_tree<test_type, test_comparator> t7;
        t7.insert(1); t7.insert(2); t7.insert(3);
        rb_tree<test_type, test_comparator> t8;
        t8.insert(1); t8.insert(2);
        rb_tree<test_type, test_comparator> t9;
        rb_tree<test_type, test_comparator> t10;
        t10.insert(2); t10.insert(3); t10.insert(4);
        if (t6 < t7)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< custom type equal\n";
        }
        if (t6 < t8)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< custom type second prequel\n";
        }
        if (!(t8 < t6))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< custom type first prequel\n";
        }
        if (t6 < t9)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< custom type second empty\n";
        }
        if (!(t9 < t6))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< custom type first empty\n";
        }
        if (t9 < t9)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< custom type both empty\n";
        }
        if (!(t6 < t10))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< custom type first element-wise less\n";
        }
        if (t10 < t6)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator< custom type second element-wise less\n";
        }
    }
    // operator<=
    {
        rb_tree<int> t1;
        t1.insert(1); t1.insert(2); t1.insert(3);
        rb_tree<int> t2;
        t2.insert(1); t2.insert(2); t2.insert(3);
        rb_tree<int> t3;
        t3.insert(1); t3.insert(2);
        rb_tree<int> t4;
        rb_tree<int> t5;
        t5.insert(2); t5.insert(3); t5.insert(4);
        if (!(t1 <= t2))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= int equal\n";
        }
        if (t1 <= t3)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= int second prequel\n";
        }
        if (!(t3 <= t1))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= int first prequel\n";
        }
        if (t1 <= t4)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= int second empty\n";
        }
        if (!(t4 <= t1))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= int first empty\n";
        }
        if (!(t4 <= t4))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= int both empty\n";
        }
        if (!(t1 <= t5))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= int first element-wise less\n";
        }
        if (t5 <= t1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= int second element-wise less\n";
        }
        rb_tree<test_type, test_comparator> t6;
        t6.insert(1); t6.insert(2); t6.insert(3);
        rb_tree<test_type, test_comparator> t7;
        t7.insert(1); t7.insert(2); t7.insert(3);
        rb_tree<test_type, test_comparator> t8;
        t8.insert(1); t8.insert(2);
        rb_tree<test_type, test_comparator> t9;
        rb_tree<test_type, test_comparator> t10;
        t10.insert(2); t10.insert(3); t10.insert(4);
        if (!(t6 <= t7))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= custom type equal\n";
        }
        if (t6 <= t8)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= custom type second prequel\n";
        }
        if (!(t8 <= t6))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= custom type first prequel\n";
        }
        if (t6 <= t9)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= custom type second empty\n";
        }
        if (!(t9 <= t6))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= custom type first empty\n";
        }
        if (!(t9 <= t9))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= custom type both empty\n";
        }
        if (!(t6 <= t10))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= custom type first element-wise less\n";
        }
        if (t10 <= t6)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator<= custom type second element-wise less\n";
        }
    }
    // operator>
    {
        rb_tree<int> t1;
        t1.insert(1); t1.insert(2); t1.insert(3);
        rb_tree<int> t2;
        t2.insert(1); t2.insert(2); t2.insert(3);
        rb_tree<int> t3;
        t3.insert(1); t3.insert(2);
        rb_tree<int> t4;
        rb_tree<int> t5;
        t5.insert(2); t5.insert(3); t5.insert(4);
        if (t1 > t2)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> int equal\n";
        }
        if (!(t1 > t3))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> int second prequel\n";
        }
        if (t3 > t1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> int first prequel\n";
        }
        if (!(t1 > t4))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> int second empty\n";
        }
        if (t4 > t1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> int first empty\n";
        }
        if (t4 > t4)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> int both empty\n";
        }
        if (t1 > t5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> int first element-wise less\n";
        }
        if (!(t5 > t1))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> int second element-wise less\n";
        }
        rb_tree<test_type, test_comparator> t6;
        t6.insert(1); t6.insert(2); t6.insert(3);
        rb_tree<test_type, test_comparator> t7;
        t7.insert(1); t7.insert(2); t7.insert(3);
        rb_tree<test_type, test_comparator> t8;
        t8.insert(1); t8.insert(2);
        rb_tree<test_type, test_comparator> t9;
        rb_tree<test_type, test_comparator> t10;
        t10.insert(2); t10.insert(3); t10.insert(4);
        if (t6 > t7)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> custom type equal\n";
        }
        if (!(t6 > t8))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> custom type second prequel\n";
        }
        if (t8 > t6)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> custom type first prequel\n";
        }
        if (!(t6 > t9))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> custom type second empty\n";
        }
        if (t9 > t6)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> custom type first empty\n";
        }
        if (t9 > t9)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> custom type both empty\n";
        }
        if (t6 > t10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> custom type first element-wise less\n";
        }
        if (!(t10 > t6))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator> custom type second element-wise less\n";
        }
    }
    // operator>=
    {
        rb_tree<int> t1;
        t1.insert(1); t1.insert(2); t1.insert(3);
        rb_tree<int> t2;
        t2.insert(1); t2.insert(2); t2.insert(3);
        rb_tree<int> t3;
        t3.insert(1); t3.insert(2);
        rb_tree<int> t4;
        rb_tree<int> t5;
        t5.insert(2); t5.insert(3); t5.insert(4);
        if (!(t1 >= t2))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= int equal\n";
        }
        if (!(t1 >= t3))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= int second prequel\n";
        }
        if (t3 >= t1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= int first prequel\n";
        }
        if (!(t1 >= t4))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= int second empty\n";
        }
        if (t4 >= t1)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= int first empty\n";
        }
        if (!(t4 >= t4))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= int both empty\n";
        }
        if (t1 >= t5)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= int first element-wise less\n";
        }
        if (!(t5 >= t1))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= int second element-wise less\n";
        }
        rb_tree<test_type, test_comparator> t6;
        t6.insert(1); t6.insert(2); t6.insert(3);
        rb_tree<test_type, test_comparator> t7;
        t7.insert(1); t7.insert(2); t7.insert(3);
        rb_tree<test_type, test_comparator> t8;
        t8.insert(1); t8.insert(2);
        rb_tree<test_type, test_comparator> t9;
        rb_tree<test_type, test_comparator> t10;
        t10.insert(2); t10.insert(3); t10.insert(4);
        if (!(t6 >= t7))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= custom type equal\n";
        }
        if (!(t6 >= t8))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= custom type second prequel\n";
        }
        if (t8 >= t6)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= custom type first prequel\n";
        }
        if (!(t6 >= t9))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= custom type second empty\n";
        }
        if (t9 >= t6)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= custom type first empty\n";
        }
        if (!(t9 >= t9))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= custom type both empty\n";
        }
        if (t6 >= t10)
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= custom type first element-wise less\n";
        }
        if (!(t10 >= t6))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::operator>= custom type second element-wise less\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
