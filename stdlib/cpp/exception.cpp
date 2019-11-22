#include "../include/exception"

#include "../include/cstdlib"
#include "../include/cxxabi"

namespace NMSP {

/******************************************************************************
 ******************************************************************************/

exception::~exception() {}

/******************************************************************************/

const char* exception::what() const noexcept
{
    // Basic exception doesn't allow specifiying the string.
    return "klib::exception";
}

/******************************************************************************
 ******************************************************************************/

// Currently installed terminate and unexpected handler.
static terminate_handler curr_terminate_handler = abort;
static unexpected_handler curr_unexpected_handler = std::terminate;

/******************************************************************************
 ******************************************************************************/

terminate_handler set_terminate(terminate_handler new_handler) noexcept
{
    // Not thread safe.
    terminate_handler old = curr_terminate_handler;
    curr_terminate_handler = new_handler;
    return old;
}

/******************************************************************************/

terminate_handler get_terminate() noexcept
{
    return curr_terminate_handler;
}

/******************************************************************************
 ******************************************************************************/

unexpected_handler set_unexpected(unexpected_handler new_handler) noexcept
{
    // Not thread safe.
    unexpected_handler old = curr_unexpected_handler;
    curr_unexpected_handler = new_handler;
    return old;
}

/******************************************************************************/

unexpected_handler get_unexpected() noexcept
{
    return curr_unexpected_handler;
}

/******************************************************************************/

void unexpected() noexcept
{
    curr_unexpected_handler();
    abort();
}

/******************************************************************************
 ******************************************************************************/

bool uncaught_exception() noexcept
{
    return (__cxxabiv1::__cxa_get_globals()->uncaughtExceptions != 0);
}

/******************************************************************************
 ******************************************************************************/

} // end NMSP namespace

/******************************************************************************
 ******************************************************************************/

namespace std {

/******************************************************************************
 ******************************************************************************/

void terminate() noexcept
{
    try
    {
        NMSP::curr_terminate_handler();
    }
    catch (...)
    {
        NMSP::abort();
    }
    
    NMSP::abort();
}

/******************************************************************************
 ******************************************************************************/

} // end std namespace
