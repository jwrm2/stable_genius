#include "../include/system_error"

#include "../include/cstring"
#include "../include/string"

namespace NMSP {

/******************************************************************************
 ******************************************************************************/

error_condition error_category::default_error_condition(int code) const noexcept
{
    return error_condition {code, *this};
}

/******************************************************************************/

bool error_category::equivalent(int code, const error_condition& cond) const
    noexcept
{
    return (default_error_condition(code) == cond);
}

bool error_category::equivalent(const error_code& code, int cond) const noexcept
{
    return (*this == code.category() && code.value() == cond);
}

/******************************************************************************
 ******************************************************************************/

/** Extend error category to provide the generic error category. */
class generic_error_category : public error_category {
    /**
        Implementation of the pure virtual function name(). Gives the name of
        the error category.

        @return The C-string "generic".
     */
    virtual const char* name() const noexcept override
    {
        return "generic";
    }

    /**
        Implementation of the pure virtual function message(). Returns a message
        describing the error code.

        @param int Error code.
        @return String for this condition in this category.
     */
    virtual string message(int i) const
    {
        return strerror(i);
    }
};

/******************************************************************************/

const error_category& generic_category() noexcept
{
    static const generic_error_category gec;
    return gec;
}

/******************************************************************************
 ******************************************************************************/

/** Extend error category to provide the system error category. */
class system_error_category : public error_category {
    /**
        Implementation of the pure virtual function name(). Gives the name of
        the error category.

        @return The C-string "system".
     */
    virtual const char* name() const noexcept override
    {
        return "system";
    }

    /**
        Implementation of the pure virtual function message(). Returns a message
        describing the error code.

        @param int Error code.
        @return String for this condition in this category.
     */
    virtual string message(int i) const
    {
        return strerror(i);
    }
};

/******************************************************************************/

const error_category& system_category() noexcept
{
    static const system_error_category sec;
    return sec;
}

/******************************************************************************
 ******************************************************************************/

string error_condition::message() const
{
    return category().message(value());
}

/******************************************************************************/

error_condition make_error_condition(errc e) noexcept
{
    return error_condition {static_cast<int>(e), generic_category()};
}

/******************************************************************************/

bool operator==(const error_condition& lhs, const error_condition& rhs) noexcept
{
    return (lhs.category() == rhs.category() && lhs.value() == rhs.value());
}

bool operator!=(const error_condition& lhs, const error_condition& rhs) noexcept
{
    return !(lhs == rhs);
}

bool operator<(const error_condition& lhs, const error_condition& rhs) noexcept
{
    return (lhs.category() < rhs.category() ||
        (lhs.category() == rhs.category() && lhs.value() < rhs.value()));
}

/******************************************************************************
 ******************************************************************************/

string error_code::message() const
{
    return category().message(value());
}

/******************************************************************************/

error_code make_error_code(errc e) noexcept
{
    return error_code {static_cast<int>(e), generic_category()};
}

/******************************************************************************/

bool operator==(const error_code& lhs, const error_code& rhs) noexcept
{
    return (lhs.category() == rhs.category() && lhs.value() == rhs.value());
}

bool operator!=(const error_code& lhs, const error_code& rhs) noexcept
{
    return !(lhs == rhs);
}

bool operator<(const error_code& lhs, const error_code& rhs) noexcept
{
    return (lhs.category() < rhs.category() ||
        (lhs.category() == rhs.category() && lhs.value() < rhs.value()));
}

/******************************************************************************
 ******************************************************************************/

bool operator==(const error_condition& cond, const error_code& code) noexcept
{
    return (code.category().equivalent(code.value(), cond) ||
            cond.category().equivalent(code, cond.value()));
}

bool operator==(const error_code& code, const error_condition& cond) noexcept
{
    return cond == code;
}

bool operator!=(const error_condition& cond, const error_code& code) noexcept
{
    return !(cond == code);
}

bool operator!=(const error_code& code, const error_condition& cond) noexcept
{
    return !(cond == code);
}

/******************************************************************************
 ******************************************************************************/

system_error::system_error(error_code e) :
    runtime_error{e.message()}, ec{e}
{}

system_error::system_error(error_code e, const string& s) :
    runtime_error{s + ": " + e.message()}, ec{e}
{}

system_error::system_error(error_code e, const char* str) :
    runtime_error{str + (": " + e.message())}, ec{e}
{}

system_error::system_error(int ev, const error_category& ecat) :
    runtime_error{error_code{ev, ecat}.message()}, ec{ev, ecat}
{}

system_error::system_error(int ev, const error_category& ecat, const string& s):
    runtime_error{s + ": " + error_code{ev, ecat}.message()}, ec{ev, ecat}
{}

system_error::system_error(int ev, const error_category& ecat, const char* str):
    runtime_error{str + (": " + error_code{ev, ecat}.message())}, ec{ev, ecat}
{}

/******************************************************************************
 ******************************************************************************/

} // end NMSP namespace
