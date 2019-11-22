#include <iostream>

#include "../include/type_traits"

using namespace klib;

// Various types for the test.
template<template <typename B, typename... Args> class A, typename B,
    typename... Args>
struct void_t_struct_1 {};
template<typename A, typename... Args>
struct void_t_struct_2 {};

template<typename A>
struct detected_or_struct_1 {};
struct detected_or_struct_2 { using a = long; };
struct detected_or_struct_3 {};
template<typename A>
using detected_or_type = typename A::a;

enum make_unsigned_type : int {};

struct is_convertible_type_1 {};
struct is_convertible_type_2 : public is_convertible_type_1 {};
struct is_convertible_type_3 {
    operator is_convertible_type_1() { return is_convertible_type_1{}; }
};
struct is_convertible_type_4 {};

struct is_pointer_type { static int test_type; };

enum is_scalar_type_1 {};
struct is_scalar_type_2 {};

class is_class_type_1 {};
struct is_class_type_2 {};
union is_class_type_3 {};

union is_union_type_1 {};
class is_union_type_2 {};

union is_object_type_1 {};
class is_object_type_2 {};

struct is_base_of_type_1 {};
struct is_base_of_type_2 {};
struct is_base_of_type_3 : is_base_of_type_1 {};

void is_destructible_function();
struct is_destructible_type_1 {};
struct is_destructible_type_2 { ~is_destructible_type_2() = delete; };

struct is_default_constructible_type_1 {};
struct is_default_constructible_type_2 { is_default_constructible_type_2() = delete; };

struct is_constructible_type_1 {
    is_constructible_type_1() {};
    is_constructible_type_1(int) {};
    is_constructible_type_1(int, int) {};
};
struct is_constructible_type_2 {
    is_constructible_type_2() = delete;
    is_constructible_type_2(const is_constructible_type_2&) = delete;
    is_constructible_type_2(is_constructible_type_2&&) = delete;
};
struct is_constructible_type_3 : public is_constructible_type_1 {};

struct is_nothrow_default_constructible_type_1 { is_nothrow_default_constructible_type_1() noexcept; };
struct is_nothrow_default_constructible_type_2 { is_nothrow_default_constructible_type_2(); };
struct is_nothrow_default_constructible_type_3 { is_nothrow_default_constructible_type_3() = delete; };

struct is_nothrow_constructible_type_1 {
    is_nothrow_constructible_type_1() noexcept;
    is_nothrow_constructible_type_1(int) noexcept;
    is_nothrow_constructible_type_1(int, int) noexcept;
};
struct is_nothrow_constructible_type_2 {
    is_nothrow_constructible_type_2();
    is_nothrow_constructible_type_2(int);
    is_nothrow_constructible_type_2(int, int);
};
struct is_nothrow_constructible_type_3 {};

struct is_nothrow_move_constructible_type_1 {
    is_nothrow_move_constructible_type_1(is_nothrow_move_constructible_type_1&&) noexcept;
};
struct is_nothrow_move_constructible_type_2 {
    is_nothrow_move_constructible_type_2(is_nothrow_move_constructible_type_2&&);
};
struct is_nothrow_move_constructible_type_3 {
    is_nothrow_move_constructible_type_3(is_nothrow_move_constructible_type_3&&) = delete;
};

struct is_assignable_type_1 {};
struct is_assignable_type_2 {
    is_assignable_type_2& operator=(const is_assignable_type_1&);
};

struct is_nothrow_assignable_type_1 {};
struct is_nothrow_assignable_type_2 {
    is_nothrow_assignable_type_2& operator=(const is_nothrow_assignable_type_1&) noexcept;
};

struct is_nothrow_move_assignable_type_1 {
    is_nothrow_move_assignable_type_1& operator=(is_nothrow_move_assignable_type_1&&) = delete;
};
struct is_nothrow_move_assignable_type_2 {
    is_nothrow_move_assignable_type_2& operator=(is_nothrow_move_assignable_type_2&&) noexcept;
};
struct is_nothrow_move_assignable_type_3 {
    is_nothrow_move_assignable_type_3& operator=(is_nothrow_move_assignable_type_3&&);
};

struct common_type_1 {};
struct common_type_2 : public common_type_1 {};

// Beginning of the tests.
int main()
{
    size_t fail_count = 0;

    // integral_constant
    if (integral_constant<int, 10>::value != 10)
    {
        ++fail_count;
        std::cout << "FAILED integral_constant<int, 10>::value\n";
    }

    // true_type
    if (true_type::value != true)
    {
        ++fail_count;
        std::cout << "FAILED true_type::value\n";
    }

    // false_type
    if (false_type::value != false)
    {
        ++fail_count;
        std::cout << "FAILED false_type::value\n";
    }

    // is_same
    struct is_same_struct {};
    if (!is_same<int, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_same<int, int>\n";
    }
    if (is_same<int, long>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_same<int, long>\n";
    }
    if (!is_same<is_same_struct, is_same_struct>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_same<is_same_struct, is_same_struct>\n";
    }
    if (is_same<int, is_same_struct>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_same<int, is_same_struct>\n";
    }

    // void_t
    if (!is_same<void, void_t<char>>::value)
    {
        ++fail_count;
        std::cout << "FAILED void_t<char>\n";
    }    
    if (!is_same<void, void_t<int, long>>::value)
    {
        ++fail_count;
        std::cout << "FAILED void_t<int, long>\n";
    }
    if (!is_same<void, void_t<void_t_struct_1<void_t_struct_2, int, long>>>::value)
    {
        ++fail_count;
        std::cout <<
            "FAILED void_t<void_t_struct_1<void_t_struct_2, int, long>>\n";
    }

    // detected_or
    if (!is_same<detected_or<int, detected_or_struct_1, char>::type,
        detected_or_struct_1<char>>::value)
    {
        ++fail_count;
        std::cout << "FAILED detected_or<int, detected_or_struct_1, char> " <<
            "detected_or_struct_1<char>\n";
    }
    if (is_same<detected_or<int, detected_or_struct_1, char>::type,
        detected_or_struct_1<int>>::value)
    {
        ++fail_count;
        std::cout << "FAILED detected_or<int, detected_or_struct_1, char> " <<
            "detected_or_struct_1<int>\n";
    }
    if (!is_same<detected_or<int, detected_or_type, detected_or_struct_2>::type,
        long>::value)
    {
        ++fail_count;
        std::cout << "FAILED detected_or<int, detected_or_type, " <<
            "detected_or_struct_2> detected_or_struct_2\n";
    }
    if (!is_same<detected_or<int, detected_or_type, detected_or_struct_3>::type,
        int>::value)
    {
        ++fail_count;
        std::cout << "FAILED detected_or<int, detected_or_type, " <<
            "detected_or_struct_3> int\n";
    }

    // conditional
    if (!is_same<conditional<true, int, long>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED conditional<true, int, long> int\n";
    }
    if (!is_same<conditional<false, int, long>::type, long>::value)
    {
        ++fail_count;
        std::cout << "FAILED conditional<false, int, long> long\n";
    }

    // enable_if
    if (!is_same<enable_if<true, int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED enable_if<true, int> int\n";
    }
    if (!is_same<enable_if<true>::type, void>::value)
    {
        ++fail_count;
        std::cout << "FAILED enable_if<true> void\n";
    }

    // remove_const
    if (!is_same<remove_const<const int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_const<const int>\n";
    }
    if (!is_same<remove_const<int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_const<int>\n";
    }
    if (!is_same<remove_const<const volatile int>::type, volatile int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_const<const volatile int>\n";
    }
    if (!is_same<remove_const<volatile int>::type, volatile int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_const<volatile int>\n";
    }

    // remove_volatile
    if (!is_same<remove_volatile<volatile int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_volatile<volatile int>\n";
    }
    if (!is_same<remove_volatile<int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_volatile<int>\n";
    }
    if (!is_same<remove_volatile<const volatile int>::type, const int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_volatile<const volatile int>\n";
    }
    if (!is_same<remove_volatile<const int>::type, const int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_volatile<const int>\n";
    }

    // remove_cv
    if (!is_same<remove_cv<int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_cv<int>\n";
    }
    if (!is_same<remove_cv<volatile int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_cv<volatile int>\n";
    }
    if (!is_same<remove_cv<const int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_cv<const int>\n";
    }
    if (!is_same<remove_cv<const volatile int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_cv<const volatile int>\n";
    }

    // is_const
    if (!is_const<const int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_const<const int>\n";
    }
    if (is_const<volatile int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_const<volatile int>\n";
    }
    if (is_const<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_const<int>\n";
    }

    // is_volatile
    if (!is_volatile<volatile int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_volatile<volatile int>\n";
    }
    if (is_volatile<const int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_volatile<const int>\n";
    }
    if (is_volatile<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_volatile<int>\n";
    }

    // is_void
    if (!is_void<void>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_void<void>\n";
    }
    if (!is_void<const void>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_void<const void>\n";
    }
    if (!is_void<const volatile void>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_void<const volatile void>\n";
    }
    if (is_void<void*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_void<void*>\n";
    }
    if (is_void<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_void<int>\n";
    }

    // is_enum
    enum enum_1 {};
    enum class enum_2 {};
    class enum_3;
    if (!is_enum<enum_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_enum<enum_1>\n";
    }
    if (!is_enum<enum_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_enum<enum_2>\n";
    }
    if (is_enum<enum_3>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_enum<enum_3>\n";
    }
    if (is_enum<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_enum<int>\n";
    }

    // underlying_type
    enum enum_4 : unsigned char {};
    enum class enum_5 : unsigned char {};
    if (!is_same<underlying_type<enum_2>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED underlying_type<> for default scoped enum\n";
    }
    if (!is_same<underlying_type<enum_4>::type, unsigned char>::value)
    {
        ++fail_count;
        std::cout << "FAILED underlying_type<> for unsigned char unscoped enum\n";
    }
    if (!is_same<underlying_type<enum_5>::type, unsigned char>::value)
    {
        ++fail_count;
        std::cout << "FAILED underlying_type<> for unsigned char scoped enum\n";
    }

    // is_integral
    class is_integral_1 {};
    enum is_integral_2 {};
    if (!is_integral<bool>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<bool>\n";
    }
    if (!is_integral<char>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<char>\n";
    }
    if (!is_integral<unsigned char>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<unsigned char>\n";
    }
    if (!is_integral<signed char>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<signed char>\n";
    }
    if (!is_integral<short>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<short>\n";
    }
    if (!is_integral<unsigned short>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<unsigned short>\n";
    }
    if (!is_integral<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<int>\n";
    }
    if (!is_integral<unsigned int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<unsigned int>\n";
    }
    if (!is_integral<long>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<long>\n";
    }
    if (!is_integral<unsigned long>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<unsigned long>\n";
    }
    if (!is_integral<long long>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<long long>\n";
    }
    if (!is_integral<unsigned long long>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<unsigned long long>\n";
    }
    if (!is_integral<const int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<const int>\n";
    }
    if (!is_integral<volatile int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<volatile int>\n";
    }
    if (!is_integral<const volatile int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<const volatile int>\n";
    }
    if (is_integral<is_integral_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<is_integral_1>\n";
    }
    if (is_integral<is_integral_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<is_integral_2>\n";
    }
    if (is_integral<double>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_integral<double>\n";
    }

    // is_floating_point
    if (!is_floating_point<float>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_floating_point<float>\n";
    }
    if (!is_floating_point<double>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_floating_point<double>\n";
    }
    if (!is_floating_point<long double>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_floating_point<long double>\n";
    }
    if (!is_floating_point<const double>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_floating_point<const double>\n";
    }
    if (is_floating_point<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_floating_point<int>\n";
    }
    if (is_floating_point<int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_floating_point<int*>\n";
    }

    // is_pointer
    if (!is_pointer<int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_pointer<int*>\n";
    }
    if (is_pointer<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_pointer<int>\n";
    }
    if (!is_pointer<const int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_pointer<const int*>\n";
    }
    if (!is_pointer<const int* const>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_pointer<const int* const>\n";
    }
    if (!is_pointer<is_pointer_type*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_pointer<is_pointer_type*>\n";
    }
    if (is_pointer<is_pointer_type>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_pointeris_pointer_type>\n";
    }

    // is_member_pointer
    if (is_member_pointer<int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_member_pointer<int*>\n";
    }
    if (is_member_pointer<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_member_pointer<int>\n";
    }
    if (!is_member_pointer<int is_pointer_type::*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_member_pointer<int is_pointer_type::*>\n";
    }
    if (!is_member_pointer<void (is_pointer_type::*)()>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_member_pointer<void (is_pointer_type::*)()>\n";
    }
    if (is_member_pointer<decltype(is_pointer_type::test_type)*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_member_pointer<decltype(is_pointer_type::test_type)*>\n";
    }

    // is_null_pointer
    if (is_null_pointer<int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_null_pointer<int*>\n";
    }
    if (!is_null_pointer<nullptr_t>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_null_pointer<nullptr_t>\n";
    }
    if (!is_null_pointer<decltype(nullptr)>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_null_pointer<decltype<nullptr>>\n";
    }

    // is_array
    if (is_array<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_array<int>\n";
    }
    if (is_array<int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_array<int&>\n";
    }
    if (!is_array<int[]>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_array<int[]>\n";
    }
    if (!is_array<int[4]>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_array<int[4]>\n";
    }
    if (!is_array<const int[]>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_array<const int[]>\n";
    }
    if (!is_array<const int[4]>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_array<const int[4]>\n";
    }

    // is_arithmetic
    if (!is_arithmetic<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_arithmetic<int>\n";
    }
    if (!is_arithmetic<const int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_arithmetic<const int>\n";
    }
    if (is_arithmetic<int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_arithmetic<int*>\n";
    }
    if (!is_arithmetic<double>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_arithmetic<double>\n";
    }
    if (!is_arithmetic<const double>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_arithmetic<const double>\n";
    }
    if (is_arithmetic<double*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_arithmetic<double*>\n";
    }

    // is_scalar
    if (!is_scalar<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_scalar<int>\n";
    }
    if (!is_scalar<double>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_scalar<double>\n";
    }
    if (!is_scalar<int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_scalar<int*>\n";
    }
    if (!is_scalar<nullptr_t>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_scalar<nullptr_t>\n";
    }
    if (!is_scalar<is_scalar_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_scalar<is_scalar_type_1>\n";
    }
    if (is_scalar<is_scalar_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_scalar<is_scalar_type_2>\n";
    }
    if (is_scalar<int[]>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_scalar<int[]>\n";
    }

    // extent
    if (extent<int>::value != 0)
    {
        ++fail_count;
        std::cout << "FAILED extent<int>\n";
    }
    if (extent<int[]>::value != 0)
    {
        ++fail_count;
        std::cout << "FAILED extent<int[]>\n";
    }
    if (extent<int[5]>::value != 5)
    {
        ++fail_count;
        std::cout << "FAILED extent<int[5]>\n";
    }
    if (extent<int[5][10]>::value != 5)
    {
        ++fail_count;
        std::cout << "FAILED extent<int[5][10]>\n";
    }
    if (extent<int[][5]>::value != 0)
    {
        ++fail_count;
        std::cout << "FAILED extent<int[][5]>\n";
    }

    // remove_extent
    if (!is_same<remove_extent<int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_extent<int>\n";
    }
    if (!is_same<remove_extent<int[]>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_extent<int[]>\n";
    }
    if (!is_same<remove_extent<int[5]>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_extent<int[5]\n";
    }
    if (!is_same<remove_extent<int[5][10]>::type, int[10]>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_extent<int[5][10]>\n";
    }
    if (!is_same<remove_extent<int[][5]>::type, int[5]>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_extent<int[][5]>\n";
    }

    // remove_all_extents
    if (!is_same<remove_all_extents<int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_all_extents<int>\n";
    }
    if (!is_same<remove_all_extents<int[]>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_all_extents<int[]>\n";
    }
    if (!is_same<remove_all_extents<int[5]>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_all_extents<int[5]\n";
    }
    if (!is_same<remove_all_extents<int[5][10]>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_all_extents<int[5][10]>\n";
    }
    if (!is_same<remove_all_extents<int[][5]>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_all_extents<int[][5]>\n";
    }

    // is_function
    if (is_function<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_function<int>\n";
    }
    if (!is_function<int(int)>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_function<int(int)>\n";
    }
    if (!is_function<int(int)&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_function<int(int)&>\n";
    }
    if (!is_function<int(int)&&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_function<int(int)&&>\n";
    }
    if (!is_function<int(int) const>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_function<int(int) const>\n";
    }

    // is_union
    if (!is_union<is_union_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_union<is_union_type_1>\n";
    }
    if (is_union<is_union_type_1&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_union<is_union_type_1&>\n";
    }
    if (is_union<is_union_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_union<is_union_type_2>\n";
    }
    if (is_union<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_union<int>\n";
    }

    // is_class
    if (!is_class<is_class_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_class<is_class_type_1>\n";
    }
    if (is_class<is_class_type_1&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_class<is_class_type_1&>\n";
    }
    if (!is_class<is_class_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_class<is_class_type_2>\n";
    }
    if (is_class<is_class_type_3>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_class<is_class_type_3>\n";
    }
    if (is_class<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_class<int>\n";
    }
    if (is_class<int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_class<int*>\n";
    }
    if (is_class<int(int)>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_class<int(int)>\n";
    }

    // is_object
    if (!is_object<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_object<int>\n";
    }
    if (!is_object<int[5]>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_object<int[5]>\n";
    }
    if (!is_object<int[]>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_object<int[]>\n";
    }
    if (!is_object<int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_object<int*>\n";
    }
    if (!is_object<is_object_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_object<is_object_type_1>\n";
    }
    if (!is_object<is_object_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_object<is_object_type_2>\n";
    }
    if (is_object<int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_object<int&>\n";
    }
    if (is_object<int(int)>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_object<int(int)>\n";
    }

    // is_lvalue_reference
    if (!is_lvalue_reference<int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_lvalue_reference<int&>\n";
    }
    if (!is_lvalue_reference<const int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_lvalue_reference<const int&>\n";
    }
    if (is_lvalue_reference<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_lvalue_reference<int>\n";
    }
    if (is_lvalue_reference<int&&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_lvalue_reference<int&&>\n";
    }

    // is_rvalue_reference
    if (!is_rvalue_reference<int&&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_rvalue_reference<int&&>\n";
    }
    if (is_rvalue_reference<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_rvalue_reference<int>\n";
    }
    if (is_rvalue_reference<int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_rvalue_reference<int&>\n";
    }

    // is_reference
    if (!is_reference<int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_reference<int&>\n";
    }
    if (!is_reference<const int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_reference<const int&>\n";
    }
    if (!is_reference<int&&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_reference<int&&>\n";
    }
    if (is_reference<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_reference<int>\n";
    }

    // remove_reference
    if (!is_same<remove_reference<int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_reference<int>\n";
    }
    if (!is_same<remove_reference<int&>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_reference<int&>\n";
    }
    if (!is_same<remove_reference<int&&>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_reference<int&&>\n";
    }
    if (!is_same<remove_reference<const int>::type, const int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_reference<const int>\n";
    }
    if (!is_same<remove_reference<const int&>::type, const int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_reference<const int&>\n";
    }
    if (!is_same<remove_reference<const int&&>::type, const int>::value)
    {
        ++fail_count;
        std::cout << "FAILED remove_reference<const int&&>\n";
    }

    // add_lvalue_reference
    if (!is_same<add_lvalue_reference<int>::type, int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_lvalue_reference<int>\n";
    }
    if (!is_same<add_lvalue_reference<int&>::type, int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_lvalue_reference<int&>\n";
    }
    if (!is_same<add_lvalue_reference<int&&>::type, int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_lvalue_reference<int&&>\n";
    }
    if (!is_same<add_lvalue_reference<const int>::type, const int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_lvalue_reference<const int>\n";
    }
    if (!is_same<add_lvalue_reference<const int&>::type, const int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_lvalue_reference<const int&>\n";
    }
    if (!is_same<add_lvalue_reference<const int&&>::type, const int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_lvalue_reference<const int&&>\n";
    }

    // add_rvalue_reference
    if (!is_same<add_rvalue_reference<int>::type, int&&>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_rvalue_reference<int>\n";
    }
    if (!is_same<add_rvalue_reference<int&>::type, int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_rvalue_reference<int&>\n";
    }
    if (!is_same<add_rvalue_reference<int&&>::type, int&&>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_rvalue_reference<int&&>\n";
    }
    if (!is_same<add_rvalue_reference<const int>::type, const int&&>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_rvalue_reference<const int>\n";
    }
    if (!is_same<add_rvalue_reference<const int&>::type, const int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_rvalue_reference<const int&>\n";
    }
    if (!is_same<add_rvalue_reference<const int&&>::type, const int&&>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_rvalue_reference<const int&&>\n";
    }

    // make_unsigned
    if (!is_same<make_unsigned<int>::type, unsigned int>::value)
    {
        ++fail_count;
        std::cout << "FAILED make_unsigned<int>\n";
    }
    if (!is_same<make_unsigned<const int>::type, const unsigned int>::value)
    {
        ++fail_count;
        std::cout << "FAILED make_unsigned<const int>\n";
    }
    if (!is_same<make_unsigned<volatile int>::type, volatile unsigned int>::value)
    {
        ++fail_count;
        std::cout << "FAILED make_unsigned<volatile int>\n";
    }
    if (!is_same<make_unsigned<const volatile int>::type, const volatile unsigned int>::value)
    {
        ++fail_count;
        std::cout << "FAILED make_unsigned<const volatile int>\n";
    }
    if (!is_same<make_unsigned<char>::type, unsigned char>::value)
    {
        ++fail_count;
        std::cout << "FAILED make_unsigned<char>\n";
    }
    if (!is_same<make_unsigned<signed char>::type, unsigned char>::value)
    {
        ++fail_count;
        std::cout << "FAILED make_unsigned<signed char>\n";
    }
    if (!is_same<make_unsigned<short>::type, unsigned short>::value)
    {
        ++fail_count;
        std::cout << "FAILED make_unsigned<short>\n";
    }
    if (!is_same<make_unsigned<long>::type, unsigned long>::value)
    {
        ++fail_count;
        std::cout << "FAILED make_unsigned<long>\n";
    }
    if (!is_same<make_unsigned<long long>::type, unsigned long long>::value)
    {
        ++fail_count;
        std::cout << "FAILED make_unsigned<long long>\n";
    }
    if (!is_same<make_unsigned<unsigned int>::type, unsigned int>::value)
    {
        ++fail_count;
        std::cout << "FAILED make_unsigned<unsigned int>\n";
    }
    if (!is_same<make_unsigned<bool>::type, bool>::value)
    {
        ++fail_count;
        std::cout << "FAILED make_unsigned<bool>\n";
    }
    if (!is_same<make_unsigned<make_unsigned_type>::type, unsigned int>::value)
    {
        ++fail_count;
        std::cout << "FAILED make_unsigned<make_unsigned_type>\n";
    }

    // is_convertible
    if (!is_convertible<char, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_convertible<char, int>\n";
    }
    if (is_convertible<char*, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_convertible<char*, int*>\n";
    }
    if (!is_convertible<void, void>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_convertible<void, void>\n";
    }
    if (is_convertible<int, void>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_convertible<int, void>\n";
    }
    if (is_convertible<void, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_convertible<void, int>\n";
    }
    if (!is_convertible<is_convertible_type_2*, is_convertible_type_1*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_convertible<is_convertible_type_2*, is_convertible_type_1*>\n";
    }
    if (is_convertible<is_convertible_type_1*, is_convertible_type_2*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_convertible<is_convertible_type_1*, is_convertible_type_2*>\n";
    }
    if (!is_convertible<is_convertible_type_3, is_convertible_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_convertible<is_convertible_type_3, is_convertible_type_1>\n";
    }
    if (is_convertible<is_convertible_type_4, is_convertible_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_convertible<is_convertible_type_4, is_convertible_type_1>\n";
    }

    // is_base_of
    if (is_base_of<is_base_of_type_1, is_base_of_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_base_of<is_base_of_type_1, is_base_of_type_2>\n";
    }
    if (!is_base_of<is_base_of_type_1, is_base_of_type_3>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_base_of<is_base_of_type_1, is_base_of_type_3>\n";
    }
    if (is_base_of<is_base_of_type_3, is_base_of_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_base_of<is_base_of_type_3, is_base_of_type_1>\n";
    }
    if (is_base_of<is_base_of_type_1, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED Is_base_of<is_base_of_type_1, int>\n";
    }

    // is_destructible
    if(is_destructible<void>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_destructible<void>\n";
    }
    if(is_destructible<int[]>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_destructible<int[]>\n";
    }
    if(is_destructible<decltype(is_destructible_function)>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_destructible<decltype(is_destructible_function)>\n";
    }
    if(!is_destructible<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_destructible<int>\n";
    }
    if(!is_destructible<int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_destructible<int*>\n";
    }
    if(!is_destructible<int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_destructible<int&>\n";
    }
    if(!is_destructible<const int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_destructible<int&>\n";
    }
    if(!is_destructible<int&&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_destructible<int&&>\n";
    }
    if(!is_destructible<void (*)()>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_destructible<void (*)()>\n";
    }
    if(!is_destructible<is_destructible_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_destructible<is_destructible_type_1>\n";
    }
    if(is_destructible<is_destructible_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_destructible<is_destructible_type_2>\n";
    }

    // is_default_constructible
    if(is_default_constructible<void>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_default_constructible<void>\n";
    }
    if(!is_default_constructible<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_default_constructible<int>\n";
    }
    if(is_default_constructible<int[]>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_default_constructible<int[]>\n";
    }
    if(!is_default_constructible<int[5]>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_default_constructible<int[5]>\n";
    }
    if(!is_default_constructible<is_default_constructible_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_default_constructible<is_default_constructible_type_1>\n";
    }
    if(is_default_constructible<is_default_constructible_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_default_constructible<is_default_constructible_type_2>\n";
    }
    if(is_default_constructible<is_default_constructible_type_2[5]>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_default_constructible<is_default_constructible_type_2[5]>\n";
    }

    // is_constructible
    if(!is_constructible<int, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<int, int>\n";
    }
    if(!is_constructible<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<int>\n";
    }
    if(!is_constructible<int*, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<int*, int*>\n";
    }
    if(!is_constructible<is_constructible_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<is_constructible_type_1>\n";
    }
    if(!is_constructible<is_constructible_type_1, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<is_constructible_type_1, int\n";
    }
    if(!is_constructible<is_constructible_type_1, int, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<is_constructible_type_1, int, int>\n";
    }
    if(is_constructible<is_constructible_type_1, int, int, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<is_constructible_type_1, int, int, int>\n";
    }
    if(is_constructible<is_constructible_type_1, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<is_constructible_type_1, int*>\n";
    }
    if(!is_constructible<is_constructible_type_1, const is_constructible_type_1&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<is_constructible_type_1, const is_constructible_type_1&>\n";
    }
    if(!is_constructible<is_constructible_type_1, is_constructible_type_1&&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<is_constructible_type_1, is_constructible_type_1&&>\n";
    }
    if(!is_constructible<is_constructible_type_1, const is_constructible_type_3&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<is_constructible_type_1, const is_constructible_type_3&>\n";
    }
    if(is_constructible<is_constructible_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<is_constructible_type_2>\n";
    }
    if(is_constructible<is_constructible_type_2, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<is_constructible_type_2, int>\n";
    }
    if(is_constructible<is_constructible_type_2, const is_constructible_type_2&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<is_constructible_type_2, is_constructible_type_2>\n";
    }
    if(is_constructible<is_constructible_type_2, is_constructible_type_2&&>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_constructible<is_constructible_type_2, is_constructible_type_2&&>\n";
    }

    // is_nothrow_default_constructible
    if(is_nothrow_default_constructible<void>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_default_constructible<void>\n";
    }
    if(!is_nothrow_default_constructible<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_default_constructible<int>\n";
    }
    if(!is_nothrow_default_constructible<is_nothrow_default_constructible_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_default_constructible<is_nothrow_default_constructible_type_1>\n";
    }
    if(is_nothrow_default_constructible<is_nothrow_default_constructible_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_default_constructible<is_nothrow_default_constructible_type_2>\n";
    }
    if(is_nothrow_default_constructible<is_nothrow_default_constructible_type_3>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_default_constructible<is_nothrow_default_constructible_type_3>\n";
    }

    // is_nothrow_constructible
    if(is_nothrow_constructible<void>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_constructible<void>\n";
    }
    if(!is_nothrow_constructible<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_constructible<int>\n";
    }
    if(!is_nothrow_constructible<is_nothrow_constructible_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_constructible<is_nothrow_constructible_type_1>\n";
    }
    if(!is_nothrow_constructible<is_nothrow_constructible_type_1, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_constructible<is_nothrow_constructible_type_1, int>\n";
    }
    if(!is_nothrow_constructible<is_nothrow_constructible_type_1, int, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_constructible<is_nothrow_constructible_type_1, int, int>\n";
    }
    if(is_nothrow_constructible<is_nothrow_constructible_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_constructible<is_nothrow_constructible_type_2>\n";
    }
    if(is_nothrow_constructible<is_nothrow_constructible_type_2, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_constructible<is_nothrow_constructible_type_2, int>\n";
    }
    if(is_nothrow_constructible<is_nothrow_constructible_type_2, int, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_constructible<is_nothrow_constructible_type_2, int, int>\n";
    }
    if(!is_nothrow_constructible<is_nothrow_constructible_type_3>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_constructible<is_nothrow_constructible_type_3>\n";
    }
    if(is_nothrow_constructible<is_nothrow_constructible_type_3, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_constructible<is_nothrow_constructible_type_3, int>\n";
    }
    if(is_nothrow_constructible<is_nothrow_constructible_type_3, int, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_constructible<is_nothrow_constructible_type_3, int, int>\n";
    }

    // is_nothrow_move_constructible
    if(is_nothrow_move_constructible<void>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_move_constructible<void>\n";
    }
    if(!is_nothrow_move_constructible<int>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_move_constructible<int>\n";
    }
    if(!is_nothrow_move_constructible<is_nothrow_move_constructible_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_move_constructible<is_nothrow_move_constructible_type_1>\n";
    }
    if(is_nothrow_move_constructible<is_nothrow_move_constructible_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_move_constructible<is_nothrow_move_constructible_type_2>\n";
    }
    if(is_nothrow_move_constructible<is_nothrow_move_constructible_type_3>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_move_constructible<is_nothrow_move_constructible_type_3>\n";
    }

    // is_assignable
    if(is_assignable<is_assignable_type_1, is_assignable_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_assignable<is_assignable_type_1, is_assignable_type_2>\n";
    }
    if(!is_assignable<is_assignable_type_2, is_assignable_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_assignable<is_assignable_type_2, is_assignable_type_1>\n";
    }

    // is_nothrow_assignable
    if(is_nothrow_assignable<is_assignable_type_1, is_assignable_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_assignable<is_assignable_type_1, is_assignable_type_2>\n";
    }
    if(is_nothrow_assignable<is_assignable_type_2, is_assignable_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_assignable<is_assignable_type_2, is_assignable_type_1>\n";
    }
    if(is_nothrow_assignable<is_nothrow_assignable_type_1, is_nothrow_assignable_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_assignable<is_nothrow_assignable_type_1, is_nothrow_assignable_type_2>\n";
    }
    if(!is_nothrow_assignable<is_nothrow_assignable_type_2, is_nothrow_assignable_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_assignable<is_nothrow_assignable_type_2, is_nothrow_assignable_type_1>\n";
    }

    // is_nothrow_move_assignable
    if(is_nothrow_move_assignable<is_nothrow_move_assignable_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_move_assignable<is_nothrow_move_assignable_type_1>\n";
    }
    if(!is_nothrow_move_assignable<is_nothrow_move_assignable_type_2>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_move_assignable<is_nothrow_move_assignable_type_2>\n";
    }
    if(is_nothrow_move_assignable<is_nothrow_move_assignable_type_3>::value)
    {
        ++fail_count;
        std::cout << "FAILED is_nothrow_move_assignable<is_nothrow_move_assignable_type_3>\n";
    }

    // add_pointer
    if (!is_same<add_pointer<int>::type, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_pointer<int>\n";
    }
    if (!is_same<add_pointer<int*>::type, int**>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_pointer<int*>\n";
    }
    if (!is_same<add_pointer<int&>::type, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_pointer<int&>\n";
    }
    if (!is_same<add_pointer<int&&>::type, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_pointer<int&&>\n";
    }
    if (!is_same<add_pointer<const int>::type, const int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED add_pointer<const int>\n";
    }

    // decay
    if (!is_same<decay<int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED decay<int>\n";
    }
    if (!is_same<decay<int*>::type, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED decay<int*>\n";
    }
    if (!is_same<decay<int&>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED decay<int&>\n";
    }
    if (!is_same<decay<int&&>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED decay<int&&>\n";
    }
    if (!is_same<decay<int[]>::type, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED decay<int[]>\n";
    }
    if (!is_same<decay<int[5]>::type, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED decay<int[5]>\n";
    }
    if (!is_same<decay<int(int)>::type, int(*)(int)>::value)
    {
        ++fail_count;
        std::cout << "FAILED decay<int(int)>\n";
    }
    if (!is_same<decay<int(*)(int)>::type, int(*)(int)>::value)
    {
        ++fail_count;
        std::cout << "FAILED decay<int(*)(int)>\n";
    }

    // strip_reference_wrapper
    if (!is_same<helper::strip_reference_wrapper<int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::strip_reference_wrapper<int>\n";
    }
    if (!is_same<helper::strip_reference_wrapper<reference_wrapper<int>>::type,
        int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::strip_reference_wrapper<reference_wrapper<int>>\n";
    }
    if (!is_same<helper::strip_reference_wrapper<reference_wrapper<int&>>::type,
        int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::strip_reference_wrapper<reference_wrapper<int&>>\n";
    }

    // decay_and_strip
    if (!is_same<helper::decay_and_strip<int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::decay_and_strip<int>\n";
    }
    if (!is_same<helper::decay_and_strip<int*>::type, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::decay_and_strip<int*>\n";
    }
    if (!is_same<helper::decay_and_strip<int&>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::decay_and_strip<int&>\n";
    }
    if (!is_same<helper::decay_and_strip<int&&>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::decay_and_strip<int&&>\n";
    }
    if (!is_same<helper::decay_and_strip<int[]>::type, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::decay_and_strip<int[]>\n";
    }
    if (!is_same<helper::decay_and_strip<int[5]>::type, int*>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::decay_and_strip<int[5]>\n";
    }
    if (!is_same<helper::decay_and_strip<int(int)>::type, int(*)(int)>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::decay_and_strip<int(int)>\n";
    }
    if (!is_same<helper::decay_and_strip<int(*)(int)>::type,
        int(*)(int)>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::decay_and_strip<int(*)(int)>\n";
    }
    if (!is_same<helper::decay_and_strip<reference_wrapper<int>>::type,
        int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::decay_and_strip<reference_wrapper<int>>\n";
    }
    if (!is_same<helper::decay_and_strip<reference_wrapper<int&>>::type,
        int&>::value)
    {
        ++fail_count;
        std::cout << "FAILED helper::decay_and_strip<reference_wrapper<int&>>\n";
    }

    // common_type
    if (!is_same<common_type<char, short, int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED common_type<char, short, int>\n";
    }
    if (!is_same<common_type<float, double>::type, double>::value)
    {
        ++fail_count;
        std::cout << "FAILED common_type<float, double>\n";
    }
    if (!is_same<common_type<const volatile int, const int>::type, int>::value)
    {
        ++fail_count;
        std::cout << "FAILED common_type<const volatile int, const int>\n";
    }
    if (!is_same<common_type<common_type_1, common_type_2>::type, common_type_1>::value)
    {
        ++fail_count;
        std::cout << "FAILED common_type<common_type_1, common_type_2>\n";
    }
    if (!is_same<common_type<common_type_1*, common_type_2*>::type, common_type_1*>::value)
    {
        ++fail_count;
        std::cout << "FAILED common_type<common_type_1*, common_type_2*>\n";
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
