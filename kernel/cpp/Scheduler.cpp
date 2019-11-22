#include "Scheduler.h"

#include <stddef.h>

#include "Kernel.h"
#include "Logger.h"
#include "Process.h"
#include "ProcTable.h"

/******************************************************************************
 ******************************************************************************/

bool switch_blocked_for_switch = false;
bool switch_blocked_for_init = true;
bool switch_blocked_for_exec = false;
bool switch_blocked_for_notify = false;

/******************************************************************************
 ******************************************************************************/

void RoundRobin::next_proc(const InterruptRegisters& ir,
        const InterruptStack& is)
{
//    global_kernel->syslog()->info("RoundRobin: beginning next_proc\n");
    // TODO Timed sleeping processes will have their clocks decremented here.

    // Reference to the process table, for convenience.
    ProcTable& tab = global_kernel->get_proc_table();

    if (tab.empty())
        // This should only ever happen if init has exited. Basically, it's time
        // to shutdown.
        global_kernel->shutdown();

    // This loop means that if we don't find a runnable process in the table,
    // we'll just keep looping here.
    auto it = tab.find(current_proc);
    for (++it; true; ++it)
    {
        if (it == tab.end())
            it = tab.begin();

        // Select the first runnable process. Active means we've looped back to
        // the previously running process, which is also fine.
        if (it->second->get_status() == ProcStatus::runnable ||
            it->second->get_status() == ProcStatus::active)
            break;
    }

    // If we're changing process, swap out the old one and swap in the new
    // one.
    if (it->second->get_status() != ProcStatus::active)
    {
        tab.swap_out(current_proc, ir, is);
        current_proc = it->first;
        if(!tab.swap_in(current_proc))
            // Swapping the process in failed for some reason. Panic for
            // now.
            global_kernel->panic("Failed to swap in process with PID %d",
                current_proc);
    }

    // If we aren't changing process, we don't need to swap anything. The
    // interrupt can just unwind and iret to the active process.

//    global_kernel->syslog()->info("RoundRobin: ending next_proc\n");
}

/******************************************************************************/

void RoundRobin::start(size_t init)
{
    // Reference to the process table, for convenience.
    ProcTable& tab = global_kernel->get_proc_table();

    // Check the init process exists.
    if (tab.count(init) == 0)
        global_kernel->panic(
            "Scheduler told to start init process with non-existent PID.");

    // Launch the process.
    current_proc = init;
    tab.get_process(init)->launch(*global_kernel->get_pdt());

    // We'll only reach here if we failed to launch the user mode process.
    global_kernel->panic("RoundRobin::start Failed to launch init process");
}

/******************************************************************************/

void RoundRobin::yield()
{
    // We won't fiddle with the state of the process. It may or not have been
    // set to sleep. We will assume that we want to switch even if there are
    // flags saying not to, which therefore need to be set to false or we'll
    // never switch again.
    switch_blocked_for_switch = false;
    switch_blocked_for_init = false;
    switch_blocked_for_exec = false;
    switch_blocked_for_notify = false;

    // We'll need the register states to resume. The framework makes this
    // easiest with an interrupt. Transfer to assembly to interrupt.
    asm_yield();
}

/******************************************************************************
 ******************************************************************************/
