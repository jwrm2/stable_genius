#include "../include/stdexcept"

#include "../include/string"

namespace NMSP {

/******************************************************************************
 ******************************************************************************/

namespace helper {

ref_count_str::ref_count_str() : msg {new string {}}, refs{new size_t {1}} {}

ref_count_str::ref_count_str(const string& s) :
    msg {new string {s}}, refs{new size_t {1}}
{}

ref_count_str::ref_count_str(const char* str) :
    msg {new string {str}}, refs{new size_t {1}}
{}

ref_count_str::ref_count_str(const ref_count_str& other) noexcept :
    msg {other.msg}, refs{other.refs}
{
    ++(*refs);
}

ref_count_str::ref_count_str(ref_count_str&& other) noexcept :
    msg {other.msg}, refs{other.refs}
{
    other.msg = nullptr;
    other.refs = nullptr;
}

/******************************************************************************/

ref_count_str& ref_count_str::operator=(const ref_count_str& other) noexcept
{
    if (*refs == 1)
    {
        delete msg;
        delete refs;
    }
    else
        --(*refs);

    msg = other.msg;
    refs = other.refs;
    ++(*refs);

    return *this;
}

ref_count_str& ref_count_str::operator=(ref_count_str&& other) noexcept
{
    if (*refs == 1)
    {
        delete msg;
        delete refs;
    }
    else
        --(*refs);

    msg = other.msg;
    refs = other.refs;

    other.msg = nullptr;
    other.refs = nullptr;

    return *this;
}

/******************************************************************************/

ref_count_str::~ref_count_str()
{
    if (*refs == 1)
    {
        delete msg;
        delete refs;
    }
    else
        --(*refs);
}

/******************************************************************************/

const char* ref_count_str::get() const noexcept { return msg->c_str(); }

} // end namesapce helper

/******************************************************************************
 ******************************************************************************/

runtime_error::runtime_error(const string& s) : msg {s} {}

runtime_error::runtime_error(const char* str) : msg {str} {}

runtime_error::runtime_error(const runtime_error& other) noexcept :
    msg {other.msg}
{}

/******************************************************************************/

runtime_error& runtime_error::operator=(const runtime_error& other) noexcept
{
    msg = other.msg;
    return *this;
}

/******************************************************************************/

runtime_error::~runtime_error() {}

/******************************************************************************/

const char* runtime_error::what() const noexcept { return msg.get(); }

/******************************************************************************
 ******************************************************************************/

} // end NMSP namespace
