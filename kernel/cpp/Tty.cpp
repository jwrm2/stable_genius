#include "Tty.h"

#include <stddef.h>

#include <algorithm>
#include <string>

#include "Kernel.h"
#include "Logger.h"
#include "SignalManager.h"
#include "VgaController.h"

/******************************************************************************
 ******************************************************************************/

void Tty::write_char(char c)
{
    vga->write_char(c);
}

/******************************************************************************/

PollType Tty::poll_check(PollType cond) const
{
    PollType ret_val = PollType::pollnone;

    // We can always write.
    ret_val |= (cond & PollType::pollout);

    // We can only read if there's a newline in the buffer.
    if (newline_pos != -1)
        ret_val |= (cond & PollType::pollin);

    return ret_val;
}

/******************************************************************************/

klib::string Tty::read_chars(size_t count)
{
    // We can only read if we have a newline in the buffer.
    if (newline_pos == -1)
        return klib::string {""};

    size_t end_pos;
    if (count == 0)
        // If count is 0, just return up to and including the first newline.
        end_pos = newline_pos + 1;
    else
        // If count is not zero, return that many characters, or up to the first
        // newline, whichever is smaller.
        end_pos = klib::min(count, static_cast<size_t>(newline_pos) + 1);

    // Get our substring and delete it from the buffer.
    klib::string ret_val = buffer.substr(0, end_pos);
    buffer.erase(0, end_pos);
    // Reset the newline position.
    size_t pos = buffer.find('\n');
    if (pos == klib::string::npos)
        newline_pos = -1;
    else
        newline_pos = static_cast<int>(pos);

    return ret_val;
}

/******************************************************************************/

void Tty::add_char(char c)
{
    // Add character to the buffer.
    buffer += c;

    // Display the character on the display device.
    write_char(c);

    // If it's a newline, we signal that data is available for reading.
    if (c == '\n')
    {
        if (newline_pos == -1)
            newline_pos = buffer.size() - 1;

        SignalManager* sig = global_kernel->get_signal_manager();
        if (sig != nullptr)
            sig->notify_file(this, PollType::pollin);
    }
    
}

/******************************************************************************
 ******************************************************************************/
