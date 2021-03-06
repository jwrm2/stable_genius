#include <stdarg.h>

#include <map>
#include <string>
#include <vector>

#include "KernelHeap.h"

// Forward declarations.
namespace __cxxabiv1 { class __cxa_eh_globals; }
class DevFileSystem;
class FileTable;
class Gdt;
class IdeController;
class Idt;
class Kernel;
class KernelHeap;
class Logger;
class MultiBootInfo;
class PageDescriptorTable;
class PciDevice;
class PicDriver;
class Pit;
class ProcTable;
class Ps2Controller;
class Ps2Keyboard;
class Scheduler;
class Serial;
class SignalManager;
class Tss;
class VgaController;
class VirtualFileSystem;

/**
    Linker inserted symbol. Start of the eh_frame section.
 */
extern size_t start_eh_frame;

/**
    Global pointer to the kernel instance, to allow access from anywhere.
 */
extern Kernel* global_kernel;

/**
    Kernel class. Stores various driver pointers and carries out initialisation.
 */
class Kernel {
public:
    /**
        Kernel constructor. Sets up the log, GDT, IDT and some drivers like the
        VGA display and keyboard. Initialises the kernel heap.

        @param kvs Virtual memory address of the start of the kernel.
        @param kve Virtual memory address of the end of the kernel.
        @param kps Physical memory address of the end of the kernel.
        @param kpe Physical memory address of the end of the kernel.
        @param pdt The basic Page Descriptor Table set up in the loader.
        @param tpt Location reserved for a temporary page table.
        @param mbp Start of Multiboot information, generated by GRUB.
     */
    Kernel (void* kvs, void* kve, void* kps, void* kpe, void* pdt, void* tpt,
        void* mbp);

    /**
        Destructor. This should never be called, so it generates a panic.
     */
    virtual ~Kernel();

    /**
        Enter an infinite loop, in which the kernel will remain until
        interrupted or panicking.
     */
    virtual void loop() { while (true); }

    /**
        Disable all interrupts.
     */
    virtual void default_disable();

    /**
        Gets a pointer to the system log, to allow writing.

        @return Pointer to the system log.
     */
    virtual Logger* syslog() { return log; }

    /**
        Gets a pointer to the heap. Needed for operator new.

        @return Pointer to the kernel heap, or nullptr if the heap doesn't exist
                yet.
     */
    virtual KernelHeap* get_heap();

    /**
        Gets a pointer to the keyboard driver. Needed for keyboard interrupt
        handling.

        @return Pointer to the keyboard driver.
     */
    virtual Ps2Keyboard* get_keyboard() { return keyboard; }

    /**
        Gets the location of the start of the kernel in physical memory.

        @return Pointer to the physical start of the kernel.
     */
    virtual const void* get_physical_start() { return physical_start; }

    /**
        Gets the location of the end of the kernel in physical memory.

        @return Pointer to the physical start of the kernel.
     */
    virtual const void* get_physical_end() { return physical_end; }

    /**
        Gets the location of the start of the kernel in virtual memory.

        @return Pointer to the virtual start of the kernel.
     */
    virtual const void* get_virtual_start() { return virtual_start; }

    /**
        Gets the location of the end of the kernel in virtual memory.

        @return Pointer to the virtual start of the kernel.
     */
    virtual const void* get_virtual_end() { return virtual_end; }

    /**
        Gets the kernel initialisation exception information.

        @return Pointer to the kernel exception information.
     */
    virtual __cxxabiv1::__cxa_eh_globals* get_eh_globals()
    {
        return eh_globals;
    }

    /**
        Sets the kernel initialisation exception information.

        @param eh Pointer to the new kernel exception information.
     */
    virtual void set_eh_globals(__cxxabiv1::__cxa_eh_globals* eh)
    {
        eh_globals = eh;
    }

    /**
        Gets a pointer to the file description table.

        @return Reference to the file description table.
     */
    virtual FileTable* get_file_table() { return file_tab; }

    /**
        Gets the GDT information structure.

        @return Reference to the GDT information.
     */
    virtual const Gdt& get_gdt() const { return *gdt; }

    /**
        Gets a reference to the list of IDE devices.

        @return Vector of IDE devices.
     */
    virtual klib::vector<IdeController>& get_ide() { return *ide_controllers; }

    /**
        Gets the MultiBoot information structure.

        @return Reference to the MultiBoot information.
     */
    virtual const MultiBootInfo& get_multiboot() const { return *multiboot; }

    /**
        Gets a pointer to the current PDT. Needed when we need to deal with
        physical addresses, such as when reading the multiboot information.

        @return Pointer to the PDT.
     */
    virtual PageDescriptorTable* get_pdt() { return pdt; }

    /**
        Gets a reference to the list of PCI devices.

        @return Vector of PCI devices.
     */
    virtual klib::vector<PciDevice>& get_pci() { return *pci_devices; }

    /**
        Gets a pointer to the PIC driver. Needed for PIC interrupt
        handling.

        @return Pointer to the PIC driver.
     */
    virtual PicDriver* get_pic() { return pic; }

    /**
        Gets a pointer to the PIT driver. Needed for the system uptime clock.

        @return Pointer to the PIT driver.
     */
    virtual Pit* get_pit() { return pit; }

    /**
        Gets a pointer to the process table.

        @return Reference to the process table.
     */
    virtual ProcTable& get_proc_table() { return *proc_tab; }

    /**
        Gets the scheduler.

        @return Reference to the scheduler.
     */
    virtual Scheduler& get_scheduler() const { return *sched; }

    /**
        Gets the signal manager.

        @return Reference to the signal manager.
     */
    virtual SignalManager* get_signal_manager() const { return sig_man; }

    /**
        Gets the Task State Segment structure. Useful for setting the esp
        interrupt value.

        @return Reference to the TSS.
     */
    virtual Tss& get_tss() const { return *tss; }

    /**
        Gets a pointer to the Virtual File System. All file system operations
        should go through this.

        @return Pointer to the VFS.
     */
    virtual VirtualFileSystem* get_vfs() { return vfs; }

    /**
        Display an error message, if provided, then abort the kernel.

        @param fmt printf style format string containing the error message.
        @param ... Substitutions for fmt.
        @param msg Error message to display on the VGA and to send to the system
               log.
     */
    virtual void panic(const char* fmt = "", ...);
    virtual void panic(const klib::string& msg);

    /**
        Shutdown the system. Should do things like finishing all reads and
        writes.
     */
    virtual void shutdown();

protected:
    // Virtual and physical starts and ends of the kernel.
    void* virtual_start;
    void* virtual_end;
    void* physical_start;
    void* physical_end;

    // PIC driver.
    PicDriver* pic;

    // PIT driver.
    Pit* pit;

    // PS/2 Controller driver.
    Ps2Controller* ps2;

    // Keyboard driver.
    Ps2Keyboard* keyboard;

    // File descriptions table.
    FileTable* file_tab;

    // GDT location.
    Gdt* gdt;

    // Kernel Heap.
    KernelHeap heap;

    // IDT location.
    Idt* idt;

    // System Log.
    Logger* log;

    // PDT location.
    PageDescriptorTable* pdt;

    // Multiboot information.
    MultiBootInfo* multiboot;

    // List of the PCI devices.
    klib::vector<PciDevice>* pci_devices;

    // List of the IDE controllers.
    klib::vector<IdeController>* ide_controllers;

    // TSS location.
    Tss* tss;

    // Virtual file system, which stores mappings between mount points and
    // file systems.
    VirtualFileSystem* vfs;

    // Process table, storing pointers to all the processes.
    ProcTable* proc_tab;

    // Scheduler, decides which process to pass CPU control to.
    Scheduler* sched;

    // Signal manager, which handles asynchronous events.
    SignalManager* sig_man;

    // Thread global exception information. This on is used in initialisation,
    // after that it'll be the active process's one.
    __cxxabiv1::__cxa_eh_globals* eh_globals;

    // Dump the address information.
    virtual void dump_address();

    // Set up PDTs and paging in the standard way.
    // pdt is the address of the boot page table, tpt is the space for a
    // temporary page table.
    virtual void default_paging(void* pdt, void* tpt);

    // Sets up a heap to start after the kernel.
    virtual void default_heap();

    // Sets up a GDT with sensible kernel and user mode segments.
    virtual void default_gdt();

    // Creates an empty dev file system.
    virtual void default_dev();

    // Sets the logger to the first serial port in the dev file system.
    virtual void default_logger();

    // Reads the multiboot information and copies it onto the heap.
    virtual void read_multiboot(void* start);

    // Sets up the programmable interrupt controller.
    virtual void default_pic();

    // Sets up the programmable interval timer.
    virtual void default_pit();

    // Creates and populates an interrupt descriptor table.
    virtual void default_idt();

    // Sets up the PS/2 controller.
    virtual void default_ps2();

    // Sets up the PS/2 keyboard driver.
    virtual void default_keyboard();

    // Gets the list of PCI devices.
    virtual void default_pci();

    // Sets up IDE controllers.
    virtual void default_ide();

    // Reads the init process and adds it to the new process table. Does not
    // launch it yet.
    virtual void default_proc_table();

    // Mounts the root partition. Uses the first available partition it can
    // mount.
    virtual void default_root();

    // Creates a new scheduler. The default is round robin.
    virtual void default_scheduler();

    // Enables interrupts of all descriptions.
    virtual void default_enable();
};
