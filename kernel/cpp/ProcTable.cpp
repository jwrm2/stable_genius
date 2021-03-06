#include "ProcTable.h"

#include <stddef.h>

#include <map>

#include "DevFileSystem.h"
#include "Device.h"
#include "FileSystem.h"
#include "Kernel.h"
#include "Logger.h"
#include "Process.h"

/******************************************************************************
 ******************************************************************************/

ProcTable::ProcTable(Process* p) : tab{}, last_pid{init_pid}
{
    if (p != nullptr)
        tab[init_pid] = p;
}

/******************************************************************************/

void ProcTable::erase(size_t pid)
{
    if (tab.count(pid))
    {
        delete tab[pid];
        tab.erase(pid);
    }
}

/******************************************************************************/

size_t ProcTable::insert(Process* p)
{
    size_t start_pid = last_pid;

    if (last_pid == max_pid - 1)
        last_pid = 0;

    for (++last_pid; tab.count(last_pid); ++last_pid)
    {
        if (last_pid == start_pid)
            return 0;
        else if (last_pid == max_pid - 1)
            last_pid = 0;
    }

    tab[last_pid] = p;
    return last_pid;
}

/******************************************************************************/

bool ProcTable::swap_in(size_t pid)
{
//    global_kernel->syslog()->info("swap_in: pid = %X\n", pid);

    // Check whether the process exists.
    if (tab.count(pid) == 0)
        return false;

    Process* p = tab[pid];

    // Check whether the process is runnable (active is also ok).
    if (p->get_status() != ProcStatus::runnable &&
        p->get_status() != ProcStatus::active)
        return false;

//    p->get_pdt().dump(*global_kernel->syslog()->device());

    // Resume process execution.
    p->resume();

    // We'll only get here if we failed to resume the process.
    return false;
}

/******************************************************************************/

void ProcTable::swap_out(size_t pid, const InterruptRegisters& ir,
        const InterruptStack& is)
{
    Process* p = tab[pid];

    // The pid is just a hint. Whatever is provided, we want the currently
    // active process.
/*    if (!p || p->get_status() != ProcStatus::active)
    {
        auto it = tab.begin();
        for (; it != tab.end(); ++it)
            if (it->second->get_status() == ProcStatus::active)
                break;

        if (it == tab.end())
            // There is no active process. This might happen if the active
            // process has gone to sleep.
            return;

        p = it->second;
    }*/

    // Actually, if we're yielding, the process might have been put to sleep
    // already, so we can't rely on the current process being active.

    // Save the stack and registers state.
    p->set_registers(ir);
    p->set_interrupt_stack(is);

    // If the process is active, put it to runnable. It might already be asleep.
    if (p->get_status() == ProcStatus::active)
        p->set_status(ProcStatus::runnable);
}

/******************************************************************************
 ******************************************************************************/

FileTable::FileDescription::FileDescription(const klib::string& n,
    klib::ios_base::openmode mode) :
    name {n},
    count {1},
    dev {nullptr},
    fs {}
{
    // The file name could be a /dev device, in which case we simply need to ask
    // the dev file system for the device, or it might be from a mounted file
    // system, in which case we first need to translate to a dev device. We can
    // find out by asking the VFS what file system the file resides on.
    klib::string s = global_kernel->get_vfs()->lookup(n)->get_drv_name();

    if (s == "")
    {
        // If the drive name is blank, it's some sort of virtual file system,
        // maybe dev, maybe not. It it's dev, we can just look up the original
        // name. If it's not, the dev lookup will return nothing, which is fine
        // cos there's no underlying device for a virtual fs.
        dev = global_kernel->get_vfs()->get_dev()->get_device_driver(n);
        // If dev is a character device, we want the file stream to be
        // unbuffered. Strictly, setting the stream to unbuffered should occur
        // before opening, which is why we have default initialised the file
        // stream above.
        if (dev != nullptr && (dev->get_type() == DeviceType::serial_port ||
            dev->get_type() == DeviceType::console))
            fs.rdbuf()->pubsetbuf(nullptr, 0);
    }
    else
        // If the drive name is not blank, it's been set to a dev name, so now
        // we can look it up.
        dev = global_kernel->get_vfs()->get_dev()->get_device_driver(s);

    // Initialise the file stream.
    fs.open(name, mode);

    // Set count to zero on failure to open.
    if (!fs)
        count = 0;
}

/******************************************************************************
 ******************************************************************************/

int FileTable::open_file(const klib::string& name,
    klib::ios_base::openmode mode)
{
    // Each call to open creates a new file description. The same file can have
    // multiple open descriptions.
    // Find an unused key. There's probably a more efficient way to do this.
    int key;
    for (key = 1; file_list.find(key) != file_list.end(); ++key) ;

    // Create the description.
    file_list.insert(klib::pair<int, FileDescription>
        {key, FileDescription {name, mode}});
    auto it = file_list.find(key);
    if (it != file_list.end() && it->second.count == 0)
    {
        // Failed to open.
        file_list.erase(key);
        return -1;
    }

    return it->first;
}

/******************************************************************************/

int FileTable::close_file(int key)
{
    auto it = file_list.find(key);
    if (it == file_list.end())
        return -1;

    FileDescription& fd = it->second;
    --fd.count;
    if (fd.count <= 0)
    {
        // Erasing the key will call close() on the file, which will delete the
        // file if the hard link count has dropped to zero.
        file_list.erase(key);
    }

    return 0;
}

/******************************************************************************/

int FileTable::copy_file(int key)
{
    auto it = file_list.find(key);
    if (it == file_list.end())
        return -1;

    ++(it->second.count);

    return 0;
}

/******************************************************************************/

const klib::string& FileTable::get_name(int key) const
{
    static klib::string empty {""};
    auto it = file_list.find(key);
    if (it == file_list.end())
        return empty;

    return it->second.name;
}

/******************************************************************************/

Device* FileTable::get_dev(int key) const
{
    auto it = file_list.find(key);
    if (it == file_list.end())
        return nullptr;

    return it->second.dev;
}

/******************************************************************************/

klib::fstream& FileTable::get_stream(int key)
{
    static klib::fstream empty {};
    auto it = file_list.find(key);
    if (it == file_list.end())
        return empty;

    return it->second.fs;
}

/******************************************************************************/

int FileTable::is_open(const klib::string& name) const
{
    // Search the descriptions list for the name file.
    for (auto it = file_list.begin(); it != file_list.end(); ++it)
        if (name == it->second.name)
            // File found. Return the opne count.
            return it->second.count;

    // File not found. Return 0.
    return 0;
}

/******************************************************************************
 ******************************************************************************/
