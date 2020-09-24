#include "../include/new"

#ifndef KLIB
#include "../include/cstdlib"
#include "../include/UserHeap.h"
#endif /* KLIB not defined */

namespace NMSP {

/******************************************************************************
 ******************************************************************************/

nothrow_t nothrow;

/******************************************************************************
 ******************************************************************************/

bad_alloc::~bad_alloc() noexcept {}

/******************************************************************************/

const char* bad_alloc::what() const noexcept
{
    return "NMSP::bad_alloc";
}

/******************************************************************************
 ******************************************************************************/

bad_array_new_length::~bad_array_new_length() noexcept {}

/******************************************************************************/

const char* bad_array_new_length::what() const noexcept
{
    return "NMSP::bad_array_new_length";
}

/******************************************************************************
 ******************************************************************************/

} // end NMSP namespace

/******************************************************************************
 ******************************************************************************/

// The following are implemented in the kernel for klib, but need to be here
// for the user space library.
#ifndef KLIB

/******************************************************************************
 ******************************************************************************/

void* operator new(size_t n)
{
    void* ret_val = operator new(n, NMSP::nothrow);
    if (ret_val == nullptr)
        throw NMSP::bad_alloc {};

    return ret_val;
}

void* operator new(size_t n, const NMSP::nothrow_t&) noexcept
{
    return NMSP::helper::user_heap->malloc(n);
}

void* operator new(size_t n, void* ptr) noexcept { (void)n; return ptr; }

/******************************************************************************/

void* operator new[](size_t n)
{
    void* ret_val = operator new[](n, NMSP::nothrow);
    if (ret_val == nullptr)
        throw NMSP::bad_alloc {};

    return ret_val;
}

void* operator new[](size_t n, const NMSP::nothrow_t&) noexcept
{
    return NMSP::helper::user_heap->malloc(n);
}

void* operator new[](size_t n, void* ptr) noexcept { (void)n; return ptr; }

/******************************************************************************
 ******************************************************************************/

void operator delete(void* p) noexcept
{
    NMSP::helper::user_heap->free(p);
}

void operator delete(void* p, size_t) noexcept
{
    operator delete(p);
}

/******************************************************************************/

void operator delete[](void* p) noexcept
{
    NMSP::helper::user_heap->free(p);
}

void operator delete[](void* p, size_t) noexcept
{
    operator delete[](p);
}

/******************************************************************************
 ******************************************************************************/

#endif /* KLIB not defined */
