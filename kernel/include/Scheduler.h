#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>

#include "InterruptHandler.h"

// We have a set of flags for blocking a swtich for various reasons. These are
// global and have C linkage, so we can switch them from assembly.

/**
    Flag to block task switching if a task switch was already in progress.
 */
extern "C" bool switch_blocked_for_switch;

/**
    Flag to block task switching if we're still in kernel initialisation.
 */
extern "C" bool switch_blocked_for_init;

/**
    Flag to block task switching in the middle of an exec.
 */
extern "C" bool switch_blocked_for_exec;

/**
    Flag to block task switching during an event notification.
 */
extern "C" bool switch_blocked_for_notify;

/**
    Abstract base class for schedulers.
 */
class Scheduler {
public:
    /**
        Get the last process allocated time by the scheduler.

        @return Last process to get time from the scheduler.
     */
    virtual size_t get_last() const { return current_proc; }

    /**
        Switch to the next scheduled process. We need the state of the old
        process so we can save it if we swap out.

        @param ir General purpose registers of the current process.
        @param is Interrupt stack state of the current process.
     */
    virtual void next_proc(const InterruptRegisters& ir,
        const InterruptStack& is) = 0;

    /**
        Starts running the init process.

        @param init PID of the init process.
     */
    virtual void start(size_t init) = 0;

    /**
        Give up remaining time and start the next process.
     */
    virtual void yield() = 0;

protected:
    // Last process PID given time by the scheduler.
    size_t current_proc = 0;
};

/**
    Simple round robin scheduler with equal priorities.
 */
class RoundRobin : public Scheduler {
public:
    /**
        Switch to the next scheduled process. We need the state of the old
        process so we can save it if we swap out.

        @param ir General purpose registers of the current process.
        @param is Interrupt stack state of the current process.
     */
    virtual void next_proc(const InterruptRegisters& ir,
        const InterruptStack& is) override;

    /**
        Starts running the init process.

        @param init PID of the init process.
     */
    virtual void start(size_t init) override;

    /**
        Give up remaining time and start the next process.
     */
    virtual void yield() override;
};

/**
    Assembly function to execute a syscall interrupt with code indicating yield.
 */
extern "C"
void asm_yield();

#endif /* SCHEDULER_H */
