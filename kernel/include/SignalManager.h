#ifndef SIGNAL_MANAGER_H
#define SIGNAL_MANAGER_H

#include <stddef.h>

#include <ios>
#include <string>
#include <vector>

// Forward declarations
class Device;
class Process;
class ProcTable;

/**
    Enum with the possible file polling events.
 */
enum class PollType {
    /** No events. */
    pollnone = 0x0,
    /** There is data to read from the file. */
    pollin = 0x1,
    /** Exceptional condition occured. */
    pollpri = 0x2,
    /** It is possible to write to the file. */
    pollout = 0x4,
    /** Mask for requestable events. */
    pollrequestable = pollin | pollpri | pollout,
    /** Error on the file. Cannot be requested in events, but will be set in
        revents. */
    pollerr = 0x8,
    /** Hang up on the file. Cannot be requested in events, but will be set
        in revents. */
    pollhup = 0x10,
    /** Event requested was invalid, eg if the file descriptor is not open.
        Cannot be requested in events, but will be set in revents. */
    pollnval = 0x20
};

/**
    Template overloads to allow PollType enum to be used as Bitfields. We need
    to use the klib bitwise operators, since PollType is not a member of klib
    and Argument-Dependent Lookup will not be triggered.
 */
namespace klib {
template <>
struct BitmaskEnable<PollType> : public true_type {};
}
using klib::operator|;
using klib::operator&;
using klib::operator^;
using klib::operator~;
using klib::operator|=;
using klib::operator&=;
using klib::operator^=;

/**
    The purpose of this class is to manage signals to processes and events such
    as waited for by poll(). A process can notify the event manager that it is
    waiting for an event, and the manager will put it to sleep. When events
    occur, the signal manager should be notified so that it can wake up waiting
    processes.
 */
class SignalManager {
public:
    /**
        Constructor. The signal manager needs to access the process table.

        @param p Process table containing the processes to be managed.
     */
    explicit SignalManager(ProcTable& p) :
        pt {p},
        pending_polls {},
        pending_waits {},
        happened_waits{}
    {}

    /**
        This class is used for specifying requests passed to poll().
     */
    struct pollfd {
        /** Index of the file descriptor. */
        int fd;
        /** Bitmask specifying which events to wait for. */
        PollType events;
        /** Return bitmask indicating which events occured. */
        PollType revents;
    };

    /**
        Puts a process to sleep until the requested file events have occured.
        For example, wait for the user to provide keyboard input to the tty.
        Multiple file descriptors and multiple events may be specified. Task
        switching is disabled while the events list is scanned to prevent events
        firing while the process is switched out being missed.

        @param fds List of the file descriptors and events. Must be a pointer to
               a heap variable, as the active process stack may change.
        @param nfds Number of objects in the list.
        @param timeout Time after which to return if no event occured,
               in milliseconds. A negative value means infinite.
        @return Number of fds with successful events, 0 if the timeout was
                reached or -1 on error.
     */
    int poll(pollfd* fds, size_t nfds, int timeout);

    /**
        Notify the manager that a file descriptor event has occured. If matching
        events are found in the poll events list, the associated processes will
        be woken up. Task switching is disabled while the events list is scanned
        to make sure we process all matches immediately.

        @param dev Device that has caused the event.
        @param ev Bitmask specifying the event or events that have occured.
     */
    void notify_file(const Device* dev, PollType ev);

    /**
        Decreases the timeout counters for all pending events. Processes with
        events reaching their timeouts will be woken up. Switching is blocked
        to ensure we wake everything up immedately.

        @param tick_size Number of milliseconds to decrease each timer by.
     */
    void tick_down(int tick_size);

    /**
        Registers a process to wait for an event, going to sleep until the
        event occurs. First checks the list of events that have aleady happened.
        If a match is found, the wait succeeds and returns immediately.
        Otherwise, the process will go to sleep and will be woken up when one
        of the processes in the list sends an event matching the requirements.

        @param pid Indicates which process to wait for. Must be a child of the
               process making the request. -1 indicates any child.
        @param wstatus Address of a field used to return data about the event.
        @param options Specifies what event to wait for. The default is
               termination of the child. TODO only termination is supported.
        @return PID of the process that notified the matching event on success,
                -1 on failure.
     */
    int waitpid(int pid, int* wstatus, int options);

    /**
        Notifies that an event has occured. If a process is waiting for the
        event, that process will be woken up. If not, the event is added to a
        list of events that have happened, for checking when new waits are
        received.

        @param ev What event has occured. TODO only termination is supported.
     */
    void notify_wait(int ev);

    /**
        Purge all the pending events for a specific PID. Useful for ending
        processes. Deletes poll events where the process is waiting for a poll,
        wait events that have happened for this process and wait requests for
        the process. Does not delete wait requests where other processes are
        waiting for this one.

        @param pid PID of process to purge.
        @param proc Process to purge.
     */
    void purge_process(size_t pid);
    void purge_process(const Process* proc);

protected:
    // The process table.
    ProcTable& pt;

    // This class is used to store polling events. As well as the address of the
    // polling request, it contains the remaining time till timeout and the PID.
    struct poll_event {
        pollfd* req;
        int time_left;
        size_t pid;
    };

    // This class is used to store waiting requests. It has the PID of the
    // waiting process, a list of PIDs of processes it is waiting for, the
    // address of a user space integer used to return information, a field to
    // store the data to put into that address (since the relevant user space
    // process may not be in virtual memory when events are notified) and an
    // options field.
    struct wait_request {
        size_t pid;
        klib::vector<size_t> wait_pids;
        int* wstatus;
        int local_wstatus;
        int options;
    };

    // This class is used to store events from processes that have happened. It
    // has the PID of the process that caused the event and a field specifying
    // the nature of the event.
    struct wait_event {
        size_t pid;
        int ev;
    };

    // List of polling events. This class will not free any memory associated
    // with polling requests (pollfd).
    klib::vector<poll_event> pending_polls;

    // List of processes waiting for events.
    klib::vector<wait_request> pending_waits;

    // List of events that have been notified that no process has asked for yet.
    klib::vector<wait_event> happened_waits;
};

#endif /* SIGNAL_MANAGER_H */
