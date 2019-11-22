#include "SignalManager.h"

#include <vector>

#include "Device.h"
#include "Kernel.h"
#include "Logger.h"
#include "Process.h"
#include "ProcTable.h"
#include "Scheduler.h"

/******************************************************************************
 ******************************************************************************/

int SignalManager::poll(pollfd* fds, size_t nfds, int timeout)
{
    if (nfds == 0 || fds == nullptr)
        return -1;

    if (timeout == 0)
        return 0;

    // Block task switching.
    switch_blocked_for_notify = true;

//    global_kernel->syslog()->info("SignalManager::poll fds at %p, nfds = %u, timeout = %d\n", fds, nfds, timeout);
//    for (size_t i = 0; i < nfds; ++i)
//        global_kernel->syslog()->info("fds[%u]: fd = %d, events = %u, revents = %u\n", i, fds[i].fd, fds[i].events, fds[i].revents);

    // Get the current PID and process.
    size_t pid = global_kernel->get_scheduler().get_last();
    Process* active = pt.get_process(pid);
    if (active == nullptr)
        return -1;

    // Cycle over events.
    // ret_count records the number of events that are immediately true.
    // found_count records the number of valid events added to the list.
    int ret_count = 0;
    size_t found_count = 0;
    for (size_t i = 0; i < nfds; ++i)
    {
        // Get the global file table key (return 0 if it doesn't exist).
        int key = active->get_fd_key(fds[i].fd);
        if (key == 0)
        {
            fds[i].revents = PollType::pollnval;
            continue;
        }

        // Check that the event is requestable.
        if ((fds[i].events & PollType::pollrequestable) == PollType::pollnone)
        {
            fds[i].revents = PollType::pollnval;
            continue;
        }

        // Reset return events.
        fds[i].revents = PollType::pollnone;

        // Check whether the condition already exists.
        Device* dev = global_kernel->get_file_table().get_dev(key);
        PollType ret_mask =
            dev->poll_check(fds[i].events & PollType::pollrequestable);
        if (ret_mask != PollType::pollnone)
        {
            ++ret_count;
            fds[i].revents |= ret_mask;
        }
        else
        {
            // Add event to the list.
            pending_polls.emplace_back(fds + i, timeout, pid);
            ++found_count;
        }
    }

    // See whether we found any valid events.
    if (found_count == 0 && ret_count == 0)
    {
        switch_blocked_for_notify = false;
        return -1;
    }

    // Interrupts might have already triggered events. See how many are left.
    size_t pid_count = 0;
    for (const auto& p : pending_polls)
        if (p.pid == pid && p.req->revents == PollType::pollnone)
            ++pid_count;

    // If conditions are already true, remove events from the list and return
    // immediately.
    if (ret_count != 0 || found_count != pid_count)
    {
        // Remove all pending events for this pid.
        for (auto it = pending_polls.begin(); it != pending_polls.end(); )
        {
            if (it->pid == pid)
                it = pending_polls.erase(it);
            else
                ++it;
        }
        // Release task switching.
        switch_blocked_for_notify = false;
        return ret_count + found_count - pid_count;
    }

    // Found events. Put the process to sleep and yield remaining time.
    active->set_status(ProcStatus::sleeping);
    global_kernel->get_scheduler().yield();

    // Block again for the count, or we may find ourselves swapped out,
    // allowing more events. We could still interrupt and trigger more events,
    // but we'll deem that unlikely and not very serious.
    switch_blocked_for_notify = true;

    // We get back here after an event has fired and the process has been woken
    // up. See how many events have fired. Wipe any pending ones for the pid. It
    // is also possible to get here on timeout. In that case there will be no
    // fired events and we return 0 as required.
    pid_count = 0;
    for (auto it = pending_polls.begin(); it != pending_polls.end(); )
    {
        if (it->pid == pid)
        {
            if (it->req->revents != PollType::pollnone)
                ++pid_count;
            it = pending_polls.erase(it);
        }
        else
            ++it;
    }

    // The poll has successfully concluded after the event. Go back to whatever
    // we were doing before.
    switch_blocked_for_notify = false;
    return pid_count;
}

/******************************************************************************/

void SignalManager::notify_file(const Device* dev, PollType ev)
{
    // Disable block switching to make sure we scan the whole list before going
    // elsewhere. Otherwise new polls made after this call could be concluded. 
    switch_blocked_for_notify = true;

    // Search through the pending events and see if any match the device and
    // event.
    for (auto it = pending_polls.begin(); it != pending_polls.end(); )
    {
        // Get the process.
        Process* proc = pt.get_process(it->pid);
        // If the process doesn't exist any more, remove the event.
        if (proc == nullptr)
            it = pending_polls.erase(it);
        else {
            // Get the process device.
            Device* d = global_kernel->get_file_table().get_dev(
                proc->get_fd_key(it->req->fd));
            if (d == dev && (it->req->events & ev) != PollType::pollnone)
            {
                // We have a match. Set the revent and wake up the process.
                it->req->revents |= (it->req->events & ev);
                if (proc->get_status() == ProcStatus::sleeping)
                    proc->set_status(ProcStatus::runnable);
            }
            ++it;
        }
    }

    switch_blocked_for_notify = false;
}

/******************************************************************************/

int SignalManager::waitpid(int pid, int* wstatus, int options)
{
    // Get the active process.
    size_t active_pid = global_kernel->get_scheduler().get_last();
    Process* active = pt.get_process(active_pid);
    if (active == nullptr)
        return -1;

    // Make the list of children to wait for. Check the pid is a child.
    klib::vector<size_t> child_pids;
    if (pid == -1)
        child_pids = active->get_children();
    else if (pid > 0)
    {
        child_pids = active->get_children();
        if (klib::find(child_pids.begin(), child_pids.end(),
            static_cast<size_t>(pid)) == child_pids.end())
            child_pids = klib::vector<size_t> {};
        else
            child_pids = klib::vector<size_t> {static_cast<size_t>(pid)};
    }

    if (child_pids.empty())
        // No valid PIDs to wait for.
        return -1;

    // Block task switchin now. This is a bit lazy and we could probably do it
    // without, butit would be more fiddly.
    switch_blocked_for_notify = true;

    // Create the wait request and add it to the list. Get a reference to the
    // event.
    pending_waits.emplace_back(active_pid, child_pids, wstatus, 0, options);
    const wait_request& wr = pending_waits.back();

    // Look through the list of happened events to see if there's a match.
    for (auto it = happened_waits.begin(); it != happened_waits.end(); ++it)
    {
        // TODO check the nature of the event too.
        auto it2 =
            klib::find(wr.wait_pids.begin(), wr.wait_pids.end(), it->pid);
        if (it2 != wr.wait_pids.end())
        {
            // We have a match. Set the return properties.
            const Process* child = pt.get_process(*it2);
            if (wr.wstatus != nullptr)
                *wr.wstatus = child->get_ret_status();
            int ret_val = static_cast<int>(*it2);

            // Remove the happened event and the request.
            happened_waits.erase(it);
            pending_waits.pop_back();

            switch_blocked_for_notify = false;
            return ret_val;
        }
    }

    // If we've reached here, we're waiting for an event that hasn't happened
    // yet, so put the process to sleep.
    active->set_status(ProcStatus::sleeping);
    global_kernel->get_scheduler().yield();

    // If we've reached here, we've been woken up and the event has happened.
    // The notify event will have set the local status of the request, but not
    // user space value. It will have removed all wait PIDs from the list
    // except the one that caused the event. The wr reference may not be valid
    // any more, as the vector might have been reallocated, and it doesn't have
    // to be at the back any more, but I think each process can only have one
    // wait at a time.
    for (auto it = pending_waits.begin(); it != pending_waits.end(); ++it)
    {
        if (it->pid == active_pid)
        {
            // Set the return values.
            if (it->wstatus != nullptr)
                *(it->wstatus) = it->local_wstatus;
            int ret_val = static_cast<int>(it->wait_pids.front());

            // Remove the request from the list.
            pending_waits.erase(it);

            return ret_val;
        }
    }

    // We shouldn't be able to get here. Return an error.
    return -1;
}

/******************************************************************************/

void SignalManager::notify_wait(int ev)
{
    // Get the active process.
    size_t active_pid = global_kernel->get_scheduler().get_last();
    const Process* active = pt.get_process(active_pid);

    // Block switching. We don't want requests being added and deleted
    // underneath us.
    switch_blocked_for_notify = true;

    // Search through each pending wait request.
    for (wait_request& wr : pending_waits)
    {
        // TODO check the nature of the event matches.
        // Search through the wait PIDs of the request.
        auto it2 =
            klib::find(wr.wait_pids.begin(), wr.wait_pids.end(), active_pid);
        if (it2 != wr.wait_pids.end())
        {
            // We have a match. Update the local status of the request. We can't
            // change the user space value as it won't be in virtual memory.
            wr.local_wstatus = active->get_ret_status();

            // Delete all values from the wait PID list, except the one that
            // caused the event.
            wr.wait_pids = klib::vector<size_t> {active_pid};

            // Wake up the process.
            Process* proc = pt.get_process(wr.pid);
            if (proc->get_status() == ProcStatus::sleeping)
                proc->set_status(ProcStatus::runnable);

            switch_blocked_for_notify = false;
            return;
        }
    }

    // If we got here, no process is waiting for this event. Add it to the list
    // of events that have happened.
    happened_waits.emplace_back(active_pid, ev);
    switch_blocked_for_notify = false;
}

/******************************************************************************/

void SignalManager::purge_process(size_t pid)
{
    // Search pending polls and remove matches.
    for (auto it = pending_polls.begin(); it != pending_polls.end(); )
    {
        if (it->pid == pid)
            it = pending_polls.erase(it);
        else
            ++it;
    }

    // Search the list of wait events that have happened.
    for (auto it = happened_waits.begin(); it != happened_waits.end(); )
    {
        if (it->pid == pid)
            it = happened_waits.erase(it);
        else
            ++it;
    }

    // Search the list of wait requests.
    for (auto it = pending_waits.begin(); it != pending_waits.end(); )
    {
        if (it->pid == pid)
            it = pending_waits.erase(it);
        else
            ++it;
    }
}

/******************************************************************************/

void SignalManager::purge_process(const Process* proc)
{
    // Look up the processes PID.
    for (const auto& p : pt)
    {
        if (p.second == proc)
        {
            purge_process(p.first);
            return;
        }
    }

    // We didn't find the process. Guess there's nothing much to do. Orphaned
    // events will eventually get removed by their timeouts.
}

/******************************************************************************/

void SignalManager::tick_down(int tick_size)
{
    // Block switiching to make sure we decrease all time outs.
    switch_blocked_for_notify = true;

    // Search through the pending events and decrease their timeouts.
    for (auto it = pending_polls.begin(); it != pending_polls.end(); )
    {
        // Only consider positive values, negative disables timeout.
        if (it->time_left >= 0)
        {
            it->time_left -= tick_size;
            if (it->time_left <= 0)
            {
                // Get the process.
                Process* proc = pt.get_process(it->pid);
                // If the process doesn't exist any more, remove the event.
                if (proc == nullptr)
                {
                    it = pending_polls.erase(it);
                    continue;
                }
                // Wake up timed out processes.
                if (proc->get_status() == ProcStatus::sleeping)
                    proc->set_status(ProcStatus::runnable);
            }
        }
        ++it;
    }

    switch_blocked_for_notify = false;
}

/******************************************************************************
 ******************************************************************************/
