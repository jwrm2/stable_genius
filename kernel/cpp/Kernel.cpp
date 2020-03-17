#include "Kernel.h"

#include <cstdlib>
#include <cxxabi>
#include <exception>
#include <map>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "DevFileSystem.h"
#include "DiskPartition.h"
#include "File.h"
#include "FileSystem.h"
#include "Gdt.h"
#include "Ide.h"
#include "Idt.h"
#include "interrupt.h"
#include "KernelHeap.h"
#include "Keyboard.h"
#include "Logger.h"
#include "MemoryFileSystem.h"
#include "MultiBoot.h"
#include "PageDescriptorTable.h"
#include "PageFrameAllocator.h"
#include "paging.h"
#include "Pci.h"
#include "Pic.h"
#include "Pit.h"
#include "Process.h"
#include "ProcTable.h"
#include "Ps2Controller.h"
#include "Scheduler.h"
#include "Serial.h"
#include "SignalManager.h"

/******************************************************************************
 ******************************************************************************/

Kernel* global_kernel = nullptr;

/******************************************************************************
 ******************************************************************************/

Kernel::Kernel(void* kvs,
               void* kve,
               void* kps,
               void* kpe,
               void* void_pdt,
               void* tpt,
               void* mbp) :
    virtual_start{kvs},
    virtual_end{kve},
    physical_start{kps},
    physical_end{kpe}
{
    // Set the global kernel pointer.
    global_kernel = this;

    // Make a temporary serial port and system log, for use until the heap is
    // up and running.
    Serial ser {SerialAddress::com1};
    Logger tmp_log {ser};
    log = &tmp_log;

    log->write("----- Stable Genius OS initialising -----\n");
    log->info("Initialised temporary serial port driver for COM1 at %p\n",
        &ser);
    log->info("Set system log to device at %p\n", log->device());

    // Dump initial state information.
    dump_address();

    // Set up paging stuff.
    default_paging(void_pdt, tpt);

    // Create a heap.
    default_heap();

    // Now we can do exceptions.
    __cxxabiv1::__register_frame(&start_eh_frame);
    try {
        // Create a GDT. Up to this point we've been relying on the one created
        // by the bootloader. Possibly a bit dodgy, but we want the GDT on the
        // heap.
        default_gdt();

        // Initialise the dev file system.
        default_dev();

        // Set the log to the first serial port, through the dev file system.
        default_logger();

        // Populate the multiboot information.
        read_multiboot(mbp);

        // Set up the PIC controller.
        default_pic();

        // Create an IDT.
        default_idt();

        // Set up the PIT driver and start timing.
        default_pit();

        // Set up the PS/2 controller.
        default_ps2();

        // Set up the PS/2 keyboard.
        default_keyboard();

        // Get the list of PCI devices.
        default_pci();

        // Set up the IDE drivers.
        default_ide();

        // Mount root partition.
        default_root();

        // Read init process and create the process table.
        default_proc_table();

        // Create a scheduler.
        default_scheduler();

        // Enable interrupts.
        default_enable();
    }
    catch (klib::exception& e)
    {
        panic("Uncaught exception during kernel initialisation: %s\n",
            e.what());
    }
    catch (...)
    {
        panic("Uncaught unknown exception during kernel initialisation\n");
    }

    log->info("Kernel initialisation complete\n");
    klib::ofstream tty {"/dev/tty"};
    tty << "Kernel initialisation complete.\n";
}

/******************************************************************************/

Kernel::~Kernel()
{
    panic("Destructing kernel");
}

/******************************************************************************/

KernelHeap* Kernel::get_heap()
{
    return (heap.get_start() == nullptr ? nullptr : &heap);
}

/******************************************************************************/

void Kernel::panic(const char* fmt, ...)
{
    // Write to the system log, if it exists.
    if (syslog() != nullptr)
    {
        syslog()->error(" *** PANIC *** ");
        // Initialise the variable argument list.
        va_list arg;
        va_start(arg, fmt);
        // Write the error message.
        syslog()->vwrite(fmt, arg);
        // Clean up.
        va_end(arg);
        syslog()->write("\n");
    }
    // Write to the TTY, if it exists.
    klib::ofstream tty {"/dev/tty"};
    if (tty)
    {
        tty << "\n *** PANIC *** \n";
        // Initialise the variable argument list.
        va_list arg;
        va_start(arg, fmt);
        // Convert to a string.
        klib::string msg;
        klib::helper::vstrprintf(msg, fmt, arg);
        // Clean up.
        va_end(arg);
        // Write the error message.
        tty << msg << '\n';
        tty.close();
    }

    // Loop forever. Panics are not recoverable.
    default_disable();
    while (true) ;
}

void Kernel::panic(const klib::string& msg)
{
    panic(msg.c_str());
}

/******************************************************************************/

void Kernel::shutdown()
{
    // TODO clean up things like disc IO. For now, we're not expecting to
    // shutdown, so we'll panic instead.
    panic("System shutdown requested.");
}

/******************************************************************************/

void Kernel::dump_address()
{
    log->info("Kernel virtual base at %X\n", kernel_virtual_base);
    log->info("Kernel max allocated at %X\n", kernel_virtual_max);
    log->info("Kernel virtual start at %p\n", virtual_start);
    log->info("Kernel virtual end at %p\n", virtual_end);
    log->info("Kernel physical start at %p\n", physical_start);
    log->info("Kernel physical end at %p\n", physical_end);
}

/******************************************************************************/

void Kernel::default_paging(void* void_pdt, void* void_tpt)
{
    // Set the current page descriptor table to that provided from assembly.
    pdt = static_cast<PageDescriptorTable*>(void_pdt);
    log->info("Page Descriptor Table loaded at %p\n", pdt);

    // Set the location for the reserved temporary page table space.
    PageDescriptorTable::temp_page_table = static_cast<PageTable*>(void_tpt);

    // Give the PDT a default allocator.
    pdt->set_allocator(PageFrameAllocator{});
    pfa_initialise(physical_start, physical_end);

    // Now trim the pdt to get rid of any pages unused by the kernel.
    trim_pdt(pdt, reinterpret_cast<size_t>(virtual_end), kernel_virtual_max);
    log->info("Freed memory: end of kernel at %p, freed up to %X\n",
        virtual_end, kernel_virtual_max);
}

/******************************************************************************/

void Kernel::default_heap()
{
    // Create a heap, starting after the kernel, using the current PDT.
    heap = KernelHeap{virtual_end, pdt};

    log->info("Initialised kernel heap at %p, heap begins at %p\n",
        &heap, heap.get_start());
    //heap.dump_state(log->device());
}

/******************************************************************************/

void Kernel::default_gdt()
{
    // Create an empty GDT.
    gdt = new Gdt{};

    // Add a kernel code entry and a data entry.
    gdt->push_back(
        GdtEntry{0x0, 0x000FFFFF, true, true, true, false, true, false, true,
            false});
    gdt->set_kernel_mode_cs(gdt->size() - 1);
    gdt->push_back(
        GdtEntry{0x0, 0x000FFFFF, true, true, true, false, false, false, true,
            false});
    gdt->set_kernel_mode_ds(gdt->size() - 1);

    // Add a user mode code entry and a data entry.
    gdt->push_back(
        GdtEntry{0x0, 0x000FFFFF, true, true, false, false, true, false, true,
            false});
    gdt->set_user_mode_cs(gdt->size() - 1);
    gdt->push_back(
        GdtEntry{0x0, 0x000FFFFF, true, true, false, false, false, false, true,
            false});
    gdt->set_user_mode_ds(gdt->size() - 1);

    // Add a TSS entry. We'll leave it pointing at nothing for now, as we don't
    // have any processes to worry about yet.
    GdtRegister ss {gdt->kernel_mode_ds().index(), true, false};
    tss = new Tss {nullptr, ss};
    log->info("TSS starts at %p, size is %X.\n", tss, Tss::tss_size);
    gdt->push_back(
        GdtEntry{reinterpret_cast<uint32_t>(tss->base()), Tss::tss_size, false,
            true, true, true, true, false, false, true});
    gdt->set_user_mode_tss(gdt->size() - 1);

    log->info("Initialised GDT at %p\n", gdt);
    log->info("GDT contains %u segment descriptors\n", gdt->size());
    for (size_t i = 0; i < gdt->size(); ++i)
        log->info("GDT entry %u: %llX\n", i, gdt->inspect(i).val());

    // Try to load the GDT.
    if (gdt->load())
    {
        log->info("GDT loaded\n");
        log->info("Code segment register set to %X\n",
            GdtRegister{1, true, false}.val());
        log->info("Data segment register set to %X\n",
            GdtRegister{2, true, false}.val());
    }
    else
        panic("GDT load failed");
}

/******************************************************************************/

void Kernel::default_dev()
{
    // Create the VFS. We'll only add dev to it for now, so we have no root
    // file system. Hmmm...
    vfs = new VirtualFileSystem {};

    // Create the dev file system.
    DevFileSystem* devfs = new DevFileSystem {};
    vfs->mount_virtual("/dev", devfs);
    log->info("Mounted dev file system\n");

    // Create a temp file system.
    MemoryFileSystem* tmpfs = new MemoryFileSystem {};
    vfs->mount_virtual("/tmp", tmpfs);
    log->info("Mounted tmp file system\n");

    // Add serial ports.
    devfs->add_serial();
}

/******************************************************************************/

void Kernel::default_logger()
{
    // This is a character device. The FILE object does not have a buffer. Turn
    // off the stream buffer too.
    klib::ofstream* ofs = new klib::ofstream {};
    ofs->rdbuf()->pubsetbuf(0, 0);
    ofs->open("/dev/ttyS0");
    log = new Logger {ofs};

    log->info("Set system log to /dev/ttyS0\n");
}

/******************************************************************************/

void Kernel::read_multiboot(void* start)
{
    log->info("Reading multiboot information from physical address %p.\n",
        start);
    multiboot = new MultiBootInfo{start};

    // Now we have the multiboot information, we can add the TTY to /dev.
    vfs->get_dev()->add_tty();
}

/******************************************************************************/

void Kernel::default_pic()
{
    // Keep all interrupts disabled for now
    pic = new PicDriver {PicMask::disable, PicMask::disable};
    log->info("Initialised PIC driver at %p\n", pic);
}

/******************************************************************************/

void Kernel::default_pit()
{
    // Create a new PIT driver, using the default frequency.
    pit = new Pit;
    log->info("Initialised PIT driver at %p\n", pit);

    // Enable PIT interrupts.
    pic->set_mask(PicType::master,
        static_cast<PicMask>(~static_cast<uint8_t>(PicMask::master_pit)));
    enable_interrupts();

    log->info("System uptime clock begun\n", pic);

    if (log->stream())
        pit->dump(*log->stream());
}

/******************************************************************************/

void Kernel::default_idt()
{
    idt = new Idt{};
    fill_idt(idt);
    log->info("Initialised IDT at %p\n", idt);
    idt->load();
    log->info("IDT loaded\n");
}

/******************************************************************************/

void Kernel::default_ps2()
{
    ps2 = new Ps2Controller {};
    log->info("Initialised PS/2 controller at %p\n", ps2);
}

/******************************************************************************/

void Kernel::default_keyboard()
{
    keyboard = new Ps2Keyboard {*ps2, "/dev/tty", KeyboardMode::ascii,
        KeyboardScan::sc2};
    log->info("Initialised PS/2 keyboard driver at %p\n", keyboard);
    if (keyboard->self_test())
        log->info("Keyboard self-test passed\n");
    else
        log->warn("Keyboard self-test failed\n");
    if (keyboard->check_sc())
        log->info("Keyboard scan code check passed\n");
    else
        log->warn("Keyboard scan code check failed\n");

}

/******************************************************************************/

void Kernel::default_pci()
{
    pci_devices = new klib::vector<PciDevice> {find_pci()};
    log->info("Found %u PCI devices\n", pci_devices->size());

    if (log->stream())
        for (const PciDevice& p : *pci_devices)
            p.dump(*log->stream());
}

/******************************************************************************/

void Kernel::default_ide()
{
    log->info("Detecting IDE drives\n");

    ide_controllers = new klib::vector<IdeController>;
    for (const PciDevice& p : *pci_devices)
    {
        if (p.get_class() == PciDevice::cl_mass_storage &&
            p.get_subclass() == PciDevice::scl_mass_storage_ide)
        {
//            p.dump(*log->stream());
            ide_controllers->emplace_back(p);
        }
    }

    // Add the drivers to the dev file system, which will also search their
    // partition tables.
    DevFileSystem* devfs = vfs->get_dev();
    if (devfs)
        devfs->add_ide(*ide_controllers);

//    if (log->stream())
//        for (const IdeController& p : *ide_controllers)
//            p.dump(*log->stream());
}

/******************************************************************************/

void Kernel::default_proc_table()
{
    // Try running a simple program read from disk.
    klib::ifstream ifs {"/bin/init"};
    if (!ifs)
        panic("Unable to open /bin/init for reading");
    else
    {
        // Create the first process.
        syslog()->info("Reading binary /bin/init\n");
        Process* p = new Process {"/bin/init"};
        syslog()->info("Created init process.\n");
        // Create the process table.
        proc_tab = new ProcTable {p};
        syslog()->info("Created process table.\n");
    }

    // Create the file table.
    file_tab = new FileTable {};

    // Create the signal manager.
    sig_man = new SignalManager {*proc_tab};
}

/******************************************************************************/

void Kernel::default_root()
{
    // TODO implement command line specification of root partition.
    // Search through available drives. If the file system is not listed as
    // none, try to mount it. If that fails, try the next one.
    DevFileSystem* devfs = vfs->get_dev();
    if (devfs == nullptr)
        panic("Could not mount root file system (/dev does not exist)");
    klib::map<klib::string, Device*> devices = devfs->get_drivers();

    for (auto it = devices.begin(); it != devices.end(); ++it)
    {
        // Only try block devices.
        if (it->second->get_type() != DeviceType::ata_disk)
            continue;
        BlockDevice* bl = dynamic_cast<BlockDevice*>(it->second);

        // Check the file system.
        if (bl->get_fst() != FileSystemType::none)
        {
            // Attempt to mount. This will only report success if the file
            // system was successfully created.
            if(vfs->mount("/", it->first))
            {
                log->info("Mounted %s as root partition.\n", it->first.c_str());
                break;
            }
        }
    }

    // If we didn't manage to mount anything, panic.
    if (vfs->lookup("/") == nullptr)
        panic("No root file system found.");
}

/******************************************************************************/

void Kernel::default_scheduler()
{
    sched = new RoundRobin {};
}

/******************************************************************************/

void Kernel::default_enable()
{
    // Enable keyboard and mouse interrupts.
    if (ps2 != nullptr)
        ps2->enable_interrupts();

    // Enable all PIC interrupts.
    if (pic != nullptr)
    {
        pic->set_mask(PicType::master, PicMask::enable);
        pic->set_mask(PicType::slave, PicMask::enable);
    }

    // Enable all exceptions and software interrupts.
    enable_interrupts();

    log->info("Interrupts enabled\n");
}

/******************************************************************************/

void Kernel::default_disable()
{
    // Disable keyboard and mouse interrupts.
    if (ps2 != nullptr)
        ps2->disable_interrupts();

    // Disable all PIC interrupts.
    if (pic != nullptr)
    {
        pic->set_mask(PicType::master, PicMask::disable);
        pic->set_mask(PicType::slave, PicMask::disable);
    }

    // Disable all exceptions and software interrupts.
    disable_interrupts();

    log->info("Interrupts disabled\n");
}

/******************************************************************************
 ******************************************************************************/
