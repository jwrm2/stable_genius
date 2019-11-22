#include <chrono>
#include <iostream>
#include <random>

#include "../include/iterator"
#include "../include/map"
#include "../include/utility"
#include "../include/vector"

using namespace klib;

// Make a test tree with n random int values between +/- x. Uses the move
// constructor, so check that without using this function.
/*map<int, int> make_test(size_t n, int x)
{
    x = (x < 0 ? -x : x);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine gen {seed};
    std::uniform_int_distribution<int> dist(-x, x);
    map<int, int> t {};

    while (n-- != 0)
        t.insert(pair<int, int>{dist(gen), dist(gen)});

    return t;
}*/

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

    template<typename V>
    bool operator()(const pair<const test_type, V>& lhs, int rhs) const
    {
        return lhs.first.val < rhs;
    }

    template<typename V>
    bool operator()(int lhs, const pair<const test_type, V>& rhs) const
    {
        return lhs < rhs.first.val;
    }
};

// Test type that does a heap allocation.
class test_type_2 {
public:
    int* val;

    test_type_2() : val{nullptr} {}

    explicit test_type_2(int v) : val{new int {v}} {}

    test_type_2(const test_type_2& other) :
        val{other.val == nullptr ? nullptr : new int {*other.val}} {}

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
        if (!is_same<map<int, char>::key_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::key_type\n";
        }
        if (!is_same<map<int, char>::mapped_type, char>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::mapped_type\n";
        }
        if (!is_same<map<int, char>::value_type, pair<const int, char>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::value_type\n";
        }
        if (!is_same<map<int, char>::size_type, size_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::size_type\n";
        }
        if (!is_same<map<int, char>::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::difference_type\n";
        }
        if (!is_same<map<int, char>::reference, pair<const int, char>&>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::reference\n";
        }
        if (!is_same<map<int, char>::const_reference,
            const pair<const int, char>&>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_reference\n";
        }
        if (!is_same<map<int, char>::pointer, pair<const int, char>*>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::pointer\n";
        }
        if (!is_same<map<int, char>::const_pointer,
            const pair<const int, char>*>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_pointer\n";
        }
        if (!is_same<map<int, char>::iterator,
            helper::rb_tree<pair<const int, char>,
                            helper::pair_compare<int, char, less<int>>,
                            int>::
                rb_iterator<true,
                            pair<const int, char>,
                            pair<const int, char>*,
                            pair<const int, char>&>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator\n";
        }
        if (!is_same<map<int, char>::const_iterator,
            helper::rb_tree<pair<const int, char>,
                            helper::pair_compare<int, char, less<int>>,
                            int>::
                rb_iterator<false,
                            const pair<const int, char>,
                            const pair<const int, char>*,
                            const pair<const int, char>&>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_iterator\n";
        }
        if (!is_same<map<int, char>::reverse_iterator,
                reverse_iterator<helper::rb_tree<pair<const int, char>,
                        helper::pair_compare<int, char, less<int>>,
                        int>::
                    rb_iterator<true,
                                pair<const int, char>,
                                pair<const int, char>*,
                                pair<const int, char>&>>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::reverse_iterator\n";
        }
        if (!is_same<map<int, char>::const_reverse_iterator,
                reverse_iterator<helper::rb_tree<pair<const int, char>,
                        helper::pair_compare<int, char, less<int>>,
                        int>::
                    rb_iterator<false,
                                const pair<const int, char>,
                                const pair<const int, char>*,
                                const pair<const int, char>&>>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_reverse_iterator\n";
        }
        if (!is_same<map<int, char>::allocator_type,
                allocator<pair<const int, char>>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::allocator\n";
        }
    }
    // Constructors.
    {
        // Default constructor.
        map<int, char> t1;
        if (!t1.empty() || t1.get_allocator() != allocator<int> {} ||
            t1.value_comp()(4, 2) ||
            t1.value_comp()(pair<int, char>{4, 'a'}, pair<int, char>{2, 'b'}))
        {
            ++fail_count;
            std::cout << "FAILED map()\n";
        }
        // Constructor with allocator.
        map<int, char> t2 {allocator<pair<const int, char>>{}};
        if (!t2.empty() ||
            t2.get_allocator() != allocator<pair<const int, char>> {} ||
            t2.value_comp()(4, 2) ||
            t2.value_comp()(pair<int, char>{4, 'a'}, pair<int, char>{2, 'b'}))
        {
            ++fail_count;
            std::cout << "FAILED map(allocator<pair<const int, char>{})\n";
        }
        // Constructor with different allocator.
        map<int, char> t3 {allocator<char>{}};
        if (!t3.empty() ||
            t3.get_allocator() != allocator<pair<const int, char>> {} ||
            t3.value_comp()(4, 2) ||
            t3.value_comp()(pair<int, char>{4, 'a'}, pair<int, char>{2, 'b'}))
        {
            ++fail_count;
            std::cout << "FAILED map(allocator<char>{})\n";
        }
        // Constructor with comparator
        map<int, char, greater<int>> t4 {greater<int>{}};
        if (!t4.empty() ||
            t4.get_allocator() != allocator<pair<const int, char>> {} ||
            t4.value_comp()(2, 4) ||
            t4.value_comp()(pair<int, char>{2, 'b'}, pair<int, char>{4, 'a'}))
        {
            ++fail_count;
            std::cout << "FAILED map(greater<int>{})\n";
        }
        // Constructor with comparator and allocator
        map<int, char, greater<int>> t5 {greater<int>{},
            allocator<pair<int, char>>{}};
        if (!t5.empty() ||
            t5.get_allocator() != allocator<pair<const int, char>> {} ||
            t5.value_comp()(2, 4) ||
            t5.value_comp()(pair<int, char>{2, 'b'}, pair<int, char>{4, 'a'}))
        {
            ++fail_count;
            std::cout << "FAILED map(greater<int>{}, allocator<int>{})\n";
        }
        // Copy constructor when empty.
        map<int, char> t6 {};
        map<int, char> t7 {t6};
        if (!t6.empty() ||
            t6.get_allocator() != allocator<pair<const int, char>> {} ||
            t6.value_comp()(4, 2) ||
            t6.value_comp()(pair<int, char>{4, 'a'}, pair<int, char>{2, 'b'}) ||
            !t7.empty() ||
            t7.get_allocator() != allocator<pair<const int, char>> {} ||
            t7.value_comp()(4, 2) ||
            t7.value_comp()(pair<int, char>{4, 'a'}, pair<int, char>{2, 'b'}))
        {
            ++fail_count;
            std::cout << "FAILED map(const map&) empty\n";
        }
        // Move constructor when empty.
        map<int, char> t8 {};
        map<int, char> t9 {klib::move(t8)};
        if (!t9.empty() ||
            t9.get_allocator() != allocator<pair<const int, char>> {} ||
            t9.value_comp()(4, 2) ||
            t9.value_comp()(pair<int, char>{4, 'a'}, pair<int, char>{2, 'b'}))
        {
            ++fail_count;
            std::cout << "FAILED map(map&&) empty\n";
        }
        // Copy constructor when not empty.
        map<int, char> t10 {};
        t10[10] = 'a'; t10[19] = 'b'; t10[13] = 'c'; t10[29] = 'd';
        t10[9] = 'e'; t10[12] = 'f'; t10[18] = 'g'; t10[3] = 'h';
        map<int, char> t11 {t10};
        if (t11.empty() || t11.size() != 8 || (*t11.begin()).first != 3 ||
            (*t11.begin()).second != 'h' || (*--t11.end()).first != 29 ||
            (*--t11.end()).second != 'd' ||
            t10.empty() || t10.size() != 8 || (*t10.begin()).first != 3 ||
            (*t10.begin()).second != 'h' || (*--t10.end()).first != 29 ||
            (*--t10.end()).second != 'd')

        {
            ++fail_count;
            std::cout << "FAILED map(const map&) not empty\n";
        }
        // Move constructor when not empty.
        map<int, char> t12 {};
        t12[10] = 'a'; t12[19] = 'b'; t12[13] = 'c'; t12[29] = 'd';
        t12[9] = 'e'; t12[12] = 'f'; t12[18] = 'g'; t12[3] = 'h';
        map<int, char> t13 {t12};
        if (t13.empty() || t13.size() != 8 || (*t13.begin()).first != 3 ||
            (*t13.begin()).second != 'h' || (*--t13.end()).first != 29 ||
            (*--t13.end()).second != 'd')
        {
            ++fail_count;
            std::cout << "FAILED map(map&&) not empty\n";
        }
        // Copy constructor with heap data type.
        map<int, test_type_2> t14 {};
        t14[10] = test_type_2{1}; t14[19] = test_type_2{2};
        t14[13] = test_type_2{3}; t14[29] = test_type_2{4};
        t14[9] = test_type_2{5}; t14[12] = test_type_2{6};
        t14[18] = test_type_2{7}; t14[3] = test_type_2{8};
        map<int, test_type_2> t15 {t14};
        if (t15.empty() || t15.size() != 8 || (*t15.begin()).first != 3 ||
            *(*t15.begin()).second.val != 8 || (*--t15.end()).first != 29 ||
            *(*--t15.end()).second.val != 4 ||
            t14.empty() || t14.size() != 8 || (*t14.begin()).first != 3 ||
            *(*t14.begin()).second.val != 8 || (*--t14.end()).first != 29 ||
            *(*--t14.end()).second.val != 4)
        {
            ++fail_count;
            std::cout << "FAILED map(const map&) heap type\n";
        }
        // Move constructor with heap data type.
        map<int, test_type_2> t16 {};
        t16[10] = test_type_2{1}; t16[19] = test_type_2{2};
        t16[13] = test_type_2{3}; t16[29] = test_type_2{4};
        t16[9] = test_type_2{5}; t16[12] = test_type_2{6};
        t16[18] = test_type_2{7}; t16[3] = test_type_2{8};
        map<int, test_type_2> t17 {klib::move(t16)};
        if (t17.empty() || t17.size() != 8 || (*t17.begin()).first != 3 ||
            *(*t17.begin()).second.val != 8 || (*--t17.end()).first != 29 ||
            *(*--t17.end()).second.val != 4)
        {
            ++fail_count;
            std::cout << "FAILED map(map&&) heap type\n";
        }
        // Initialiser list constructor.
        map<int, char> t18 {pair<int, char>{10, 'a'}, pair<int, char>{19, 'b'},
            pair<int, char>{13, 'c'}, pair<int, char>{29, 'd'},
            pair<int, char>{9, 'e'}, pair<int, char>{12, 'f'},
            pair<int, char>{18, 'g'}, pair<int, char>{3, 'h'},
            pair<int, char>{9, 'i'}, pair<int, char>{3, 'j'}};
        if (t18.empty() || t18.size() != 8 || (*t18.begin()).first != 3 ||
            (*t18.begin()).second != 'h' || (*--t18.end()).first != 29 ||
            (*--t18.end()).second != 'd')
        {
            ++fail_count;
            std::cout << "FAILED map(initializer_list) with duplicates\n";
        }
        // Initialiser list constructor with allocator.
        map<int, char> t19 {{pair<int, char>{10, 'a'}, pair<int, char>{19, 'b'},
            pair<int, char>{13, 'c'}, pair<int, char>{29, 'd'},
            pair<int, char>{9, 'e'}, pair<int, char>{12, 'f'},
            pair<int, char>{18, 'g'}, pair<int, char>{3, 'h'},
            pair<int, char>{9, 'i'}, pair<int, char>{3, 'j'}},
            allocator<pair<int, char>>{}};
        if (t19.empty() || t19.size() != 8 || (*t19.begin()).first != 3 ||
            (*t19.begin()).second != 'h' || (*--t19.end()).first != 29 ||
            (*--t19.end()).second != 'd')
        {
            ++fail_count;
            std::cout << "FAILED map(initializer_list, allocator) with duplicates\n";
        }
        // Iterator range constructor.
        vector<pair<int, char>> v1 {pair<int, char>{10, 'a'},
            pair<int, char>{19, 'b'}, pair<int, char>{13, 'c'},
            pair<int, char>{29, 'd'}, pair<int, char>{9, 'e'},
            pair<int, char>{12, 'f'}, pair<int, char>{18, 'g'},
            pair<int, char>{3, 'h'}, pair<int, char>{9, 'i'},
            pair<int, char>{3, 'j'}};
        map<int, char> t20 {v1.begin(), v1.end()};
        if (t20.empty() || t20.size() != 8 || (*t20.begin()).first != 3 ||
            (*t20.begin()).second != 'h' || (*--t20.end()).first != 29 ||
            (*--t20.end()).second != 'd')
        {
            ++fail_count;
            std::cout << "FAILED map(Iter, Iter) with duplicates\n";
        }
        // Iterator range constructor with allocator.
        vector<pair<int, char>> v2 {pair<int, char>{10, 'a'},
            pair<int, char>{19, 'b'}, pair<int, char>{13, 'c'},
            pair<int, char>{29, 'd'}, pair<int, char>{9, 'e'},
            pair<int, char>{12, 'f'}, pair<int, char>{18, 'g'},
            pair<int, char>{3, 'h'}, pair<int, char>{9, 'i'},
            pair<int, char>{3, 'j'}};
        map<int, char> t21 {v2.begin(), v2.end(), allocator<pair<int, char>>{}};
        if (t21.empty() || t21.size() != 8 || (*t21.begin()).first != 3 ||
            (*t21.begin()).second != 'h' || (*--t21.end()).first != 29 ||
            (*--t21.end()).second != 'd')
        {
            ++fail_count;
            std::cout << "FAILED map(Iter, Iter, allocator) with duplicates\n";
        }
    }
    // Assignment.
    {
        map<int, char> t1;
        t1[15] = 'a'; t1[18] = 'b'; t1[17] = 'c'; t1[2] = 'd';
        t1[10] = 'e'; t1[13] = 'f'; t1[9] = 'g'; t1[1] = 'h';
        map<int, char> t2;
        t2 = t1;
        if (t1.empty() || t1.size() != 8 || (*t1.begin()).first != 1 ||
            (*t1.begin()).second != 'h' || (*--t1.end()).first != 18 ||
            (*--t1.end()).second != 'b' ||
            t2.empty() || t2.size() != 8 || (*t2.begin()).first != 1 ||
            (*t2.begin()).second != 'h' || (*--t2.end()).first != 18 ||
            (*--t2.end()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map copy assignment not empty\n";
        }
        map<int, char> t3;
        map<int, char> t4;
        t4[15] = 'a'; t4[18] = 'b'; t4[17] = 'c'; t4[2] = 'd';
        t4[10] = 'e'; t4[13] = 'f'; t4[9] = 'g'; t4[1] = 'h';
        t4 = t3;
        if (!t3.empty() || t3.size() != 0 || !t4.empty() || t4.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED map copy assignment empty\n";
        }
        map<int, char> t5;
        t5[15] = 'a'; t5[18] = 'b'; t5[17] = 'c'; t5[2] = 'd';
        t5[10] = 'e'; t5[13] = 'f'; t5[9] = 'g'; t5[1] = 'h';
        map<int, char> t6;
        t6 = klib::move(t5);
        if (t6.empty() || t6.size() != 8 || (*t6.begin()).first != 1 ||
            (*t6.begin()).second != 'h' || (*--t6.end()).first != 18 ||
            (*--t6.end()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map move assignment not empty\n";
        }
        map<int, char> t7;
        map<int, char> t8;
        t8[15] = 'a'; t8[18] = 'b'; t8[17] = 'c'; t8[2] = 'd';
        t8[10] = 'e'; t8[13] = 'f'; t8[9] = 'g'; t8[1] = 'h';
        t8 = klib::move(t7);
        if (!t8.empty() || t8.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED map move assignment empty\n";
        }
        map<int, test_type_2> t9;
        t9[15] = test_type_2{1}; t9[18] = test_type_2{2};
        t9[17] = test_type_2{3}; t9[2] = test_type_2{4};
        t9[10] = test_type_2{5}; t9[13] = test_type_2{6};
        t9[9] = test_type_2{7}; t9[1] = test_type_2{8};
        map<int, test_type_2> t10;
        t10 = t9;
        if (t9.empty() || t9.size() != 8 || (*t9.begin()).first != 1 ||
            *(*t9.begin()).second.val != 8 || (*--t9.end()).first != 18 ||
            *(*--t9.end()).second.val != 2 ||
            t10.empty() || t10.size() != 8 || (*t10.begin()).first != 1 ||
            *(*t10.begin()).second.val != 8 || (*--t10.end()).first != 18 ||
            *(*--t10.end()).second.val != 2)
        {
            ++fail_count;
            std::cout << "FAILED map copy assignment not empty heap type\n";
        }
        map<int, test_type_2> t11;
        map<int, test_type_2> t12;
        t12[15] = test_type_2{1}; t12[18] = test_type_2{2};
        t12[17] = test_type_2{3}; t12[2] = test_type_2{4};
        t12[10] = test_type_2{5}; t12[13] = test_type_2{6};
        t12[9] = test_type_2{7}; t12[1] = test_type_2{8};
        t12 = t11;
        if (!t11.empty() || t11.size() != 0 || !t12.empty() || t12.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED map copy assignment empty heap type\n";
        }
        map<int, test_type_2> t13;
        t13[15] = test_type_2{1}; t13[18] = test_type_2{2};
        t13[17] = test_type_2{3}; t13[2] = test_type_2{4};
        t13[10] = test_type_2{5}; t13[13] = test_type_2{6};
        t13[9] = test_type_2{7}; t13[1] = test_type_2{8};
        map<int, test_type_2> t14;
        t14 = klib::move(t13);
        if (t14.empty() || t14.size() != 8 || (*t14.begin()).first != 1 ||
            *(*t14.begin()).second.val != 8 || (*--t14.end()).first != 18 ||
            *(*--t14.end()).second.val != 2)
        {
            ++fail_count;
            std::cout << "FAILED map move assignment not empty heap type\n";
        }
        map<int, test_type_2> t15;
        map<int, test_type_2> t16;
        t16[15] = test_type_2{1}; t16[18] = test_type_2{2};
        t16[17] = test_type_2{3}; t16[2] = test_type_2{4};
        t16[10] = test_type_2{5}; t16[13] = test_type_2{6};
        t16[9] = test_type_2{7}; t16[1] = test_type_2{8};
        t16 = klib::move(t15);
        if (!t16.empty() || t16.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED map move assignment empty heap type\n";
        }
        map<int, char> t17;
        t17[15] = 'a'; t17[18] = 'b'; t17[17] = 'c'; t17[2] = 'd';
        t17[10] = 'e'; t17[13] = 'f'; t17[9] = 'g'; t17[1] = 'h';
        t17 = {pair<int, char>{10, 'a'}, pair<int, char>{19, 'b'},
            pair<int, char>{13, 'c'}, pair<int, char>{29, 'd'},
            pair<int, char>{9, 'e'}, pair<int, char>{12, 'f'},
            pair<int, char>{18, 'g'}, pair<int, char>{3, 'h'},
            pair<int, char>{9, 'i'}, pair<int, char>{3, 'j'}};
        if (t17.empty() || t17.size() != 8 || (*t17.begin()).first != 3 ||
            (*t1.begin()).second != 'h' || (*--t17.end()).first != 29 ||
            (*--t17.end()).second != 'd')
        {
            ++fail_count;
            std::cout << "FAILED map initialiser list assignment not empty\n";
        }
        map<int, char> t18;
        t18[15] = 'a'; t18[18] = 'b'; t18[17] = 'c'; t18[2] = 'd';
        t18[10] = 'e'; t18[13] = 'f'; t18[9] = 'g'; t18[1] = 'h';
        t18 = {};
        if (!t18.empty() || t18.size() != 0 || !t18.empty() || t18.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED map initialiser list assignment empty\n";
        }
    }
    // get_allocator
    {
        map<int, char> t1;
        if (t1.get_allocator() != allocator<pair<const int, char>> {})
        {
            ++fail_count;
            std::cout << "FAILED map::get_allocator default int char\n";
        }
        map<int, char> t2 {allocator<int>{}};
        if (t2.get_allocator() != allocator<pair<const int, char>> {})
        {
            ++fail_count;
            std::cout << "FAILED map::get_allocator default rebind pair<int, char>\n";
        }
        map<test_type, char> t3;
        if (t3.get_allocator() != allocator<pair<const test_type, char>> {})
        {
            ++fail_count;
            std::cout << "FAILED map::get_allocator default cutsom\n";
        }
        map<test_type, char> t4 {allocator<int>{}};
        if (t4.get_allocator() != allocator<pair<const test_type, char>> {})
        {
            ++fail_count;
            std::cout << "FAILED map::get_allocator default rebind custom\n";
        }
    }
    // operator[]
    {
        map<int, int> t1;
        t1[15] = 1; t1[18] = 8; t1[17] = 3; t1[2] = 7;
        t1[10] = 3; t1[13] = 6; t1[9] = 4; t1[1] = 5;
        if (t1.empty() || t1.size() != 8 || (*t1.begin()).first != 1 ||
            (*t1.begin()).second != 5 || (*--t1.end()).first != 18 ||
            (*--t1.end()).second != 8)
        {
            ++fail_count;
            std::cout << "FAILED map::operator[] insert\n";
        }
        if (t1[15] != 1 || t1[18] != 8 || t1[17] != 3 || t1[2] != 7 ||
            t1[10] != 3 || t1[13] != 6 || t1[9] != 4 || t1[1] != 5)
        {
            ++fail_count;
            std::cout << "FAILED map::operator[] access\n";
        }
        t1[1] = 9; t1[18] = 10;
        if (t1.empty() || t1.size() != 8 || (*t1.begin()).first != 1 ||
            (*t1.begin()).second != 9 || (*--t1.end()).first != 18 ||
            (*--t1.end()).second != 10)
        {
            ++fail_count;
            std::cout << "FAILED map::operator[] change\n";
        }
    }
    // iterator types
    {
        if (!is_same<map<int, char>::iterator::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator::iterator_category\n";
        }
        if (!is_same<map<int, char>::iterator::value_type,
            pair<const int, char>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator::value_type\n";
        }
        if (!is_same<map<int, char>::iterator::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator::difference_type\n";
        }
        if (!is_same<map<int, char>::iterator::pointer,
            pair<const int, char>*>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator::pointer\n";
        }
        if (!is_same<map<int, char>::iterator::reference,
            pair<const int, char>&>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator::reference\n";
        }
        if (!is_same<iterator_traits<map<int, char>::iterator>::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::iterator_category\n";
        }
        if (!is_same<iterator_traits<map<int, char>::iterator>::value_type,
            pair<const int, char>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::value_type\n";
        }
        if (!is_same<iterator_traits<map<int, char>::iterator>::difference_type,
            ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::difference_type\n";
        }
        if (!is_same<iterator_traits<map<int, char>::iterator>::pointer,
            pair<const int, char>*>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::pointer\n";
        }
        if (!is_same<iterator_traits<map<int, char>::iterator>::reference,
            pair<const int, char>&>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::reference\n";
        }
    }
    // const_iterator types
    {
        if (!is_same<map<int, char>::const_iterator::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_iterator::iterator_category const\n";
        }
        if (!is_same<map<int, char>::const_iterator::value_type,
            const pair<const int, char>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_iterator::value_type const\n";
        }
        if (!is_same<map<int, char>::const_iterator::difference_type,
            ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_iterator::difference_type const\n";
        }
        if (!is_same<map<int, char>::const_iterator::pointer,
            const pair<const int, char>*>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_iterator::pointer const\n";
        }
        if (!is_same<map<int, char>::const_iterator::reference,
            const pair<const int, char>&>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_iterator::reference const\n";
        }
        if (!is_same<
            iterator_traits<map<int, char>::const_iterator>::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::iterator_category const\n";
        }
        if (!is_same<iterator_traits<map<int, char>::const_iterator>::value_type,
            const pair<const int, char>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::value_type const\n";
        }
        if (!is_same<
            iterator_traits<map<int, char>::const_iterator>::difference_type,
            ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::difference_type const\n";
        }
        if (!is_same<iterator_traits<map<int, char>::const_iterator>::pointer,
            const pair<const int, char>*>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::pointer const\n";
        }
        if (!is_same<iterator_traits<map<int, char>::const_iterator>::reference,
            const pair<const int, char>&>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::reference const\n";
        }
    }
    // reverse_iterator types
    {
        if (!is_same<map<int, char>::reverse_iterator::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::reverse_iterator::iterator_category\n";
        }
        if (!is_same<map<int, char>::reverse_iterator::value_type,
            pair<const int, char>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::reverse_iterator::value_type\n";
        }
        if (!is_same<map<int, char>::reverse_iterator::difference_type,
            ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::reverse_iterator::difference_type\n";
        }
        if (!is_same<map<int, char>::reverse_iterator::pointer,
            pair<const int, char>*>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::reverse_iterator::pointer\n";
        }
        if (!is_same<map<int, char>::reverse_iterator::reference,
            pair<const int, char>&>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::reverse_iterator::reference\n";
        }
        if (!is_same<
            iterator_traits<map<int, char>::reverse_iterator>::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::iterator_category reverse\n";
        }
        if (!is_same<
            iterator_traits<map<int, char>::reverse_iterator>::value_type,
            pair<const int, char>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::value_type reverse\n";
        }
        if (!is_same<
            iterator_traits<map<int, char>::reverse_iterator>::difference_type,
            ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::difference_type reverse\n";
        }
        if (!is_same<iterator_traits<map<int, char>::reverse_iterator>::pointer,
            pair<const int, char>*>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::pointer reverse\n";
        }
        if (!is_same<iterator_traits<map<int, char>::reverse_iterator>::reference,
            pair<const int, char>&>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::reference reverse\n";
        }
    }
    // const_reverse_iterator types
    {
        if (!is_same<map<int, char>::const_reverse_iterator::iterator_category,
            bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_reverse_iterator::iterator_category const\n";
        }
        if (!is_same<map<int, char>::const_reverse_iterator::value_type,
            const pair<const int, char>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_reverse_iterator::value_type const\n";
        }
        if (!is_same<map<int, char>::const_reverse_iterator::difference_type,
            ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_reverse_iterator::difference_type const\n";
        }
        if (!is_same<map<int, char>::const_reverse_iterator::pointer,
            const pair<const int, char>*>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_reverse_iterator::pointer const\n";
        }
        if (!is_same<map<int, char>::const_reverse_iterator::reference,
            const pair<const int, char>&>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::const_reverse_iterator::reference const\n";
        }
        if (!is_same< iterator_traits<map<int, char>::const_reverse_iterator>
            ::iterator_category, bidirectional_iterator_tag>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::iterator_category const reverse\n";
        }
        if (!is_same<
            iterator_traits<map<int, char>::const_reverse_iterator>::value_type,
            const pair<const int, char>>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::value_type const reverse\n";
        }
        if (!is_same<iterator_traits<map<int, char>::const_reverse_iterator>
            ::difference_type, ptrdiff_t>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::difference_type const reverse\n";
        }
        if (!is_same<
            iterator_traits<map<int, char>::const_reverse_iterator>::pointer,
            const pair<const int, char>*>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::pointer const reverse\n";
        }
        if (!is_same<
            iterator_traits<map<int, char>::const_reverse_iterator>::reference,
            const pair<const int, char>&>::value)
        {
            ++fail_count;
            std::cout << "FAILED map::iterator_traits::reference const reverse\n";
        }
    }
    // begin/end
    {
        map<int, char> t1;
        if (t1.begin() != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::begin empty\n";
        }
        map<int, char> t2;
        t2[10] = 'a';
        if ((*t2.begin()).first != 10 || (*t2.begin()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::begin 1 element\n";
        }
        t2[15] = 'b';
        if ((*t2.begin()).first != 10 || (*t2.begin()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::begin insert after\n";
        }
        t2[5] = 'c';
        if ((*t2.begin()).first != 5 || (*t2.begin()).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::begin insert before\n";
        }
        t2.erase(10);
        if ((*t2.begin()).first != 5 || (*t2.begin()).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::begin erase middle\n";
        }
        t2.erase(5);
        if ((*t2.begin()).first !=15 || (*t2.begin()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::begin erase start\n";
        }
        t2.erase(15);
        if (t2.begin() != t2.end())
        {
            ++fail_count;
            std::cout << "FAILED map::begin erase empty\n";
        }
        map<int, char> t3;
        t3[10] = 'a';
        if ((*--t3.end()).first != 10 || (*--t3.end()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::end 1 element\n";
        }
        t3[15] = 'b';
        if ((*--t3.end()).first != 15 || (*--t3.end()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::end insert after\n";
        }
        t3[5] = 'c';
        if ((*--t3.end()).first != 15 || (*--t3.end()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::end insert before\n";
        }
        t3.erase(10);
        if ((*--t3.end()).first != 15 || (*--t3.end()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::end erase middle\n";
        }
        t3.erase(15);
        if ((*--t3.end()).first != 5 || (*--t3.end()).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::end erase end\n";
        }
        t3.erase(5);
        if (t3.begin() != t3.end())
        {
            ++fail_count;
            std::cout << "FAILED map::end erase empty\n";
        }
    }
    // cbegin/cend
    {
        map<int, char> t1;
        if (t1.cbegin() != t1.cend())
        {
            ++fail_count;
            std::cout << "FAILED map::cbegin empty\n";
        }
        map<int, char> t2;
        t2[10] = 'a';
        if ((*t2.cbegin()).first != 10 || (*t2.cbegin()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::cbegin 1 element\n";
        }
        t2[15] = 'b';
        if ((*t2.cbegin()).first != 10 || (*t2.cbegin()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::cbegin insert after\n";
        }
        t2[5] = 'c';
        if ((*t2.cbegin()).first != 5 || (*t2.cbegin()).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::cbegin insert before\n";
        }
        t2.erase(10);
        if ((*t2.cbegin()).first != 5 || (*t2.cbegin()).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::cbegin erase middle\n";
        }
        t2.erase(5);
        if ((*t2.cbegin()).first !=15 || (*t2.cbegin()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::cbegin erase start\n";
        }
        t2.erase(15);
        if (t2.cbegin() != t2.cend())
        {
            ++fail_count;
            std::cout << "FAILED map::cbegin erase empty\n";
        }
        map<int, char> t3;
        t3[10] = 'a';
        if ((*--t3.cend()).first != 10 || (*--t3.cend()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::cend 1 element\n";
        }
        t3[15] = 'b';
        if ((*--t3.cend()).first != 15 || (*--t3.cend()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::cend insert after\n";
        }
        t3[5] = 'c';
        if ((*--t3.cend()).first != 15 || (*--t3.cend()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::cend insert before\n";
        }
        t3.erase(10);
        if ((*--t3.cend()).first != 15 || (*--t3.cend()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::cend erase middle\n";
        }
        t3.erase(15);
        if ((*--t3.cend()).first != 5 || (*--t3.cend()).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::cend erase end\n";
        }
        t3.erase(5);
        if (t3.cbegin() != t3.cend())
        {
            ++fail_count;
            std::cout << "FAILED map::cend erase empty\n";
        }
    }
    // iterator increments/decrements
    {
        map<int, char> t1;
        t1[1] = 'a'; t1[2] = 'b'; t1[3] = 'c';
        auto it1 = t1.begin();
        auto it2 = ++it1;
        if ((*it1).first != 2 || (*it1).second != 'b'||
            (*it2).first != 2 || (*it2).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator pre operator++\n";
        }
        auto it3 = it1++;
        if ((*it1).first != 3 || (*it1).second != 'c'||
            (*it3).first != 2 || (*it2).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator post operator++\n";
        }
        auto it4 = --it1;
        if ((*it1).first != 2 || (*it1).second != 'b'||
            (*it4).first != 2 || (*it4).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator pre operator--\n";
        }
        auto it5 = it1--;
        if ((*it1).first != 1 || (*it1).second != 'a'||
            (*it5).first != 2 || (*it5).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator post operator--\n";
        }
        it1 = t1.end();
        --it1;
        if ((*it1).first != 3 || (*it1).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator operator-- on end\n";
        }
        ++it1;
        if (it1 != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::iterator operator++ to end\n";
        }
    }
    // rbegin/rend
    {
        map<int, char> t1;
        if (t1.rbegin() != t1.rend())
        {
            ++fail_count;
            std::cout << "FAILED map::rbegin empty\n";
        }
        map<int, char> t2;
        t2[10] = 'a';
        if ((*t2.rbegin()).first != 10 || (*t2.rbegin()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::rbegin 1 element\n";
        }
        t2[15] = 'b';
        if ((*t2.rbegin()).first != 15 || (*t2.rbegin()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::rbegin insert after\n";
        }
        t2[5] = 'c';
        if ((*t2.rbegin()).first != 15 || (*t2.rbegin()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::rbegin insert before\n";
        }
        t2.erase(10);
        if ((*t2.rbegin()).first != 15 || (*t2.rbegin()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::rbegin erase middle\n";
        }
        t2.erase(15);
        if ((*t2.rbegin()).first != 5 || (*t2.rbegin()).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::rbegin erase end\n";
        }
        t2.erase(5);
        if (t2.rbegin() != t2.rend())
        {
            ++fail_count;
            std::cout << "FAILED map::rbegin erase empty\n";
        }
        map<int, char> t3;
        t3[10] = 'a';
        if ((*--t3.rend()).first != 10 || (*--t3.rend()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::rend 1 element\n";
        }
        t3[15] = 'b';
        if ((*--t3.rend()).first != 10 || (*--t3.rend()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::rend insert after\n";
        }
        t3[5] = 'c';
        if ((*--t3.rend()).first != 5 || (*--t3.rend()).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::rend insert before\n";
        }
        t3.erase(10);
        if ((*--t3.rend()).first != 5 || (*--t3.rend()).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::rend erase middle\n";
        }
        t3.erase(5);
        if ((*--t3.rend()).first != 15 || (*--t3.rend()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::rend erase start\n";
        }
        t3.erase(15);
        if (t3.rbegin() != t3.rend())
        {
            ++fail_count;
            std::cout << "FAILED map::rend erase empty\n";
        }
    }
    // crbegin/crend
    {
        map<int, char> t1;
        if (t1.crbegin() != t1.crend())
        {
            ++fail_count;
            std::cout << "FAILED map::crbegin empty\n";
        }
        map<int, char> t2;
        t2[10] = 'a';
        if ((*t2.crbegin()).first != 10 || (*t2.crbegin()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::crbegin 1 element\n";
        }
        t2[15] = 'b';
        if ((*t2.crbegin()).first != 15 || (*t2.crbegin()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::crbegin insert after\n";
        }
        t2[5] = 'c';
        if ((*t2.crbegin()).first != 15 || (*t2.crbegin()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::crbegin insert before\n";
        }
        t2.erase(10);
        if ((*t2.crbegin()).first != 15 || (*t2.crbegin()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::crbegin erase middle\n";
        }
        t2.erase(15);
        if ((*t2.crbegin()).first != 5 || (*t2.crbegin()).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::crbegin erase end\n";
        }
        t2.erase(5);
        if (t2.crbegin() != t2.crend())
        {
            ++fail_count;
            std::cout << "FAILED map::crbegin erase empty\n";
        }
        map<int, char> t3;
        t3[10] = 'a';
        if ((*--t3.crend()).first != 10 || (*--t3.crend()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::crend 1 element\n";
        }
        t3[15] = 'b';
        if ((*--t3.crend()).first != 10 || (*--t3.crend()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::crend insert after\n";
        }
        t3[5] = 'c';
        if ((*--t3.crend()).first != 5 || (*--t3.crend()).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::crend insert before\n";
        }
        t3.erase(10);
        if ((*--t3.crend()).first != 5 || (*--t3.crend()).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::crend erase middle\n";
        }
        t3.erase(5);
        if ((*--t3.crend()).first != 15 || (*--t3.crend()).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::crend erase start\n";
        }
        t3.erase(15);
        if (t3.crbegin() != t3.crend())
        {
            ++fail_count;
            std::cout << "FAILED map::crend erase empty\n";
        }
    }
    // reverse_iterator increments/decrements
    {
        map<int, char> t1;
        t1[1] = 'a'; t1[2] = 'b'; t1[3] = 'c';
        auto it1 = t1.rbegin();
        auto it2 = ++it1;
        if ((*it1).first != 2 || (*it1).second != 'b' ||
            (*it2).first != 2 || (*it2).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::reverse_iterator pre operator++\n";
        }
        auto it3 = it1++;
        if ((*it1).first != 1 || (*it1).second != 'a' ||
            (*it3).first != 2 || (*it3).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::reverse_iterator post operator++\n";
        }
        auto it4 = --it1;
        if ((*it1).first != 2 || (*it1).second != 'b' ||
            (*it4).first != 2 || (*it4).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::reverse_iterator pre operator--\n";
        }
        auto it5 = it1--;
        if ((*it1).first != 3 || (*it1).second != 'c' ||
            (*it5).first != 2 || (*it5).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::reverse_iterator post operator--\n";
        }
        it1 = t1.rend();
        --it1;
        if ((*it1).first != 1 || (*it1).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::reverse_iterator operator-- on rend\n";
        }
        ++it1;
        if (it1 != t1.rend())
        {
            ++fail_count;
            std::cout << "FAILED map::reverse_iterator operator++ to rend\n";
        }
    }
    // Iterator invalidation.
    {
        map<int, char> t1;
        auto it1 = t1.begin();
        auto it2 = t1.end();
        t1[1] = 'a'; t1[3] = 'c'; t1[5] = 'e';
        if (it1 != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::iterator start, insert on empty, invalidated\n";
        }
        if (it2 != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::iterator end, insert on empty, invalidated\n";
        }
        auto it3 = t1.begin();
        auto it4 = ++t1.begin();
        auto it5 = --t1.end();
        auto it6 = t1.end();
        t1[2] = 'b'; t1[4] = 'd'; t1[6] = 'f';
        if ((*it3).first != 1 || (*it3).second != 'a' || it3 != t1.begin())
        {
            ++fail_count;
            std::cout << "FAILED map::iterator start, insert, invalidated\n";
        }
        if ((*it4).first != 3 || (*it4).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator middle, insert, invalidated\n";
        }
        if ((*it5).first != 5 || (*it5).second != 'e')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator last, insert, invalidated\n";
        }
        if (it6 != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::iterator end, insert, invalidated\n";
        }
        ++it3;
        ++it4;
        ++it5;
        if ((*it3).first != 2 || (*it3).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator start, increment after insert, invalidated\n";
        }
        if ((*it4).first != 4 || (*it4).second != 'd')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator middle, increment after insert, invalidated\n";
        }
        if ((*it5).first != 6 || (*it5).second != 'f')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator last, increment after insert, invalidated\n";
        }
        t1.erase(1);
        t1.erase(3);
        t1.erase(5);
        if ((*it3).first != 2 || (*it3).second != 'b' || it3 != t1.begin())
        {
            ++fail_count;
            std::cout << "FAILED map::iterator start, erase, invalidated\n";
        }
        if ((*it4).first != 4 || (*it4).second != 'd')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator middle, erase, invalidated\n";
        }
        if ((*it5).first != 6 || (*it5).second != 'f')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator last, erase, invalidated\n";
        }
        ++it3;
        ++it4;
        ++it5;
        if ((*it3).first != 4 || (*it3).second != 'd')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator start, increment after erase, invalidated\n";
        }
        if ((*it4).first != 6 || (*it4).second != 'f')
        {
            ++fail_count;
            std::cout << "FAILED map::iterator middle, increment after erase, invalidated\n";
        }
        if (it5 != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::iterator last, increment after erase, invalidated\n";
        }  
    }
    // empty
    {
        map<int, char> t1;
        if (!t1.empty())
        {
            ++fail_count;
            std::cout << "FAILED map::empty on construction\n";
        }
        t1[1] = 'a'; t1[3] = 'b'; t1[5] = 'c';
        if (t1.empty())
        {
            ++fail_count;
            std::cout << "FAILED map::empty after insert\n";
        }
        for (size_t i = 0; i < 3; ++i)
            t1.erase(t1.begin());
        if (!t1.empty())
        {
            ++fail_count;
            std::cout << "FAILED map::empty after erase by iterator\n";
        }
        t1[1] = 'a'; t1[3] = 'b'; t1[5] = 'c';
        if (t1.empty())
        {
            ++fail_count;
            std::cout << "FAILED map::empty after reinsert\n";
        }
        t1.erase(1); t1.erase(3); t1.erase(5);
        if (!t1.empty())
        {
            ++fail_count;
            std::cout << "FAILED map::empty after erase by value\n";
        }
    }
    // size
    {
        map<int, char> t1;
        if (t1.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED map::size on construction\n";
        }
        t1[1] = 'a'; t1[3] = 'b'; t1[5] = 'c';
        if (t1.size() != 3)
        {
            ++fail_count;
            std::cout << "FAILED map::size after insert\n";
        }
        for (size_t i = 0; i < 2; ++i)
            t1.erase(t1.begin());
        if (t1.size() != 1)
        {
            ++fail_count;
            std::cout << "FAILED map::size after erase by iterator\n";
        }
        t1.erase(t1.begin());
        if (t1.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED map::size after erase to empty by iterator\n";
        }
        t1[1] = 'a'; t1[3] = 'b'; t1[5] = 'c';
        if (t1.size() != 3)
        {
            ++fail_count;
            std::cout << "FAILED map::size after reinsert\n";
        }
        t1.erase(3);
        if (t1.size() != 2)
        {
            ++fail_count;
            std::cout << "FAILED map::size after double erase by value\n";
        }
        t1.erase(1); t1.erase(5);
        if (t1.size() != 0)
        {
            ++fail_count;
            std::cout << "FAILED map::size after erase to empty by value\n";
        }
    }
    // max_size
    {
        map<int, char> t1;
        if (t1.max_size() != allocator_traits<map<int, char>::allocator_type>
            ::max_size(t1.get_allocator()))
        {
            ++fail_count;
            std::cout << "FAILED map::max_size default int allocator\n";
        }
        map<test_type, int> t2;
        if (t2.max_size() !=
            allocator_traits<map<test_type, int>::allocator_type>
            ::max_size(t2.get_allocator()))
        {
            ++fail_count;
            std::cout << "FAILED map::max_size default custom type allocator\n";
        }
    }
    // clear
    {
        map<int, char> t1;
        t1.clear();
        if (!t1.empty() || t1.size() != 0 || t1.begin() != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::clear from empty\n";
        }
        map<int, char> t2;
        t1[1] = 'a'; t1[2] = 'b'; t1[3] = 'c';
        t2.clear();
        if (!t2.empty() || t2.size() != 0 || t2.begin() != t2.end())
        {
            ++fail_count;
            std::cout << "FAILED map::clear after insert\n";
        }
        map<int, char> t3;
        t1[1] = 'a'; t1[2] = 'b'; t1[3] = 'c';
        t3.erase(2);
        t3.clear();
        if (!t3.empty() || t3.size() != 0 || t3.begin() != t3.end())
        {
            ++fail_count;
            std::cout << "FAILED map::clear after erase\n";
        }
        t3[2] = 'a';
        if (t3.empty() || t3.size() != 1 ||
            (*t3.begin()).first != 2 || (*t3.begin()).second != 'a' ||
            (*--t3.end()).first != 2 || (*--t3.end()).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::insert after clear\n";
        }
    }
    // insert
    {
        map<test_type_2, test_type_2> t1;
        const pair<const test_type_2, test_type_2> p1
            {test_type_2{17}, test_type_2{1}};
        auto r1 = t1.insert(p1);
        if (t1.empty() || t1.size() != 1 ||
            *(*t1.begin()).first.val != 17 || *(*t1.begin()).second.val != 1 ||
            r1.first != t1.begin() || r1.second != true)
        {
            ++fail_count;
            std::cout << "FAILED map::insert by const reference\n";
        }
        const pair<const test_type_2, test_type_2> p2
            {test_type_2{17}, test_type_2{2}};
        auto r2 = t1.insert(p2);
        if (t1.empty() || t1.size() != 1 ||
            *(*t1.begin()).first.val != 17 || *(*t1.begin()).second.val != 1 ||
            r2.first != t1.begin() || r2.second != false)
        {
            ++fail_count;
            std::cout << "FAILED map::insert by const reference duplicate\n";
        }
        map<test_type_2, test_type_2> t2;
        auto r3 = t2.insert(pair<test_type_2, test_type_2>
            {test_type_2{18}, test_type_2{3}});
        if (t2.empty() || t2.size() != 1 ||
            *(*t2.begin()).first.val != 18 || *(*t2.begin()).second.val != 3 ||
            r3.first != t2.begin() || r3.second != true)
        {
            ++fail_count;
            std::cout << "FAILED map::insert by rvalue reference construction\n";
        }
        auto r4 = t2.insert(pair<test_type_2, test_type_2>
            {test_type_2{18}, test_type_2{4}});
        if (t2.empty() || t2.size() != 1 ||
            *(*t2.begin()).first.val != 18 || *(*t2.begin()).second.val != 3 ||
            r4.first != t2.begin() || r4.second != false)
        {
            ++fail_count;
            std::cout << "FAILED map::insert by rvalue reference construction duplicate\n";
        }
        map<test_type_2, test_type_2> t3;
        auto r5 = t3.insert(pair<const test_type_2, test_type_2>
            {test_type_2{19}, test_type_2{5}});
        if (t3.empty() || t3.size() != 1 ||
            *(*t3.begin()).first.val != 19 || *(*t3.begin()).second.val != 5 ||
            r5.first != t3.begin() || r5.second != true)
        {
            ++fail_count;
            std::cout << "FAILED map::insert by rvalue reference\n";
        }
        auto r6 = t3.insert(pair<const test_type_2, test_type_2>
            {test_type_2{19}, test_type_2{6}});
        if (t3.empty() || t3.size() != 1 ||
            *(*t3.begin()).first.val != 19 || *(*t3.begin()).second.val != 5 ||
            r6.first != t3.begin() || r6.second != false)
        {
            ++fail_count;
            std::cout << "FAILED map::insert by rvalue reference duplicate\n";
        }
        // Hint, const reference
        {
            map <int, char> t1;
            t1[1] = 'a'; t1[3] = 'b';
            const pair<const int, char> p1 {2, 'c'};
            auto r1 = t1.insert(t1.begin(), p1);
            if (t1.empty() || t1.size() != 3 || (*++t1.begin()).first != 2 ||
                (*++t1.begin()).second != 'c' || (*r1).first != 2 ||
                (*r1).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by const reference hint miss beginning\n";
            }
            map <int, char> t2;
            t2[1] = 'a'; t2[3] = 'b';
            const pair<const int, char> p2 {0, 'c'};
            auto r2 = t2.insert(++t2.begin(), p2);
            if (t2.empty() || t2.size() != 3 || (*t2.begin()).first != 0 ||
                (*t2.begin()).second != 'c' || (*r2).first != 0 ||
                (*r2).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by const reference hint miss middle\n";
            }
            map <int, char> t3;
            t3[1] = 'a'; t3[3] = 'b';
            const pair<const int, char> p3 {2, 'c'};
            auto r3 = t3.insert(t3.end(), p3);
            if (t3.empty() || t3.size() != 3 || (*++t3.begin()).first != 2 ||
                (*++t3.begin()).second != 'c' || (*r3).first != 2 ||
                (*r3).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by const reference hint miss end\n";
            }
            map <int, char> t4;
            t4[1] = 'a'; t4[3] = 'b';
            const pair<const int, char> p4 {3, 'c'};
            auto r4 = t4.insert(t4.begin(), p4);
            if (t4.empty() || t4.size() != 2 || (*r4).first != 3 ||
                (*r4).second != 'b')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by const reference hint miss beginning duplicate\n";
            }
            map <int, char> t5;
            t5[1] = 'a'; t5[3] = 'b'; t5[5] = 'c';
            const pair<const int, char> p5 {5, 'd'};
            auto r5 = t5.insert(++t5.begin(), p5);
            if (t5.empty() || t5.size() != 3 || (*r5).first != 5 ||
                (*r5).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by const reference hint miss middle duplicate\n";
            }
            map <int, char> t6;
            t6[1] = 'a'; t6[3] = 'b';
            const pair<const int, char> p6 {1, 'c'};
            auto r6 = t6.insert(t6.end(), p6);
            if (t6.empty() || t6.size() != 2 || (*r6).first != 1 ||
                (*r6).second != 'a')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by const reference hint miss end duplicate\n";
            }
            map <int, char> t7;
            t7[1] = 'a'; t7[3] = 'b';
            const pair<const int, char> p7 {0, 'c'};
            auto r7 = t7.insert(t7.begin(), p7);
            if (t7.empty() || t7.size() != 3 || (*t7.begin()).first != 0 ||
                (*t7.begin()).second != 'c' || (*r7).first != 0 ||
                (*r7).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by const reference hint hit beginning\n";
            }
            map <int, char> t8;
            t8[1] = 'a'; t8[3] = 'b';
            const pair<const int, char> p8 {2, 'c'};
            auto r8 = t8.insert(++t8.begin(), p8);
            if (t8.empty() || t8.size() != 3 || (*++t8.begin()).first != 2 ||
                (*++t8.begin()).second != 'c' || (*r8).first != 2 ||
                (*r8).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by const reference hint hit middle\n";
            }
            map <int, char> t9;
            t9[1] = 'a'; t9[3] = 'b';
            const pair<const int, char> p9 {5, 'c'};
            auto r9 = t9.insert(t9.end(), p9);
            if (t9.empty() || t9.size() != 3 || (*--t9.end()).first != 5 ||
                (*--t9.end()).second != 'c' || (*r9).first != 5 ||
                (*r9).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by const reference hint hit end\n";
            }
            map <int, char> t10;
            t10[1] = 'a'; t10[3] = 'b';
            const pair<const int, char> p10 {1, 'c'};
            auto r10 = t10.insert(t10.begin(), p10);
            if (t10.empty() || t10.size() != 2 || (*r10).first != 1 ||
                (*r10).second != 'a')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by const reference hint hit beginning duplicate\n";
            }
            map <int, char> t11;
            t11[1] = 'a'; t11[3] = 'b';
            const pair<const int, char> p11 {3, 'c'};
            auto r11 = t11.insert(++t11.begin(), p11);
            if (t11.empty() || t11.size() != 2 || (*r11).first != 3 ||
                (*r11).second != 'b')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by const reference hint hit middle duplicate\n";
            }
            map <int, char> t12;
            t12[1] = 'a'; t12[3] = 'b';
            const pair<const int, char> p12 {3, 'c'};
            auto r12 = t12.insert(t12.end(), p12);
            if (t12.empty() || t12.size() != 2 || (*r12).first != 3 ||
                (*r12).second != 'b')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by const reference hint hit end duplicate\n";
            }
        }
        // Hint, rvalue reference construction
        {
            map <int, char> t1;
            t1[1] = 'a'; t1[3] = 'b';
            auto r1 = t1.insert(t1.begin(), pair<int, char>{2, 'c'});
            if (t1.empty() || t1.size() != 3 || (*++t1.begin()).first != 2 ||
                (*++t1.begin()).second != 'c' || (*r1).first != 2 ||
                (*r1).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference construction hint miss beginning\n";
            }
            map <int, char> t2;
            t2[1] = 'a'; t2[3] = 'b';
            auto r2 = t2.insert(++t2.begin(), pair<int, char>{0, 'c'});
            if (t2.empty() || t2.size() != 3 || (*t2.begin()).first != 0 ||
                (*t2.begin()).second != 'c' || (*r2).first != 0 ||
                (*r2).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference construction hint miss middle\n";
            }
            map <int, char> t3;
            t3[1] = 'a'; t3[3] = 'b';
            auto r3 = t3.insert(t3.end(), pair<int, char>{2, 'c'});
            if (t3.empty() || t3.size() != 3 || (*++t3.begin()).first != 2 ||
                (*++t3.begin()).second != 'c' || (*r3).first != 2 ||
                (*r3).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference construction hint miss end\n";
            }
            map <int, char> t4;
            t4[1] = 'a'; t4[3] = 'b';
            auto r4 = t4.insert(t4.begin(), pair<int, char>{3, 'c'});
            if (t4.empty() || t4.size() != 2 || (*r4).first != 3 ||
                (*r4).second != 'b')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference construction hint miss beginning duplicate\n";
            }
            map <int, char> t5;
            t5[1] = 'a'; t5[3] = 'b'; t5[5] = 'c';
            auto r5 = t5.insert(++t5.begin(), pair<int, char>{5, 'd'});
            if (t5.empty() || t5.size() != 3 || (*r5).first != 5 ||
                (*r5).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference construction hint miss middle duplicate\n";
            }
            map <int, char> t6;
            t6[1] = 'a'; t6[3] = 'b';
            auto r6 = t6.insert(t6.end(), pair<int, char>{1, 'c'});
            if (t6.empty() || t6.size() != 2 || (*r6).first != 1 ||
                (*r6).second != 'a')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference construction hint miss end duplicate\n";
            }
            map <int, char> t7;
            t7[1] = 'a'; t7[3] = 'b';
            auto r7 = t7.insert(t7.begin(), pair<int, char>{0, 'c'});
            if (t7.empty() || t7.size() != 3 || (*t7.begin()).first != 0 ||
                (*t7.begin()).second != 'c' || (*r7).first != 0 ||
                (*r7).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference construction hint hit beginning\n";
            }
            map <int, char> t8;
            t8[1] = 'a'; t8[3] = 'b';
            auto r8 = t8.insert(++t8.begin(), pair<int, char>{2, 'c'});
            if (t8.empty() || t8.size() != 3 || (*++t8.begin()).first != 2 ||
                (*++t8.begin()).second != 'c' || (*r8).first != 2 ||
                (*r8).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference construction hint hit middle\n";
            }
            map <int, char> t9;
            t9[1] = 'a'; t9[3] = 'b';
            auto r9 = t9.insert(t9.end(), pair<int, char>{5, 'c'});
            if (t9.empty() || t9.size() != 3 || (*--t9.end()).first != 5 ||
                (*--t9.end()).second != 'c' || (*r9).first != 5 ||
                (*r9).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference construction hint hit end\n";
            }
            map <int, char> t10;
            t10[1] = 'a'; t10[3] = 'b';
            auto r10 = t10.insert(t10.begin(), pair<int, char>{1, 'c'});
            if (t10.empty() || t10.size() != 2 || (*r10).first != 1 ||
                (*r10).second != 'a')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference construction hint hit beginning duplicate\n";
            }
            map <int, char> t11;
            t11[1] = 'a'; t11[3] = 'b';
            auto r11 = t11.insert(++t11.begin(), pair<int, char>{3, 'c'});
            if (t11.empty() || t11.size() != 2 || (*r11).first != 3 ||
                (*r11).second != 'b')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference construction hint hit middle duplicate\n";
            }
            map <int, char> t12;
            t12[1] = 'a'; t12[3] = 'b';
            auto r12 = t12.insert(t12.end(), pair<int, char>{3, 'c'});
            if (t12.empty() || t12.size() != 2 || (*r12).first != 3 ||
                (*r12).second != 'b')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference construction hint hit end duplicate\n";
            }
        }
        // Hint, rvalue reference
        {
            map <int, char> t1;
            t1[1] = 'a'; t1[3] = 'b';
            auto r1 = t1.insert(t1.begin(), pair<const int, char>{2, 'c'});
            if (t1.empty() || t1.size() != 3 || (*++t1.begin()).first != 2 ||
                (*++t1.begin()).second != 'c' || (*r1).first != 2 ||
                (*r1).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference hint miss beginning\n";
            }
            map <int, char> t2;
            t2[1] = 'a'; t2[3] = 'b';
            auto r2 = t2.insert(++t2.begin(), pair<const int, char>{0, 'c'});
            if (t2.empty() || t2.size() != 3 || (*t2.begin()).first != 0 ||
                (*t2.begin()).second != 'c' || (*r2).first != 0 ||
                (*r2).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference hint miss middle\n";
            }
            map <int, char> t3;
            t3[1] = 'a'; t3[3] = 'b';
            auto r3 = t3.insert(t3.end(), pair<const int, char>{2, 'c'});
            if (t3.empty() || t3.size() != 3 || (*++t3.begin()).first != 2 ||
                (*++t3.begin()).second != 'c' || (*r3).first != 2 ||
                (*r3).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference hint miss end\n";
            }
            map <int, char> t4;
            t4[1] = 'a'; t4[3] = 'b';
            auto r4 = t4.insert(t4.begin(), pair<const int, char>{3, 'c'});
            if (t4.empty() || t4.size() != 2 || (*r4).first != 3 ||
                (*r4).second != 'b')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference hint miss beginning duplicate\n";
            }
            map <int, char> t5;
            t5[1] = 'a'; t5[3] = 'b'; t5[5] = 'c';
            auto r5 = t5.insert(++t5.begin(), pair<const int, char>{5, 'd'});
            if (t5.empty() || t5.size() != 3 || (*r5).first != 5 ||
                (*r5).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference hint miss middle duplicate\n";
            }
            map <int, char> t6;
            t6[1] = 'a'; t6[3] = 'b';
            auto r6 = t6.insert(t6.end(), pair<const int, char>{1, 'c'});
            if (t6.empty() || t6.size() != 2 || (*r6).first != 1 ||
                (*r6).second != 'a')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference hint miss end duplicate\n";
            }
            map <int, char> t7;
            t7[1] = 'a'; t7[3] = 'b';
            auto r7 = t7.insert(t7.begin(), pair<const int, char>{0, 'c'});
            if (t7.empty() || t7.size() != 3 || (*t7.begin()).first != 0 ||
                (*t7.begin()).second != 'c' || (*r7).first != 0 ||
                (*r7).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference hint hit beginning\n";
            }
            map <int, char> t8;
            t8[1] = 'a'; t8[3] = 'b';
            auto r8 = t8.insert(++t8.begin(), pair<const int, char>{2, 'c'});
            if (t8.empty() || t8.size() != 3 || (*++t8.begin()).first != 2 ||
                (*++t8.begin()).second != 'c' || (*r8).first != 2 ||
                (*r8).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference hint hit middle\n";
            }
            map <int, char> t9;
            t9[1] = 'a'; t9[3] = 'b';
            auto r9 = t9.insert(t9.end(), pair<const int, char>{5, 'c'});
            if (t9.empty() || t9.size() != 3 || (*--t9.end()).first != 5 ||
                (*--t9.end()).second != 'c' || (*r9).first != 5 ||
                (*r9).second != 'c')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference hint hit end\n";
            }
            map <int, char> t10;
            t10[1] = 'a'; t10[3] = 'b';
            auto r10 = t10.insert(t10.begin(), pair<const int, char>{1, 'c'});
            if (t10.empty() || t10.size() != 2 || (*r10).first != 1 ||
                (*r10).second != 'a')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference hint hit beginning duplicate\n";
            }
            map <int, char> t11;
            t11[1] = 'a'; t11[3] = 'b';
            auto r11 = t11.insert(++t11.begin(), pair<const int, char>{3, 'c'});
            if (t11.empty() || t11.size() != 2 || (*r11).first != 3 ||
                (*r11).second != 'b')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference hint hit middle duplicate\n";
            }
            map <int, char> t12;
            t12[1] = 'a'; t12[3] = 'b';
            auto r12 = t12.insert(t12.end(), pair<const int, char>{3, 'c'});
            if (t12.empty() || t12.size() != 2 || (*r12).first != 3 ||
                (*r12).second != 'b')
            {
                ++fail_count;
                std::cout << "FAILED map::insert by rvalue reference hint hit end duplicate\n";
            }
        }
    }
    // emplace
    {
        map<int, int> t1;
        t1[1] = 10; t1[3] = 13; t1[5] = 15;
        auto r1 = t1.emplace(2, 12);
        if (t1.empty() || t1.size() != 4 || (*++t1.begin()).first != 2 ||
            (*++t1.begin()).second != 12 || r1.first != ++t1.begin() ||
            r1.second != true)
        {
            ++fail_count;
            std::cout << "FAILED map::emplace\n";
        }
        map<int, int> t2;
        t2[1] = 10; t2[3] = 13; t2[5] = 15;
        auto r2 = t2.emplace(1, 12);
        if (t2.empty() || t2.size() != 3 || r2.first != t2.begin() ||
            r2.second != false)
        {
            ++fail_count;
            std::cout << "FAILED map::emplace duplicate\n";
        }
    }
    // emplace_hint
    {
        map <int, char> t1;
        t1[1] = 'a'; t1[3] = 'b';
        auto r1 = t1.emplace_hint(t1.begin(), 2, 'c');
        if (t1.empty() || t1.size() != 3 || (*++t1.begin()).first != 2 ||
            (*++t1.begin()).second != 'c' || (*r1).first != 2 ||
            (*r1).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::emplace_hint hint miss beginning\n";
        }
        map <int, char> t2;
        t2[1] = 'a'; t2[3] = 'b';
        auto r2 = t2.emplace_hint(++t2.begin(), 0, 'c');
        if (t2.empty() || t2.size() != 3 || (*t2.begin()).first != 0 ||
            (*t2.begin()).second != 'c' || (*r2).first != 0 ||
            (*r2).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::emplace_hint hint miss middle\n";
        }
        map <int, char> t3;
        t3[1] = 'a'; t3[3] = 'b';
        auto r3 = t3.emplace_hint(t3.end(), 2, 'c');
        if (t3.empty() || t3.size() != 3 || (*++t3.begin()).first != 2 ||
            (*++t3.begin()).second != 'c' || (*r3).first != 2 ||
            (*r3).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::emplace_hint hint miss end\n";
        }
        map <int, char> t4;
        t4[1] = 'a'; t4[3] = 'b';
        auto r4 = t4.emplace_hint(t4.begin(), 3, 'c');
        if (t4.empty() || t4.size() != 2 || (*r4).first != 3 ||
            (*r4).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::emplace_hint hint miss beginning duplicate\n";
        }
        map <int, char> t5;
        t5[1] = 'a'; t5[3] = 'b'; t5[5] = 'c';
        auto r5 = t5.emplace_hint(++t5.begin(), 5, 'd');
        if (t5.empty() || t5.size() != 3 || (*r5).first != 5 ||
            (*r5).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::emplace_hint hint miss middle duplicate\n";
        }
        map <int, char> t6;
        t6[1] = 'a'; t6[3] = 'b';
        auto r6 = t6.emplace_hint(t6.end(), 1, 'c');
        if (t6.empty() || t6.size() != 2 || (*r6).first != 1 ||
            (*r6).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::emplace_hint hint miss end duplicate\n";
        }
        map <int, char> t7;
        t7[1] = 'a'; t7[3] = 'b';
        auto r7 = t7.emplace_hint(t7.begin(), 0, 'c');
        if (t7.empty() || t7.size() != 3 || (*t7.begin()).first != 0 ||
            (*t7.begin()).second != 'c' || (*r7).first != 0 ||
            (*r7).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::emplace_hint hint hit beginning\n";
        }
        map <int, char> t8;
        t8[1] = 'a'; t8[3] = 'b';
        auto r8 = t8.emplace_hint(++t8.begin(), 2, 'c');
        if (t8.empty() || t8.size() != 3 || (*++t8.begin()).first != 2 ||
            (*++t8.begin()).second != 'c' || (*r8).first != 2 ||
            (*r8).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::emplace_hint hint hit middle\n";
        }
        map <int, char> t9;
        t9[1] = 'a'; t9[3] = 'b';
        auto r9 = t9.emplace_hint(t9.end(), 5, 'c');
        if (t9.empty() || t9.size() != 3 || (*--t9.end()).first != 5 ||
            (*--t9.end()).second != 'c' || (*r9).first != 5 ||
            (*r9).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::emplace_hint hint hit end\n";
        }
        map <int, char> t10;
        t10[1] = 'a'; t10[3] = 'b';
        auto r10 = t10.emplace_hint(t10.begin(), 1, 'c');
        if (t10.empty() || t10.size() != 2 || (*r10).first != 1 ||
            (*r10).second != 'a')
        {
            ++fail_count;
            std::cout << "FAILED map::emplace_hint hint hit beginning duplicate\n";
        }
        map <int, char> t11;
        t11[1] = 'a'; t11[3] = 'b';
        auto r11 = t11.emplace_hint(++t11.begin(), 3, 'c');
        if (t11.empty() || t11.size() != 2 || (*r11).first != 3 ||
            (*r11).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::emplace_hint hint hit middle duplicate\n";
        }
        map <int, char> t12;
        t12[1] = 'a'; t12[3] = 'b';
        auto r12 = t12.emplace_hint(t12.end(), 3, 'c');
        if (t12.empty() || t12.size() != 2 || (*r12).first != 3 ||
            (*r12).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::emplace_hint hint hit end duplicate\n";
        }
    }
    // erase
    {
        map <test_type_2, test_type_2> t1;
        t1[test_type_2{17}] = test_type_2{1};
        t1[test_type_2{11}] = test_type_2{4};
        t1[test_type_2{3}] = test_type_2{2};
        t1[test_type_2{29}] = test_type_2{3};
        auto r1 = t1.erase(t1.begin());
        if (t1.empty() || t1.size() != 3 || *(*r1).first.val != 11 ||
            *(*r1).second.val != 4)
        {
            ++fail_count;
            std::cout << "FAILED map::erase single iterator beginning\n";
        }
        map <test_type_2, test_type_2> t2;
        t2[test_type_2{17}] = test_type_2{1};
        t2[test_type_2{11}] = test_type_2{4};
        t2[test_type_2{3}] = test_type_2{2};
        t2[test_type_2{29}] = test_type_2{3};
        auto r2 = t2.erase(++t2.begin());
        if (t2.empty() || t2.size() != 3 || *(*r2).first.val != 17 ||
            *(*r2).second.val != 1)
        {
            ++fail_count;
            std::cout << "FAILED map::erase single iterator middle\n";
        }
        map <test_type_2, test_type_2> t3;
        t3[test_type_2{17}] = test_type_2{1};
        t3[test_type_2{11}] = test_type_2{4};
        t3[test_type_2{3}] = test_type_2{2};
        t3[test_type_2{29}] = test_type_2{3};
        auto r3 = t3.erase(--t3.end());
        if (t3.empty() || t3.size() != 3 || r3 != t3.end())
        {
            ++fail_count;
            std::cout << "FAILED map::erase single iterator end\n";
        }
        map <test_type_2, test_type_2> t4;
        t4[test_type_2{17}] = test_type_2{1};
        t4[test_type_2{11}] = test_type_2{4};
        t4[test_type_2{3}] = test_type_2{2};
        t4[test_type_2{29}] = test_type_2{3};
        auto r4 = t4.erase(t4.begin(), ++++t4.begin());
        if (t4.empty() || t4.size() != 2 || *(*r4).first.val != 17 ||
            *(*r4).second.val != 1)
        {
            ++fail_count;
            std::cout << "FAILED map::erase iterator range beginning\n";
        }
        map <test_type_2, test_type_2> t5;
        t5[test_type_2{17}] = test_type_2{1};
        t5[test_type_2{11}] = test_type_2{4};
        t5[test_type_2{3}] = test_type_2{2};
        t5[test_type_2{29}] = test_type_2{3};
        auto r5 = t5.erase(++t5.begin(), --t5.end());
        if (t5.empty() || t5.size() != 2 || *(*r5).first.val != 29 ||
            *(*r5).second.val != 3)
        {
            ++fail_count;
            std::cout << "FAILED map::erase iterator range middle\n";
        }
        map <test_type_2, test_type_2> t6;
        t6[test_type_2{17}] = test_type_2{1};
        t6[test_type_2{11}] = test_type_2{4};
        t6[test_type_2{3}] = test_type_2{2};
        t6[test_type_2{29}] = test_type_2{3};
        auto r6 = t6.erase(----t6.end(), t6.end());
        if (t6.empty() || t6.size() != 2 || r6 != t6.end())
        {
            ++fail_count;
            std::cout << "FAILED map::erase iterator range end\n";
        }
        map <test_type_2, test_type_2> t7;
        t7[test_type_2{17}] = test_type_2{1};
        t7[test_type_2{11}] = test_type_2{4};
        t7[test_type_2{3}] = test_type_2{2};
        t7[test_type_2{29}] = test_type_2{3};
        auto r7 = t7.erase(t7.begin(), t7.end());
        if (!t7.empty() || t7.size() != 0 || r7 != t7.begin() || r7 != t7.end())
        {
            ++fail_count;
            std::cout << "FAILED map::erase iterator range all\n";
        }
        map <test_type_2, test_type_2> t8;
        t8[test_type_2{17}] = test_type_2{1};
        t8[test_type_2{11}] = test_type_2{4};
        t8[test_type_2{3}] = test_type_2{2};
        t8[test_type_2{29}] = test_type_2{3};
        size_t i1 = t8.erase(test_type_2{3});
        if (t8.empty() || t8.size() != 3 || i1 != 1 ||
            *(*t8.begin()).first.val != 11 || *(*t8.begin()).second.val != 4)
        {
            ++fail_count;
            std::cout << "FAILED map::erase key beginning\n";
        }
        map <test_type_2, test_type_2> t9;
        t9[test_type_2{17}] = test_type_2{1};
        t9[test_type_2{11}] = test_type_2{4};
        t9[test_type_2{3}] = test_type_2{2};
        t9[test_type_2{29}] = test_type_2{3};
        size_t i2 = t9.erase(test_type_2{11});
        if (t9.empty() || t9.size() != 3 || i2 != 1 ||
            *(*++t9.begin()).first.val != 17 ||
            *(*++t9.begin()).second.val != 1)
        {
            ++fail_count;
            std::cout << "FAILED map::erase key middle\n";
        }
        map <test_type_2, test_type_2> t10;
        t10[test_type_2{17}] = test_type_2{1};
        t10[test_type_2{11}] = test_type_2{4};
        t10[test_type_2{3}] = test_type_2{2};
        t10[test_type_2{29}] = test_type_2{3};
        size_t i3 = t10.erase(test_type_2{29});
        if (t10.empty() || t10.size() != 3 || i3 != 1 ||
            *(*--t10.end()).first.val != 17 || *(*--t10.end()).second.val != 1)
        {
            ++fail_count;
            std::cout << "FAILED map::erase key end\n";
        }
    }
    // swap
    {
        map<int, char> t1;
        map<int, char> t2;
        t2[3] = 'a'; t2[4] = 'b'; t2[5] = 'c';
        t1.swap(t2);
        if (t1.empty() || t1.size() != 3 ||
            (*t1.begin()).first != 3 || (*t1.begin()).second != 'a' ||
            (*--t1.end()).first != 5 || (*--t1.end()).second != 'c' ||
            !t2.empty() || t2.size() != 0 || t2.begin() != t2.end())
        {
            ++fail_count;
            std::cout << "FAILED map::swap int, char one empty\n";
        }
        map<int, char> t3;
        t3[6] = 'a'; t3[7] = 'b'; t3[8] = 'c'; t3[9] = 'd';
        map<int, char> t4;
        t4[3] = 'e'; t4[4] = 'f'; t4[5] = 'g';
        t3.swap(t4);
        if (t3.empty() || t3.size() != 3 ||
            (*t3.begin()).first != 3 || (*t3.begin()).second != 'e' ||
            (*--t3.end()).first != 5 || (*--t3.end()).second != 'g' ||
            t4.empty() || t4.size() != 4 ||
            (*t4.begin()).first != 6 || (*t4.begin()).second != 'a' ||
            (*--t4.end()).first != 9 || (*--t4.end()).second != 'd')
        {
            ++fail_count;
            std::cout << "FAILED map::swap int, char none empty\n";
        }
        map<int, char, greater<int>> t5;
        t5[6] = 'a'; t5[7] = 'b'; t5[8] = 'c'; t5[9] = 'd';
        map<int, char, greater<int>> t6;
        t6[3] = 'e'; t6[4] = 'f'; t6[5] = 'g';
        t5.swap(t6);
        if (t5.empty() || t5.size() != 3 || t5.check() == 0 ||
            (*t5.begin()).first != 5 || (*t5.begin()).second != 'g' ||
            (*--t5.end()).first != 3 || (*--t5.end()).second != 'e' ||
            t5.value_comp()(2, 4) ||
            t6.empty() || t6.size() != 4 || t6.check() == 0 ||
            (*t6.begin()).first != 9 || (*t6.begin()).second != 'd' ||
            (*--t6.end()).first != 6 || (*--t6.end()).second != 'a' ||
            t6.value_comp()(2, 4))
        {
            ++fail_count;
            std::cout << "FAILED map::swap int, char none empty non-default comparator\n";
        }
        map<test_type_2, test_type_2> t7;
        t7[test_type_2{6}] = test_type_2{11};
        t7[test_type_2{7}] = test_type_2{12};
        t7[test_type_2{8}] = test_type_2{13};
        t7[test_type_2{9}] = test_type_2{14};
        map<test_type_2, test_type_2> t8;
        t8[test_type_2{3}] = test_type_2{15};
        t8[test_type_2{4}] = test_type_2{16};
        t8[test_type_2{5}] = test_type_2{17};
        t8.swap(t7);
        if (t7.empty() || t7.size() != 3 ||
            *(*t7.begin()).first.val != 3 || *(*t7.begin()).second.val != 15 ||
            *(*--t7.end()).first.val != 5 || *(*--t7.end()).second.val != 17 ||
            t8.empty() || t8.size() != 4 ||
            *(*t8.begin()).first.val != 6 || *(*t8.begin()).second.val != 11 ||
            *(*--t8.end()).first.val != 9 || *(*--t8.end()).second.val != 14)
        {
            ++fail_count;
            std::cout << "FAILED map::swap heap type none empty\n";
        }
    }
    // swap
    {
        map<int, char> t1;
        map<int, char> t2;
        t2[3] = 'a'; t2[4] = 'b'; t2[5] = 'c';
        swap(t1, t2);
        if (t1.empty() || t1.size() != 3 ||
            (*t1.begin()).first != 3 || (*t1.begin()).second != 'a' ||
            (*--t1.end()).first != 5 || (*--t1.end()).second != 'c' ||
            !t2.empty() || t2.size() != 0 || t2.begin() != t2.end())
        {
            ++fail_count;
            std::cout << "FAILED map::swap int, char one empty\n";
        }
        map<int, char> t3;
        t3[6] = 'a'; t3[7] = 'b'; t3[8] = 'c'; t3[9] = 'd';
        map<int, char> t4;
        t4[3] = 'e'; t4[4] = 'f'; t4[5] = 'g';
        swap(t3, t4);
        if (t3.empty() || t3.size() != 3 ||
            (*t3.begin()).first != 3 || (*t3.begin()).second != 'e' ||
            (*--t3.end()).first != 5 || (*--t3.end()).second != 'g' ||
            t4.empty() || t4.size() != 4 ||
            (*t4.begin()).first != 6 || (*t4.begin()).second != 'a' ||
            (*--t4.end()).first != 9 || (*--t4.end()).second != 'd')
        {
            ++fail_count;
            std::cout << "FAILED map::swap int, char none empty\n";
        }
        map<int, char, greater<int>> t5;
        t5[6] = 'a'; t5[7] = 'b'; t5[8] = 'c'; t5[9] = 'd';
        map<int, char, greater<int>> t6;
        t6[3] = 'e'; t6[4] = 'f'; t6[5] = 'g';
        swap(t5, t6);
        if (t5.empty() || t5.size() != 3 || t5.check() == 0 ||
            (*t5.begin()).first != 5 || (*t5.begin()).second != 'g' ||
            (*--t5.end()).first != 3 || (*--t5.end()).second != 'e' ||
            t5.value_comp()(2, 4) ||
            t6.empty() || t6.size() != 4 || t6.check() == 0 ||
            (*t6.begin()).first != 9 || (*t6.begin()).second != 'd' ||
            (*--t6.end()).first != 6 || (*--t6.end()).second != 'a' ||
            t6.value_comp()(2, 4))
        {
            ++fail_count;
            std::cout << "FAILED map::swap int, char none empty non-default comparator\n";
        }
        map<test_type_2, test_type_2> t7;
        t7[test_type_2{6}] = test_type_2{11};
        t7[test_type_2{7}] = test_type_2{12};
        t7[test_type_2{8}] = test_type_2{13};
        t7[test_type_2{9}] = test_type_2{14};
        map<test_type_2, test_type_2> t8;
        t8[test_type_2{3}] = test_type_2{15};
        t8[test_type_2{4}] = test_type_2{16};
        t8[test_type_2{5}] = test_type_2{17};
        swap(t7, t8);
        if (t7.empty() || t7.size() != 3 ||
            *(*t7.begin()).first.val != 3 || *(*t7.begin()).second.val != 15 ||
            *(*--t7.end()).first.val != 5 || *(*--t7.end()).second.val != 17 ||
            t8.empty() || t8.size() != 4 ||
            *(*t8.begin()).first.val != 6 || *(*t8.begin()).second.val != 11 ||
            *(*--t8.end()).first.val != 9 || *(*--t8.end()).second.val != 14)
        {
            ++fail_count;
            std::cout << "FAILED map::swap heap type none empty\n";
        }
    }
    // count
    {
        map<int, char> t1;
        if (t1.count(1) != 0)
        {
            ++fail_count;
            std::cout << "FAILED map::count empty\n";
        }
        t1[15] = 'a'; t1[18] = 'b'; t1[13] = 'c'; t1[2] = 'd';
        t1[10] = 'e'; t1[12] = 'f'; t1[9] = 'g'; t1[1] = 'h';
        if (t1.count(3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED map::count not present\n";
        }
        if (t1.count(1) != 1)
        {
            ++fail_count;
            std::cout << "FAILED map::count singly present start\n";
        }
        if (t1.count(10) != 1)
        {
            ++fail_count;
            std::cout << "FAILED map::count singly present middle\n";
        }
        if (t1.count(18) != 1)
        {
            ++fail_count;
            std::cout << "FAILED map::count singly present end\n";
        }
        map<test_type, char, test_comparator> t2;
        if (t2.count(1) != 0)
        {
            ++fail_count;
            std::cout << "FAILED map::count transparent empty\n";
        }
        t2[test_type{15}] = 'a'; t2[test_type{18}] = 'b';
        t2[test_type{13}] = 'c'; t2[test_type{2}] = 'd';
        t2[test_type{10}] = 'e'; t2[test_type{12}] = 'f';
        t2[test_type{9}] = 'g'; t2[test_type{1}] = 'h';
        if (t2.count(3) != 0)
        {
            ++fail_count;
            std::cout << "FAILED map::count transparent not present\n";
        }
        if (t2.count(9) != 1)
        {
            ++fail_count;
            std::cout << "FAILED map::count transparent present\n";
        }
    }
    // find
    {
        map<int, char> t1;
        if (t1.find(1) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::find empty\n";
        }
        t1[15] = 'a'; t1[18] = 'b'; t1[13] = 'c'; t1[2] = 'd';
        t1[10] = 'e'; t1[12] = 'f'; t1[9] = 'g'; t1[1] = 'h';
        if (t1.find(3) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::find not present\n";
        }
        if ((*t1.find(1)).first != 1 || (*t1.find(1)).second != 'h')
        {
            ++fail_count;
            std::cout << "FAILED map::find singly present start\n";
        }
        if ((*t1.find(10)).first != 10 || (*t1.find(10)).second != 'e')
        {
            ++fail_count;
            std::cout << "FAILED map::find singly present middle\n";
        }
        if ((*t1.find(18)).first != 18 || (*t1.find(18)).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::find singly present end\n";
        }
        map<test_type, char, test_comparator> t2;
        if (t2.find(1) != t2.end())
        {
            ++fail_count;
            std::cout << "FAILED map::find transparent empty\n";
        }
        t2[test_type{15}] = 'a'; t2[test_type{18}] = 'b';
        t2[test_type{13}] = 'c'; t2[test_type{2}] = 'd';
        t2[test_type{10}] = 'e'; t2[test_type{12}] = 'f';
        t2[test_type{9}] = 'g'; t2[test_type{1}] = 'h';
        if (t2.find(3) != t2.end())
        {
            ++fail_count;
            std::cout << "FAILED map::find transparent not present\n";
        }
        if ((*t2.find(9)).first.val != 9 || (*t2.find(9)).second != 'g')
        {
            ++fail_count;
            std::cout << "FAILED map::find transparent present\n";
        }
    }
    // lower_bound
    {
        map<int, char> t1;
        if (t1.lower_bound(1) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::lower_bound empty\n";
        }
        t1[1] = 'a'; t1[3] = 'b'; t1[5] = 'c';
        if ((*t1.lower_bound(2)).first != 3 ||
            (*t1.lower_bound(2)).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::lower_bound non-existent middle\n";
        }
        if (t1.lower_bound(0) != t1.begin())
        {
            ++fail_count;
            std::cout << "FAILED map::lower_bound non-existent left\n";
        }
        if (t1.lower_bound(6) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::lower_bound non-existent right\n";
        }
        if (t1.lower_bound(1) != t1.begin())
        {
            ++fail_count;
            std::cout << "FAILED map::lower_bound exists left\n";
        }
        if ((*t1.lower_bound(3)).first != 3 ||
            (*t1.lower_bound(3)).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::lower_bound exists middle\n";
        }
        if ((*t1.lower_bound(5)).first != 5 ||
            (*t1.lower_bound(5)).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::lower_bound exists right\n";
        }
    }
    // upper_bound
    {
        map<int, char> t1;
        if (t1.upper_bound(1) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::upper_bound empty\n";
        }
        t1[1] = 'a'; t1[3] = 'b'; t1[5] = 'c';
        if (t1.upper_bound(0) != t1.begin())
        {
            ++fail_count;
            std::cout << "FAILED map::upper_bound non-existent left\n";
        }
        if ((*t1.upper_bound(2)).first != 3 ||
            (*t1.upper_bound(2)).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::upper_bound non-existent middle\n";
        }
        if (t1.upper_bound(6) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::upper_bound non-existent right\n";
        }
        if ((*t1.upper_bound(1)).first != 3 ||
            (*t1.upper_bound(1)).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::upper_bound exists left\n";
        }
        if ((*t1.upper_bound(3)).first != 5 ||
            (*t1.upper_bound(3)).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::upper_bound exists middle\n";
        }
        if (t1.upper_bound(5) != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::upper_bound exists right\n";
        }
    }
    // equal_range
    {
        map<int, char> t1;
        if (t1.equal_range(1).first != t1.end() ||
            t1.equal_range(1).second != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::equal_range empty\n";
        }
        t1[1] = 'a'; t1[3] = 'b'; t1[5] = 'c';
        if (t1.equal_range(0).first != t1.begin() ||
            t1.equal_range(0).first != t1.equal_range(0).second)
        {
            ++fail_count;
            std::cout << "FAILED map::equal_range non-existent left\n";
        }
        if ((*t1.equal_range(2).first).first != 3 ||
            (*t1.equal_range(2).first).second != 'b' ||
            t1.equal_range(2).first != t1.equal_range(2).second)
        {
            ++fail_count;
            std::cout << "FAILED map::equal_range non-existent middle\n";
        }
        if (t1.equal_range(6).first != t1.end() ||
            t1.equal_range(6).first != t1.equal_range(6).second)
        {
            ++fail_count;
            std::cout << "FAILED map::equal_range non-existent right\n";
        }
        if ((*t1.equal_range(1).first).first != 1 ||
            (*t1.equal_range(1).first).second != 'a' ||
            (*t1.equal_range(1).second).first != 3 ||
            (*t1.equal_range(1).second).second != 'b')
        {
            ++fail_count;
            std::cout << "FAILED map::equal_range exists left\n";
        }
        if ((*t1.equal_range(3).first).first != 3 ||
            (*t1.equal_range(3).first).second != 'b' ||
            (*t1.equal_range(3).second).first != 5 ||
            (*t1.equal_range(3).second).second != 'c')
        {
            ++fail_count;
            std::cout << "FAILED map::equal_range exists middle\n";
        }
        if ((*t1.equal_range(5).first).first != 5 ||
            (*t1.equal_range(5).first).second != 'c' ||
            t1.equal_range(5).second != t1.end())
        {
            ++fail_count;
            std::cout << "FAILED map::equal_range exists right\n";
        }
    }
    // key_comp
    {
        map<int, char> t1;
        if (!is_same<decltype(t1.key_comp()), less<int>>::value ||
            t1.key_comp()(4, 2))
        {
            ++fail_count;
            std::cout << "FAILED map::key_comp less<int>\n";
        }
        map<int, char, greater<int>> t2;
        if (!is_same<decltype(t2.key_comp()), greater<int>>::value ||
            t2.key_comp()(2, 4))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::key_comp greater<int>\n";
        }
        map<test_type, char, test_comparator> t3;
        if (!is_same<decltype(t3.key_comp()), test_comparator>::value ||
            t3.key_comp()(test_type{4}, test_type{2}))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::key_comp test_comparator\n";
        }
    }
    // value_comp
    {
        map<int, char> t1;
        if (!is_same<decltype(t1.value_comp()),
            map<int, char, less<int>>::value_compare>::value ||
            t1.value_comp()(pair<int, char>{4, 'a'}, pair<int, char>{2, 'b'}))
        {
            ++fail_count;
            std::cout << "FAILED map::value_comp less<int>\n";
        }
        map<int, char, greater<int>> t2;
        if (!is_same<decltype(t2.value_comp()),
            map<int, char, greater<int>>::value_compare>::value ||
            t2.value_comp()(pair<int, char>{2, 'b'}, pair<int, char>{4, 'a'}))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::value_comp greater<int>\n";
        }
        map<test_type, char, test_comparator> t3;
        if (!is_same<decltype(t3.value_comp()),
            map<test_type, char, test_comparator>::value_compare>::value ||
            t3.value_comp()(pair<test_type, char>{test_type{4}, 'a'},
            pair<test_type, char>{test_type{2}, 'b'}))
        {
            ++fail_count;
            std::cout << "FAILED rb_tree::value_comp test_comparator\n";
        }
    }
    // operator==
    {
        map<int, char> t1;
        map<int, char> t2;
        t2[1] = 'a'; t2[2] = 'b'; t2[3] = 'c';
        map<int, char> t3;
        t3[1] = 'a'; t3[2] = 'b'; t3[3] = 'c';
        map<int, char> t4;
        t4[1] = 'a'; t4[2] = 'b';
        map<int, char> t5;
        if (t1 == t2)
        {
            ++fail_count;
            std::cout << "FAILED map::operator== int one empty\n";
        }
        if (!(t1 == t5))
        {
            ++fail_count;
            std::cout << "FAILED map::operator== int both empty\n";
        }
        if (!(t2 == t3))
        {
            ++fail_count;
            std::cout << "FAILED map::operator== int both full equal\n";
        }
        if (t2 == t4)
        {
            ++fail_count;
            std::cout << "FAILED map::operator== int different sizes\n";
        }
        map<test_type, char, test_comparator> t6;
        map<test_type, char, test_comparator> t7;
        t7[test_type{1}] = 'a'; t7[test_type{2}] = 'b'; t7[test_type{3}] = 'c';
        map<test_type, char, test_comparator> t8;
        t8[test_type{1}] = 'a'; t8[test_type{2}] = 'b'; t8[test_type{3}] = 'c';
        map<test_type, char, test_comparator> t9;
        t9[test_type{1}] = 'a'; t9[test_type{2}] = 'b';
        map<test_type, char, test_comparator> t10;
        if (t6 == t7)
        {
            ++fail_count;
            std::cout << "FAILED map::operator== custom type one empty\n";
        }
        if (!(t6 == t10))
        {
            ++fail_count;
            std::cout << "FAILED map::operator== custom type both empty\n";
        }
        if (!(t7 == t8))
        {
            ++fail_count;
            std::cout << "FAILED map::operator== custom type both full equal\n";
        }
        if (t7 == t9)
        {
            ++fail_count;
            std::cout << "FAILED map::operator== custom type different sizes\n";
        }
    }
    // operator!=
    {
        map<int, char> t1;
        map<int, char> t2;
        t2[1] = 'a'; t2[2] = 'b'; t2[3] = 'c';
        map<int, char> t3;
        t3[1] = 'a'; t3[2] = 'b'; t3[3] = 'c';
        map<int, char> t4;
        t4[1] = 'a'; t4[2] = 'b';
        map<int, char> t5;
        if (!(t1 != t2))
        {
            ++fail_count;
            std::cout << "FAILED map::operator!= int one empty\n";
        }
        if (t1 != t5)
        {
            ++fail_count;
            std::cout << "FAILED map::operator!= int both empty\n";
        }
        if (t2 != t3)
        {
            ++fail_count;
            std::cout << "FAILED map::operator!= int both full equal\n";
        }
        if (!(t2 != t4))
        {
            ++fail_count;
            std::cout << "FAILED map::operator!= int different sizes\n";
        }
        map<test_type, char, test_comparator> t6;
        map<test_type, char, test_comparator> t7;
        t7[test_type{1}] = 'a'; t7[test_type{2}] = 'b'; t7[test_type{3}] = 'c';
        map<test_type, char, test_comparator> t8;
        t8[test_type{1}] = 'a'; t8[test_type{2}] = 'b'; t8[test_type{3}] = 'c';
        map<test_type, char, test_comparator> t9;
        t9[test_type{1}] = 'a'; t9[test_type{2}] = 'b';
        map<test_type, char, test_comparator> t10;
        if (!(t6 != t7))
        {
            ++fail_count;
            std::cout << "FAILED map::operator!= custom type one empty\n";
        }
        if (t6 != t10)
        {
            ++fail_count;
            std::cout << "FAILED map::operator!= custom type both empty\n";
        }
        if (t7 != t8)
        {
            ++fail_count;
            std::cout << "FAILED map::operator!= custom type both full equal\n";
        }
        if (!(t7 != t9))
        {
            ++fail_count;
            std::cout << "FAILED map::operator!= custom type different sizes\n";
        }
    }
    // operator<
    {
        map<int, char> t1;
        t1[1] = 'a'; t1[2] = 'b'; t1[3] = 'c';
        map<int, char> t2;
        t2[1] = 'a'; t2[2] = 'b'; t2[3] = 'c';
        map<int, char> t3;
        t3[1] = 'a'; t3[2] = 'b';
        map<int, char> t4;
        map<int, char> t5;
        t5[2] = 'a'; t5[3] = 'b'; t5[4] = 'c';
        if (t1 < t2)
        {
            ++fail_count;
            std::cout << "FAILED map::operator< int equal\n";
        }
        if (t1 < t3)
        {
            ++fail_count;
            std::cout << "FAILED map::operator< int second prequel\n";
        }
        if (!(t3 < t1))
        {
            ++fail_count;
            std::cout << "FAILED map::operator< int first prequel\n";
        }
        if (t1 < t4)
        {
            ++fail_count;
            std::cout << "FAILED map::operator< int second empty\n";
        }
        if (!(t4 < t1))
        {
            ++fail_count;
            std::cout << "FAILED map::operator< int first empty\n";
        }
        if (t4 < t4)
        {
            ++fail_count;
            std::cout << "FAILED map::operator< int both empty\n";
        }
        if (!(t1 < t5))
        {
            ++fail_count;
            std::cout << "FAILED map::operator< int first element-wise less\n";
        }
        if (t5 < t1)
        {
            ++fail_count;
            std::cout << "FAILED map::operator< int second element-wise less\n";
        }
        map<test_type, char, test_comparator> t6;
        t6[test_type{1}] = 'a'; t6[test_type{2}] = 'b'; t6[test_type{3}] = 'c';
        map<test_type, char, test_comparator> t7;
        t7[test_type{1}] = 'a'; t7[test_type{2}] = 'b'; t7[test_type{3}] = 'c';
        map<test_type, char, test_comparator> t8;
        t8[test_type{1}] = 'a'; t8[test_type{2}] = 'b';
        map<test_type, char, test_comparator> t9;
        map<test_type, char, test_comparator> t10;
        t10[test_type{2}] = 'a'; t10[test_type{3}] = 'b';
        t10[test_type{4}] = 'c';
        if (t6 < t7)
        {
            ++fail_count;
            std::cout << "FAILED map::operator< custom type equal\n";
        }
        if (t6 < t8)
        {
            ++fail_count;
            std::cout << "FAILED map::operator< custom type second prequel\n";
        }
        if (!(t8 < t6))
        {
            ++fail_count;
            std::cout << "FAILED map::operator< custom type first prequel\n";
        }
        if (t6 < t9)
        {
            ++fail_count;
            std::cout << "FAILED map::operator< custom type second empty\n";
        }
        if (!(t9 < t6))
        {
            ++fail_count;
            std::cout << "FAILED map::operator< custom type first empty\n";
        }
        if (t9 < t9)
        {
            ++fail_count;
            std::cout << "FAILED map::operator< custom type both empty\n";
        }
        if (!(t6 < t10))
        {
            ++fail_count;
            std::cout << "FAILED map::operator< custom type first element-wise less\n";
        }
        if (t10 < t6)
        {
            ++fail_count;
            std::cout << "FAILED map::operator< custom type second element-wise less\n";
        }
    }
    // operator<=
    {
        map<int, char> t1;
        t1[1] = 'a'; t1[2] = 'b'; t1[3] = 'c';
        map<int, char> t2;
        t2[1] = 'a'; t2[2] = 'b'; t2[3] = 'c';
        map<int, char> t3;
        t3[1] = 'a'; t3[2] = 'b';
        map<int, char> t4;
        map<int, char> t5;
        t5[2] = 'a'; t5[3] = 'b'; t5[4] = 'c';
        if (!(t1 <= t2))
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= int equal\n";
        }
        if (t1 <= t3)
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= int second prequel\n";
        }
        if (!(t3 <= t1))
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= int first prequel\n";
        }
        if (t1 <= t4)
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= int second empty\n";
        }
        if (!(t4 <= t1))
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= int first empty\n";
        }
        if (!(t4 <= t4))
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= int both empty\n";
        }
        if (!(t1 <= t5))
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= int first element-wise less\n";
        }
        if (t5 <= t1)
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= int second element-wise less\n";
        }
        map<test_type, char, test_comparator> t6;
        t6[test_type{1}] = 'a'; t6[test_type{2}] = 'b'; t6[test_type{3}] = 'c';
        map<test_type, char, test_comparator> t7;
        t7[test_type{1}] = 'a'; t7[test_type{2}] = 'b'; t7[test_type{3}] = 'c';
        map<test_type, char, test_comparator> t8;
        t8[test_type{1}] = 'a'; t8[test_type{2}] = 'b';
        map<test_type, char, test_comparator> t9;
        map<test_type, char, test_comparator> t10;
        t10[test_type{2}] = 'a'; t10[test_type{3}] = 'b';
        t10[test_type{4}] = 'c';
        if (!(t6 <= t7))
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= custom type equal\n";
        }
        if (t6 <= t8)
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= custom type second prequel\n";
        }
        if (!(t8 <= t6))
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= custom type first prequel\n";
        }
        if (t6 <= t9)
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= custom type second empty\n";
        }
        if (!(t9 <= t6))
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= custom type first empty\n";
        }
        if (!(t9 <= t9))
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= custom type both empty\n";
        }
        if (!(t6 <= t10))
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= custom type first element-wise less\n";
        }
        if (t10 <= t6)
        {
            ++fail_count;
            std::cout << "FAILED map::operator<= custom type second element-wise less\n";
        }
    }
    // operator>
    {
        map<int, char> t1;
        t1[1] = 'a'; t1[2] = 'b'; t1[3] = 'c';
        map<int, char> t2;
        t2[1] = 'a'; t2[2] = 'b'; t2[3] = 'c';
        map<int, char> t3;
        t3[1] = 'a'; t3[2] = 'b';
        map<int, char> t4;
        map<int, char> t5;
        t5[2] = 'a'; t5[3] = 'b'; t5[4] = 'c';
        if (t1 > t2)
        {
            ++fail_count;
            std::cout << "FAILED map::operator> int equal\n";
        }
        if (!(t1 > t3))
        {
            ++fail_count;
            std::cout << "FAILED map::operator> int second prequel\n";
        }
        if (t3 > t1)
        {
            ++fail_count;
            std::cout << "FAILED map::operator> int first prequel\n";
        }
        if (!(t1 > t4))
        {
            ++fail_count;
            std::cout << "FAILED map::operator> int second empty\n";
        }
        if (t4 > t1)
        {
            ++fail_count;
            std::cout << "FAILED map::operator> int first empty\n";
        }
        if (t4 > t4)
        {
            ++fail_count;
            std::cout << "FAILED map::operator> int both empty\n";
        }
        if (t1 > t5)
        {
            ++fail_count;
            std::cout << "FAILED map::operator> int first element-wise less\n";
        }
        if (!(t5 > t1))
        {
            ++fail_count;
            std::cout << "FAILED map::operator> int second element-wise less\n";
        }
        map<test_type, char, test_comparator> t6;
        t6[test_type{1}] = 'a'; t6[test_type{2}] = 'b'; t6[test_type{3}] = 'c';
        map<test_type, char, test_comparator> t7;
        t7[test_type{1}] = 'a'; t7[test_type{2}] = 'b'; t7[test_type{3}] = 'c';
        map<test_type, char, test_comparator> t8;
        t8[test_type{1}] = 'a'; t8[test_type{2}] = 'b';
        map<test_type, char, test_comparator> t9;
        map<test_type, char, test_comparator> t10;
        t10[test_type{2}] = 'a'; t10[test_type{3}] = 'b';
        t10[test_type{4}] = 'c';
        if (t6 > t7)
        {
            ++fail_count;
            std::cout << "FAILED map::operator> custom type equal\n";
        }
        if (!(t6 > t8))
        {
            ++fail_count;
            std::cout << "FAILED map::operator> custom type second prequel\n";
        }
        if (t8 > t6)
        {
            ++fail_count;
            std::cout << "FAILED map::operator> custom type first prequel\n";
        }
        if (!(t6 > t9))
        {
            ++fail_count;
            std::cout << "FAILED map::operator> custom type second empty\n";
        }
        if (t9 > t6)
        {
            ++fail_count;
            std::cout << "FAILED map::operator> custom type first empty\n";
        }
        if (t9 > t9)
        {
            ++fail_count;
            std::cout << "FAILED map::operator> custom type both empty\n";
        }
        if (t6 > t10)
        {
            ++fail_count;
            std::cout << "FAILED map::operator> custom type first element-wise less\n";
        }
        if (!(t10 > t6))
        {
            ++fail_count;
            std::cout << "FAILED map::operator> custom type second element-wise less\n";
        }
    }
    // operator>=
    {
        map<int, char> t1;
        t1[1] = 'a'; t1[2] = 'b'; t1[3] = 'c';
        map<int, char> t2;
        t2[1] = 'a'; t2[2] = 'b'; t2[3] = 'c';
        map<int, char> t3;
        t3[1] = 'a'; t3[2] = 'b';
        map<int, char> t4;
        map<int, char> t5;
        t5[2] = 'a'; t5[3] = 'b'; t5[4] = 'c';
        if (!(t1 >= t2))
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= int equal\n";
        }
        if (!(t1 >= t3))
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= int second prequel\n";
        }
        if (t3 >= t1)
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= int first prequel\n";
        }
        if (!(t1 >= t4))
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= int second empty\n";
        }
        if (t4 >= t1)
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= int first empty\n";
        }
        if (!(t4 >= t4))
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= int both empty\n";
        }
        if (t1 >= t5)
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= int first element-wise less\n";
        }
        if (!(t5 >= t1))
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= int second element-wise less\n";
        }
        map<test_type, char, test_comparator> t6;
        t6[test_type{1}] = 'a'; t6[test_type{2}] = 'b'; t6[test_type{3}] = 'c';
        map<test_type, char, test_comparator> t7;
        t7[test_type{1}] = 'a'; t7[test_type{2}] = 'b'; t7[test_type{3}] = 'c';
        map<test_type, char, test_comparator> t8;
        t8[test_type{1}] = 'a'; t8[test_type{2}] = 'b';
        map<test_type, char, test_comparator> t9;
        map<test_type, char, test_comparator> t10;
        t10[test_type{2}] = 'a'; t10[test_type{3}] = 'b';
        t10[test_type{4}] = 'c';
        if (!(t6 >= t7))
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= custom type equal\n";
        }
        if (!(t6 >= t8))
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= custom type second prequel\n";
        }
        if (t8 >= t6)
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= custom type first prequel\n";
        }
        if (!(t6 >= t9))
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= custom type second empty\n";
        }
        if (t9 >= t6)
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= custom type first empty\n";
        }
        if (!(t9 >= t9))
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= custom type both empty\n";
        }
        if (t6 >= t10)
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= custom type first element-wise less\n";
        }
        if (!(t10 >= t6))
        {
            ++fail_count;
            std::cout << "FAILED map::operator>= custom type second element-wise less\n";
        }
    }
    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
