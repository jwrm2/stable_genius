#include "InterruptHandler.h"

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

#include "Kernel.h"
#include "Keyboard.h"
#include "Logger.h"
#include "paging.h"
#include "Pic.h"
#include "Pit.h"
#include "Process.h"
#include "ProcTable.h"
#include "Scheduler.h"
#include "SignalManager.h"
#include "Syscall.h"

/******************************************************************************
 ******************************************************************************/

uint32_t interrupt_handler(uint32_t edi,
                       uint32_t esi,
                       uint32_t ebp,
                       uint32_t esp,
                       uint32_t ebx,
                       uint32_t edx,
                       uint32_t ecx,
                       uint32_t eax,
                       uint32_t irr,
                       uint32_t err,
                       uint32_t eip,
                       uint16_t cs,
                       uint32_t eflags,
                       uint32_t esp_int,
                       uint16_t ss)
{
    uint32_t ret_val = 0;

    try {
        InterruptRegisters ireg {edi, esi, ebp, esp, ebx, edx, ecx, eax};
        InterruptStack istack {err, eip, cs, eflags, esp_int, ss};
        InterruptNumber inum = static_cast<InterruptNumber>(irr);

        if (inum != InterruptNumber::pit && inum != InterruptNumber::syscall)
        {
    /*        global_kernel->syslog()->info("Interrupt raised\n");
            global_kernel->syslog()->info("edi = %X\n", edi);
            global_kernel->syslog()->info("esi = %X\n", esi);
            global_kernel->syslog()->info("ebp = %X\n", ebp);
            global_kernel->syslog()->info("esp after = %X\n", esp);
            global_kernel->syslog()->info("ebx = %X\n", ebx);
            global_kernel->syslog()->info("edx = %X\n", edx);
            global_kernel->syslog()->info("ecx = %X\n", ecx);
            global_kernel->syslog()->info("eax = %X\n", eax);*/
//            global_kernel->syslog()->info("interrupt number is %X\n", irr);
    /*        global_kernel->syslog()->info("code = %X\n", err);
            global_kernel->syslog()->info("eip = %X\n", eip);
            global_kernel->syslog()->info("cs = %X\n", cs);
            global_kernel->syslog()->info("eflags = %X\n", eflags);
            global_kernel->syslog()->info("esp before = %X\n", esp_int);
            global_kernel->syslog()->info("ss = %X\n", ss);*/
        }

        switch(inum)
        {
        case InterruptNumber::inv_op:
            InvOpHandler{ireg, istack, inum}.handle();
            break;
        case InterruptNumber::gpf:
            GpfHandler{ireg, istack, inum}.handle();
            break;
        case InterruptNumber::page_fault:
            PageFaultHandler{ireg, istack, inum}.handle();
            break;
        case InterruptNumber::pit:
            PitHandler{ireg, istack, inum}.handle();
            break;
        case InterruptNumber::ps2_keyboard:
            Ps2KeyboardHandler{ireg, istack, inum}.handle();
            break;
        case InterruptNumber::syscall:
            SysCallHandler{ireg, istack, inum}.handle();
            // The return value gets put into ireg.eax(). We return it from this
            // function, which puts it in eax in assembly, whence we load it
            // into the stack for popping.
            ret_val = ireg.eax();
            break;
        default:
            DefaultHandler{ireg, istack, inum}.handle();
        }

//        if (inum != InterruptNumber::pit && inum != InterruptNumber::syscall)
//        {
//            global_kernel->syslog()->info("Interrupt %X concluded\n", irr);
//        }
    }
    catch (klib::exception& e)
    {
        global_kernel->panic(
            "Uncaught exception during interrupt handling: %s\n", e.what());
    }
    catch (...)
    {
        global_kernel->panic(
            "Uncaught unknown exception during interrupt handling: %s\n");
    }

    return ret_val;
}

/******************************************************************************
 ******************************************************************************/

void DefaultHandler::handle()
{
    if (inum == InterruptNumber::pic1_end)
    {
        // Need to not send an ack if the master has sent a spurious IRQ
        uint8_t isr = global_kernel->get_pic()->get_isr(PicType::master);
        if (! (isr & static_cast<uint8_t>(PicMask::master_lpt1)))
            return;
    }
    else if(inum == InterruptNumber::pic2_end)
    {
        // Need to send an ack to the master but not the slave if the slave has
        // sent a spurious IRQ
        uint8_t isr = global_kernel->get_pic()->get_isr(PicType::slave);
        if (! (isr & static_cast<uint8_t>(PicMask::slave_ata2)))
        {
            global_kernel->get_pic()->acknowledge(PicType::master);
            return;
        }
    }

    if (inum >= InterruptNumber::pic1_start &&
        inum <= InterruptNumber::pic1_end)
        global_kernel->get_pic()->acknowledge(PicType::master);
    else if (inum >= InterruptNumber::pic2_start && 
             inum <= InterruptNumber::pic2_end)
        global_kernel->get_pic()->acknowledge(PicType::slave);
}

/******************************************************************************
 ******************************************************************************/

void PitHandler::handle()
{
    // Do nothing if the PIT driver is not yet set up.
    if (global_kernel->get_pit() == nullptr)
        return;

    // Advance the tick.
    Pit* pit {global_kernel->get_pit()};
    pit->tick();

    // Tick down pending event timeouts, if the signal manager exists.
    if (global_kernel->get_signal_manager() != nullptr)
        global_kernel->get_signal_manager()->tick_down(pit->period());

    // Send acknowledgement.
    DefaultHandler::handle();

    // If task switching is not blocked for some reason, we call the scheduler
    // to switch to the next process.
    if (!switch_blocked_for_switch && !switch_blocked_for_init &&
        !switch_blocked_for_exec && !switch_blocked_for_notify)
    {
        switch_blocked_for_switch = true;
        global_kernel->get_scheduler().next_proc(ir, is);
        switch_blocked_for_switch = false;
    }
}

/******************************************************************************
 ******************************************************************************/

void Ps2KeyboardHandler::handle()
{
//    global_kernel->syslog()->info("Keyboard interrupt raised\n");
    global_kernel->get_keyboard()->read_key();
//    global_kernel->syslog()->info("Finished reading key\n");
    DefaultHandler::handle();
}

/******************************************************************************
 ******************************************************************************/

void SysCallHandler::handle()
{
    // Pass to the syscall interface.
    syscall(ir, is);
}

/******************************************************************************
 ******************************************************************************/

void InvOpHandler::handle()
{
    klib::string msg {"Invalid Opcode"};
    global_kernel->panic(msg.c_str());
}

/******************************************************************************
 ******************************************************************************/

void GpfHandler::handle()
{
    klib::string msg {"General Protection Fault error code 0x"};
    klib::string code = klib::helper::itostr(is.code(), 16);
    msg += code;
    global_kernel->panic(msg.c_str());
}

/******************************************************************************
 ******************************************************************************/

void PageFaultHandler::handle()
{
    // If the page fault has come from user mode, and it's an attempted user
    // space access, we can try to expand the stack.
    if (is.code() & 0x4 && get_cr2() < kernel_virtual_base)
    {
        Process* p = global_kernel->get_proc_table().get_process(
            global_kernel->get_scheduler().get_last());
        size_t new_size = kernel_virtual_base - get_cr2();
        if (p != nullptr && p->set_user_stack(new_size) == 0)
        {
            // We've succeeded in expanding the stack. We can safely return to
            // user mode.
            return;
        }
        // We've failed to expand the stack. This either means it wasn't a stack
        // overflow to start with, we've run out of permitted stack space, or
        // we've run out of physical memory. Whichever, continue with the error.
    }

    klib::string msg {"Page Fault:\n"};

    // Violation or not present
    if (is.code() & 0x1)
        msg += "  Page protection violation\n";
    else
        msg += "  Page not present\n";

    // Read or write
    if (is.code() & 0x2)
        msg += "  Attempted write\n";
    else
        msg += "  Attempted read\n";

    // User or kernel
    if (is.code() & 0x4)
        msg += "  Fired from user mode\n";
    else
        msg += "  Fired from kernel mode\n";

    // Reserved write
    if (is.code() & 0x8)
        msg += "  Writing 1 in reserved field\n";

    // Instruction fetch
    if (is.code() & 0x10)
        msg += "  Instruction fetch\n";

    // Instruction address
    msg += "  Caused by instruction 0x" + klib::helper::itostr(is.eip(), 16) +
        "\n";

    // Access address
    msg += "  Attempt to access 0x" + klib::helper::itostr(get_cr2(), 16) +
        "\n";

    global_kernel->panic(msg.c_str());
}

/******************************************************************************
 ******************************************************************************/
