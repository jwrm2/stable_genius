#include "../include/ios"

#include "../include/string"

namespace NMSP {

/******************************************************************************
 ******************************************************************************/

error_code make_error_code(io_errc e) noexcept
{
    return error_code {static_cast<int>(e), iostream_category()};
}

/******************************************************************************
 ******************************************************************************/

error_condition make_error_condition(io_errc e) noexcept
{
    return error_condition {static_cast<int>(e), iostream_category()};
}

/******************************************************************************
 ******************************************************************************/

/** Extend error category to provide the iostream error category. */
class iostream_error_category : public error_category {
    /**
        Implementation of the pure virtual function name(). Gives the name of
        the error category.

        @return The C-string "iostream".
     */
    virtual const char* name() const noexcept override
    {
        return "iostream";
    }

    /**
        Implementation of the pure virtual function message(). Returns a message
        describing the error code.

        @param int Error code.
        @return String for this condition in this category.
     */
    virtual string message(int i) const
    {
        string msg;
        switch(static_cast<io_errc>(i))
        {
        case io_errc::stream:
            msg = "iostream error";
            break;
        default:
            msg = "Unknown error";
        }

        return msg;
    }
};

/******************************************************************************/

const error_category& iostream_category() noexcept
{
    static const iostream_error_category iec;
    return iec;
}

/******************************************************************************
 ******************************************************************************/

ios_base::~ios_base()
{
    // Call the callbacks in reverse order.
    for (auto p = callbacks.rbegin(); p != callbacks.rend(); ++p)
        p->func(erase_event, *this, p->index);
}

/******************************************************************************/

ios_base::fmtflags ios_base::setf(fmtflags flgs)
{
    fmtflags old_flags = fl;
    fl |= flgs;
    return old_flags;
}

ios_base::fmtflags ios_base::setf(fmtflags flgs, fmtflags mask)
{
    fmtflags old_flags = fl;
    fl = (~mask & fl);
    fl |= (flgs & mask);
    return old_flags;
}

/******************************************************************************/

void ios_base::unsetf(fmtflags flgs)
{
    fl &= ~flgs;
}

/******************************************************************************/

void ios_base::register_callback(event_callback func, int index)
{
    if (func != nullptr)
        callbacks.emplace_back(func, index);
}

/******************************************************************************/

void ios_base::init() noexcept
{
    // Set the default values for the internal flags.
    prec = 6;
    wid = 0;
    fl = skipws | dec;
}

/******************************************************************************
 ******************************************************************************/

ios_base& boolalpha(ios_base& str)
{
    str.setf(ios_base::boolalpha);
    return str;
}

ios_base& noboolalpha(ios_base& str)
{
    str.unsetf(ios_base::boolalpha);
    return str;
}

/******************************************************************************/

ios_base& showbase(ios_base& str)
{
    str.setf(ios_base::showbase);
    return str;
}

ios_base& noshowbase(ios_base& str)
{
    str.unsetf(ios_base::showbase);
    return str;
}

/******************************************************************************/

ios_base& showpoint(ios_base& str)
{
    str.setf(ios_base::showpoint);
    return str;
}

ios_base& noshowpoint(ios_base& str)
{
    str.unsetf(ios_base::showpoint);
    return str;
}

/******************************************************************************/

ios_base& showpos(ios_base& str)
{
    str.setf(ios_base::showpos);
    return str;
}

ios_base& noshowpos(ios_base& str)
{
    str.unsetf(ios_base::showpos);
    return str;
}

/******************************************************************************/

ios_base& skipws(ios_base& str)
{
    str.setf(ios_base::skipws);
    return str;
}

ios_base& noskipws(ios_base& str)
{
    str.unsetf(ios_base::skipws);
    return str;
}

/******************************************************************************/

ios_base& uppercase(ios_base& str)
{
    str.setf(ios_base::uppercase);
    return str;
}

ios_base& nouppercase(ios_base& str)
{
    str.unsetf(ios_base::uppercase);
    return str;
}

/******************************************************************************/

ios_base& unitbuf(ios_base& str)
{
    str.setf(ios_base::unitbuf);
    return str;
}

ios_base& nounitbuf(ios_base& str)
{
    str.unsetf(ios_base::unitbuf);
    return str;
}

/******************************************************************************/

ios_base& internal(ios_base& str)
{
    str.setf(ios_base::internal, ios_base::adjustfield);
    return str;
}

ios_base& left(ios_base& str)
{
    str.setf(ios_base::left, ios_base::adjustfield);
    return str;
}

ios_base& right(ios_base& str)
{
    str.setf(ios_base::right, ios_base::adjustfield);
    return str;
}

/******************************************************************************/

ios_base& dec(ios_base& str)
{
    str.setf(ios_base::dec, ios_base::basefield);
    return str;
}

ios_base& hex(ios_base& str)
{
    str.setf(ios_base::hex, ios_base::basefield);
    return str;
}

ios_base& oct(ios_base& str)
{
    str.setf(ios_base::oct, ios_base::basefield);
    return str;
}

/******************************************************************************/

ios_base& fixed(ios_base& str)
{
    str.setf(ios_base::fixed, ios_base::floatfield);
    return str;
}

ios_base& scientific(ios_base& str)
{
    str.setf(ios_base::scientific, ios_base::floatfield);
    return str;
}

ios_base& hexfloat(ios_base& str)
{
    str.setf(ios_base::fixed | ios_base::scientific, ios_base::floatfield);
    return str;
}

ios_base& defaultfloat(ios_base& str)
{
    str.unsetf(ios_base::floatfield);
    return str;
}

/******************************************************************************
 ******************************************************************************/

} // end NMSP namespace
