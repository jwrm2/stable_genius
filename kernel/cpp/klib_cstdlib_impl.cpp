#include <cstdio>
#include <cstdlib>
#include <cxxabi>
#include <new>
#include <ostream>
#include <string>

#include <stddef.h>

#include "FileSystem.h"
#include "Kernel.h"
#include "KernelHeap.h"
#include "Logger.h"
#include "no_heap_util.h"
#include "Process.h"
#include "ProcTable.h"
#include "Scheduler.h"
#include "VgaController.h"

// This file contains implementations of some klib functions that rely on
// kernel specific functionality.

namespace klib {

/******************************************************************************
 ******************************************************************************/

// Cease execution. Disable all interrupts and put the kernel into an infinite
// loop.
extern "C"
void abort()
{
    global_kernel->default_disable();
    global_kernel->syslog()->error("Abort called\n");
    while (true) ;
}

/******************************************************************************
 ******************************************************************************/

extern "C"
void* malloc(size_t size)
{
    if (global_kernel == nullptr)
        return nullptr;

    KernelHeap* kh = global_kernel->get_heap();
    if (kh == nullptr)
        return nullptr;

    return kh->malloc(size);
}

/******************************************************************************
 ******************************************************************************/

extern "C"
void free(void* ptr)
{
    if (global_kernel == nullptr)
        return;

    KernelHeap* kh = global_kernel->get_heap();
    if (kh == nullptr)
        return;

    kh->free(ptr);
}

/******************************************************************************
 ******************************************************************************/

// Default printing. I think we'll just put this into the system log. If the
// heap exists and there is a stream attached to the logger, send it there.
// Otherwise, send it to the loggers character device, if it exists. If not,
// just gobble.

int vprintf(const char* format, va_list vlist)
{
    int ret_val = EOF;

    if (!format)
        return ret_val;

    if (global_kernel->get_heap() && global_kernel->syslog()->stream())
    {
        // Perform the substitutions, then print to the destination.
        klib::string msg;
        klib::helper::vstrprintf(msg, format, vlist);

        *global_kernel->syslog()->stream() << msg;
        ret_val = msg.size();
    }
    else if (global_kernel->syslog()->device())
    {
        // No heap, so we have to use kvprintf from no_heap_util.
        ret_val = kvprintf(global_kernel->syslog()->device(), format, vlist);
    }

    return ret_val;
}
/******************************************************************************
 ******************************************************************************/

// Opens a file by forwarding the call to the VFS. Fails if there is no VFS.

FILE* fopen(const char* filename, const char* mode)
{
    VirtualFileSystem* vfs = global_kernel->get_vfs();

    if (vfs == nullptr)
        return nullptr;

    return vfs->fopen(filename, mode);
}

/******************************************************************************
 ******************************************************************************/

} // end klib namespace

/******************************************************************************
 ******************************************************************************/

namespace __cxxabiv1 {

/******************************************************************************
 ******************************************************************************/

__cxa_eh_globals* __cxa_get_globals()
{
    // Test whether we have moved on from kernel initialisation.
    if (!switch_blocked_for_init &&
        global_kernel->get_scheduler().get_last() != 0)
    {
        Process* p = global_kernel->get_proc_table().get_process(
            global_kernel->get_scheduler().get_last());
        __cxa_eh_globals* eh = p->get_eh_globals();

        if (eh == nullptr)
        {
            eh = new __cxxabiv1::__cxa_eh_globals {};
            p->set_eh_globals(eh);
        }

        return eh;      
    }

    // Get the kernel copy.
    __cxa_eh_globals* eh = global_kernel->get_eh_globals();

    if (eh == nullptr)
    {
        eh = new __cxa_eh_globals {};
        global_kernel->set_eh_globals(eh);
    }

    return eh;
}

/******************************************************************************/

__cxa_eh_globals* __cxa_get_globals_fast()
{
    // Test whether we have moved on from kernel initialisation.
    if (!switch_blocked_for_init &&
        global_kernel->get_scheduler().get_last() != 0)
    {
        return global_kernel->get_proc_table().get_process(
            global_kernel->get_scheduler().get_last())->get_eh_globals();
    }

    // Get the kernel copy.
    return global_kernel->get_eh_globals();
}

/******************************************************************************
 ******************************************************************************/

} // end __cxxabiv1 namespace

/******************************************************************************
 ******************************************************************************/

void* operator new(size_t n)
{
    void* ret_val = operator new(n, klib::nothrow);
    if (ret_val == nullptr)
        throw klib::bad_alloc {};

    return ret_val;
}

void* operator new(size_t n, const klib::nothrow_t&) noexcept
{
    if (global_kernel->get_heap() == nullptr)
        return nullptr;
    return global_kernel->get_heap()->malloc(n);
}

void* operator new(size_t n, void* ptr) noexcept { (void)n; return ptr; }

/******************************************************************************/

void* operator new[](size_t n)
{
    void* ret_val = operator new[](n, klib::nothrow);
    if (ret_val == nullptr)
        throw klib::bad_alloc {};

    return ret_val;
}

void* operator new[](size_t n, const klib::nothrow_t&) noexcept
{
    if (global_kernel->get_heap() == nullptr)
        return nullptr;
    return global_kernel->get_heap()->malloc(n);
}

void* operator new[](size_t n, void* ptr) noexcept { (void)n; return ptr; }

/******************************************************************************
 ******************************************************************************/

void operator delete(void* p)
{
    if (global_kernel->get_heap() != nullptr)
        global_kernel->get_heap()->free(p);
}

/******************************************************************************/

void operator delete[](void* p)
{
    if (global_kernel->get_heap() != nullptr)
        global_kernel->get_heap()->free(p);
}

/******************************************************************************
 ******************************************************************************/
