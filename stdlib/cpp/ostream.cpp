#include "../include/ostream"

#include <stdint.h>

#include "../include/algorithm"
#include "../include/cctype"
#include "../include/exception"
#include "../include/streambuf"
#include "../include/string"

namespace NMSP {

namespace helper {

/******************************************************************************
 ******************************************************************************/

static string signed_conversion(long long val, ios_base::fmtflags flags,
    size_t width, char fill)
{
    // Set the radix.
    unsigned int radix;
    ios_base::fmtflags fmt = flags & ios_base::basefield;
    if (fmt == ios_base::hex)
        radix = 16;
    else if (fmt == ios_base::oct)
        radix = 8;
    else
        radix = 10;

    // Get the number.
    string res = itostr(val, radix);

    // Add a leading '+' if necessary.
    if ((flags & ios_base::showpos) != 0 && val >= 0)
        res.insert(res.begin(), '+');

    // No need to show the base. hex or oct implies unsigned conversion. 

    // The conversion is upper case. Convert to lower case if necessary.
    if ((flags & ios_base::uppercase) == 0)
        transform(res.begin(), res.end(), res.begin(), tolower);

    // Check if we need any padding.
    if (res.size() < width)
    {
        ios_base::fmtflags adjust = flags & ios_base::adjustfield;
        string::iterator pos = res.begin();
        if (adjust == ios_base::left)
            pos = res.end();
        else if (adjust == ios_base::internal)
        {
            // Get after leading sign.
            if (*pos == '-' || *pos == '+')
                ++pos;
            // Get after leading 0 for hex or oct.
            if (*pos == '0')
            {
                ++pos;
                // Get after '0x' for hex.
                if (*pos == 'x' || *pos == 'X')
                    ++pos;
            }
        }

        // Now add the padding.
        for (; res.size() < width; pos = res.insert(pos, fill)) ;
    }

    // Return the completed string.
    return res;
}

static string unsigned_conversion(unsigned long long val,
    ios_base::fmtflags flags, size_t width, char fill)
{
    // Set the radix.
    unsigned int radix;
    ios_base::fmtflags fmt = flags & ios_base::basefield;
    if (fmt == ios_base::hex)
        radix = 16;
    else if (fmt == ios_base::oct)
        radix = 8;
    else
        radix = 10;

    // Get the number.
    string res = uitostr(val, radix);

    // showpos has no effect for unsigned conversions.

    // Show the base, if necessary.
    if (fmt == ios_base::hex && (flags & ios_base::showbase) != 0
        && val != 0)
        res.insert(0, "0X");
    else if (fmt == ios_base::oct &&
        (flags & ios_base::showbase) != 0 && val != 0)
        res.insert(res.begin(), '0');

    // The conversion is upper case. Convert to lower case if necessary.
    if ((flags & ios_base::uppercase) == 0)
        transform(res.begin(), res.end(), res.begin(), tolower);

    // Check if we need any padding.
    if (res.size() < width)
    {
        ios_base::fmtflags adjust = flags & ios_base::adjustfield;
        string::iterator pos = res.begin();
        if (adjust == ios_base::left)
            pos = res.end();
        else if (adjust == ios_base::internal)
        {
            // Get after '0x' for hex.
            if (*pos == '0' && (*(pos+1) == 'x' || *(pos+1) == 'X'))
                pos += 2;
        }

        // Now add the padding.
        for (; res.size() < width; pos = res.insert(pos, fill)) ;
    }

    // Return the completed string.
    return res;
}

static string float_conversion(long double val,
    ios_base::fmtflags flags, size_t width, char fill, streamsize prec)
{
    // TODO hexfloat is not implemented.
    char style;
    ios_base::fmtflags fmt = flags & ios_base::floatfield;
    if (fmt == (ios_base::scientific | ios_base::fixed))
        return "Not implemented";
    else if (fmt == ios_base::scientific)
        style = 'e';
    else if (fmt == ios_base::fixed)
        style = 'f';
    else
        style = 'g';

    // Get the number.
    string res = dtostr(val, style);

    // Manipulations for the default style.
    if (style == 'g')
    {
        // Count the number of digits.
        int count = 0;
        bool dec_point = false;
        int count_after_dec = 0;
        auto it = res.begin();
        for ( ; it != res.end() && *it != 'E'; ++it)
        {
            if (isdigit(*it))
            {
                ++count;
                if (dec_point)
                    ++count_after_dec;
            }
            else if (*it == '.')
                dec_point = true;
        }

        // Cut some digits if we need to, but only if there is a decimal point.
        if (count_after_dec > 0 && count > prec)
            it = res.erase(it - min(count - prec,
                static_cast<streamsize>(count_after_dec)), it);

        // Delete any trailing zeroes after the decimal point.
        if (dec_point && (flags & ios_base::showpoint) == 0)
        {
            while (*(--it) == '0')
                it = res.erase(it);
            // Delete a trailing ., if it's not required.
            if (*it == '.' && (flags & ios_base::showpoint) == 0)
            {
                dec_point = false;
                it = res.erase(it);
            }
        }

        // Add in a trailing '.' if necessary.
        if ((flags & ios_base::showpoint) != 0)
        {
            // Insert a .
            if (res.find('.') == string::npos)
                res.insert(res.end(), '.');

            // Add trailing zeroes up to the precision.
            if (count < prec)
                res.insert(res.end(), prec - count, '0');
        }
    }
    // Manipulations for the fixed style.
    else if (style == 'f')
    {
        // In this style, the precision indicates exactly the number of digits
        // after the decimal point.

        // Find the location of the decimal point.
        auto pos = res.find('.');
        streamoff count = 0;
        // Insert if there isn't one.
        if (pos == string::npos && (prec > 0 ||
            (flags & ios_base::showpoint) != 0))
            res.push_back('.');
        else
            count = res.size() - pos - 1;

        // Delete if there are too many.
        if (count > prec)
            res.erase(res.end() - count + prec, res.end());
        // Insert if there are too few.
        else if (count < prec)
            res.insert(res.end(), prec - count, '0');
    }
    // Manipulations for the scientific style.
    else if (style == 'e')
    {
        // In this style, the precision indicates exactly the number of digits
        // after the decimal point.

        // Find the location of the decimal point ands the E.
        auto dpos = res.find('.');
        auto epos = res.begin() + res.find('E');
        streamoff count = 0;
        // Insert if there isn't one.
        if (dpos == string::npos && (prec > 0 ||
            (flags & ios_base::showpoint) != 0))
            epos = res.insert(epos, '.') + 1;
        else
            count = (epos - res.begin()) - dpos - 1;

        // Delete if there are too many.
        if (count > prec)
            epos = res.erase(epos - count + prec, epos);
        // Insert if there are too few.
        else if (count < prec)
            epos = res.insert(epos, prec - count, '0') + (prec - count);

        // The exponent should always be at least two characters and have a
        // sign.
        ++epos;
        if (*epos != '+' && *epos != '-')
            epos = res.insert(epos, '+');
        ++epos;
        count = res.end() - epos;
        if (count < 2)
            res.insert(epos, 2 - count, '0');
    }

    // Add a leading '+' if necessary.
    if ((flags & ios_base::showpos) != 0 && val >= 0)
        res.insert(res.begin(), '+');

    // The conversion is upper case. Convert to lower case if necessary.
    if ((flags & ios_base::uppercase) == 0)
        transform(res.begin(), res.end(), res.begin(), tolower);

    // Check if we need any padding.
    if (res.size() < width)
    {
        ios_base::fmtflags adjust = flags & ios_base::adjustfield;
        string::iterator pos = res.begin();
        if (adjust == ios_base::left)
            pos = res.end();
        else if (adjust == ios_base::internal)
        {
            // Get after leading sign.
            if (*pos == '-' || *pos == '+')
                ++pos;
        }

        // Now add the padding.
        for (; res.size() < width; pos = res.insert(pos, fill)) ;
    }

    // Return the completed string.
    return res;
}

/******************************************************************************
 ******************************************************************************/

void pad_char(string& rep, const basic_ostream<char, char_traits<char>>& os)
{
    // Check if we need any padding.
    if (rep.size() < static_cast<string::size_type>(os.width()))
    {
        ios_base::fmtflags adjust = os.flags() & ios_base::adjustfield;
        string::iterator pos = rep.begin();
        if (adjust == ios_base::left)
            pos = rep.end();

        // Now add the padding.
        for (; rep.size() < static_cast<string::size_type>(os.width());
            pos = rep.insert(pos, os.fill())) ;
    }
}

/******************************************************************************
 ******************************************************************************/

void do_output(const string& output,
    basic_ostream<char, char_traits<char>>& ostr)
{
    // Reset the width.
    ostr.width(0);

    // Do output.
    try {
        for (char c : output)
        {
            int e = ostr.rdbuf()->sputc(c);
            if (e == char_traits<char>::eof())
                throw ios_base::failure {"EOF on stream write."};
        }
    }
    catch (exception& e)
    {
        ostr.setstate(ios_base::badbit);
        if ((ostr.exceptions() & ios_base::badbit) != 0)
            throw e;
    }
    catch (...)
    {
        ostr.setstate(ios_base::badbit);
    }
}

/******************************************************************************
 ******************************************************************************/

bool do_insert(short val, basic_ostream<char, char_traits<char>>& ostr)
{
    // Constuct sentry
    basic_ostream<char, char_traits<char>>::sentry sen {ostr};

    // Carry on if the sentry is fine
    if (sen)
    {
        // The value is cast to long, or unsigned long for hex or octal, then
        // converted to a string.
        string string_val;
        ios_base::fmtflags fmt = ostr.flags() & ios_base::basefield;
        if (fmt == ios_base::hex || fmt == ios_base::oct)
        {
            unsigned long cast_val = static_cast<unsigned short>(val);
            string_val = unsigned_conversion(cast_val, ostr.flags(),
                ostr.width(), ostr.fill());
        }
        else
        {
            string_val = signed_conversion(val, ostr.flags(),
                ostr.width(), ostr.fill());
        } 

        // Do the actual output.
        helper::do_output(string_val, ostr);

        // If we've got here without throwing, it's all good.
        return true;
    }

    // If we got here, the sentry construction failed.
    return false;
}

bool do_insert(int val, basic_ostream<char, char_traits<char>>& ostr)
{
    // Constuct sentry
    basic_ostream<char, char_traits<char>>::sentry sen {ostr};

    // Carry on if the sentry is fine
    if (sen)
    {
        // The value is cast to long, or unsigned long for hex or octal, then
        // converted to a string.
        string string_val;
        ios_base::fmtflags fmt = ostr.flags() & ios_base::basefield;
        if (fmt == ios_base::hex || fmt == ios_base::oct)
        {
            unsigned long cast_val = static_cast<unsigned int>(val);
            string_val = unsigned_conversion(cast_val, ostr.flags(),
                ostr.width(), ostr.fill());
        }
        else
        {
            string_val = signed_conversion(val, ostr.flags(),
                ostr.width(), ostr.fill());
        }

        // Do the actual output.
        helper::do_output(string_val, ostr);

        // If we've got here without throwing, it's all good.
        return true;
    }

    // If we got here, the sentry construction failed.
    return false;
}

bool do_insert(long val, basic_ostream<char, char_traits<char>>& ostr)
{
    // Constuct sentry
    basic_ostream<char, char_traits<char>>::sentry sen {ostr};

    // Carry on if the sentry is fine
    if (sen)
    {
        // The value is cast unsigned long for hex or octal, then
        // converted to a string.
        string string_val;
        ios_base::fmtflags fmt = ostr.flags() & ios_base::basefield;
        if (fmt == ios_base::hex || fmt == ios_base::oct)
        {
            unsigned long cast_val = static_cast<unsigned long>(val);
            string_val = unsigned_conversion(cast_val, ostr.flags(),
                ostr.width(), ostr.fill());
        }
        else
        {
            string_val = signed_conversion(val, ostr.flags(),
                ostr.width(), ostr.fill());
        }

        // Do the actual output.
        helper::do_output(string_val, ostr);

        // If we've got here without throwing, it's all good.
        return true;
    }

    // If we got here, the sentry construction failed.
    return false;
}

bool do_insert(long long val, basic_ostream<char, char_traits<char>>& ostr)
{
    // Constuct sentry
    basic_ostream<char, char_traits<char>>::sentry sen {ostr};

    // Carry on if the sentry is fine
    if (sen)
    {
        // The value is cast unsigned long long for hex or octal, then
        // converted to a string.
        string string_val;
        ios_base::fmtflags fmt = ostr.flags() & ios_base::basefield;
        if (fmt == ios_base::hex || fmt == ios_base::oct)
        {
            unsigned long long cast_val = static_cast<unsigned long long>(val);
            string_val = unsigned_conversion(cast_val, ostr.flags(),
                ostr.width(), ostr.fill());
        }
        else
        {
            string_val = signed_conversion(val, ostr.flags(),
                ostr.width(), ostr.fill());
        }

        // Do the actual output.
        helper::do_output(string_val, ostr);

        // If we've got here without throwing, it's all good.
        return true;
    }

    // If we got here, the sentry construction failed.
    return false;
}

bool do_insert(unsigned long long val,
    basic_ostream<char, char_traits<char>>& ostr)
{
    // Constuct sentry
    basic_ostream<char, char_traits<char>>::sentry sen {ostr};

    // Carry on if the sentry is fine
    if (sen)
    {
        // Get the string representation.
        string string_val = unsigned_conversion(val, ostr.flags(), ostr.width(),
            ostr.fill());

        // Do the actual output.
        helper::do_output(string_val, ostr);

        // If we've got here without throwing, it's all good.
        return true;
    }

    // If we got here, the sentry construction failed.
    return false;
}

bool do_insert(long double val, basic_ostream<char, char_traits<char>>& ostr)
{
    // Constuct sentry
    basic_ostream<char, char_traits<char>>::sentry sen {ostr};

    // Carry on if the sentry is fine
    if (sen)
    {
        // Get the string representation.
        string string_val = float_conversion(val, ostr.flags(), ostr.width(),
            ostr.fill(), ostr.precision());

        // Do the actual output.
        helper::do_output(string_val, ostr);

        // If we've got here without throwing, it's all good.
        return true;
    }

    // If we got here, the sentry construction failed.
    return false;
}

bool do_insert(bool val, basic_ostream<char, char_traits<char>>& ostr)
{
    // If not boolalpha, converts to an int and calls the int version instead.
    if ((ostr.flags() & ios_base::boolalpha) == 0)
        return do_insert(static_cast<int>(val), ostr);

    // Constuct sentry
    basic_ostream<char, char_traits<char>>::sentry sen {ostr};

    // Carry on if the sentry is fine
    if (sen)
    {
        // Get the string representation.
        string res = (val ? "true" : "false");

        // Check if we need any padding.
        if (res.size() < static_cast<string::size_type>(ostr.width()))
        {
            ios_base::fmtflags adjust = ostr.flags() & ios_base::adjustfield;
            string::iterator pos = res.begin();
            if (adjust == ios_base::left)
                pos = res.end();

            // Now add the padding.
            for (; res.size() < static_cast<string::size_type>(ostr.width());
                pos = res.insert(pos, ostr.fill())) ;
        }

        // Do the actual output.
        helper::do_output(res, ostr);

        // If we've got here without throwing, it's all good.
        return true;
    }

    // If we got here, the sentry construction failed.
    return false;
}

bool do_insert(basic_streambuf<char, char_traits<char>>* val,
    basic_ostream<char, char_traits<char>>& ostr)
{
    // Constuct sentry
    basic_ostream<char, char_traits<char>>::sentry sen {ostr};

    // Carry on if the sentry is fine
    if (sen && val != nullptr)
    {
        // Whether we have managed to do at least one character.
        bool done_one = false;

        // Read characters in from the sb and output them to the stream.
        try {
            while (true)
            {
                int c = val->sbumpc();
                if (c == char_traits<char>::eof())
                    break;

                int e = ostr.rdbuf()->sputc(c);
                if (e == char_traits<char>::eof())
                {
                    val->sputbackc(c);
                    throw ios_base::failure
                        {"Output from input stream failed to output"};
                }

                done_one = true;
            }
        }
        catch (...)
        {
            ostr.setstate(ios_base::failbit);
            if ((ostr.exceptions() & ios_base::failbit) != 0)
                throw;
        }

        if (!done_one)
        {
            ostr.setstate(ios_base::failbit);
            return false;
        }

        // If we've got here without throwing, it's all good.
        return true;
    }

    // If we got here, the sentry construction failed.
    return false;
}

bool do_insert(const void* val, basic_ostream<char, char_traits<char>>& ostr)
{
    // Constuct sentry
    basic_ostream<char, char_traits<char>>::sentry sen {ostr};

    // Carry on if the sentry is fine
    if (sen)
    {
        string res;
        // Check for nullptr.
        if (val == nullptr)
            res = '0';
        else
        {
            // Get the string representation, in hex.
            res = uitostr(reinterpret_cast<uintptr_t>(val), 16);

            // Add the hex base indicator.
            res.insert(0, "0X");

            // The conversion is upper case. Convert to lower case if necessary.
            if ((ostr.flags() & ios_base::uppercase) == 0)
                transform(res.begin(), res.end(), res.begin(), tolower);
        }

        // Check if we need any padding.
        if (res.size() < static_cast<size_t>(ostr.width()))
        {
            ios_base::fmtflags adjust = ostr.flags() & ios_base::adjustfield;
            string::iterator pos = res.begin();
            if (adjust == ios_base::left)
                pos = res.end();
            else if (adjust == ios_base::internal)
            {
                // Get after the leading 0x
                if (res[0] == '0' && (res[1] == 'x' || res[1] == 'X'))
                    pos += 2;
            }

            // Now add the padding.
            for (; res.size() < static_cast<size_t>(ostr.width());
                pos = res.insert(pos, ostr.fill())) ;
        }

        // Do the actual output.
        helper::do_output(res, ostr);

        // If we've got here without throwing, it's all good.
        return true;
    }

    // If we got here, the sentry construction failed.
    return false;
}

template <typename CharT, typename Traits>
basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os,
    CharT ch)
{
    // Constuct sentry
    typename basic_ostream<CharT, Traits>::sentry sen {os};

    // Carry on if the sentry is fine
    if (sen)
    {
        // Make a string representation.
        string res {ch};

        // Pad the string.
        pad_char(res, os);

        // Do the actual output.
        helper::do_output(res, os);
    }

    return os;
}

/******************************************************************************
 ******************************************************************************/

} // end helper namespace

/******************************************************************************
 ******************************************************************************/

} // end NMSP namespace
