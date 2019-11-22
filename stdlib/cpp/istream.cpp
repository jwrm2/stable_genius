#include "../include/istream"

#include "../include/cctype"
#include "../include/cstdlib"
#include "../include/ios"
#include "../include/streambuf"
#include "../include/string"

namespace NMSP {

namespace helper {

/******************************************************************************
 ******************************************************************************/

static string read_int(istream& istr, ios_base::fmtflags flags)
{
    // Extract the base field.
    ios_base::fmtflags base = (flags & ios_base::basefield);

    string res;
    bool start = true;
    int in_0x = 0;
    while (true)
    {
        // Get a character.
        istream::traits_type::int_type c = istr.get();

        // Check for eof.
        if (istream::traits_type::eq_int_type(c, istream::traits_type::eof()))
        {
            try {
                istr.clear();
                istr.setstate(ios_base::eofbit);
            }
            catch (...) {}
            break;
        }

        // Leading whitespace will have been gobbled, if necessary, by the
        // senty.
        // Leading + or -
        if ((c == '+' || c == '-') && start)
        {
            start = false;
            res += c;
        }
        // First character of 0x
        else if (base == ios_base::hex && in_0x == 0 && c == '0')
        {
            start = false;
            in_0x = 1;
            res += c;
        }
        // Second character of 0x
        else if (base == ios_base::hex && in_0x == 1 && (c == 'x' || c == 'X'))
        {
            in_0x = 2;
            res += c;
        }
        // Normal digit
        else if ((base == ios_base::oct && isdigit(c) && c != '8' && c != '9')||
            (base == ios_base::dec && isdigit(c)) ||
            (base == ios_base::hex && isxdigit(c)))
        {
            in_0x = -1;
            start = false;
            res += c;
        }
        // Invalid
        else
        {
            istr.unget();
            break;
        }
    }

    return res;
}

/******************************************************************************
 ******************************************************************************/

static string read_float(istream& istr)
{
    // Note hex is not implemented. It also seems that NAN and INF/INFINITY are
    // not allowed by gcc.
    string res;
    bool start = true;
    bool decimal = false;
    bool exp = false;
    bool got_digit = false;
    while (istream::traits_type::int_type c = istr.get())
    {
        // Check for eof.
        if (istream::traits_type::eq_int_type(c, istream::traits_type::eof()))
        {
            try {
                istr.clear();
                istr.setstate(ios_base::eofbit);
            }
            catch (...) {}
            break;
        }

        // Leading whitespace will have been gobbled, if necessary, by the
        // senty.
        // Leading + or -
        if ((c == '+' || c == '-') && start)
        {
            start = false;
            got_digit = false;
            res += c;
        }
        // Normal digit
        else if (isdigit(c))
        {
            start = false;
            got_digit = true;
            res += c;
        }
        // Decimal point
        else if (c == '.' && !decimal && !exp)
        {
            start = false;
            decimal = true;
            res += c;
        }
        // Start of exponent.
        else if ((c == 'e' || c == 'E') && !exp && got_digit)
        {
            start = true;
            got_digit = false;
            exp = true;
            decimal = true;
            res += c;
        }
        // Invalid
        else
        {
            istr.unget();
            break;
        }
    }
    return res;
}

/******************************************************************************
 ******************************************************************************/

bool do_read(long long& val, istream& istr)
{
    bool success = false;
    val= 0;

    // Constuct sentry
    istream::sentry sen {istr};

    // Carry on if the sentry is fine
    if (sen)
    {
        // Read digits into a string.
        string res = read_int(istr, istr.flags());

        // Work out the base.
        int base = 10;
        if ((istr.flags() & ios_base::oct) != 0)
            base = 8;
        else if ((istr.flags() & ios_base::hex) != 0)
            base = 16;

        // Now we have a string to pass for conversion.
        char* end;
        val = strtoll(res.c_str(), &end, base);

        // Check whether that worked.
        if (val == 0 && end == res.c_str())
        {
            val = 0;
            try {
               istr.setstate(ios_base::failbit);
            }
            catch (...) {}
        }
        else
            success = true;
    
        // Allow exceptions for fail and eof to be thrown.
        if (istr.fail() && (istr.exceptions() & ios_base::failbit) != 0)
            throw ios_base::failure("Failure in istream signed read");
        if (istr.eof() && (istr.exceptions() & ios_base::eofbit) != 0)
            throw ios_base::failure("EoF in istream signed read");
    }

    return success;
}

bool do_read(unsigned long long& val,
    basic_istream<char, char_traits<char>>& istr, bool& neg)
{
    bool success = false;
    val = 0;
    neg = false;

    // Constuct sentry
    istream::sentry sen {istr};

    // Carry on if the sentry is fine
    if (sen)
    {
        // Read digits into a string.
        string res = read_int(istr, istr.flags());

        // Check for negative values.
        if (res[0] == '-')
            neg = true;

        // Work out the base.
        int base = 10;
        if ((istr.flags() & ios_base::oct) != 0)
            base = 8;
        else if ((istr.flags() & ios_base::hex) != 0)
            base = 16;

        // Now we have a string to pass for conversion.
        char* end;
        val = strtoull(res.c_str(), &end, base);

        // Check whether that worked.
        if (val == 0 && end == res.c_str())
        {
            val = 0;
            try {
               istr.setstate(ios_base::failbit);
            }
            catch (...) {}
        }
        else
            success = true;
    
        // Allow exceptions for fail and eof to be thrown.
        if (istr.fail() && (istr.exceptions() & ios_base::failbit) != 0)
            throw ios_base::failure("Failure in istream unsigned read");
        if (istr.eof() && (istr.exceptions() & ios_base::eofbit) != 0)
            throw ios_base::failure("EoF in istream unsigned read");
    }

    return success;
}

bool do_read(long double& val, basic_istream<char, char_traits<char>>& istr)
{
    bool success = false;
    val = 0;

    // Constuct sentry
    istream::sentry sen {istr};

    // Carry on if the sentry is fine
    if (sen)
    {
        // Read digits into a string.
        string res = read_float(istr);

        // Now we have a string to pass for conversion.
        char* end;
        val = strtold(res.c_str(), &end);

        // Check whether that worked.
        if (val == 0 && end == res.c_str())
        {
            val = 0;
            try {
               istr.setstate(ios_base::failbit);
            }
            catch (...) {}
        }
        else
            success = true;
    
        // Allow exceptions for fail and eof to be thrown.
        if (istr.fail() && (istr.exceptions() & ios_base::failbit) != 0)
            throw ios_base::failure("Failure in istream float read");
        if (istr.eof() && (istr.exceptions() & ios_base::eofbit) != 0)
            throw ios_base::failure("EoF in istream float read");
    }

    return success;
}

bool do_read(bool& val, basic_istream<char, char_traits<char>>& istr)
{
    bool success = false;

    // Constuct sentry
    istream::sentry sen {istr};

    // Carry on if the sentry is fine
    if (sen)
    {
        if ((istr.flags() & ios_base::boolalpha) == 0)
        {
            // Read digits into a string.
            string res = read_int(istr, istr.flags());

            // Now we have a string to pass for conversion.
            char* end;
            long long v = strtoll(res.c_str(), &end, 0);

            // Check whether that worked.
            if (v == 0 && end == res.c_str())
            {
                success = false;
                val = false;
                try {
                    istr.setstate(ios_base::failbit);
                }
                catch (...) {}
            }
            else
            {
                if (v == 0)
                {
                    success = true;
                    val = false;
                }
                else if (v == 1)
                {
                    success = true;
                    val = true;
                }
                else
                {
                    success = false;
                    val = true;
                    try {
                        istr.setstate(ios_base::failbit);
                    }
                    catch (...) {}
                }
            }
        }
        else
        {
            int index = 0;
            bool in_true = true;
            bool in_false = true;
            while (istream::traits_type::int_type c = istr.get())
            {
                // Check for eof.
                if (istream::traits_type::eq_int_type(c,
                    istream::traits_type::eof()))
                {
                    try {
                        istr.setstate(ios_base::eofbit);
                    }
                    catch (...) {}
                    break;
                }

                c = tolower(c);

                if (in_true && ((index == 0 && c == 't') ||
                    (index == 1 && c == 'r') || (index == 2 && c == 'u') ||
                    (index == 3 && c == 'e')))
                {
                    ++index;
                    in_false = false;
                    if (index == 4)
                        break;
                }
                else if (in_false && ((index == 0 && c == 'f') ||
                    (index == 1 && c == 'a') || (index == 2 && c == 'l') ||
                    (index == 3 && c == 's') || (index == 4 && c == 'e')))
                {
                    ++index;
                    in_true = false;
                    if (index == 5)
                        break;
                }
                else
                {
                    istr.unget();
                    break;
                }
            }

            if (index == 4 && in_true)
            {
                val = true;
                success = true;
            }
            else if (index == 5 && in_false)
            {
                val = false;
                success = true;
            }
            else
            {
                success = false;
                val = false;
                try {
                    istr.setstate(ios_base::failbit);
                }
                catch (...) {}
            }
        }
    
        // Allow exceptions for fail and eof to be thrown.
        if (istr.fail() && (istr.exceptions() & ios_base::failbit) != 0)
            throw ios_base::failure("Failure in istream bool read");
        if (istr.eof() && (istr.exceptions() & ios_base::eofbit) != 0)
            throw ios_base::failure("EoF in istream bool read");
    }

    return success;
}

streamsize do_read(basic_streambuf<char, char_traits<char>>* val,
    basic_istream<char, char_traits<char>>& istr)
{
    streamsize gcount_val = 0;

    // Constuct sentry
    istream::sentry sen {istr};

    // Carry on if the sentry is fine
    if (sen && val != nullptr)
    {
        while (istream::traits_type::int_type c = istr.rdbuf()->sbumpc())
        {
            // Check for eof.
            if (istream::traits_type::eq_int_type(c,
                istream::traits_type::eof()))
            {
                try {
                    istr.setstate(ios_base::eofbit);
                }
                catch (...) {}
                break;
            }

            // Insert into the streambuffer.
            c = val->sputc(c);

            // Check for eof.
            if (istream::traits_type::eq_int_type(c,
                istream::traits_type::eof()))
            {
                istr.unget();
                break;
            }

            ++gcount_val;
        }
    }

    if (gcount_val == 0)
    {
        try {
            istr.setstate(ios_base::failbit);
        }
        catch (...) {}
    }

    // Allow exceptions for eof to be thrown.
    if (istr.fail() && (istr.exceptions() & ios_base::failbit) != 0)
        throw ios_base::failure("Failure in istream streambuf read");
    if (istr.eof() && (istr.exceptions() & ios_base::eofbit) != 0)
        throw ios_base::failure("EoF in istream streambuf read");

    return gcount_val;
}

/******************************************************************************
 ******************************************************************************/

} // end helper namespace

/******************************************************************************
 ******************************************************************************/

} // end NMSP namespace
