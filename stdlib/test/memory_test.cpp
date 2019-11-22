#include <iostream>
#include <typeinfo>

#include "../include/limits"
#include "../include/memory"
#include "../include/utility"

using namespace klib;

// Types for testing
struct up_move_type_1 { int a; };
struct up_move_type_2 : up_move_type_1 {
    int b;
    up_move_type_2(int ca, int cb) : up_move_type_1{ca}, b{cb} {};
};

struct up_dereference_type { int a; };

int main()
{
    size_t fail_count = 0;

    // unique_ptr<T>
    { // indent block so test variable names can be reused
        // pointer member type
        if (!is_same<typename unique_ptr<int>::pointer, int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>::pointer\n";
        }
        // element_type member type
        if (!is_same<typename unique_ptr<int>::element_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>::element_type\n";
        }
        // deleter_type member type
        if (!is_same<typename unique_ptr<int>::deleter_type,
            default_delete<int>>::value)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>::element_type\n";
        }
        // default constructor
        unique_ptr<int> i1;
        int* i2{};
        if (i1 || i1.get() != i2)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>()\n";
        }
        // nullptr_t constructor
        nullptr_t n1;
        int* i3 {};
        unique_ptr<int> i4 {n1};
        if (i4 || i4.get() != i3)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>(nullptr_t)\n";
        }
        // Standard pointer constructor
        int* i5 = new int{5};
        unique_ptr<int> i6 {i5};
        if (i6.get() != i5 || *i6 != 5)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>(pointer)\n";
        }
        // pointer and deleter constructor
        default_delete<int> d1;
        int* i7 = new int{10};
        unique_ptr<int> i8 {i7, d1};
        if (i8.get() != i7 || *i8 != 10)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>(pointer, deleter_type)\n";
        }
        // move constructor from same type
        unique_ptr<int> i9 {new int{15}};
        unique_ptr<int> i10 {move(i9)};
        if (*i10 != 15)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>(unique_ptr&&)\n";
        }
        // move constructor from different type
        default_delete<up_move_type_1> d2;
        unique_ptr<up_move_type_2, default_delete<up_move_type_1>> i11
            {new up_move_type_2{5, 10}, d2};
        unique_ptr<up_move_type_1> i12 {move(i11)};
        if (i12->a != 5)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T><up_move_type_1>(unique_ptr<up_move_type_2>)\n";
        }
        // Reset to blank by nullptr assignment.
        int* i13 {};
        nullptr_t n2;
        unique_ptr<int> i14 {new int{5}};
        i14 = n2;
        if (i14 || i14.get() != i13)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T> = nullptr_t\n";
        }
        // release
        int* i15 {};
        unique_ptr<int> i16 {new int{10}};
        int* i17 = i16.release();
        if (i16 || i16.get() != i15 || *i17 != 10)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>::release\n";
        }
        delete i17;
        // reset to blank
        int* i18 {};
        unique_ptr<int> i19 {new int{15}};
        i19.reset();
        if (i19 || i19.get() != i18)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>::reset()\n";
        }
        // reset to another value
        unique_ptr<int> i20 {new int{5}};
        i20.reset(new int{10});
        if (*i20 != 10)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>::reset(int*)\n";
        }
        // swap
        unique_ptr<int> i21 {new int{5}};
        unique_ptr<int> i22 {new int{10}};
        i21.swap(i22);
        if (*i21 != 10 || *i22 != 5)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>::swap\n";
        }
        // get
        int* i23 = new int{5};
        unique_ptr<int> i24 {i23};
        if (i24.get() != i23)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>::get\n";
        }
        // get_deleter
        unique_ptr<int> i25 {new int{5}};
        default_delete<int> d3 = i25.get_deleter();
        (void)d3;
        // operator bool
        unique_ptr<int> i26;
        unique_ptr<int> i27 {new int{5}};
        if (i26 || !i27)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>::operator bool\n";
        }
        // dereference
        unique_ptr<up_dereference_type> i28 {new up_dereference_type{5}};
        if (i28->a != 5)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>::operator->\n";
        }
        if ((*i28).a != 5)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T>::operator*\n";
        }
    } // end unique_ptr<T> block

    // unique_ptr<T[]>
    { // indent block so test variable names can be reused
        // pointer member type
        if (!is_same<typename unique_ptr<int[]>::pointer, int*>::value)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>::pointer\n";
        }
        // element_type member type
        if (!is_same<typename unique_ptr<int[]>::element_type, int>::value)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>::element_type\n";
        }
        // deleter_type member type
        if (!is_same<typename unique_ptr<int[]>::deleter_type,
            default_delete<int[]>>::value)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>::element_type\n";
        }
        // default constructor
        unique_ptr<int[]> i1;
        int* i2{};
        if (i1 || i1.get() != i2)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>()\n";
        }
        // nullptr_t constructor
        nullptr_t n1;
        int* i3 {};
        unique_ptr<int[]> i4 {n1};
        if (i4 || i4.get() != i3)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>(nullptr_t)\n";
        }
        // Standard pointer constructor
        int* i5 = new int[3]{1, 2, 3};
        unique_ptr<int[]> i6 {i5};
        if (i6.get() != i5 || i6[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>(pointer)\n";
        }
        // pointer and deleter constructor
        default_delete<int[]> d1;
        int* i7 = new int[3]{4, 5, 6};
        unique_ptr<int[]> i8 (i7, d1);
        if (i8.get() != i7 || i8[2] != 6)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>(pointer, deleter_type)\n";
        }
        // move constructor from same type
        unique_ptr<int[]> i9 {new int[3]{1, 2, 3}};
        unique_ptr<int[]> i10 {move(i9)};
        if (i10[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>(unique_ptr&&)\n";
        }
        // Reset to blank by nullptr assignment.
        int* i13 {};
        nullptr_t n2;
        unique_ptr<int[]> i14 {new int[3]{1, 2, 3}};
        i14 = n2;
        if (i14 || i14.get() != i13)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]> = nullptr_t\n";
        }
        // release
        int* i15 {};
        unique_ptr<int[]> i16 {new int[3]{1, 2, 3}};
        int* i17 = i16.release();
        if (i16 || i16.get() != i15 || i17[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>::release\n";
        }
        delete[] i17;
        // reset to blank
        int* i18 {};
        unique_ptr<int[]> i19 {new int[3]{4, 5, 6}};
        i19.reset();
        if (i19 || i19.get() != i18)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>::reset()\n";
        }
        // reset to another value
        unique_ptr<int[]> i20 {new int[3]{1, 2, 3}};
        i20.reset(new int[3]{4, 5, 6});
        if (i20[2] != 6)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>::reset(int*)\n";
        }
        // swap
        unique_ptr<int[]> i21 {new int[3]{1, 2, 3}};
        unique_ptr<int[]> i22 {new int[3]{4, 5, 6}};
        i21.swap(i22);
        if (i21[2] != 6 || i22[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>::swap\n";
        }
        // get
        int* i23 = new int[3]{1, 2, 3};
        unique_ptr<int[]> i24 {i23};
        if (i24.get() != i23)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>::get\n";
        }
        // get_deleter
        unique_ptr<int[]> i25 {new int[3]{1, 2, 3}};
        default_delete<int[]> d3 = i25.get_deleter();
        (void)d3;
        // operator bool
        unique_ptr<int[]> i26;
        unique_ptr<int[]> i27 {new int[3]{1, 2, 3}};
        if (i26 || !i27)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>::operator bool\n";
        }
        // operator[]
        unique_ptr<int[]> i28 {new int[3]{1, 2, 3}};
        if (i28[0] != 1 || i28[1] != 2 || i28[2] != 3)
        {
            ++fail_count;
            std::cout << "FAILED unique_ptr<T[]>::operator[]\n";
        }
    } // end unique_ptr<T> block

    // unique_ptr helper functions
    { // Use a seperate block to allow reusing test variable names
        // operator=
        int* i1 = new int{5};
        int* i2 = new int{10};
        unique_ptr<int> i3 {i1};
        unique_ptr<int> i4 {i1};
        unique_ptr<int> i5 {i2};
        if (!(i3 == i4) || i3 == i5)
        {
            ++fail_count;
            std::cout << "FAILED operator=(unique_ptr, unique_ptr)\n";
        }
        i3.release(); // necessary to avoid deleting i1 twice
        // operator= nullptr_t
        nullptr_t n1;
        unique_ptr<int> i6 {new int{5}};
        unique_ptr<int> i7;
        if (i6 == n1 || !(i7 == n1))
        {
            ++fail_count;
            std::cout << "FAILED operator=(unique_ptr, nullptr_t)\n";
        }
        if (n1 == i6 || !(n1 == i7))
        {
            ++fail_count;
            std::cout << "FAILED operator=(nullptr_t, unique_ptr)\n";
        }
    } // end unique_ptr operator= block
    { // Use a seperate block to allow reusing test variable names
        // operator!=
        int* i1 = new int{5};
        int* i2 = new int{10};
        unique_ptr<int> i3 {i1};
        unique_ptr<int> i4 {i1};
        unique_ptr<int> i5 {i2};
        if (i3 != i4 || !(i3 != i5))
        {
            ++fail_count;
            std::cout << "FAILED operator!=(unique_ptr, unique_ptr)\n";
        }
        i3.release(); // necessary to avoid deleting i1 twice
        // operator!= with nullptr_t
        nullptr_t n1;
        unique_ptr<int> i6 {new int{5}};
        unique_ptr<int> i7;
        if (!(i6 != n1) || i7 != n1)
        {
            ++fail_count;
            std::cout << "FAILED operator!=(unique_ptr, nullptr_t)\n";
        }
        if (!(n1 != i6) || n1 != i7)
        {
            ++fail_count;
            std::cout << "FAILED operator!=(nullptr_t, unique_ptr)\n";
        }
    } // end unique_ptr operator!= block
    { // Use a seperate block to allow reusing test variable names
        // operator<
        int* i1 = new int[2]{1, 2};
        unique_ptr<int> i2 {&(i1[0])};
        unique_ptr<int> i3 {&(i1[0])};
        unique_ptr<int> i4 {&(i1[1])};
        if (i2 < i3 || !(i2 < i4) || i4 < i2)
        {
            ++fail_count;
            std::cout << "FAILED operator<(unique_ptr, unique_ptr)\n";
        }
        i2.release(); // necessary to avoid horrible things
        i3.release();
        i4.release();
        delete[] i1;
        // operator< with nullptr_t
        nullptr_t n1;
        int* i5 {};
        unique_ptr<int> i6 {i5};
        unique_ptr<int> i7 {new int{1}};
        if (i6 < n1 || i7 < n1)
        {
            ++fail_count;
            std::cout << "FAILED operator<(unique_ptr, nullptr_t)\n";
        }
        if (n1 < i6 || !(n1 < i7))
        {
            ++fail_count;
            std::cout << "FAILED operator<(nullptr_t, unique_ptr)\n";
        }
    } // end unique_ptr operator< block
    { // Use a seperate block to allow reusing test variable names
        // operator<=
        int* i1 = new int[2]{1, 2};
        unique_ptr<int> i2 {&(i1[0])};
        unique_ptr<int> i3 {&(i1[0])};
        unique_ptr<int> i4 {&(i1[1])};
        if (!(i2 <= i3) || !(i2 <= i4) || i4 <= i2)
        {
            ++fail_count;
            std::cout << "FAILED operator<=(unique_ptr, unique_ptr)\n";
        }
        i2.release(); // necessary to avoid horrible things
        i3.release();
        i4.release();
        delete[] i1;
        // operator<= with nullptr_t
        nullptr_t n1;
        int* i5 {};
        unique_ptr<int> i6 {i5};
        unique_ptr<int> i7 {new int{1}};
        if (!(i6 <= n1) || i7 <= n1)
        {
            ++fail_count;
            std::cout << "FAILED operator<=(unique_ptr, nullptr_t)\n";
        }
        if (!(n1 <= i6) || !(n1 <= i7))
        {
            ++fail_count;
            std::cout << "FAILED operator<=(nullptr_t, unique_ptr)\n";
        }
    } // end unique_ptr operator<= block
    { // Use a seperate block to allow reusing test variable names
        // operator>
        int* i1 = new int[2]{1, 2};
        unique_ptr<int> i2 {&(i1[0])};
        unique_ptr<int> i3 {&(i1[0])};
        unique_ptr<int> i4 {&(i1[1])};
        if (i2 > i3 || i2 > i4 || !(i4 > i2))
        {
            ++fail_count;
            std::cout << "FAILED operator>(unique_ptr, unique_ptr)\n";
        }
        i2.release(); // necessary to avoid horrible things
        i3.release();
        i4.release();
        delete[] i1;
        // operator> with nullptr_t
        nullptr_t n1;
        int* i5 {};
        unique_ptr<int> i6 {i5};
        unique_ptr<int> i7 {new int{1}};
        if (i6 > n1 || !(i7 > n1))
        {
            ++fail_count;
            std::cout << "FAILED operator>(unique_ptr, nullptr_t)\n";
        }
        if (n1 > i6 || n1 > i7)
        {
            ++fail_count;
            std::cout << "FAILED operator>(nullptr_t, unique_ptr)\n";
        }
    } // end unique_ptr operator> block
    { // Use a seperate block to allow reusing test variable names
        // operator>=
        int* i1 = new int[2]{1, 2};
        unique_ptr<int> i2 {&(i1[0])};
        unique_ptr<int> i3 {&(i1[0])};
        unique_ptr<int> i4 {&(i1[1])};
        if (!(i2 >= i3) || i2 >= i4 || !(i4 >= i2))
        {
            ++fail_count;
            std::cout << "FAILED operator>=(unique_ptr, unique_ptr)\n";
        }
        i2.release(); // necessary to avoid horrible things
        i3.release();
        i4.release();
        delete[] i1;
        // operator>= with nullptr_t
        nullptr_t n1;
        int* i5 {};
        unique_ptr<int> i6 {i5};
        unique_ptr<int> i7 {new int{1}};
        if (!(i6 >= n1) || !(i7 >= n1))
        {
            ++fail_count;
            std::cout << "FAILED operator>=(unique_ptr, nullptr_t)\n";
        }
        if (!(n1 >= i6) || n1 >= i7)
        {
            ++fail_count;
            std::cout << "FAILED operator>=(nullptr_t, unique_ptr)\n";
        }
    } // end unique_ptr operator<= block
    { // Use a seperate block to allow reusing test variable names
        // swap
        unique_ptr<int> i1 {new int{5}};
        unique_ptr<int> i2 {new int{10}};
        swap(i1, i2);
        if (*i1 != 10 || *i2 != 5)
        {
            ++fail_count;
            std::cout << "FAILED swap(unique_ptr, unique_ptr)\n";
        }
        // make_unique single object
        unique_ptr<int> i3 = make_unique<int>(5);
        if (*i3 != 5)
        {
            ++fail_count;
            std::cout << "FAILED make_unique<int>\n";
        }
        // make_unique array
        unique_ptr<int[]> i4 = make_unique<int[]>(3);
        i4[2] = 10;
        if (i4[2] != 10)
        {
            ++fail_count;
            std::cout << "FAILED make_unique<int[]>\n";
        }
    }

    // pointer_traits, basic type
    if (!is_same<pointer_traits<int*>::pointer, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED pointer_traits<int*>::pointer\n";
    }
    if (!is_same<pointer_traits<int*>::element_type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED pointer_traits<int*>::element_type\n";
    }
    if (!is_same<pointer_traits<int*>::difference_type, ptrdiff_t>::value)
    {
        ++fail_count;
        std::cout << "FAILED pointer_traits<int*>::difference_type\n";
    }
    if (!is_same<pointer_traits<int*>::rebind<char>, char*>::value)
    {
        ++fail_count;
        std::cout << "FAILED pointer_traits<int*>::rebind<char>\n";
    }
    {
        int i1 = 5;
        if (&i1 != pointer_traits<int*>::pointer_to(i1))
        {
            ++fail_count;
            std::cout << "FAILED pointer_traits<int*>::pointer_to\n";
        }
    }
    // pointer_traits, smart type
    if (!is_same<pointer_traits<unique_ptr<int>>::pointer, unique_ptr<int>>::value)
    {
        ++fail_count;
        std::cout << "FAILED pointer_traits<unique_ptr<int>>::pointer\n";
    }
    if (!is_same<pointer_traits<unique_ptr<int>>::element_type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED pointer_traits<unique_ptr<int>>::element_type\n";
    }
    if (!is_same<pointer_traits<unique_ptr<int>>::difference_type, ptrdiff_t>::value)
    {
        ++fail_count;
        std::cout << "FAILED pointer_traits<unique_ptr<int>>::difference_type\n";
    }
    if (!is_same<pointer_traits<unique_ptr<int>>::rebind<char>, unique_ptr<char, default_delete<int>>>::value)
    {
        ++fail_count;
        std::cout << "FAILED pointer_traits<unique_ptr<int>>::rebind<char>\n";
    }

    // default allocator
    // types
    if (!is_same<typename allocator<int>::value_type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator::value_type\n";
    }
    if (!is_same<typename allocator<int>::pointer, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator::pointer\n";
    }
    if (!is_same<typename allocator<int>::const_pointer, const int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator::pointer\n";
    }
    if (!is_same<typename allocator<int>::reference, int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator::reference\n";
    }
    if (!is_same<typename allocator<int>::const_reference, const int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator::const reference\n";
    }
    if (!is_same<typename allocator<int>::size_type, size_t>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator::size_type\n";
    }
    if (!is_same<typename allocator<int>::difference_type, ptrdiff_t>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator::difference_type\n";
    }
    if (!is_same<typename allocator<int>::rebind<char>::other, allocator<char>>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator::rebind\n";
    }
    {
        // Default constructor
        allocator<int> a1;
        if (a1 != allocator<int>{})
        {
            ++fail_count;
            std::cout << "FAILED allocator()\n";
        }
        // Copy constructor, same type
        allocator<int> a2 {a1};
        if (a2 != allocator<int>{})
        {
            ++fail_count;
            std::cout << "FAILED allocator(const allocator&)\n";
        }
        // Copy constructor, different type
        allocator<char> a3 {a1};
        if (a3 != allocator<char>{})
        {
            ++fail_count;
            std::cout << "FAILED allocator<char>(const allocator<int>&)\n";
        }
    }
    {
        // address
        int i1 = 5;
        const int i2 = 10;
        if (allocator<int>{}.address(i1) != &i1 ||
            allocator<int>{}.address(i2) != &i2)
        {
            ++fail_count;
            std::cout << "FAILED allocator::address\n";
        }
        // allocate and construct for single object
        allocator<int> a1;
        int* i3 = a1.allocate(1);
        a1.construct(i3, 5);
        if (*i3 != 5)
        {
            ++fail_count;
            std::cout << "FAILED allocator::construct for single object\n";
        }
        // destroy and reconstruct
        a1.destroy(i3);
        a1.construct(i3, 10);
        if (*i3 != 10)
        {
            ++fail_count;
            std::cout << "FAILED allocator::destroy for single object\n";
        }
        a1.deallocate(i3, 1);
        // allocate and construct for array object
        allocator<int> a2;
        int* i4 = a2.allocate(2);
        a2.construct(i4, 5);
        a2.construct(i4+1, 7);
        if (*i4 != 5 || *(i4 + 1) != 7)
        {
            ++fail_count;
            std::cout << "FAILED allocator::construct for array object\n";
        }
        // destroy and reconstruct
        a2.destroy(i4);
        a2.destroy(i4 + 1);
        a2.construct(i4, 10);
        a2.construct(i4 + 1, 12);
        if (*i4 != 10 || *(i4 + 1) != 12)
        {
            ++fail_count;
            std::cout << "FAILED allocator::destroy for array object\n";
        }
        a2.deallocate(i4, 2);
        // max_size
        allocator<int> a3;
        if(a3.max_size() != numeric_limits<size_t>::max() / sizeof(int))
        {
            ++fail_count;
            std::cout << "FAILED allocator::max_size\n";
        }
        // operator == and !=
        allocator<int> a4;
        allocator<char> a5;
        if (!(a4 == a5))
        {
            ++fail_count;
            std::cout << "FAILED allocator::operator==\n";
        }
        if (a4 != a5)
        {
            ++fail_count;
            std::cout << "FAILED allocator::operator!=\n";
        }
    } // end default allocator block

    // allocator_traits
    // types
    if (!is_same<typename allocator_traits<allocator<int>>::allocator_type,
        allocator<int>>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator_traits::allocator_type\n";
    }
    if (!is_same<typename allocator_traits<allocator<int>>::value_type,
        int>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator_traits::value_type\n";
    }
    if (!is_same<typename allocator_traits<allocator<int>>::pointer,
        int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator_traits::pointer\n";
    }
    if (!is_same<typename allocator_traits<allocator<int>>::const_pointer,
        const int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator_traits::const_pointer\n";
    }
    if (!is_same<typename allocator_traits<allocator<int>>::void_pointer,
        void*>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator_traits::void_pointer\n";
    }
    if (!is_same<typename allocator_traits<allocator<int>>::const_void_pointer,
        const void*>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator_traits::const_void_pointer\n";
    }
    // rebind
    if (!is_same<typename allocator_traits<allocator<int>>::rebind_alloc<char>,
        allocator<char>>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator_traits::rebind_alloc\n";
    }
    if (!is_same<typename allocator_traits<allocator<int>>::rebind_traits<char>,
        allocator_traits<allocator<char>>>::value)
    {
        ++fail_count;
        std::cout << "FAILED allocator_traits::rebind_traits\n";
    }
    {
        // allocate and construct
        allocator<int> a1;
        int* i1 = allocator_traits<allocator<int>>::allocate(a1, 1);
        allocator_traits<allocator<int>>::construct(a1, i1, 5);
        if (*i1 != 5)
        {
            ++fail_count;
            std::cout << "FAILED allocator_traits::construct for single object\n";
        }
        // destroy and reconstruct
        allocator_traits<allocator<int>>::destroy(a1, i1);
        allocator_traits<allocator<int>>::construct(a1, i1, 10);
        if (*i1 != 10)
        {
            ++fail_count;
            std::cout << "FAILED allocator_traits::destroy for single object\n";
        }
        allocator_traits<allocator<int>>::deallocate(a1, i1, 1);
        // size
        allocator<int> a2;
        if(allocator_traits<allocator<int>>::max_size(a2) !=
            numeric_limits<size_t>::max() /sizeof(int))
        {
            ++fail_count;
            std::cout << "FAILED allocator_traits::max_size\n";
        }
        // select on container copy construction
        allocator<int> a3;
        if (!is_same<decltype(a3), decltype(allocator_traits<allocator<int>>::
            select_on_container_copy_construction(a3))>::value)
        {
            ++fail_count;
            std::cout << "FAILED allocator_traits::select_on_container_copy_construction\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
