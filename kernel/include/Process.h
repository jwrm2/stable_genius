#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#include <fstream>
#include <ios>
#include <map>
#include <string>

#include "Elf.h"
#include "InterruptHandler.h"
#include "PageDescriptorTable.h"

// Forward declarations
namespace __cxxabiv1 { class __cxa_eh_globals; }

/**
    Process statuses.
 */
enum class ProcStatus {
    /** Process is currently running. */
    active,
    /** Process is runnable and would like processor time. */
    runnable,
    /** Process is waiting for something and should not be given CPU time. */
    sleeping,
    /** Process has ended and should be cleaned up. */
    zombie,
    /** Invalid, probably on creation. */
    invalid
};

/**
    Stores information concerning user mode processes.
 */
class Process {
public:
    /**
        Constructor. Reads the ELF headers and checks everything looks good.
        Does not create a PDT or allocate any memory for the binary itself.

        @param name File name of the binary.
     */
    Process(const klib::string& name);

    /**
        Default constructor, used to create an 'empty' process. The PDT is
        created, but left blank, no binary or elf headers are provided, the
        registers and interrupt stack are left as zeroes, but the kernel stack
        is allocated.
     */
    Process();

    /**
        Copying processes is disallowed. The only reason to copy would be fork,
        and that's done through a defualt constructor followed by duplicate().

        @param other Other Process to (not) copy.
        @return This process.
     */
    Process(const Process& other) = delete;
    Process& operator=(const Process& other) = delete;

    /**
        Move constructor. Follows the normal pattern of pirating data from the
        other Process.

        @param other Other Process to pirate from.
     */
    Process(Process&& other);

    /**
        Move assignment. Follows the normal pattern of pirating data from the
        other Process.

        @param other Other Process to pirate from.
        @return This Process.
     */
    Process& operator=(Process&& other);

    /**
        Destructor. Deletes the PDT and any process only PTs. One hopes the
        kernel PDT has been loaded before this is called.
     */
    ~Process();

    /**
        Used for exec. Copies the parent and child PIDs and the open file
        descriptors.

        @param Process to duplicate some information from.
     */
    void exec_duplicate(const Process& other);

    /**
        Used for fork, duplicates a Process by copying its interrupt stack
        and registers and copying all the user space memory, then setting the
        state to runnable (the other process must be active). Does not copy
        kernel stack, as the other process is in the middle of a fork system
        call that it will just unwind so we can skip that in the child. Sets the
        eax interrupt register to 0, the return value for the child. Copies
        the open file descriptors.

        @param Parent process to duplicate.
     */
    void fork_duplicate(const Process& other);

    /**
        Gets the process's kernel exception information.

        @return Pointer to the process's kernel exception information.
     */
    __cxxabiv1::__cxa_eh_globals* get_eh_globals() { return eh_globals; }

    /**
        Sets the process's kernel exception information.

        @param eh Pointer to the new process kernel exception information.
     */
    void set_eh_globals(__cxxabiv1::__cxa_eh_globals* eh)
    {
        eh_globals = eh;
    }

    /**
        Gets the process's page descriptor table.

        @return Reference to the process's PDT.
     &*/
    const PageDescriptorTable& get_pdt() const { return *pdt; }

    /**
        Gets the current values of the interrupt stack, containing values for
        the eip, cs, eflags, esp and ss registers that must be set when swapping
        this process back in.

        @return Interrupt stack state at the last swap out.
     */
    const InterruptStack& get_interrupt_stack() const { return is; }

    /**
        Sets the stored values of the interrupt stack, containing values for the
        eip, cs, eflags, esp and ss registers so they can be restored at the
        next swap in.

        @param i Stack state to store.
     */
    void set_interrupt_stack(const InterruptStack& i) { is = i; }

    /**
        Gets the current values of the general purpose registers, as stored at
        the previous swap out for restoration at the next resumption. The esp
        value is not generally correct and should be ignored.

        @return General purpose registers at the last swap out.
     */
    InterruptRegisters& get_registers() { return ir; }

    /**
        Sets the stored values of the general purpose registers. Should be done
        at swap out so they can be restored on the next swap in. The value of
        esp will generally not be correct and is ignored. It should be set
        through the interrupt stack instead.

        @param i Register values to store.
     */
    void set_registers(const InterruptRegisters& i) { ir = i; }

    /**
        Gets the current state of the process.

        @return Current process state.
     */
    ProcStatus get_status() const { return stat; }

    /**
        Sets the current state of the process.

        @param st State to set this process to.
     */
    void set_status(ProcStatus st) { stat = st; }
 
    /**
        Swaps the stack pointers of this process and another. Useful for exec.

        @param other Process to swap stacks with.
     */
    void swap_stack(Process& other);

    /**
        Transfer to this process. Allocates and loads the process PDT, copies
        the binary to code/data address, works out the necessary data values,
        then transfers to assembly to carry out an iret.

        @param PDT with kernel information.
     */
    void launch(PageDescriptorTable& k_pdt);

    /**
        Resumes running this process. Loads the kernel stack into the TSS, sets
        up the stack for an iret and jumps back to the relevant code location.
     */
    void resume();

    /**
        Opens a file descriptor.

        @param name Full path to the file.
        @param mode Mode in which to open the file.
        @return File descriptor index on success, -1 on failure.
     */
    int open_file(const klib::string& name, klib::ios_base::openmode mode);

    /**
        Closes a file descriptor, or all file descriptors. Flushes them first.

        @param File descriptor to close. -1 indicates close all files.
        @return 0 on success, -1 on failure.
     */
    int close_file(int fd);

    /**
        Gets the key for the file descriptor in the global file table.

        @param fd File descriptor index to check.
        @return key for the global file table, or 0 if the file descriptor does
                not exist.
     */
    int get_fd_key(int fd);

    /**
        Get the return status. Only meaningful if the process has terminated.
        The lowest 8 bits are the return value, which is 0 for normal exit. The
        other bits are various flags. See the wait syscall for the meaning of
        different flags.

        @return Return value of the process.
     */
    size_t get_ret_status() const { return ret_val; }

    /**
        Gets the parent PID.

        @return Parent PID.
     */
    size_t get_ppid() const { return parent_pid; }

    /**
        Gets or sets the maximum user space stack size. The maximum size will be
        rounded up to an integer number of pages.

        @param max Maximum stack size to set.
        @return Current maximum stack size.
     */
    size_t user_max_stack() const { return max_stack; }
    void user_max_stack(size_t max)
    {
        max_stack = max - max % PageDescriptorTable::page_size +
            PageDescriptorTable::page_size;
    }

    /**
        Sets the current user space stack size. The set will do nothing
        successfully if the specified value is less than the current stack size,
        and will fail if the requested size is greater than the current maximum
        stack size. It will allocate new memory to the stack below the current
        base and move the base downwards.

        @param sz New size for the user space stack.
        @return 0 on success, -1 on failure.
     */
    int set_user_stack(size_t sz);

    /**
        Sets the break point, ie changes the size of the heap. Will fail if the
        new address is into stack memory, or before the start of the heap. As a
        special case, if the value 0 is passed, instead returns the current
        break point.

        @param addr New address for the break point.
        @return 0 on success, -1 on error, or current break point if addr was 0.
     */
    int brk(void* addr);

    /**
        Sets the parent PID to the given value. There is no checking on the
        state of the process corressponding to the value.

        @param ppid Parent PID to set.
     */
    void set_ppid(size_t ppid) { parent_pid = ppid; }

    /**
        Adds a PID to the list of child PIDs. There is no checking on the
        state of the process corresponding to the value.

        @param pid Child PID to add.
     */
    void add_child(size_t pid);

    /**
        Clears the list of child PIDs. Doesn't actually affect the child
        processes.
     */
    void clear_children() { child_pids.clear(); }

    /**
        Gets the list of child PIDs. Not modifyable.

        @return Reference to the list of child PIDs.
     */
    const klib::vector<size_t>& get_children() const { return child_pids; }

private:
    // Initial size of the user stack, currently one page.
    static constexpr size_t start_stack = PageDescriptorTable::page_size;
    // Size of the kernel stack for this process. This is only used for
    // executing system calls.
    static constexpr size_t kernel_stack_size = PageDescriptorTable::page_size;
    // Default maximum size of the user space stack. The user stack will be
    // dynamically expanded (by catching page faults) until it is this size.
    static constexpr size_t default_max_stack = 1 << 23;

    // Entry point for the process, as read from the ELF header.
    uintptr_t entry_point;
    // Page Descriptor Table for this process. PDTs own their associated Page
    // Tables, so we don't need a list of PTs here.
    PageDescriptorTable* pdt;
    // Current size of the user stack and maximum size.
    size_t current_stack;
    size_t max_stack = default_max_stack;
    // Bottom of the kernel stack for this process.
    uintptr_t* kernel_stack;
    // Virtual break point, that is the memory address of the end of the
    // programme data, bss and heap area. Can be increased to allocate more
    // heap space.
    uintptr_t* break_point;
    // Status of the process.
    ProcStatus stat;
    // Status of the stack at the last interrupt, containing values of eip, cs,
    // eflags, esp and ss to restore.
    InterruptStack is;
    // Current values of the 8 general purpose registers. Note that the value
    // of esp is not correct and should be ignored.
    InterruptRegisters ir;
    // Thread global kernel exception information for this process.
    __cxxabiv1::__cxa_eh_globals* eh_globals;
    // Elf header information
    Elf elf;
    // Map between Process file descriptors and global file descriptions.
    klib::map<int, int> file_desc;
    // Return value of the process. Only set when the user space process exits,
    // but can later be read by the parent.
    uint8_t ret_val;
    // The parent pid.
    size_t parent_pid;
    // List of the pids of children.
    klib::vector<size_t> child_pids;
};

/**
    Assembly instruction to carry out the iret to transfer to a user mode
    process. This will disable interrupts for the moment.

    @param edi Register value.
    @param esi Register value.
    @param ebp Register value.
    @param esp_fake Placeholder for register value. Gets ignored.
    @param ebx Register value.
    @param edx Register value.
    @param ecx Register value.
    @param eax Register value.
    @param eip Instruction pointer.
    @param cs Code segment selector.
    @param eflags Processor flags value.
    @param esp Stack pointer.
    @param ss Stack segment selector.
 */
extern "C"
void launch_process(uint32_t edi,
                    uint32_t esi,
                    uint32_t ebp,
                    uint32_t esp_fake,
                    uint32_t ebx,
                    uint32_t edx,
                    uint32_t ecx,
                    uint32_t eax,
                    uint32_t eip,
                    uint16_t cs,
                    uint32_t eflags,
                    uint32_t esp,
                    uint16_t ss);

/**
    Assembly instruction to carry out the iret to transfer to a kernel mode
    process. This will happen if a process got interrupted while performing a
    system call. This will disable interrupts for the moment.

    @param edi Register value.
    @param esi Register value.
    @param ebp Register value.
    @param esp_fake Placeholder for register value. Gets ignored.
    @param ebx Register value.
    @param edx Register value.
    @param ecx Register value.
    @param eax Register value.
    @param eip Instruction pointer.
    @param cs Code segment selector.
    @param eflags Processor flags value.
    @param esp Stack pointer.
 */
extern "C"
void launch_kernel_process(uint32_t edi,
                          uint32_t esi,
                          uint32_t ebp,
                          uint32_t esp_fake,
                          uint32_t ebx,
                          uint32_t edx,
                          uint32_t ecx,
                          uint32_t eax,
                          uint32_t eip,
                          uint16_t cs,
                          uint32_t eflags,
                          uint32_t esp);

/**
    Assembly function to get the current value of eflags.

    @return Current eflags value.
 */
extern "C"
uint32_t get_eflags();

#endif /* PROCESS_H */
