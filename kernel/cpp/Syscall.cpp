#include "Syscall.h"

#include <stddef.h>
#include <stdint.h>

#include <fstream>
#include <ios>
#include <map>
#include <string>
#include <utility>

#include "Kernel.h"
#include "Logger.h"
#include "PageDescriptorTable.h"
#include "paging.h"
#include "Process.h"
#include "ProcTable.h"
#include "Scheduler.h"
#include "SignalManager.h"

/******************************************************************************
 ******************************************************************************/

void syscall(InterruptRegisters& ir, const InterruptStack& is)
{
    // Do a big switch on the function index. The index is stored in %eax before
    // the call.
    int32_t ret_val = 0;
    syscall_ind ind = static_cast<syscall_ind>(ir.eax());

    // Mapping from syscall indices to name.
    static const klib::map<syscall_ind, klib::string> function_names = {
        klib::pair<syscall_ind, klib::string> {syscall_ind::fork, "fork"},
        klib::pair<syscall_ind, klib::string> {syscall_ind::read, "read"},
        klib::pair<syscall_ind, klib::string> {syscall_ind::write, "write"},
        klib::pair<syscall_ind, klib::string> {syscall_ind::open, "open"},
        klib::pair<syscall_ind, klib::string> {syscall_ind::close, "close"},
        klib::pair<syscall_ind, klib::string> {syscall_ind::wait, "wait"},
        klib::pair<syscall_ind, klib::string> {syscall_ind::execve, "execve"},
        klib::pair<syscall_ind, klib::string> {syscall_ind::getpid, "getpid"},
        klib::pair<syscall_ind, klib::string> {syscall_ind::yield, "yield"}
    };


    auto it = function_names.find(ind);
    if (it == function_names.end())
        global_kernel->syslog()->info(
            "Syscall function index is %X: unknown function\n", ir.eax());
//    else
//        global_kernel->syslog()->info(
//            "Syscall function index is %X: %s\n", ir.eax(), it->second.c_str());

    switch (ind)
    {
        case syscall_ind::fork:
            // Create a copy of the current process.
            ret_val = syscalls::fork(ir, is);
            break;
        case syscall_ind::read:
            // Read from a file.
            ret_val = syscalls::read(ir.ebx(),
                reinterpret_cast<char*>(ir.ecx()), ir.edx());
            break;
        case syscall_ind::write:
            // Print to a file.
            ret_val = syscalls::write(ir.ebx(),
                reinterpret_cast<const char*>(ir.ecx()), ir.edx());
            break;
        case syscall_ind::open:
            // Open a file descriptor.
            ret_val = syscalls::open(reinterpret_cast<const char*>(ir.ebx()),
                static_cast<open_flags>(ir.ecx()), ir.edx());
            break;
        case syscall_ind::close:
            // Close a file descriptor.
            ret_val = syscalls::close(ir.ebx());
            break;
        case syscall_ind::wait:
            // Wait for a child process.
            ret_val = syscalls::wait(ir.ebx(), reinterpret_cast<int*>(ir.ecx()),
                ir.edx());
            break;
        case syscall_ind::execve:
            // Exec a new executable.
            ret_val = syscalls::execve(reinterpret_cast<const char*>(ir.ebx()),
                reinterpret_cast<char**>(ir.ecx()),
                reinterpret_cast<char**>(ir.edx()));
            break;
        case syscall_ind::getpid:
            // Get the curent process PID.
            ret_val = syscalls::getpid();
            break;
        case syscall_ind::yield:
            // Move onto the next process.
            ret_val = syscalls::yield(ir, is);
            break;
        default:
            global_kernel->syslog()->warn(
                "Unknown syscall function index %X\n", ir.eax());
    }

    // Put the return value in %eax.
//    global_kernel->syslog()->info("syscall retval = %u\n", ret_val);
    ir.set_eax(ret_val);
}

/******************************************************************************
 ******************************************************************************/

namespace syscalls {

/******************************************************************************
 ******************************************************************************/

int32_t fork(const InterruptRegisters& ir, const InterruptStack& is)
{
    // A new process will be created. The memory will be copied and the
    // registers set from the current interrupt, as the parent values aren't
    // updated. The new process is left as sleeping until ready. When first run,
    // it will go straight to user mode. The parent process will unwind the
    // interrupt.

    int32_t ret_val = 0;

    // Lookup the old process.
    size_t pid = global_kernel->get_scheduler().get_last();
    Process& old_p = *global_kernel->get_proc_table().get_process(pid);

    // This check shouldn't be necessary, but it's a good sanity check for now.
    if (old_p.get_status() != ProcStatus::active)
        global_kernel->panic("fork: Process with PID %d reported by scheduler is not the active process", pid);

    // Make a new blank process.
    Process* new_p = new Process {};

    // Duplicate the parent process.
    new_p->fork_duplicate(old_p);

    // Copy the registers and interrupt stack. Setting eax to 0 sets the return
    // value of the fork system call as seen by the child. Duplicate clears the
    // child PIDs, but can't set the PPID correctly.
    new_p->set_registers(ir);
    new_p->get_registers().set_eax(0);
    new_p->set_interrupt_stack(is);
    new_p->set_ppid(pid);

    // The new process is runnable and ready to go, but it won't get time until
    // it's in the ProcTable. Add the new process to the ProcTable. ret_val gets
    // the new PID, which is what the parent needs to return.
    ret_val = global_kernel->get_proc_table().insert(new_p);

    // 0 indicates failure. Probably out of PIDs.
    if (ret_val == 0)
    {
        // Delete the new process.
        delete new_p;
        ret_val = -1;
    }

    // Add the new process to the list of the old processes children.
    old_p.add_child(ret_val);

    return ret_val;
}

/******************************************************************************
 ******************************************************************************/

int32_t read(int fd, char* buf, size_t count)
{
    global_kernel->syslog()->info("read: fd = %d, buf at %p, count = %u\n", fd, buf, count);

    // We require the whole string to be in user space.
    if (reinterpret_cast<size_t>(buf) + count >= kernel_virtual_base)
    {
        global_kernel->syslog()->warn(
            "read syscall was given an address in kernel space.\n");
        return -1;
    }

    // Get the active process.
    Process* p = global_kernel->get_proc_table().get_process(
        global_kernel->get_scheduler().get_last());

    // Get the file description from the global table.
    int key = p->get_fd_key(fd);
    if (key == 0)
    {
        global_kernel->syslog()->warn("read syscall was given a file descriptor that does not exist for the process\n");
        return -1;
    }
    klib::fstream& f = global_kernel->get_file_table().get_stream(key);
    if (!f)
    {
        global_kernel->syslog()->warn(
            "read syscall file stream is not good\n");
        return -1;
    }

    // Wait until the file is ready.
    SignalManager::pollfd pfd {fd, PollType::pollin, PollType::pollnone};
    if(global_kernel->get_signal_manager()->poll(&pfd, 1, -1) <= 0)
    {
        global_kernel->syslog()->warn("read syscall poll failed\n");
        return -1;
    }

    // Read from file.
    f.read(buf, count);
    int32_t ret_val = f.gcount();
    global_kernel->syslog()->info("read gcount() is %d\n", ret_val);

    // Read sets fail on eof, but eof here is ok.
    if (f.eof())
        f.clear();
    else if (!f)
    {
        global_kernel->syslog()->warn("read syscall failed to read file\n");
        return -1;
    }

    return ret_val;
}

/******************************************************************************
 ******************************************************************************/

int32_t write(int fd, const char* buf, size_t count)
{
    global_kernel->syslog()->info("write: fd = %d, buf at %p, count = %u\n", fd, buf, count);

    // We require the whole string to be in user space.
    if (reinterpret_cast<size_t>(buf) + count >= kernel_virtual_base)
    {
        global_kernel->syslog()->warn(
            "write syscall was given an address in kernel space.\n");
        return -1;
    }

    // Get the active process.
    Process* p = global_kernel->get_proc_table().get_process(
        global_kernel->get_scheduler().get_last());

    // Get the file description from the global table.
    int key = p->get_fd_key(fd);
    if (key == 0)
    {
        global_kernel->syslog()->warn( "write syscall was given a file descriptor that does not exist for the process\n");
        return -1;
    }
    klib::fstream& f = global_kernel->get_file_table().get_stream(key);
    if (!f)
    {
        global_kernel->syslog()->warn(
            "write syscall file stream is not good\n");
        return -1;
    }

    // Wait until the file is ready.
    SignalManager::pollfd pfd {fd, PollType::pollout, PollType::pollnone};
    if(global_kernel->get_signal_manager()->poll(&pfd, 1, -1) < 0)
    {
        global_kernel->syslog()->warn( "write syscall poll failed\n");
        return -1;
    }

    klib::string tmp {buf, count};
    f << tmp;
    f.flush();
    return tmp.size();
}

/******************************************************************************
 ******************************************************************************/

int32_t open(const char* filename, open_flags flags, int mode)
{
    (void)mode;
    global_kernel->syslog()->info("open: filename = %s, open_flags = %u, mode = %d\n", filename, flags, mode);

    // We require the file name string to be in user space.
    size_t len = klib::strlen(filename);
    if (reinterpret_cast<size_t>(filename) + len >= kernel_virtual_base)
    {
        global_kernel->syslog()->warn(
            "open syscall was given a filename in kernel space.\n");
        return -1;
    }

    // Create a C++ style open mode.
    klib::ios_base::openmode m {};
    if ((flags & open_flags::rdonly) != open_flags::none)
        m |= klib::ios_base::in;
    if ((flags & open_flags::wronly) != open_flags::none)
        m |= klib::ios_base::out;
    if ((flags & open_flags::append) != open_flags::none)
        m |= klib::ios_base::app;
    if ((flags & open_flags::trunc) != open_flags::none)
        m |= klib::ios_base::trunc;

    // Get the active process.
    Process* p = global_kernel->get_proc_table().get_process(
        global_kernel->get_scheduler().get_last());

    // Forward the call to the process.
    int32_t ret_val = p->open_file(filename, m);

    if (ret_val == -1)
        global_kernel->syslog()->
            warn("open syscall process file open failed\n");

    return ret_val;
}

/******************************************************************************
 ******************************************************************************/

int32_t close(int fd)
{
    global_kernel->syslog()->info("close: fd = %d\n", fd);
    // Get the active process.
    Process* p = global_kernel->get_proc_table().get_process(
        global_kernel->get_scheduler().get_last());

    // Forward the call to the process.
    return p->close_file(fd);
}

/******************************************************************************
 ******************************************************************************/

int32_t wait(int pid, int* wstatus, int options)
{
    // We require wstatus to be in user space, or be nullptr.
    if (wstatus != nullptr &&
        reinterpret_cast<size_t>(wstatus + 1) > kernel_virtual_base)
        return -1;

    // Forward the call to the signal manager.
    int32_t ret_val =
        global_kernel->get_signal_manager()->waitpid(pid, wstatus, options);

    // It is the parent's job to clean up zombie processes that have been waited
    // for.
    if (ret_val != -1)
    {
        ProcTable& pt = global_kernel->get_proc_table();
        Process* child = pt.get_process(ret_val);
        if (child != nullptr && child->get_status() == ProcStatus::zombie)
            pt.erase(ret_val);
    }

    return ret_val;
}

/******************************************************************************
 ******************************************************************************/

int32_t execve(const char* filename, char* const argv[], char* const envp[])
{
    // Ignore argv and envp for now.
    (void)argv; (void)envp;

    // We require the file name string to be in user space.
    size_t len = klib::strlen(filename);
    if (reinterpret_cast<size_t>(filename) + len >= kernel_virtual_base)
    {
        global_kernel->syslog()->warn(
            "execve syscall was given a filename in kernel space.\n");
        return -1;
    }

    // Lookup the old process.
    size_t pid = global_kernel->get_scheduler().get_last();
    Process* old_p = global_kernel->get_proc_table().get_process(pid);
    Process* new_p = nullptr;
    
    // Make a block so destructors get called even if this function never ends.
    {

    // This check shouldn't be necessary, but it's a good sanity check for now.
    if (old_p->get_status() != ProcStatus::active)
        global_kernel->panic("getpid: Process with PID %d reported by scheduler is not the active process", pid);

    // We need to copy the arguments into kernel memory.
    klib::string tmp_filename {filename};

    // We need to block switching now. Otherwise if we swap out here we'll have
    // memory problems.
    switch_blocked_for_exec = true;

    // Now free up virtual memory so it's available for the new process.
    global_kernel->get_pdt()->clean_user_space(
        reinterpret_cast<void*>(kernel_virtual_base));

    // Create the process.
    new_p = new Process {tmp_filename};
    if (new_p->get_status() == ProcStatus::invalid)
    {
        // Something went wrong with creating the new process. We need to
        // restore the old one and bail.
        delete new_p;
        global_kernel->get_pdt()->update_user_space(old_p->get_pdt(),
            reinterpret_cast<void*>(kernel_virtual_base));
        return -1;
    }

    // We're about to delete the old process, but we're currently using it's
    // stack. Let's swap the stacks between the two processes.
    old_p->swap_stack(*new_p);

    // Swap the process table entries.
    if (!global_kernel->get_proc_table().set_process(pid, new_p))
        return -1;

    // Do what copying is necessary to the new process. So far, this is open
    // file descriptors, PPID and child PIDs.
    new_p->exec_duplicate(*old_p);

    // Clean up the old process. We're done with it now.
    delete old_p;

    // End block for destructors.
    }

    // Launch the new process
    if (new_p)
        new_p->launch(*global_kernel->get_pdt());

    // If we get here, something went wrong launching the process.
    return -1;
}

/******************************************************************************
 ******************************************************************************/

int32_t getpid()
{
    // For now, check that the process reported by the scheduler is the active
    // one. This should be unnecessary.
    int32_t ret_val = global_kernel->get_scheduler().get_last();

    if (global_kernel->get_proc_table().get_process(ret_val)->get_status() !=
        ProcStatus::active)
        global_kernel->panic("getpid: Process with PID %d reported by scheduler is not the active process", ret_val);

    return ret_val;
}

/******************************************************************************
 ******************************************************************************/

int32_t yield(const InterruptRegisters& ir, const InterruptStack& is)
{
    // Call the scheduler to do the swapping.
    global_kernel->get_scheduler().next_proc(ir, is);
    // We will return here once the process gets time again. Return success.
    return 0;
}

/******************************************************************************
 ******************************************************************************/

} // namespace syscalls
