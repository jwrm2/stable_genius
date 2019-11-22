#ifndef SERIAL_H
#define SERIAL_H

#include <stdarg.h>
#include <stdint.h>

#include <string>

#include "Device.h"

// Forward declarations
enum class PollType;

/**
    List of address of known serial ports.
 */
enum class SerialAddress : uint16_t {
    com1 = 0x03F8
};

/**
    List of the offsets of various ports from the base port address.
 */
enum class PortOffset : uint16_t {
    /** Data */
    data = 0x0,
    /** Interrupt enable */
    interrupt = 0x1,
    /** FIFO command */
    fifo = 0x2,
    /** Line command */
    line = 0x3,
    /** Modem command */
    modem = 0x4,
    /** Line status */
    line_status = 0x5,
    /** Modem status */
    modem_status = 0x6,
    /** Scratch */
    scratch = 0x7
};

/**
    Manages a serial port.
 */
class Serial : public CharacterDevice {
public:
    /**
        Sets up the Serial port for communication.

        @param port Base address of the serial port.
        @param d Divisor to set the baud rate. Rate is 115200/d bit/s. A divisor
                 of 12 gives 9600 bit/s.
        @param ic Interrupt control byte. 0x00 means all interrupts disabled.
        @param lc Line control byte. 0x03 means no break control, no parity
                  bits, one stop bit and a data length of 8 bits (8/N/1).
        @param bc Buffer control byte. 0xC7 means 14 byte buffers and clears
                  the buffers.
        @param mc Modem control byte. 0x03 is for output only.
     */
    explicit Serial(SerialAddress port,
                    uint16_t d = 1,
                    uint8_t ic = 0x00,
                    uint8_t lc = 0x03,
                    uint8_t bc = 0xC7,
                    uint8_t mc = 0x03);

    /**
        Destructor. Calls close().
     */
    virtual ~Serial() { close(); }

    /**
        Sets the baud rate.

        @param d The divisor: the baud rate become 115200/d bit/s.
     */
    void set_divisor(uint16_t d);

    /**
        Gets the current divisor.

        @return The divisor: the baud rate is 115200/d bit/s.
     */
    uint16_t get_divisor() const { return divisor; }

    /**
        Sets the interrupt configuration (message received, etc.).

        @param lc The configuration to set. See a serial port manual for
                  details.
     */
    void set_interrupt_conf(uint8_t ic);

    /**
        Get the current interrupt configuration (message received, etc.).

        @return The cuurent configuration. See a serial port manual for details.
     */
    uint8_t get_interrupt_conf() const { return interrupt_conf; }

    /**
        Sets the line configuration (parity bits, stop bits, etc.).

        @param lc The configuration to set. See a serial port manual for
                  details.
     */
    void set_line_conf(uint8_t lc);

    /**
        Get the current line configuration (parity bits, stop bits, etc.).

        @return The cuurent configuration. See a serial port manual for details.
     */
    uint8_t get_line_conf() const { return line_conf; }

    /**
        Sets the buffer configuration (size of buffers, etc.).

        @param bc The configuration to set. See a serial port manual for
                  details.
     */
    void set_buffer_conf(uint8_t bc);

    /**
        Get the current buffer configuration (size of buffers, etc.).

        @return The cuurent configuration. See a serial port manual for details.
     */
    uint8_t get_buffer_conf() const { return buffer_conf; }

    /**
        Sets the modem configuration (send/receive, interrupts, etc.).

        @param mc The configuration to set. See a serial port manual for
                  details.
     */
    void set_modem_conf(uint8_t mc);

    /**
        Get the current line configuration (send/receive, interrupts, etc.).

        @return The cuurent configuration. See a serial port manual for details.
     */
    uint8_t get_modem_conf() const { return modem_conf; }

    /**
        Send a character down the serial port.

        @param c Character to send.
     */
    virtual void write_char(char c) override;

    /**
        Waits until the pending write is complete, by polling the transmission
        buffer.

        @return Always returns 0, since all characters will have been written by
                the time it returns.
     */
    virtual int flush() override;

    /**
        Makes sure all characters have been sent, in preparation for destroying
        a FILE conncted to the port. Doesn't actually shut down the port, so it
        can be reattached to another FILE after close.

        @return Always returns 0, since all characters will have been written by
                the time it returns.
     */
    virtual int close() override;

    /**
        Determines whether a poll condition is currently satisfied.

        @param cond Bitmask of polling conditions to check.
        @return Mask representing the currently true events.
     */
    virtual PollType poll_check(PollType cond) const override;

    /**
        Reads characters. Stops at the requested number of characters or when
        no more characters are availble. TODO not yet implemented. Always
        returns an empty string.

        @param count Number of characters to read. 0 (the default) means read
               all availble characters.
        @return String containing all the characters read.
     */
    virtual klib::string read_chars(size_t = 0) override
    {
        return klib::string {""};
    }

private:
    // Address of data port.
    uint16_t data_port;
    // Address of interrupt enable port.
    uint16_t interrupt_port;
    // Address of FIFO command port.
    uint16_t fifo_command_port;
    // Address of line command port.
    uint16_t line_command_port;
    // Address of modem command port.
    uint16_t modem_command_port;
    // Address of line status port.
    uint16_t line_status_port;
    // Divisor to set baud rate.
    uint16_t divisor;
    // interrupt configuration byte/
    uint8_t interrupt_conf;
    // Line configuration byte.
    uint8_t line_conf;
    // Buffer configuration byte.
    uint8_t buffer_conf;
    // Modem configuartion byte.
    uint8_t modem_conf;

    /**
        Checks whether the transmission buffer is empty. This must be true
        before adding new data to the buffer.

        @return True if the transmit buffer is empty.
     */
    bool transmit_buffer_empty() const;

    /**
        Checks whether the receive buffer is full. This must be true before
        reading data from the buffer.

        @return True if the receive buffer is full.
     */
    bool receive_buffer_full() const;
};

#endif
