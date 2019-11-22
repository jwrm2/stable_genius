#include "../include/cstring"
#include "../include/cstdio"
#include "../include/system_error"

namespace NMSP {

/******************************************************************************
 ******************************************************************************/

char* strcpy(char* dest, const char* src)
{
    if (!src || !dest)
        return nullptr;

    for (size_t i = 0; i <= strlen(src); ++i)
        dest[i] = src[i];

    return dest;
}

/******************************************************************************
 ******************************************************************************/

char* strncpy(char* dest, const char* src, size_t count)
{
    if (!dest || !src)
        return nullptr;

    size_t i;
    for (i = 0; src[i] && i < count; ++i)
        dest[i] = src[i];
    for (; i < count; ++i)
        dest[i] = 0;

    return dest; 
}

/******************************************************************************
 ******************************************************************************/

char* strcat(char* dest, const char* src)
{
    if (!dest || !src)
        return nullptr;

    size_t start = strlen(dest);
    size_t i;
    for (i = 0; src[i]; ++i)
        dest[start + i] = src[i];
    dest[start + i] = 0;

    return dest; 
}

/******************************************************************************
 ******************************************************************************/

extern "C"
size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len])
        ++len;
    return len;
}

/******************************************************************************
 ******************************************************************************/

int strcmp(const char* lhs, const char* rhs)
{
    const unsigned char* l = reinterpret_cast<const unsigned char*>(lhs);
    const unsigned char* r = reinterpret_cast<const unsigned char*>(rhs);

    while(*l && (*l == *r))
    {
        ++l;
        ++r;
    }

    return *l - *r;
}

/******************************************************************************
 ******************************************************************************/

int strncmp(const char* lhs, const char* rhs, size_t count)
{
    if (count == 0)
        return 0;

    const unsigned char* l = reinterpret_cast<const unsigned char*>(lhs);
    const unsigned char* r = reinterpret_cast<const unsigned char*>(rhs);

    while(count - 1 && *l && (*l == *r))
    {
        ++l;
        ++r;
        --count;
    }

    return *l - *r;
}

/******************************************************************************
 ******************************************************************************/

const char* strerror(int errnum)
{
    if (errnum == 0)
        return "Success";

    errc err = static_cast<errc>(errnum);
    switch (err)
    {
    case errc::operation_not_permitted:
        return "Operation not permitted";
    case errc::no_such_file_or_directory:
        return "No such file or directory";
    case errc::no_such_process:
        return "No such process";
    case errc::interrupted:
        return "Interrupted system call";
    case errc::io_error:
        return "Input/output error";
    case errc::no_such_device_or_address:
        return "No such device or address";
    case errc::argument_list_too_long:
        return "Argument list too long";
    case errc::executable_format_error:
        return "Exec format error";
    case errc::bad_file_descriptor:
        return "Bad file descriptor";
    case errc::no_child_process:
        return "No child processes";
    case errc::resource_unavailable_try_again:
        return "Resource temporarily unavailable";
// This is just a duplicate of resource_unavailable_try_again
//    case errc::operation_would_block:
//        return "Resource deadlock avoided";
    case errc::not_enough_memory:
        return "Cannot allocate memory";
    case errc::permission_denied:
        return "Permission denied";
    case errc::bad_address:
        return "Bad address";
    case errc::device_or_resource_busy:
        return "Device or resource busy";
    case errc::file_exists:
        return "File exists";
    case errc::cross_device_link:
        return "Invalid cross-device link";
    case errc::no_such_device:
        return "No such device";
    case errc::not_a_directory:
        return "Not a directory";
    case errc::is_a_directory:
        return "Is a directory";
    case errc::invalid_argument:
        return "Invalid argument";
    case errc::too_many_files_open_in_system:
        return "Too many open files";
    case errc::too_many_files_open:
        return "Too many open files in system";
    case errc::inappropriate_io_control_operation:
        return "Inappropriate ioctl for device";
    case errc::text_file_busy:
        return "Text file busy";
    case errc::file_too_large:
        return "File too large";
    case errc::no_space_on_device:
        return "No space left on device";
    case errc::invalid_seek:
        return "Illegal seek";
    case errc::read_only_file_system:
        return "Read-only file system";
    case errc::too_many_links:
        return "Too many links";
    case errc::broken_pipe:
        return "Broken pipe";
    case errc::argument_out_of_domain:
        return "Numerical argument out of domain";
    case errc::result_out_of_range:
        return "Numerical result out of range";
    case errc::resource_deadlock_would_occur:
        return "Operation would block";
    case errc::filename_too_long:
        return "File name too long";
    case errc::no_lock_available:
        return "No locks available";
    case errc::function_not_supported:
        return "Function not implemented";
    case errc::directory_not_empty:
        return "Directory not empty";
    case errc::too_many_symbolic_link_levels:
        return "Too many levels of symbolic links";
    case errc::no_message:
        return "No message of desired type";
    case errc::identifier_removed:
        return "Identifier removed";
    case errc::not_a_stream:
        return "Device not a stream";
    case errc::no_message_available:
        return "No data available";
    case errc::stream_timeout:
        return "Timer expired";
    case errc::no_stream_resources:
        return "Out of streams resources";
    case errc::no_link:
        return "Link has been severed";
    case errc::protocol_error:
        return "Protocol error";
    case errc::bad_message:
        return "Bad message";
    case errc::value_too_large:
        return "Value too large for defined data type";
    case errc::illegal_byte_sequence:
        return "Invalid or incomplete multibyte or wide character";
    case errc::not_a_socket:
        return "Socket operation on non-socket";
    case errc::destination_address_required:
        return "Destination address required";
    case errc::message_size:
        return "Message too long";
    case errc::wrong_protocol_type:
        return "Protocol wrong type for socket";
    case errc::no_protocol_option:
        return "Protocol not available";
    case errc::protocol_not_supported:
        return "Protocol not supported";
    case errc::operation_not_supported:
        return "Operation not supported";
// This is just a duplicate of operation_not_supported
//    case errc::not_supported:
//        return "Not supported";
    case errc::address_family_not_supported:
        return "Address family not supported by protocol";
    case errc::address_in_use:
        return "Address already in use";
    case errc::address_not_available:
        return "Cannot assign requested address";
    case errc::network_down:
        return "Network is down";
    case errc::network_unreachable:
        return "Network is unreachable";
    case errc::network_reset:
        return "Network dropped connection on reset";
    case errc::connection_aborted:
        return "Software caused connection abort";
    case errc::connection_reset:
        return "Connection reset by peer";
    case errc::no_buffer_space:
        return "No buffer space available";
    case errc::already_connected:
        return "Transport endpoint is already connected";
    case errc::not_connected:
        return "Transport endpoint is not connected";
    case errc::timed_out:
        return "Connection timed out";
    case errc::connection_refused:
        return "Connection refused";
    case errc::host_unreachable:
        return "No route to host";
    case errc::connection_already_in_progress:
        return "Operation already in progress";
    case errc::operation_in_progress:
        return "Operation now in progress";
    case errc::operation_canceled:
        return "Operation canceled";
    case errc::owner_dead:
        return "Owner died";
    case errc::state_not_recoverable:
        return "State not recoverable";
    }

    return "Unknown error";
}

/******************************************************************************
 ******************************************************************************/

const void* memchr(const void* ptr, int ch, size_t count)
{
    if (!ptr)
        return nullptr;

    unsigned char c = static_cast<unsigned char>(ch);
    const unsigned char* p = static_cast<const unsigned char*>(ptr);

    while (count && c != *p)
    {
        ++p;
        --count;
    }

    if (count == 0)
        return NULL;

    return p;
}

/******************************************************************************
 ******************************************************************************/

void* memchr(void* ptr, int ch, size_t count)
{
    if (!ptr)
        return nullptr;

    unsigned char c = static_cast<unsigned char>(ch);
    unsigned char* p = static_cast<unsigned char*>(ptr);

    while (count && c != *p)
    {
        ++p;
        --count;
    }

    if (count == 0)
        return NULL;

    return p;
}

/******************************************************************************
 ******************************************************************************/

int memcmp(const void* lhs, const void* rhs, size_t count)
{
    if (count == 0)
        return 0;

    const unsigned char* l = static_cast<const unsigned char*>(lhs);
    const unsigned char* r = static_cast<const unsigned char*>(rhs);

    while (count--)
    {
        if (*l != *r)
            return *l - *r;
        else
        {
            ++l;
            ++r;
        }
    }

    return 0;
}

/******************************************************************************
 ******************************************************************************/

void* memcpy(void* dest, const void* src, size_t count)
{
    if (!dest || !src)
        return nullptr;

    unsigned char* d = static_cast<unsigned char*>(dest);
    const unsigned char* s = static_cast<const unsigned char*>(src);

    for (size_t i = 0; i < count; ++i)
        d[i] = s[i];

    return dest;
}

/******************************************************************************
 ******************************************************************************/

void* memmove(void* dest, const void* src, size_t count)
{
    if (!dest || !src)
        return nullptr;

    if (count == 0)
        return dest;

    unsigned char* d = static_cast<unsigned char*>(dest);
    const unsigned char* s = static_cast<const unsigned char*>(src);
    unsigned char* tmp = new unsigned char[count];

    for (size_t i = 0; i < count; ++i)
        tmp[i] = s[i];

    for (size_t i = 0; i < count; ++i)
        d[i] = tmp[i];

    delete[] tmp;
    return dest;
}

/******************************************************************************
 ******************************************************************************/

void* memset(void* dest, int value, size_t count)
{
    if (!dest)
        return nullptr;

    unsigned char* d = static_cast<unsigned char*>(dest);
    unsigned char v = static_cast<unsigned char>(value);

    for (size_t i = 0; i < count; ++i)
        d[i] = v;

    return dest;
}

} // NMSP namespace
