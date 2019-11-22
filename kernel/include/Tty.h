#ifndef TTY_H
#define TTY_H

#include <stddef.h>

#include <string>

#include "Device.h"

// Forward declarations
class VgaController;
enum class KeyCode;
class KeyState;

/**
    This class manages a TTY. It has a device to display the output, currently
    the VGA controller. Input is received, currently from the keyboard. The
    class has an input buffer that can be modified and is displayed on the
    output. This is a character device and appeares in the device file system as
    /dev/tty. The device reports that it is unable to be read from until it
    receives a return. It signals the SignalManager when it receives an enter.
    Thereafter, a read will give everything in the buffer up to the first enter.
    The buffer is stored as a string and has o limit other than the physical
    memory available and the largest number stored by a size_t.
 */
class Tty : public CharacterDevice {
public:
    /**
        Constructor. Sets the output device.

        @param v Output display device.
     */
    Tty(VgaController* v) :
        CharacterDevice {DeviceType::console},
        vga {v},
        buffer {},
        newline_pos {-1}
    {}

    /**
        Destructor. Calls close.
     */
    virtual ~Tty() { close(); }

    /**
        Send a character to the display device.

        @param c Character to add.
     */
    virtual void write_char(char c) override;

    /**
        Waits until the pending write is complete. This does nothing as output
        is always synchronously sent to the input buffer.

        @return 0 on success, otherwise EoF.
     */
    virtual int flush() override { return 0; };

    /**
        Performs any clean up operations required. Clears the input buffer.

        @return 0 on success, otherwise EoF.
     */
    virtual int close() override { buffer.clear(); return 0; }

    /**
        Determines whether a poll condition is currently satisfied. We can
        always write. We can only read if a complete line is stored.

        @param cond Bitmask of polling conditions to check.
        @return Mask representing the currently true events.
     */
    virtual PollType poll_check(PollType cond) const override;

    /**
        Reads characters. Stops at the requested number of characters, a newline
        or when no more characters are availble.

        @param count Number of characters to read. 0 (the default) means read
               all availble characters. Stops at a newline in all cases.
        @return String containing all the characters read.
     */
    virtual klib::string read_chars(size_t count = 0) override;

    /**
        Add a character to the input buffer.

        @param c Character to add.
     */
    virtual void add_char(char c);

    /**
        Receives a key code from the input device. This is full information
        about the key pressed and the state of control buttons.
        TODO not yet implemented.

        @param k Keycode for the character pressed.
        @param s State of modifier keys.
     */
    virtual void key(KeyCode k, const KeyState& s) { (void)k; (void)s; }

protected:
    // Display device.
    VgaController* vga;
    // Input buffer.
    klib::string buffer;
    // Position of the first newline in the buffer. -1 means no new line.
    int newline_pos;
};

#endif /* TTY_H */
