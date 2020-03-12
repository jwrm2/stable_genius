#include "Process.h"

#include <stdint.h>

#include <algorithm>
#include <cstring>
#include <cxxabi>
#include <new>
#include <string>

#include "Elf.h"
#include "Gdt.h"
#include "Kernel.h"
#include "Logger.h"
#include "paging.h"
#include "PageDescriptorTable.h"
#include "ProcTable.h"
#include "Scheduler.h"
#include "SignalManager.h"

/******************************************************************************
 ******************************************************************************/

Process::Process(const klib::string& name) :
    entry_point{},
    pdt{nullptr},
    current_stack{start_stack},
    kernel_stack{nullptr},
    break_point{nullptr},
    stat{ProcStatus::sleeping},
    is{},
    ir{},
    eh_globals{nullptr},
    elf {name},
    file_desc {},
    ret_val {},
    parent_pid {},
    child_pids {},
    pdt_changed {false}
{
//    elf.dump(*global_kernel->syslog()->stream());

    // Check validity of binary.
    if (!elf.valid())
    {
        global_kernel->syslog()->warn("Invalid ELF binary %s\n", name.c_str());
        stat = ProcStatus::invalid;
        return;
    }

    // Set the entry and break points.
    entry_point = elf.get_file_header().get_entry();
    break_point = elf.get_break_point();
}

/******************************************************************************/

Process::Process() :
    entry_point{},
    pdt{new PageDescriptorTable{}},
    current_stack{0},
    kernel_stack{new uintptr_t[kernel_stack_size / sizeof(kernel_stack)]},
    break_point{nullptr},
    stat{ProcStatus::sleeping},
    is{},
    ir{},
    eh_globals{},
    elf {},
    file_desc {},
    ret_val {},
    parent_pid {},
    child_pids {},
    pdt_changed {false}
{
    // This constructor is specifically designed to set the process up to be
    // duplicated in a fork call. Thus we create a new PDT but leave it blank,
    // we create a kernel stack, leave ELF headers blank, set the registers to
    // zero and the status to sleeping (to make sure it doesn't get time until
    // after the duplication).
}

/******************************************************************************/

Process::Process(Process&& other) :
    entry_point {other.entry_point},
    pdt {other.pdt},
    current_stack {other.current_stack},
    kernel_stack {other.kernel_stack},
    break_point {other.break_point},
    stat {other.stat},
    is {other.is},
    ir {other.ir},
    eh_globals {other.eh_globals},
    elf {other.elf},
    file_desc {other.file_desc},
    ret_val {other.ret_val},
    parent_pid {other.parent_pid},
    child_pids {klib::move(other.child_pids)},
    pdt_changed {other.pdt_changed}
{
    // Set the pointers in other to nullptr. Prevents the other destructor from
    // messing this up.
    other.pdt = nullptr;
    other.kernel_stack = nullptr;
    other.eh_globals = nullptr;
}

/******************************************************************************/

Process& Process::operator=(Process&& other)
{
    // Free pointers of this process.
    // Let's just hope we're not currently using this process's kernel stack.
    global_kernel->get_pdt()->free_user_space(
        reinterpret_cast<void*>(kernel_virtual_base));
    delete pdt;
    delete kernel_stack;
    delete eh_globals;

    // Pirate data from other process.
    entry_point = other.entry_point;
    pdt = other.pdt;
    current_stack = other.current_stack;
    kernel_stack = other.kernel_stack;
    break_point = other.break_point;
    stat = other.stat;
    is = other.is;
    ir = other.ir;
    eh_globals = other.eh_globals;
    elf = other.elf;
    file_desc = other.file_desc;
    ret_val = other.ret_val;
    parent_pid = other.parent_pid;
    child_pids = klib::move(other.child_pids);
    pdt_changed = other.pdt_changed;

    // Set the pointers in other to nullptr. Prevents the other destructor from
    // messing this up.
    other.pdt = nullptr;
    other.kernel_stack = nullptr;
    other.eh_globals = nullptr;

    return *this;
}

/******************************************************************************/

Process::~Process()
{
    // If the ELF was invalid, we won't have created at PDT.
    if (pdt)
    {
        // Free any mappings and physical memory used in user space.
        global_kernel->get_pdt()->free_user_space(
            reinterpret_cast<void*>(kernel_virtual_base));

        // Now just delete the pdt. This doesn't delete any PTs, so the kernel
        // should be ok.
        delete pdt;
    }

    // Remove all pending signals.
    global_kernel->get_signal_manager()->purge_process(this);

    // Close all file descriptors. This will only flush if it's the last
    // reference to the file.
    close_file(-1);

    // Free the kernel stack.
    // TODO: think about whether this is a good idea. We might end up freeing
    // the stack currently in use. I think I've set up exec so it's ok.
    delete[] kernel_stack;

    delete eh_globals;
}

/******************************************************************************/

void Process::exec_duplicate(const Process& other)
{
    // Copy the open file descriptors. This should just increment the file
    // description reference counts.
    file_desc = other.file_desc;
    for (const auto& p : file_desc)
        global_kernel->get_file_table()->copy_file(p.second);

    // Copy the parent and child PIDs.
    parent_pid = other.parent_pid;
    child_pids = other.child_pids;
}

/******************************************************************************/

void Process::fork_duplicate(const Process& other)
{
    // Duplicates everything from the other process apart from the status,
    // kernel stack, exception globals and PPID. Clears the child PID list.
    // Essentially a copy, but I want better control by deleting the copy
    // constructor and assignment.

    // Copy the interrupt registers and stack.
    is = other.is;
    ir = other.ir;

    // Copy the size of the user space stack.
    current_stack = other.current_stack;

    // The entry point is irrelevant for now, but we might as copy it.
    entry_point = other.entry_point;

    // The memory is complicated, as we want to copy all the pages. Delegate to
    // the PDT. This copies from the currently loaded PDT, so it only works if
    // the other process is active.
    pdt->duplicate_user_space(reinterpret_cast<void*>(kernel_virtual_base));
    break_point = other.break_point;
    pdt_changed = true;

    // Duplicate the file description table. We need to increment the counts of
    // each entry in the global table.
    file_desc = other.file_desc;
    for (const auto& p : file_desc)
        global_kernel->get_file_table()->copy_file(p.second);

    // Duplicate other process information. ret_val should be meaningless so
    // far.
    ret_val = other.ret_val;

    // The PPID should be the PID of the other process, but processes don't know
    // their own PIDs, so we can't do that here. It's taken care of in the fork
    // syscall afterwards. 1 is a 'safe' value, since it refers to init.
    parent_pid = 1;

    // Wipe the child PID list.
    child_pids.clear();

    // Status becomes runnable. The other process should be active. This won't
    // actually get time until it's added to the process table.
    stat = ProcStatus::runnable;
}

/******************************************************************************/

void Process::swap_stack(Process& other)
{
    klib::swap(other.kernel_stack, kernel_stack);
}

/******************************************************************************/

void Process::launch(PageDescriptorTable& k_pdt)
{
    // Return if the ELF is not valid.
    if (!elf.valid())
    {
        global_kernel->syslog()->warn("Invalid ELF in process launch.\n");
        return;
    }

    // Create some space for the binary. We don't care where it is physically
    // and we'll read the virtual addresses from the program header. We really
    // want to create these in the new PDT, but creating them makes new PTs
    // that need to go on the kernel heap. We'll allocate them all in the kernel
    // PDT, copy to new one, then clean the kernel PDT.
    elf.allocate(k_pdt);

    // The page configuration must be set to present and user mode. It does need
    // to be writable to copy the data to it. We might consider setting .text
    // and .rodata to unwritable later.
    uint32_t conf = static_cast<uint32_t>(PdeSettings::present) | 
        static_cast<uint32_t>(PdeSettings::writable) |
        static_cast<uint32_t>(PdeSettings::user_access);

    // Allocate pages for the stack.
    for (uintptr_t i = kernel_virtual_base - current_stack;
        i < kernel_virtual_base;
        i += PageDescriptorTable::page_size)
    {
        if (!k_pdt.allocate(reinterpret_cast<void*>(i), conf))
        {
            global_kernel->panic(
                "Failed to allocate virtual memory for new process.\n");
            return;
        }
    }
    pdt_changed = true;

    // Allocate kernel stack space.
    kernel_stack = new uintptr_t[kernel_stack_size / sizeof(kernel_stack)];

    // We need to make sure the PDT is 4K aligned.
    pdt = static_cast<PageDescriptorTable*>(
        global_kernel->get_heap()->malloc(sizeof(PageDescriptorTable),
        PageDescriptorTable::page_size));
    // Copy from the existing kernel PDT.
    pdt = new (pdt) PageDescriptorTable{k_pdt};

    // Now clean all the user space mappings from the kernel PDT.
//    k_pdt.clean_user_space(reinterpret_cast<void*>(kernel_virtual_base));

    // Set up the interrupt stack state. The values of parity etc. in eflags
    // doesn't really matter. We'll get sensible values for the reserved ones by
    // using the current values.
    is = InterruptStack{0,
        entry_point,
        global_kernel->get_gdt().user_mode_cs().val(),
        get_eflags(),
        kernel_virtual_base - sizeof(uintptr_t),
        global_kernel->get_gdt().user_mode_ds().val()
        };

    // Merge user space mappings for this process into the kernel page
    // descriptor table.
//    global_kernel->get_pdt()->update_user_space(*pdt,
//        reinterpret_cast<void*>(kernel_virtual_base));

    // Copy the binary from the ELF to the virtual mapping we created for it.
    elf.load();
//    global_kernel->syslog()->info("Binary copied.\n");

    // Set the state to active so we don't reload the PDT in resume().
    stat = ProcStatus::active;

    // Unblock task switching for kernel init. I think it's safe here: if we
    // switch between now and actually launching init, we'll just end up
    // launching init.
    switch_blocked_for_init = false;

    // Unblock task switching for exec. If we swap out now, we should just be
    // able to start normally with the resume.
    switch_blocked_for_exec = false;

    // Starting the process is now the same as resuming.
//    global_kernel->syslog()->info("Launching process.\n");
    resume();
}

/******************************************************************************/

void Process::resume()
{
    // Reload the PDT for this process.
    // This is fairly expensive. We'll skip it if this is already the active
    // process. However, we still need to do it if we've been making adjustments
    // to the PDT.
    if (stat != ProcStatus::active || pdt_changed)
    {
        global_kernel->get_pdt()->update_user_space(*pdt,
            reinterpret_cast<void*>(kernel_virtual_base));
        pdt_changed = false;
    }

//    global_kernel->get_pdt()->dump(*global_kernel->syslog()->device());
//    global_kernel->syslog()->info("process resume %%eip, %%cs, %%eflags, %%esp, %%ss is %X, %X, %X, %X, %X\n", is.eip(), is.cs(), is.eflags(), is.esp(), is.ss());
//    global_kernel->syslog()->info("process resume %%edi, %%esi, %%ebp, %%ebx, %%edx, %%ecx, %%eax is %X, %X, %X, %X, %X, %X, %X\n", ir.edi(), ir.esi(), ir.ebp(), ir.ebx(), ir.edx(), ir.ecx(), ir.eax());

    if (is.eip() >= kernel_virtual_base)
    {
        // Set the TSS. This isn't actually necessary yet, but it needs to be
        // ready for when whatever syscall the process was executing irets back
        // to user mode.
        global_kernel->get_tss().set_esp(kernel_stack +
        kernel_stack_size / sizeof(kernel_stack) - sizeof(uintptr_t));
        // Update the status.
        stat = ProcStatus::active;
        // Transfer to assembly to iret.
        launch_kernel_process(ir.edi(),
                              ir.esi(),
                              ir.ebp(),
                              0, // placeholder for esp, that gets ignored
                              ir.ebx(),
                              ir.edx(),
                              ir.ecx(),
                              ir.eax(),
                              is.eip(),
                              is.cs(),
                              is.eflags(),
                              is.esp());
    }
    else
    {
        // Set the TSS.
        global_kernel->get_tss().set_esp(kernel_stack +
        kernel_stack_size / sizeof(kernel_stack) - sizeof(uintptr_t));
        // Update the status.
        stat = ProcStatus::active;
        // Transfer to assembly to iret.
        launch_process(ir.edi(),
                       ir.esi(),
                       ir.ebp(),
                       0, // placeholder for esp, that gets ignored
                       ir.ebx(),
                       ir.edx(),
                       ir.ecx(),
                       ir.eax(),
                       is.eip(),
                       is.cs(),
                       is.eflags(),
                       is.esp(),
                       is.ss());
    }
}

/******************************************************************************/

int Process::open_file(const klib::string& name, klib::ios_base::openmode mode)
{
    // Forward to the global file table.
    int ret_val = global_kernel->get_file_table()->open_file(name, mode);

    // Failure.
    if (ret_val == -1)
        return -1;

    // Success. Add to the process file descriptor list.
    int key;
    for (key = 0; file_desc.find(key) != file_desc.end(); ++key) ;
    file_desc.insert(klib::pair<int, int> {key, ret_val});

    return key;
}

/******************************************************************************/

int Process::close_file(int fd)
{
    int ret_val = 0;

    if (fd == -1)
    {
        // Close all files.
        for (const auto& p : file_desc)
        {
            int ret_val2 =
                global_kernel->get_file_table()->close_file(p.second);
            if (ret_val2 == -1)
                ret_val = -1;
        }
        file_desc.clear();
    }
    else
    {
        // Find the file description key.
        auto it = file_desc.find(fd);
        if (it == file_desc.end())
            ret_val = -1;
        else
        {
            // Close single file.
            ret_val =
                global_kernel->get_file_table()->close_file(it->second);
            file_desc.erase(fd);
        }
    }

    return ret_val;
}

/******************************************************************************/

int Process::get_fd_key(int fd)
{
    auto it = file_desc.find(fd);
    if (it == file_desc.end())
        return 0;
    return it->second;
}

/******************************************************************************/

int Process::set_user_stack(size_t sz)
{
    // Do nothing successfully if the requested size is smaller than the current
    // size.
    if (sz < current_stack)
        return 0;

    // Round sz up to an integer number of pages.
    sz = sz - sz % PageDescriptorTable::page_size +
        PageDescriptorTable::page_size;

    // Fail if the requested size is larger than the current maximum size.
    if (sz > max_stack)
        return -1;

    // Fail if the new size would clash with the heap, ie. we have run out of
    // virtual address space.
    if (kernel_virtual_base - sz < reinterpret_cast<uintptr_t>(break_point))
        return -1;

    // The page configuration must be set to present, user mode and
    // writable.
    uint32_t conf = static_cast<uint32_t>(PdeSettings::present) | 
        static_cast<uint32_t>(PdeSettings::writable) |
        static_cast<uint32_t>(PdeSettings::user_access);

    // Increase the current stack size to match the new size.
    while (current_stack < sz)
    {
        void* new_stack = reinterpret_cast<void*>(kernel_virtual_base -
            current_stack - PageDescriptorTable::page_size);
        if (!pdt->allocate(new_stack, conf))
            return -1;
        pdt_changed = true;
        current_stack += PageDescriptorTable::page_size;
    }

    // Update the kernel PDT.
    if (pdt_changed)
    {
        global_kernel->get_pdt()->update_user_space(*pdt,
            reinterpret_cast<void*>(kernel_virtual_base));
        pdt_changed = false;
    }

    return 0;
}

/******************************************************************************/

int Process::brk(void* addr)
{
    uintptr_t v_addr = reinterpret_cast<uintptr_t>(addr);
    uintptr_t v_bp = reinterpret_cast<uintptr_t>(break_point);

    // If addr is zero, we return the current break point instead.
    if (v_addr == 0)
        return reinterpret_cast<int>(break_point);

    if (addr < elf.get_break_point())
        return -1;

    // Round the requested address up to a page boundary.
    uintptr_t addr_top = v_addr - v_addr % PageDescriptorTable::page_size +
        PageDescriptorTable::page_size;
    if (addr_top >= kernel_virtual_base - current_stack)
        return -1;

    // Round the current address down to the highest allocated page start.
    uintptr_t break_bot;
    if (v_bp % PageDescriptorTable::page_size == 0)
        break_bot = v_bp - PageDescriptorTable::page_size;
    else
        break_bot = v_bp - v_bp % PageDescriptorTable::page_size;

    // The page configuration must be set to present, user mode and
    // writable.
    uint32_t conf = static_cast<uint32_t>(PdeSettings::present) | 
        static_cast<uint32_t>(PdeSettings::writable) |
        static_cast<uint32_t>(PdeSettings::user_access);

    // Test whether we need to allocate more pages.
    while (v_addr > break_bot + PageDescriptorTable::page_size)
    {
        if (!pdt->allocate(
            reinterpret_cast<void*>(break_bot + PageDescriptorTable::page_size),
            conf))
            return -1;
        break_bot += PageDescriptorTable::page_size;
        pdt_changed = true;
    }
    // Test whether we need to deallocate pages.
    while (addr_top <= v_bp)
    {
        pdt->free(reinterpret_cast<void*>(v_bp));
        v_bp -= PageDescriptorTable::page_size;
        pdt_changed = true;
    }

    break_point = static_cast<uintptr_t*>(addr);

    // Update the kernel PDT.
    if (pdt_changed)
    {
        global_kernel->get_pdt()->update_user_space(*pdt,
            reinterpret_cast<void*>(kernel_virtual_base));
        pdt_changed = false;
    }

    return 0;
}

/******************************************************************************/

void Process::add_child(size_t pid)
{
    // The only check we currently do is to not add duplicate values.
    if (klib::find(child_pids.begin(), child_pids.end(), pid) ==
        child_pids.end())
        child_pids.push_back(pid);
}

/******************************************************************************
 ******************************************************************************/
