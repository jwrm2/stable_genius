#include "Logger.h"

#include <stdarg.h>

#include <cstdio>
#include <ostream>
#include <string>

#include "Device.h"
#include "Kernel.h"
#include "no_heap_util.h"
#include "Pit.h"

static constexpr const char* null_time = "[0:00:00.00]";

/******************************************************************************
 ******************************************************************************/

Logger::Logger(Logger&& other) :
    dest{other.dest}, char_dest{other.char_dest}, owned {other.owned}
{
    other.dest = nullptr;
    other.char_dest = nullptr;
    other.owned = false;
}

/******************************************************************************/

Logger& Logger::operator=(Logger&& other)
{
    if (owned)
    {
        delete dest;
        delete char_dest;
    }

    dest = other.dest;
    char_dest = other.char_dest;
    owned = other.owned;

    other.dest = nullptr;
    other.char_dest = nullptr;
    other.owned = false;

    return *this;
}

/******************************************************************************/

Logger::~Logger()
{
    if (owned)
    {
        delete dest;
        delete char_dest;
    }
}

/******************************************************************************/

void Logger::error(const char* fmt, ...) 
{
    // Include time, but only if the heap exists.
    if (global_kernel->get_heap() != nullptr)
        write(time());
    else
        write(null_time);

    // Prepend message
    write(" ERROR: ");

    // Initialise varibale argument list
    va_list arg;
    va_start(arg, fmt);

    // Pass onto vwrite
    vwrite(fmt, arg);

    // Clean up
    va_end(arg);
}

/******************************************************************************/

void Logger::warn(const char* fmt, ...) 
{
    // Include time, but only if the heap exists.
    if (global_kernel->get_heap() != nullptr)
        write(time());
    else
        write(null_time);

    // Prepend message
    write(" WARNING: ");

    // Initialise varibale argument list
    va_list arg;
    va_start(arg, fmt);

    // Pass onto vwrite
    vwrite(fmt, arg);

    // Clean up
    va_end(arg);
}

/******************************************************************************/

void Logger::info(const char* fmt, ...) 
{
    // Include time, but only if the heap exists.
    if (global_kernel->get_heap() != nullptr)
        write(time());
    else
        write(null_time);

    // Prepend message
    write(" INFO: ");

    // Initialise varibale argument list
    va_list arg;
    va_start(arg, fmt);

    // Pass onto vwrite
    vwrite(fmt, arg);

    // Clean up
    va_end(arg);
}

/******************************************************************************/

void Logger::write(const char* fmt, ...) 
{
    // Initalise variable argument list
    va_list arg;
    va_start(arg, fmt);
    // Pass onto vwrite
    vwrite(fmt, arg);
    // Clean up
    va_end(arg);
}

void Logger::write(const klib::string& msg)
{
    write(msg.c_str());
}

/******************************************************************************/

void Logger::vwrite(const char* fmt, va_list arg) 
{
    if (dest)
    {
        klib::string tmp;
        klib::helper::vstrprintf(tmp, fmt, arg);
        *dest << tmp;
        dest->flush();
    }
    else if (char_dest)
        kvprintf(char_dest, fmt, arg);
}

/******************************************************************************/

klib::string Logger::time()
{
    if (global_kernel->get_pit() == nullptr)
        return null_time;

    unsigned int ms = global_kernel->get_pit()->time();
    unsigned int cs = (ms % 1000) / 10;
    unsigned int sec = (ms / 1000) % 60;
    unsigned int min = (ms / (1000 * 60)) % 60;
    unsigned int hour = ms / (1000 * 60 * 60);

    klib::string ret_val = "[";
    klib::string tmp;
    klib::helper::strprintf(tmp, "%u", hour);
    ret_val += tmp + ':';

    klib::helper::strprintf(tmp, "%u", min);
    if (tmp.size() == 1)
        tmp = '0' + tmp;
    ret_val += tmp + ':';

    klib::helper::strprintf(tmp, "%u", sec);
    if (tmp.size() == 1)
        tmp = '0' + tmp;
    ret_val += tmp + '.';

    klib::helper::strprintf(tmp, "%u", cs);
    if (tmp.size() == 1)
        tmp = '0' + tmp;
    ret_val += tmp + ']';

    return ret_val;
}

/******************************************************************************/

void Logger::device(CharacterDevice* s)
{
    if (owned)
    {
        delete dest;
        delete char_dest;
    }

    dest = nullptr;
    char_dest = s;
    owned = true;
}

void Logger::device(CharacterDevice& s)
{
    if (owned)
    {
        delete dest;
        delete char_dest;
    }

    dest = nullptr;
    char_dest = &s;
    owned = false;
}

/******************************************************************************/

void Logger::stream(klib::ostream* s)
{
    if (owned)
    {
        delete dest;
        delete char_dest;
    }

    dest = s;
    char_dest = nullptr;
    owned = true;
}

void Logger::stream(klib::ostream& s)
{
    if (owned)
    {
        delete dest;
        delete char_dest;
    }

    dest = &s;
    char_dest = nullptr;
    owned = false;
}

/******************************************************************************
 ******************************************************************************/
