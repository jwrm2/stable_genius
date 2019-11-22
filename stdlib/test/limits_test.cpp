#include <iostream>

#include "../include/limits"

#include "../include/cmath"

using namespace klib;

// Non numeric type for testing general numeric_limits
class dummy_type {
private:
    int i;

public:
    dummy_type()
    {
        i = 10;
    }

    bool operator==(const dummy_type& other) const
    {
        return i == other.i;
    }

    bool operator!=(const dummy_type& other) const
    {
        return !(*this == other);
    }
};

int main()
{
    size_t fail_count = 0;

    // Preprocessor definitions
#ifndef CHAR_MAX
    ++fail_count;
    std::cout << "FAILED CHAR_MAX not defined\n";
#endif /* CHAR_MAX */
#ifndef CHAR_MIN
    ++fail_count;
    std::cout << "FAILED CHAR_MIN not defined\n";
#endif /* CHAR_MIN */
#ifndef SCHAR_MAX
    ++fail_count;
    std::cout << "FAILED SCHAR_MAX not defined\n";
#endif /* SCHAR_MAX */
#ifndef SCHAR_MIN
    ++fail_count;
    std::cout << "FAILED SCHAR_MIN not defined\n";
#endif /* SCHAR_MIN */
#ifndef UCHAR_MAX
    ++fail_count;
    std::cout << "FAILED UCHAR_MAX not defined\n";
#endif /* UCHAR_MAX */
#ifndef SHRT_MAX
    ++fail_count;
    std::cout << "FAILED SHRT_MAX not defined\n";
#endif /* SHRT_MAX */
#ifndef SHRT_MIN
    ++fail_count;
    std::cout << "FAILED SHRT_MIN not defined\n";
#endif /* SHRT_MIN */
#ifndef USHRT_MAX
    ++fail_count;
    std::cout << "FAILED USHRT_MAX not defined\n";
#endif /* USHRT_MAX */
#ifndef INT_MAX
    ++fail_count;
    std::cout << "FAILED INT_MAX not defined\n";
#endif /* INT_MAX */
#ifndef INT_MIN
    ++fail_count;
    std::cout << "FAILED INT_MIN not defined\n";
#endif /* INT_MIN */
#ifndef UINT_MAX
    ++fail_count;
    std::cout << "FAILED UINT_MAX not defined\n";
#endif /* UINT_MAX */
#ifndef LONG_MAX
    ++fail_count;
    std::cout << "FAILED LONG_MAX not defined\n";
#endif /* LONG_MAX */
#ifndef LONG_MIN
    ++fail_count;
    std::cout << "FAILED LONG_MIN not defined\n";
#endif /* LONG_MIN */
#ifndef ULONG_MAX
    ++fail_count;
    std::cout << "FAILED ULONG_MAX not defined\n";
#endif /* ULONG_MAX */
#ifndef LLONG_MAX
    ++fail_count;
    std::cout << "FAILED LLONG_MAX not defined\n";
#endif /* LLONG_MAX */
#ifndef LLONG_MIN
    ++fail_count;
    std::cout << "FAILED LLONG_MIN not defined\n";
#endif /* LLONG_MIN */
#ifndef ULLONG_MAX
    ++fail_count;
    std::cout << "FAILED ULLONG_MAX not defined\n";
#endif /* ULLONG_MAX */
#ifndef __FLT_HAS_INFINITY__
    ++fail_count;
    std::cout << "FAILED __FLT_HAS_INFINITY__ not defined\n";
#endif /* __FLT_HAS_INFINITY__ */
#ifndef __FLT_HAS_QUIET_NAN__
    ++fail_count;
    std::cout << "FAILED __FLT_HAS_QUIET_NAN__ not defined\n";
#endif /* __FLT_HAS_INFINITY__ */
#ifndef FLT_MANT_DIG
    ++fail_count;
    std::cout << "FAILED FLT_MANT_DIG not defined\n";
#endif /* FLT_MANT_DIG */
#ifndef FLT_DIG
    ++fail_count;
    std::cout << "FAILED FLT_DIG not defined\n";
#endif /* FLT_DIG */
#ifndef __FLT_DECIMAL_DIG__
    ++fail_count;
    std::cout << "FAILED __FLT_DECIMAL_DIG__ not defined\n";
#endif /* __FLT_DECIMAL_DIG__ */
#ifndef FLT_RADIX
    ++fail_count;
    std::cout << "FAILED FLT_RADIX not defined\n";
#endif /* FLT_RADIX */
#ifndef FLT_MIN_EXP
    ++fail_count;
    std::cout << "FAILED FLT_MIN_EXP not defined\n";
#endif /* FLT_MIN_EXP */
#ifndef FLT_MIN_10_EXP
    ++fail_count;
    std::cout << "FAILED FLT_MIN_10_EXP not defined\n";
#endif /* FLT_MIN_10_EXP */
#ifndef FLT_MAX_EXP
    ++fail_count;
    std::cout << "FAILED FLT_MAX_EXP not defined\n";
#endif /* FLT_MAX_EXP */
#ifndef FLT_MAX_10_EXP
    ++fail_count;
    std::cout << "FAILED FLT_MAX_10_EXP not defined\n";
#endif /* FLT_MAX_10_EXP */
#ifndef FLT_MIN
    ++fail_count;
    std::cout << "FAILED FLT_MIN not defined\n";
#endif /* FLT_MIN */
#ifndef FLT_MAX
    ++fail_count;
    std::cout << "FAILED FLT_MAX not defined\n";
#endif /* FLT_MAX */
#ifndef FLT_EPSILON
    ++fail_count;
    std::cout << "FAILED FLT_EPSILON not defined\n";
#endif /* FLT_EPSILON */
#ifndef HUGE_VALF
    ++fail_count;
    std::cout << "FAILED HUGE_VALF not defined\n";
#endif /* HUGE_VALF */
#ifndef NAN
    ++fail_count;
    std::cout << "FAILED NAN not defined\n";
#endif /* NAN */
#ifndef __FLT_DENORM_MIN__
    ++fail_count;
    std::cout << "FAILED __FLT_DENORM_MIN__ not defined\n";
#endif /* __FLT_DENORM_MIN__ */
#ifndef __DBL_HAS_INFINITY__
    ++fail_count;
    std::cout << "FAILED __DBL_HAS_INFINITY__ not defined\n";
#endif /* __DBL_HAS_INFINITY__ */
#ifndef __DBL_HAS_QUIET_NAN__
    ++fail_count;
    std::cout << "FAILED __DBL_HAS_QUIET_NAN__ not defined\n";
#endif /* __DBL_HAS_INFINITY__ */
#ifndef DBL_MANT_DIG
    ++fail_count;
    std::cout << "FAILED DBL_MANT_DIG not defined\n";
#endif /* DBL_MANT_DIG */
#ifndef DBL_DIG
    ++fail_count;
    std::cout << "FAILED DBL_DIG not defined\n";
#endif /* DBL_DIG */
#ifndef __DBL_DECIMAL_DIG__
    ++fail_count;
    std::cout << "FAILED __DBL_DECIMAL_DIG__ not defined\n";
#endif /* __DBL_DECIMAL_DIG__ */
#ifndef DBL_MIN_EXP
    ++fail_count;
    std::cout << "FAILED DBL_MIN_EXP not defined\n";
#endif /* DBL_MIN_EXP */
#ifndef DBL_MIN_10_EXP
    ++fail_count;
    std::cout << "FAILED DBL_MIN_10_EXP not defined\n";
#endif /* DBL_MIN_10_EXP */
#ifndef DBL_MAX_EXP
    ++fail_count;
    std::cout << "FAILED DBL_MAX_EXP not defined\n";
#endif /* DBL_MAX_EXP */
#ifndef DBL_MAX_10_EXP
    ++fail_count;
    std::cout << "FAILED DBL_MAX_10_EXP not defined\n";
#endif /* DBL_MAX_10_EXP */
#ifndef DBL_MIN
    ++fail_count;
    std::cout << "FAILED DBL_MIN not defined\n";
#endif /* DBL_MIN */
#ifndef DBL_MAX
    ++fail_count;
    std::cout << "FAILED DBL_MAX not defined\n";
#endif /* DBL_MAX */
#ifndef DBL_EPSILON
    ++fail_count;
    std::cout << "FAILED DBL_EPSILON not defined\n";
#endif /* DBL_EPSILON */
#ifndef HUGE_VAL
    ++fail_count;
    std::cout << "FAILED HUGE_VAL not defined\n";
#endif /* HUGE_VAL */
#ifndef __DBL_DENORM_MIN__
    ++fail_count;
    std::cout << "FAILED __DBL_DENORM_MIN__ not defined\n";
#endif /* __DBL_DENORM_MIN__ */
#ifndef __LDBL_HAS_INFINITY__
    ++fail_count;
    std::cout << "FAILED __LDBL_HAS_INFINITY__ not defined\n";
#endif /* __LDBL_HAS_INFINITY__ */
#ifndef __LDBL_HAS_QUIET_NAN__
    ++fail_count;
    std::cout << "FAILED __LDBL_HAS_QUIET_NAN__ not defined\n";
#endif /* __LDBL_HAS_INFINITY__ */
#ifndef LDBL_MANT_DIG
    ++fail_count;
    std::cout << "FAILED LDBL_MANT_DIG not defined\n";
#endif /* LDBL_MANT_DIG */
#ifndef LDBL_DIG
    ++fail_count;
    std::cout << "FAILED LDBL_DIG not defined\n";
#endif /* LDBL_DIG */
#ifndef __LDBL_DECIMAL_DIG__
    ++fail_count;
    std::cout << "FAILED __LDBL_DECIMAL_DIG__ not defined\n";
#endif /* __LDBL_DECIMAL_DIG__ */
#ifndef LDBL_MIN_EXP
    ++fail_count;
    std::cout << "FAILED LDBL_MIN_EXP not defined\n";
#endif /* LDBL_MIN_EXP */
#ifndef LDBL_MIN_10_EXP
    ++fail_count;
    std::cout << "FAILED LDBL_MIN_10_EXP not defined\n";
#endif /* LDBL_MIN_10_EXP */
#ifndef LDBL_MAX_EXP
    ++fail_count;
    std::cout << "FAILED LDBL_MAX_EXP not defined\n";
#endif /* LDBL_MAX_EXP */
#ifndef LDBL_MAX_10_EXP
    ++fail_count;
    std::cout << "FAILED LDBL_MAX_10_EXP not defined\n";
#endif /* LDBL_MAX_10_EXP */
#ifndef LDBL_MIN
    ++fail_count;
    std::cout << "FAILED LDBL_MIN not defined\n";
#endif /* LDBL_MIN */
#ifndef LDBL_MAX
    ++fail_count;
    std::cout << "FAILED LDBL_MAX not defined\n";
#endif /* LDBL_MAX */
#ifndef LDBL_EPSILON
    ++fail_count;
    std::cout << "FAILED LDBL_EPSILON not defined\n";
#endif /* LDBL_EPSILON */
#ifndef HUGE_VALL
    ++fail_count;
    std::cout << "FAILED HUGE_VALL not defined\n";
#endif /* HUGE_VALL */
#ifndef __LDBL_DENORM_MIN__
    ++fail_count;
    std::cout << "FAILED __LDBL_DENORM_MIN__ not defined\n";
#endif /* __LDBL_DENORM_MIN__ */

    // Generic
    if (numeric_limits<dummy_type>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::is_specialized\n";
    }
    if (numeric_limits<dummy_type>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::is_signed\n";
    }
    if (numeric_limits<dummy_type>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::is_integer\n";
    }
    if (numeric_limits<dummy_type>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::is_exact\n";
    }
    if (numeric_limits<dummy_type>::has_infinity)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::has_infinity\n";
    }
    if (numeric_limits<dummy_type>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::has_quiet_NaN\n";
    }
    if (numeric_limits<dummy_type>::has_signaling_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::has_signaling_NaN\n";
    }
    if (numeric_limits<dummy_type>::has_denorm != denorm_absent)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::has_denorm\n";
    }
    if (numeric_limits<dummy_type>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::has_denorm_loss\n";
    }
    if (numeric_limits<dummy_type>::round_style != round_toward_zero)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::round_style\n";
    }
    if (numeric_limits<dummy_type>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::is_iec559\n";
    }
    if (numeric_limits<dummy_type>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::is_bounded\n";
    }
    if (numeric_limits<dummy_type>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::is_modulo\n";
    }
    if (numeric_limits<dummy_type>::digits != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::digits\n";
    }
    if (numeric_limits<dummy_type>::digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::digits10\n";
    }
    if (numeric_limits<dummy_type>::max_digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::max_digits10\n";
    }
    if (numeric_limits<dummy_type>::radix != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::radix\n";
    }
    if (numeric_limits<dummy_type>::min_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::min_exponent\n";
    }
    if (numeric_limits<dummy_type>::min_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::min_exponent10\n";
    }
    if (numeric_limits<dummy_type>::max_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::max_exponent\n";
    }
    if (numeric_limits<dummy_type>::max_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::max_exponent10\n";
    }
    if (numeric_limits<dummy_type>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::traps\n";
    }
    if (numeric_limits<dummy_type>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::tinyness_before\n";
    }
    if (numeric_limits<dummy_type>::min() != dummy_type{})
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::min()\n";
    }
    if (numeric_limits<dummy_type>::lowest() != dummy_type{})
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::lowest()\n";
    }
    if (numeric_limits<dummy_type>::max() != dummy_type{})
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::max()\n";
    }
    if (numeric_limits<dummy_type>::epsilon() != dummy_type{})
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::epsilon()\n";
    }
    if (numeric_limits<dummy_type>::round_error() != dummy_type{})
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::round_error()\n";
    }
    if (numeric_limits<dummy_type>::infinity() != dummy_type{})
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::infinity()\n";
    }
    if (numeric_limits<dummy_type>::quiet_NaN() != dummy_type{})
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::quiet_NaN()\n";
    }
    if (numeric_limits<dummy_type>::signaling_NaN() != dummy_type{})
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::signaling_NaN()\n";
    }
    if (numeric_limits<dummy_type>::denorm_min() != dummy_type{})
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<dummy_type>::denorm_min()\n";
    }
    // char
    if (!numeric_limits<char>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::is_specialized\n";
    }
#ifdef __CHAR_UNSIGNED__
    if (numeric_limits<char>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::is_signed\n";
    }
#else /* __CHAR_UNSIGNED__ */
    if (!numeric_limits<char>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::is_signed\n";
    }
#endif /* __CHAR_UNSIGNED__ */
    if (!numeric_limits<char>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::is_integer\n";
    }
    if (!numeric_limits<char>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::is_exact\n";
    }
    if (numeric_limits<char>::has_infinity)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::has_infinity\n";
    }
    if (numeric_limits<char>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::has_quiet_NaN\n";
    }
    if (numeric_limits<char>::has_signaling_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::has_signaling_NaN\n";
    }
    if (numeric_limits<char>::has_denorm != denorm_absent)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::has_denorm\n";
    }
    if (numeric_limits<char>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::has_denorm_loss\n";
    }
    if (numeric_limits<char>::round_style != round_toward_zero)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::round_style\n";
    }
    if (numeric_limits<char>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::is_iec559\n";
    }
    if (!numeric_limits<char>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::is_bounded\n";
    }
    if (numeric_limits<char>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::is_modulo\n";
    }
#ifdef __CHAR_UNSIGNED__
    if (numeric_limits<char>::digits != CHAR_BIT*sizeof(char))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::digits\n";
    }
#else /* __CHAR_UNSIGNED__ */
    if (numeric_limits<char>::digits != CHAR_BIT*sizeof(char) - 1)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::digits\n";
    }
#endif /* __CHAR_UNSIGNED__ */
    if (numeric_limits<char>::digits10 !=
        static_cast<char>(numeric_limits<char>::digits * 0.3010299957))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::digits10\n";
    }
    if (numeric_limits<char>::max_digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::max_digits10\n";
    }
    if (numeric_limits<char>::radix != 2)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::radix\n";
    }
    if (numeric_limits<char>::min_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::min_exponent\n";
    }
    if (numeric_limits<char>::min_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::min_exponent10\n";
    }
    if (numeric_limits<char>::max_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::max_exponent\n";
    }
    if (numeric_limits<char>::max_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::max_exponent10\n";
    }
    if (!numeric_limits<char>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::traps\n";
    }
    if (numeric_limits<char>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::tinyness_before\n";
    }
    if (numeric_limits<char>::min() != CHAR_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::min()\n";
    }
    if (numeric_limits<char>::lowest() != CHAR_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::lowest()\n";
    }
    if (numeric_limits<char>::max() != CHAR_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::max()\n";
    }
    if (numeric_limits<char>::epsilon() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::epsilon()\n";
    }
    if (numeric_limits<char>::round_error() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::round_error()\n";
    }
    if (numeric_limits<char>::infinity() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::infinity()\n";
    }
    if (numeric_limits<char>::quiet_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::quiet_NaN()\n";
    }
    if (numeric_limits<char>::signaling_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::signaling_NaN()\n";
    }
    if (numeric_limits<char>::denorm_min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<char>::denorm_min()\n";
    }
    // signed char
    if (!numeric_limits<signed char>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::is_specialized\n";
    }
    if (!numeric_limits<signed char>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::is_signed\n";
    }
    if (!numeric_limits<signed char>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::is_integer\n";
    }
    if (!numeric_limits<signed char>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::is_exact\n";
    }
    if (numeric_limits<signed char>::has_infinity)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::has_infinity\n";
    }
    if (numeric_limits<signed char>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::has_quiet_NaN\n";
    }
    if (numeric_limits<signed char>::has_signaling_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::has_signaling_NaN\n";
    }
    if (numeric_limits<signed char>::has_denorm != denorm_absent)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::has_denorm\n";
    }
    if (numeric_limits<signed char>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::has_denorm_loss\n";
    }
    if (numeric_limits<signed char>::round_style != round_toward_zero)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::round_style\n";
    }
    if (numeric_limits<signed char>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::is_iec559\n";
    }
    if (!numeric_limits<signed char>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::is_bounded\n";
    }
    if (numeric_limits<signed char>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::is_modulo\n";
    }
    if (numeric_limits<signed char>::digits != CHAR_BIT*sizeof(char) - 1)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::digits\n";
    }
    if (numeric_limits<signed char>::digits10 != static_cast<signed char>(
        numeric_limits<signed char>::digits * 0.3010299957))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::digits10\n";
    }
    if (numeric_limits<signed char>::max_digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::max_digits10\n";
    }
    if (numeric_limits<signed char>::radix != 2)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::radix\n";
    }
    if (numeric_limits<signed char>::min_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::min_exponent\n";
    }
    if (numeric_limits<signed char>::min_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::min_exponent10\n";
    }
    if (numeric_limits<signed char>::max_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::max_exponent\n";
    }
    if (numeric_limits<signed char>::max_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::max_exponent10\n";
    }
    if (!numeric_limits<signed char>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::traps\n";
    }
    if (numeric_limits<signed char>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::tinyness_before\n";
    }
    if (numeric_limits<signed char>::min() != SCHAR_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::min()\n";
    }
    if (numeric_limits<signed char>::lowest() != SCHAR_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::lowest()\n";
    }
    if (numeric_limits<signed char>::max() != SCHAR_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::max()\n";
    }
    if (numeric_limits<signed char>::epsilon() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::epsilon()\n";
    }
    if (numeric_limits<signed char>::round_error() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::round_error()\n";
    }
    if (numeric_limits<signed char>::infinity() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::infinity()\n";
    }
    if (numeric_limits<signed char>::quiet_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::quiet_NaN()\n";
    }
    if (numeric_limits<signed char>::signaling_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::signaling_NaN()\n";
    }
    if (numeric_limits<signed char>::denorm_min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<signed char>::denorm_min()\n";
    }
    // unsigned char
    if (!numeric_limits<unsigned char>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::is_specialized\n";
    }
    if (numeric_limits<unsigned char>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::is_signed\n";
    }
    if (!numeric_limits<unsigned char>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::is_integer\n";
    }
    if (!numeric_limits<unsigned char>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::is_exact\n";
    }
    if (numeric_limits<unsigned char>::has_infinity)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::has_infinity\n";
    }
    if (numeric_limits<unsigned char>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::has_quiet_NaN\n";
    }
    if (numeric_limits<unsigned char>::has_signaling_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::has_signaling_NaN\n";
    }
    if (numeric_limits<unsigned char>::has_denorm != denorm_absent)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::has_denorm\n";
    }
    if (numeric_limits<unsigned char>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::has_denorm_loss\n";
    }
    if (numeric_limits<unsigned char>::round_style != round_toward_zero)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::round_style\n";
    }
    if (numeric_limits<unsigned char>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::is_iec559\n";
    }
    if (!numeric_limits<unsigned char>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::is_bounded\n";
    }
    if (!numeric_limits<unsigned char>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::is_modulo\n";
    }
    if (numeric_limits<unsigned char>::digits != CHAR_BIT*sizeof(char))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::digits\n";
    }
    if (numeric_limits<unsigned char>::digits10 !=
        static_cast<unsigned char>(
        numeric_limits<unsigned char>::digits * 0.3010299957))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::digits10\n";
    }
    if (numeric_limits<unsigned char>::max_digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::max_digits10\n";
    }
    if (numeric_limits<unsigned char>::radix != 2)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::radix\n";
    }
    if (numeric_limits<unsigned char>::min_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::min_exponent\n";
    }
    if (numeric_limits<unsigned char>::min_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::min_exponent10\n";
    }
    if (numeric_limits<unsigned char>::max_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::max_exponent\n";
    }
    if (numeric_limits<unsigned char>::max_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::max_exponent10\n";
    }
    if (!numeric_limits<unsigned char>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::traps\n";
    }
    if (numeric_limits<unsigned char>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::tinyness_before\n";
    }
    if (numeric_limits<unsigned char>::min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::min()\n";
    }
    if (numeric_limits<unsigned char>::lowest() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::lowest()\n";
    }
    if (numeric_limits<unsigned char>::max() != UCHAR_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::max()\n";
    }
    if (numeric_limits<unsigned char>::epsilon() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::epsilon()\n";
    }
    if (numeric_limits<unsigned char>::round_error() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::round_error()\n";
    }
    if (numeric_limits<unsigned char>::infinity() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::infinity()\n";
    }
    if (numeric_limits<unsigned char>::quiet_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::quiet_NaN()\n";
    }
    if (numeric_limits<unsigned char>::signaling_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::signaling_NaN()\n";
    }
    if (numeric_limits<unsigned char>::denorm_min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned char>::denorm_min()\n";
    }
    // short
    if (!numeric_limits<short>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::is_specialized\n";
    }
    if (!numeric_limits<short>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::is_signed\n";
    }
    if (!numeric_limits<short>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::is_integer\n";
    }
    if (!numeric_limits<short>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::is_exact\n";
    }
    if (numeric_limits<short>::has_infinity)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::has_infinity\n";
    }
    if (numeric_limits<short>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::has_quiet_NaN\n";
    }
    if (numeric_limits<short>::has_signaling_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::has_signaling_NaN\n";
    }
    if (numeric_limits<short>::has_denorm != denorm_absent)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::has_denorm\n";
    }
    if (numeric_limits<short>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::has_denorm_loss\n";
    }
    if (numeric_limits<short>::round_style != round_toward_zero)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::round_style\n";
    }
    if (numeric_limits<short>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::is_iec559\n";
    }
    if (!numeric_limits<short>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::is_bounded\n";
    }
    if (numeric_limits<short>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::is_modulo\n";
    }
    if (numeric_limits<short>::digits != CHAR_BIT*sizeof(short) - 1)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::digits\n";
    }
    if (numeric_limits<short>::digits10 !=
        static_cast<short>(numeric_limits<short>::digits * 0.3010299957))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::digits10\n";
    }
    if (numeric_limits<short>::max_digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::max_digits10\n";
    }
    if (numeric_limits<short>::radix != 2)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::radix\n";
    }
    if (numeric_limits<short>::min_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::min_exponent\n";
    }
    if (numeric_limits<short>::min_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::min_exponent10\n";
    }
    if (numeric_limits<short>::max_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::max_exponent\n";
    }
    if (numeric_limits<short>::max_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::max_exponent10\n";
    }
    if (!numeric_limits<short>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::traps\n";
    }
    if (numeric_limits<short>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::tinyness_before\n";
    }
    if (numeric_limits<short>::min() != SHRT_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::min()\n";
    }
    if (numeric_limits<short>::lowest() != SHRT_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::lowest()\n";
    }
    if (numeric_limits<short>::max() != SHRT_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::max()\n";
    }
    if (numeric_limits<short>::epsilon() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::epsilon()\n";
    }
    if (numeric_limits<short>::round_error() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::round_error()\n";
    }
    if (numeric_limits<short>::infinity() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::infinity()\n";
    }
    if (numeric_limits<short>::quiet_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::quiet_NaN()\n";
    }
    if (numeric_limits<short>::signaling_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::signaling_NaN()\n";
    }
    if (numeric_limits<short>::denorm_min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<short>::denorm_min()\n";
    }
    // unsigned short
    if (!numeric_limits<unsigned short>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::is_specialized\n";
    }
    if (numeric_limits<unsigned short>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::is_signed\n";
    }
    if (!numeric_limits<unsigned short>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::is_integer\n";
    }
    if (!numeric_limits<unsigned short>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::is_exact\n";
    }
    if (numeric_limits<unsigned short>::has_infinity)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::has_infinity\n";
    }
    if (numeric_limits<unsigned short>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::has_quiet_NaN\n";
    }
    if (numeric_limits<unsigned short>::has_signaling_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::has_signaling_NaN\n";
    }
    if (numeric_limits<unsigned short>::has_denorm != denorm_absent)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::has_denorm\n";
    }
    if (numeric_limits<unsigned short>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::has_denorm_loss\n";
    }
    if (numeric_limits<unsigned short>::round_style != round_toward_zero)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::round_style\n";
    }
    if (numeric_limits<unsigned short>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::is_iec559\n";
    }
    if (!numeric_limits<unsigned short>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::is_bounded\n";
    }
    if (!numeric_limits<unsigned short>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::is_modulo\n";
    }
    if (numeric_limits<unsigned short>::digits != CHAR_BIT*sizeof(short))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::digits\n";
    }
    if (numeric_limits<unsigned short>::digits10 !=
        static_cast<unsigned short>(
        numeric_limits<unsigned short>::digits * 0.3010299957))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::digits10\n";
    }
    if (numeric_limits<unsigned short>::max_digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::max_digits10\n";
    }
    if (numeric_limits<unsigned short>::radix != 2)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::radix\n";
    }
    if (numeric_limits<unsigned short>::min_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::min_exponent\n";
    }
    if (numeric_limits<unsigned short>::min_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::min_exponent10\n";
    }
    if (numeric_limits<unsigned short>::max_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::max_exponent\n";
    }
    if (numeric_limits<unsigned short>::max_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::max_exponent10\n";
    }
    if (!numeric_limits<unsigned short>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::traps\n";
    }
    if (numeric_limits<unsigned short>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::tinyness_before\n";
    }
    if (numeric_limits<unsigned short>::min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::min()\n";
    }
    if (numeric_limits<unsigned short>::lowest() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::lowest()\n";
    }
    if (numeric_limits<unsigned short>::max() != USHRT_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::max()\n";
    }
    if (numeric_limits<unsigned short>::epsilon() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::epsilon()\n";
    }
    if (numeric_limits<unsigned short>::round_error() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::round_error()\n";
    }
    if (numeric_limits<unsigned short>::infinity() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::infinity()\n";
    }
    if (numeric_limits<unsigned short>::quiet_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::quiet_NaN()\n";
    }
    if (numeric_limits<unsigned short>::signaling_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::signaling_NaN()\n";
    }
    if (numeric_limits<unsigned short>::denorm_min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned short>::denorm_min()\n";
    }
    // int
    if (!numeric_limits<int>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::is_specialized\n";
    }
    if (!numeric_limits<int>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::is_signed\n";
    }
    if (!numeric_limits<int>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::is_integer\n";
    }
    if (!numeric_limits<int>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::is_exact\n";
    }
    if (numeric_limits<int>::has_infinity)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::has_infinity\n";
    }
    if (numeric_limits<int>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::has_quiet_NaN\n";
    }
    if (numeric_limits<int>::has_signaling_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::has_signaling_NaN\n";
    }
    if (numeric_limits<int>::has_denorm != denorm_absent)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::has_denorm\n";
    }
    if (numeric_limits<int>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::has_denorm_loss\n";
    }
    if (numeric_limits<int>::round_style != round_toward_zero)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::round_style\n";
    }
    if (numeric_limits<int>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::is_iec559\n";
    }
    if (!numeric_limits<int>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::is_bounded\n";
    }
    if (numeric_limits<int>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::is_modulo\n";
    }
    if (numeric_limits<int>::digits != CHAR_BIT*sizeof(int) - 1)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::digits\n";
    }
    if (numeric_limits<int>::digits10 !=
        static_cast<int>(numeric_limits<int>::digits * 0.3010299957))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::digits10\n";
    }
    if (numeric_limits<int>::max_digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::max_digits10\n";
    }
    if (numeric_limits<int>::radix != 2)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::radix\n";
    }
    if (numeric_limits<int>::min_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::min_exponent\n";
    }
    if (numeric_limits<int>::min_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::min_exponent10\n";
    }
    if (numeric_limits<int>::max_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::max_exponent\n";
    }
    if (numeric_limits<int>::max_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::max_exponent10\n";
    }
    if (!numeric_limits<int>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::traps\n";
    }
    if (numeric_limits<int>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::tinyness_before\n";
    }
    if (numeric_limits<int>::min() != INT_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::min()\n";
    }
    if (numeric_limits<int>::lowest() != INT_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::lowest()\n";
    }
    if (numeric_limits<int>::max() != INT_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::max()\n";
    }
    if (numeric_limits<int>::epsilon() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::epsilon()\n";
    }
    if (numeric_limits<int>::round_error() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::round_error()\n";
    }
    if (numeric_limits<int>::infinity() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::infinity()\n";
    }
    if (numeric_limits<int>::quiet_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::quiet_NaN()\n";
    }
    if (numeric_limits<int>::signaling_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::signaling_NaN()\n";
    }
    if (numeric_limits<int>::denorm_min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<int>::denorm_min()\n";
    }
    // unsigned int
    if (!numeric_limits<unsigned int>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::is_specialized\n";
    }
    if (numeric_limits<unsigned int>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::is_signed\n";
    }
    if (!numeric_limits<unsigned int>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::is_integer\n";
    }
    if (!numeric_limits<unsigned int>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::is_exact\n";
    }
    if (numeric_limits<unsigned int>::has_infinity)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::has_infinity\n";
    }
    if (numeric_limits<unsigned int>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::has_quiet_NaN\n";
    }
    if (numeric_limits<unsigned int>::has_signaling_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::has_signaling_NaN\n";
    }
    if (numeric_limits<unsigned int>::has_denorm != denorm_absent)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::has_denorm\n";
    }
    if (numeric_limits<unsigned int>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::has_denorm_loss\n";
    }
    if (numeric_limits<unsigned int>::round_style != round_toward_zero)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::round_style\n";
    }
    if (numeric_limits<unsigned int>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::is_iec559\n";
    }
    if (!numeric_limits<unsigned int>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::is_bounded\n";
    }
    if (!numeric_limits<unsigned int>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::is_modulo\n";
    }
    if (numeric_limits<unsigned int>::digits != CHAR_BIT*sizeof(int))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::digits\n";
    }
    if (numeric_limits<unsigned int>::digits10 !=
        static_cast<unsigned int>(
        numeric_limits<unsigned int>::digits * 0.3010299957))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::digits10\n";
    }
    if (numeric_limits<unsigned int>::max_digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::max_digits10\n";
    }
    if (numeric_limits<unsigned int>::radix != 2)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::radix\n";
    }
    if (numeric_limits<unsigned int>::min_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::min_exponent\n";
    }
    if (numeric_limits<unsigned int>::min_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::min_exponent10\n";
    }
    if (numeric_limits<unsigned int>::max_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::max_exponent\n";
    }
    if (numeric_limits<unsigned int>::max_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::max_exponent10\n";
    }
    if (!numeric_limits<unsigned int>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::traps\n";
    }
    if (numeric_limits<unsigned int>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::tinyness_before\n";
    }
    if (numeric_limits<unsigned int>::min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::min()\n";
    }
    if (numeric_limits<unsigned int>::lowest() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::lowest()\n";
    }
    if (numeric_limits<unsigned int>::max() != UINT_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::max()\n";
    }
    if (numeric_limits<unsigned int>::epsilon() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::epsilon()\n";
    }
    if (numeric_limits<unsigned int>::round_error() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::round_error()\n";
    }
    if (numeric_limits<unsigned int>::infinity() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::infinity()\n";
    }
    if (numeric_limits<unsigned int>::quiet_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::quiet_NaN()\n";
    }
    if (numeric_limits<unsigned int>::signaling_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::signaling_NaN()\n";
    }
    if (numeric_limits<unsigned int>::denorm_min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned int>::denorm_min()\n";
    }
    // long
    if (!numeric_limits<long>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::is_specialized\n";
    }
    if (!numeric_limits<long>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::is_signed\n";
    }
    if (!numeric_limits<long>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::is_integer\n";
    }
    if (!numeric_limits<long>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::is_exact\n";
    }
    if (numeric_limits<long>::has_infinity)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::has_infinity\n";
    }
    if (numeric_limits<long>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::has_quiet_NaN\n";
    }
    if (numeric_limits<long>::has_signaling_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::has_signaling_NaN\n";
    }
    if (numeric_limits<long>::has_denorm != denorm_absent)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::has_denorm\n";
    }
    if (numeric_limits<long>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::has_denorm_loss\n";
    }
    if (numeric_limits<long>::round_style != round_toward_zero)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::round_style\n";
    }
    if (numeric_limits<long>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::is_iec559\n";
    }
    if (!numeric_limits<long>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::is_bounded\n";
    }
    if (numeric_limits<long>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::is_modulo\n";
    }
    if (numeric_limits<long>::digits != CHAR_BIT*sizeof(long) - 1)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::digits\n";
    }
    if (numeric_limits<long>::digits10 !=
        static_cast<long>(numeric_limits<long>::digits * 0.3010299957))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::digits10\n";
    }
    if (numeric_limits<long>::max_digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::max_digits10\n";
    }
    if (numeric_limits<long>::radix != 2)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::radix\n";
    }
    if (numeric_limits<long>::min_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::min_exponent\n";
    }
    if (numeric_limits<long>::min_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::min_exponent10\n";
    }
    if (numeric_limits<long>::max_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::max_exponent\n";
    }
    if (numeric_limits<long>::max_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::max_exponent10\n";
    }
    if (!numeric_limits<long>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::traps\n";
    }
    if (numeric_limits<long>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::tinyness_before\n";
    }
    if (numeric_limits<long>::min() != LONG_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::min()\n";
    }
    if (numeric_limits<long>::lowest() != LONG_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::lowest()\n";
    }
    if (numeric_limits<long>::max() != LONG_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::max()\n";
    }
    if (numeric_limits<long>::epsilon() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::epsilon()\n";
    }
    if (numeric_limits<long>::round_error() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::round_error()\n";
    }
    if (numeric_limits<long>::infinity() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::infinity()\n";
    }
    if (numeric_limits<long>::quiet_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::quiet_NaN()\n";
    }
    if (numeric_limits<long>::signaling_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::signaling_NaN()\n";
    }
    if (numeric_limits<long>::denorm_min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long>::denorm_min()\n";
    }
    // unsigned long
    if (!numeric_limits<unsigned long>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::is_specialized\n";
    }
    if (numeric_limits<unsigned long>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::is_signed\n";
    }
    if (!numeric_limits<unsigned long>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::is_integer\n";
    }
    if (!numeric_limits<unsigned long>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::is_exact\n";
    }
    if (numeric_limits<unsigned long>::has_infinity)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::has_infinity\n";
    }
    if (numeric_limits<unsigned long>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::has_quiet_NaN\n";
    }
    if (numeric_limits<unsigned long>::has_signaling_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::has_signaling_NaN\n";
    }
    if (numeric_limits<unsigned long>::has_denorm != denorm_absent)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::has_denorm\n";
    }
    if (numeric_limits<unsigned long>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::has_denorm_loss\n";
    }
    if (numeric_limits<unsigned long>::round_style != round_toward_zero)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::round_style\n";
    }
    if (numeric_limits<unsigned long>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::is_iec559\n";
    }
    if (!numeric_limits<unsigned long>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::is_bounded\n";
    }
    if (!numeric_limits<unsigned long>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::is_modulo\n";
    }
    if (numeric_limits<unsigned long>::digits != CHAR_BIT*sizeof(long))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::digits\n";
    }
    if (numeric_limits<unsigned long>::digits10 !=
        static_cast<unsigned long>(
        numeric_limits<unsigned long>::digits * 0.3010299957))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::digits10\n";
    }
    if (numeric_limits<unsigned long>::max_digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::max_digits10\n";
    }
    if (numeric_limits<unsigned long>::radix != 2)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::radix\n";
    }
    if (numeric_limits<unsigned long>::min_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::min_exponent\n";
    }
    if (numeric_limits<unsigned long>::min_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::min_exponent10\n";
    }
    if (numeric_limits<unsigned long>::max_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::max_exponent\n";
    }
    if (numeric_limits<unsigned long>::max_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::max_exponent10\n";
    }
    if (!numeric_limits<unsigned long>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::traps\n";
    }
    if (numeric_limits<unsigned long>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::tinyness_before\n";
    }
    if (numeric_limits<unsigned long>::min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::min()\n";
    }
    if (numeric_limits<unsigned long>::lowest() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::lowest()\n";
    }
    if (numeric_limits<unsigned long>::max() != ULONG_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::max()\n";
    }
    if (numeric_limits<unsigned long>::epsilon() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::epsilon()\n";
    }
    if (numeric_limits<unsigned long>::round_error() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::round_error()\n";
    }
    if (numeric_limits<unsigned long>::infinity() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::infinity()\n";
    }
    if (numeric_limits<unsigned long>::quiet_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::quiet_NaN()\n";
    }
    if (numeric_limits<unsigned long>::signaling_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::signaling_NaN()\n";
    }
    if (numeric_limits<unsigned long>::denorm_min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long>::denorm_min()\n";
    }
    // long long
    if (!numeric_limits<long long>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::is_specialized\n";
    }
    if (!numeric_limits<long long>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::is_signed\n";
    }
    if (!numeric_limits<long long>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::is_integer\n";
    }
    if (!numeric_limits<long long>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::is_exact\n";
    }
    if (numeric_limits<long long>::has_infinity)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::has_infinity\n";
    }
    if (numeric_limits<long long>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::has_quiet_NaN\n";
    }
    if (numeric_limits<long long>::has_signaling_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::has_signaling_NaN\n";
    }
    if (numeric_limits<long long>::has_denorm != denorm_absent)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::has_denorm\n";
    }
    if (numeric_limits<long long>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::has_denorm_loss\n";
    }
    if (numeric_limits<long long>::round_style != round_toward_zero)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::round_style\n";
    }
    if (numeric_limits<long long>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::is_iec559\n";
    }
    if (!numeric_limits<long long>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::is_bounded\n";
    }
    if (numeric_limits<long long>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::is_modulo\n";
    }
    if (numeric_limits<long long>::digits != CHAR_BIT*sizeof(long long) - 1)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::digits\n";
    }
    if (numeric_limits<long long>::digits10 !=
        static_cast<long long>(numeric_limits<long long>::digits * 0.3010299957))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::digits10\n";
    }
    if (numeric_limits<long long>::max_digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::max_digits10\n";
    }
    if (numeric_limits<long long>::radix != 2)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::radix\n";
    }
    if (numeric_limits<long long>::min_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::min_exponent\n";
    }
    if (numeric_limits<long long>::min_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::min_exponent10\n";
    }
    if (numeric_limits<long long>::max_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::max_exponent\n";
    }
    if (numeric_limits<long long>::max_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::max_exponent10\n";
    }
    if (!numeric_limits<long long>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::traps\n";
    }
    if (numeric_limits<long long>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::tinyness_before\n";
    }
    if (numeric_limits<long long>::min() != LLONG_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::min()\n";
    }
    if (numeric_limits<long long>::lowest() != LLONG_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::lowest()\n";
    }
    if (numeric_limits<long long>::max() != LLONG_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::max()\n";
    }
    if (numeric_limits<long long>::epsilon() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::epsilon()\n";
    }
    if (numeric_limits<long long>::round_error() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::round_error()\n";
    }
    if (numeric_limits<long long>::infinity() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::infinity()\n";
    }
    if (numeric_limits<long long>::quiet_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::quiet_NaN()\n";
    }
    if (numeric_limits<long long>::signaling_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::signaling_NaN()\n";
    }
    if (numeric_limits<long long>::denorm_min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long long>::denorm_min()\n";
    }
    // unsigned long long
    if (!numeric_limits<unsigned long long>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::is_specialized\n";
    }
    if (numeric_limits<unsigned long long>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::is_signed\n";
    }
    if (!numeric_limits<unsigned long long>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::is_integer\n";
    }
    if (!numeric_limits<unsigned long long>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::is_exact\n";
    }
    if (numeric_limits<unsigned long long>::has_infinity)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::has_infinity\n";
    }
    if (numeric_limits<unsigned long long>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::has_quiet_NaN\n";
    }
    if (numeric_limits<unsigned long long>::has_signaling_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::has_signaling_NaN\n";
    }
    if (numeric_limits<unsigned long long>::has_denorm != denorm_absent)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::has_denorm\n";
    }
    if (numeric_limits<unsigned long long>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::has_denorm_loss\n";
    }
    if (numeric_limits<unsigned long long>::round_style != round_toward_zero)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::round_style\n";
    }
    if (numeric_limits<unsigned long long>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::is_iec559\n";
    }
    if (!numeric_limits<unsigned long long>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::is_bounded\n";
    }
    if (!numeric_limits<unsigned long long>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::is_modulo\n";
    }
    if (numeric_limits<unsigned long long>::digits != CHAR_BIT*sizeof(long long))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::digits\n";
    }
    if (numeric_limits<unsigned long long>::digits10 !=
        static_cast<unsigned long long>(
        numeric_limits<unsigned long long>::digits * 0.3010299957))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::digits10\n";
    }
    if (numeric_limits<unsigned long long>::max_digits10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::max_digits10\n";
    }
    if (numeric_limits<unsigned long long>::radix != 2)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::radix\n";
    }
    if (numeric_limits<unsigned long long>::min_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::min_exponent\n";
    }
    if (numeric_limits<unsigned long long>::min_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::min_exponent10\n";
    }
    if (numeric_limits<unsigned long long>::max_exponent != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::max_exponent\n";
    }
    if (numeric_limits<unsigned long long>::max_exponent10 != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::max_exponent10\n";
    }
    if (!numeric_limits<unsigned long long>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::traps\n";
    }
    if (numeric_limits<unsigned long long>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::tinyness_before\n";
    }
    if (numeric_limits<unsigned long long>::min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::min()\n";
    }
    if (numeric_limits<unsigned long long>::lowest() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::lowest()\n";
    }
    if (numeric_limits<unsigned long long>::max() != ULLONG_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::max()\n";
    }
    if (numeric_limits<unsigned long long>::epsilon() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::epsilon()\n";
    }
    if (numeric_limits<unsigned long long>::round_error() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::round_error()\n";
    }
    if (numeric_limits<unsigned long long>::infinity() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::infinity()\n";
    }
    if (numeric_limits<unsigned long long>::quiet_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::quiet_NaN()\n";
    }
    if (numeric_limits<unsigned long long>::signaling_NaN() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::signaling_NaN()\n";
    }
    if (numeric_limits<unsigned long long>::denorm_min() != 0)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<unsigned long long>::denorm_min()\n";
    }
    // float
    if (!numeric_limits<float>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::is_specialized\n";
    }
    if (!numeric_limits<float>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::is_signed\n";
    }
    if (numeric_limits<float>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::is_integer\n";
    }
    if (numeric_limits<float>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::is_exact\n";
    }
    if (numeric_limits<float>::has_infinity != __FLT_HAS_INFINITY__)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::has_infinity\n";
    }
    if (numeric_limits<float>::has_quiet_NaN != __FLT_HAS_QUIET_NAN__)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::has_quiet_NaN\n";
    }
    if (numeric_limits<float>::has_signaling_NaN != 
        numeric_limits<float>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::has_signaling_NaN\n";
    }
    if (numeric_limits<float>::has_denorm != denorm_present)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::has_denorm\n";
    }
    if (numeric_limits<float>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::has_denorm_loss\n";
    }
    if (numeric_limits<float>::round_style != round_to_nearest)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::round_style\n";
    }
    if (!numeric_limits<float>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::is_iec559\n";
    }
    if (!numeric_limits<float>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::is_bounded\n";
    }
    if (numeric_limits<float>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::is_modulo\n";
    }
    if (numeric_limits<float>::digits != FLT_MANT_DIG)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::digits\n";
    }
    if (numeric_limits<float>::digits10 != FLT_DIG)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::digits10\n";
    }
    if (numeric_limits<float>::max_digits10 != __FLT_DECIMAL_DIG__)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::max_digits10\n";
    }
    if (numeric_limits<float>::radix != FLT_RADIX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::radix\n";
    }
    if (numeric_limits<float>::min_exponent != FLT_MIN_EXP)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::min_exponent\n";
    }
    if (numeric_limits<float>::min_exponent10 != FLT_MIN_10_EXP)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::min_exponent10\n";
    }
    if (numeric_limits<float>::max_exponent != FLT_MAX_EXP)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::max_exponent\n";
    }
    if (numeric_limits<float>::max_exponent10 != FLT_MAX_10_EXP)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::max_exponent10\n";
    }
    if (numeric_limits<float>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::traps\n";
    }
    if (numeric_limits<float>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::tinyness_before\n";
    }
    if (numeric_limits<float>::min() != FLT_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::min()\n";
    }
    if (numeric_limits<float>::lowest() != -FLT_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::lowest()\n";
    }
    if (numeric_limits<float>::max() != FLT_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::max()\n";
    }
    if (numeric_limits<float>::epsilon() != FLT_EPSILON)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::epsilon()\n";
    }
    if (numeric_limits<float>::round_error() != 0.5f)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::round_error()\n";
    }
    if (numeric_limits<float>::infinity() != HUGE_VALF)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::infinity()\n";
    }
    if (!isnan(numeric_limits<float>::quiet_NaN()))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::quiet_NaN()\n";
    }
    if (!isnan(numeric_limits<float>::signaling_NaN()))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::signaling_NaN()\n";
    }
    if (numeric_limits<float>::denorm_min() != __FLT_DENORM_MIN__)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<float>::denorm_min()\n";
    }
    // double
    if (!numeric_limits<double>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::is_specialized\n";
    }
    if (!numeric_limits<double>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::is_signed\n";
    }
    if (numeric_limits<double>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::is_integer\n";
    }
    if (numeric_limits<double>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::is_exact\n";
    }
    if (numeric_limits<double>::has_infinity != __DBL_HAS_INFINITY__)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::has_infinity\n";
    }
    if (numeric_limits<double>::has_quiet_NaN != __DBL_HAS_QUIET_NAN__)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::has_quiet_NaN\n";
    }
    if (numeric_limits<double>::has_signaling_NaN != 
        numeric_limits<double>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::has_signaling_NaN\n";
    }
    if (numeric_limits<double>::has_denorm != denorm_present)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::has_denorm\n";
    }
    if (numeric_limits<double>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::has_denorm_loss\n";
    }
    if (numeric_limits<double>::round_style != round_to_nearest)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::round_style\n";
    }
    if (!numeric_limits<double>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::is_iec559\n";
    }
    if (!numeric_limits<double>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::is_bounded\n";
    }
    if (numeric_limits<double>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::is_modulo\n";
    }
    if (numeric_limits<double>::digits != DBL_MANT_DIG)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::digits\n";
    }
    if (numeric_limits<double>::digits10 != DBL_DIG)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::digits10\n";
    }
    if (numeric_limits<double>::max_digits10 != __DBL_DECIMAL_DIG__)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::max_digits10\n";
    }
    if (numeric_limits<double>::radix != FLT_RADIX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::radix\n";
    }
    if (numeric_limits<double>::min_exponent != DBL_MIN_EXP)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::min_exponent\n";
    }
    if (numeric_limits<double>::min_exponent10 != DBL_MIN_10_EXP)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::min_exponent10\n";
    }
    if (numeric_limits<double>::max_exponent != DBL_MAX_EXP)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::max_exponent\n";
    }
    if (numeric_limits<double>::max_exponent10 != DBL_MAX_10_EXP)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::max_exponent10\n";
    }
    if (numeric_limits<double>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::traps\n";
    }
    if (numeric_limits<double>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::tinyness_before\n";
    }
    if (numeric_limits<double>::min() != DBL_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::min()\n";
    }
    if (numeric_limits<double>::lowest() != -DBL_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::lowest()\n";
    }
    if (numeric_limits<double>::max() != DBL_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::max()\n";
    }
    if (numeric_limits<double>::epsilon() != DBL_EPSILON)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::epsilon()\n";
    }
    if (numeric_limits<double>::round_error() != 0.5f)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::round_error()\n";
    }
    if (numeric_limits<double>::infinity() != HUGE_VAL)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::infinity()\n";
    }
    if (!isnan(numeric_limits<double>::quiet_NaN()))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::quiet_NaN()\n";
    }
    if (!isnan(numeric_limits<double>::signaling_NaN()))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::signaling_NaN()\n";
    }
    if (numeric_limits<double>::denorm_min() != __DBL_DENORM_MIN__)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<double>::denorm_min()\n";
    }
    // long double
    if (!numeric_limits<long double>::is_specialized)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::is_specialized\n";
    }
    if (!numeric_limits<long double>::is_signed)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::is_signed\n";
    }
    if (numeric_limits<long double>::is_integer)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::is_integer\n";
    }
    if (numeric_limits<long double>::is_exact)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::is_exact\n";
    }
    if (numeric_limits<long double>::has_infinity != __LDBL_HAS_INFINITY__)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::has_infinity\n";
    }
    if (numeric_limits<long double>::has_quiet_NaN != __LDBL_HAS_QUIET_NAN__)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::has_quiet_NaN\n";
    }
    if (numeric_limits<long double>::has_signaling_NaN != 
        numeric_limits<long double>::has_quiet_NaN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::has_signaling_NaN\n";
    }
    if (numeric_limits<long double>::has_denorm != denorm_present)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::has_denorm\n";
    }
    if (numeric_limits<long double>::has_denorm_loss)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::has_denorm_loss\n";
    }
    if (numeric_limits<long double>::round_style != round_to_nearest)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::round_style\n";
    }
    if (!numeric_limits<long double>::is_iec559)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::is_iec559\n";
    }
    if (!numeric_limits<long double>::is_bounded)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::is_bounded\n";
    }
    if (numeric_limits<long double>::is_modulo)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::is_modulo\n";
    }
    if (numeric_limits<long double>::digits != LDBL_MANT_DIG)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::digits\n";
    }
    if (numeric_limits<long double>::digits10 != LDBL_DIG)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::digits10\n";
    }
    if (numeric_limits<long double>::max_digits10 != __LDBL_DECIMAL_DIG__)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::max_digits10\n";
    }
    if (numeric_limits<long double>::radix != FLT_RADIX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::radix\n";
    }
    if (numeric_limits<long double>::min_exponent != LDBL_MIN_EXP)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::min_exponent\n";
    }
    if (numeric_limits<long double>::min_exponent10 != LDBL_MIN_10_EXP)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::min_exponent10\n";
    }
    if (numeric_limits<long double>::max_exponent != LDBL_MAX_EXP)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::max_exponent\n";
    }
    if (numeric_limits<long double>::max_exponent10 != LDBL_MAX_10_EXP)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::max_exponent10\n";
    }
    if (numeric_limits<long double>::traps)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::traps\n";
    }
    if (numeric_limits<long double>::tinyness_before)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::tinyness_before\n";
    }
    if (numeric_limits<long double>::min() != LDBL_MIN)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::min()\n";
    }
    if (numeric_limits<long double>::lowest() != -LDBL_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::lowest()\n";
    }
    if (numeric_limits<long double>::max() != LDBL_MAX)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::max()\n";
    }
    if (numeric_limits<long double>::epsilon() != LDBL_EPSILON)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::epsilon()\n";
    }
    if (numeric_limits<long double>::round_error() != 0.5f)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::round_error()\n";
    }
    if (numeric_limits<long double>::infinity() != HUGE_VALL)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::infinity()\n";
    }
    if (!isnan(numeric_limits<long double>::quiet_NaN()))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::quiet_NaN()\n";
    }
    if (!isnan(numeric_limits<long double>::signaling_NaN()))
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::signaling_NaN()\n";
    }
    if (numeric_limits<long double>::denorm_min() != __LDBL_DENORM_MIN__)
    {
        ++fail_count;
        std::cout << "FAILED numeric_limits<long double>::denorm_min()\n";
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
