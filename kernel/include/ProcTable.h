#ifndef PROC_TABLE_H
#define PROC_TABLE_H

#include <stddef.h>

#include <fstream>
#include <ios>
#include <map>
#include <string>

// Forward declarations
class Device;
class InterruptRegisters;
class InterruptStack;

/**
    PID of the init process.
 */
constexpr size_t init_pid = 1; 

// Forward decalartions.
class Process;

/**
    The process table. Stores pointers to all the current processes, keyed by
    their PIDs. Basically a wrapper around klib::map. */
class ProcTable {
public:
    /** Iterator type for this container. */
    using iterator = klib::map<size_t, Process*>::iterator;
    /** Contstant iterator type for this container. */
    using const_iterator = klib::map<size_t, Process*>::const_iterator;

    /**
        Constructor. Must be initialised with the init process, which gets a
        PID of init_pid.
     */
    explicit ProcTable(Process* p);

    /**
        Tests whether a process with the given PID exists.

        @param pid PID to test.
        @return 1 if the PID is in use, 0 otherwise.
     */
    bool count(size_t pid) { return tab.count(pid); }

    /**
        Tests whether there are any processes.

        @return True if there are no processes, false otherwise.
     */
    bool empty() { return tab.empty(); }

    /**
        Frees the resources associated with the given PID. Does nothing if the
        PID is unused.

        @param pid PID to free.
     */
    void erase(size_t pid);

    /**
        Returns an iterator to the element matching the given PID, or one past
        the last element if the PID doesn't exist.

        @param pid PID to look up.
        @return Iterator to the element with the given PID.
     */
    iterator find(size_t pid) { return tab.find(pid); }
    const_iterator find(size_t pid) const { return tab.find(pid); }

    /**
        Get the process with the given PID.

        @param pid PID to look up.
        @return Process with the given PID, or nullptr if it doesn't exist.
     */
    Process* get_process(size_t pid)
    {
        return (tab.count(pid) ? (*tab.find(pid)).second : nullptr);
    }

    const Process* get_process(size_t pid) const
    {
        return (tab.count(pid) ? (*tab.find(pid)).second : nullptr);
    }

    /**
        Set the process with the given pid. Used in exec. The PID must already
        exist.

        @param pid Exisiting PID to swap.
        @param p New process to swap in.
        @return True for success, false for failure.
     */
    bool set_process(size_t pid, Process* p)
    {
        if (tab.count(pid) == 0)
            return false;

        tab[pid] = p;
        return true;
    }

    /**
        Adds a new process to the table. The PID is autmatically determined.

        @param p Pointer to the new process.
        @return PID of the new process. 0 indicates failure.
     */
    size_t insert(Process* p);

    /**
        Gets an iterator to the first process in the table.

        @return Iterator to the first process.
     */
    iterator begin() { return tab.begin(); }
    const_iterator begin() const { return tab.begin(); }
    const_iterator cbegin() const { return tab.cbegin(); }

    /**
        Gets an iterator to one past the last process in the table.

        @return Iterator to one past the last process.
     */
    iterator end() { return tab.end(); }
    const_iterator end() const { return tab.end(); }
    const_iterator cend() const { return tab.cend(); }

    /**
        Swaps the specified process into context and resumes its execution.

        @param pid Process to switch to.
        @return false if the swap failed. Will not return if successful.
     */
    bool swap_in(size_t pid);

    /**
        Swaps the current process out of context, saving all necessary
        information to the process.

        @param pid Hint of the currently active process. Will be checked for
               correctness and the actually active one searched for if not
               correct.
        @param ir Registers in the process executing before the interrupt.
        @param is State of the stack on interrupt.
     */
    void swap_out(size_t pid, const InterruptRegisters& ir,
        const InterruptStack& is);

private:
    // Contains the list of processes. Key is the PID.
    klib::map<size_t, Process*> tab;
    // Last PID allocated. Useful for finding an available one quickly.
    size_t last_pid;
    // Maximum allowed PID.
    static constexpr size_t max_pid = 32768;
};

/**
    This class stores the gloobal set of file descriptions. Entries are pointed
    to by file descriptors in individual processes. Multiple processes can have
    the same file opened and the file description is shared between them. The
    list is stored as a map, with file descriptors storing keys. This allows
    files to deleted and created without affecting addresses and indexes in
    other processes.
 */
class FileTable {
public:
    /**
        Default constructor. Creates an empty list.
     */
    FileTable() : file_list{} {}

    /**
        Opens a file. Checks whether a file descriptor already exists and
        creates one if it does not.

        @param name Full path to the file.
        @param mode Mode to open the file in.
        @return File description key on success, -1 on failure.
     */
    int open_file(const klib::string& name, klib::ios_base::openmode mode);

    /**
        Closes a file description. Decrements reference counter and removes the
        description if no processes refer to it any more. Flushes before close
        if removing the description.

        @param key File descriptor to close.
        @return 0 on success, -1 on failure.
     */
    int close_file(int key);

    /**
        Increments the reference counter of an exisiting file.
        Useful for fork().

        @param key File descriptor to increment..
        @return 0 on success, -1 on failure.
     */
    int copy_file(int key);

    /**
        Gets the name for the given file.

        @param key File description key.
        @return Name of the file. Blank if the key does not exist.
     */
    const klib::string& get_name(int key) const;

    /**
        Gets the device pointer for the given file.

        @param key File description key.
        @return Device the file is on. nullptr if the file is not on a device or
                the key does not exist.
     */
    Device* get_dev(int key) const;

    /**
        Gets the file stream for the given file.

        @param key File description key.
        @return Device the file is on. nullptr if the file is not on a device or
                the key does not exist.
     */
    klib::fstream& get_stream(int key);

protected:
    // Entries in the file table. COntains the information necessary for a file
    // context. Note that subsequent calls to open() a file from different
    // processes will create new descriptions, so will have independent flags
    // and offsets. Conversely, calls such as fork() duplicate the process's
    // file descriptors, so the new process points to the same file
    // descriptions.
    struct FileDescription {
        /**
            Constructor. Opens the file with the given mode.

            @param n Name of the file.
            @param mode Mode to open the file in.
         */
        FileDescription(const klib::string& name,
            klib::ios_base::openmode mode);

        // Name of the file.
        klib::string name;
        // Number of processes with file descriptors for this description.
        int count;
        // Device on which the file exists. Nullptr means no device.
        Device* dev;
        // Stream used for reading and writing.
        klib::fstream fs;
    };

    // Map of the currently opened file descriptors.
    klib::map<int, FileDescription> file_list;
};

#endif /* PROC_TABLE_H */
