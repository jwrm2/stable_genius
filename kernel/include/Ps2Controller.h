#ifndef PS2CONTROLLER_H
#define PS2CONTROLLER_H

#include <stdint.h>

/**
    IO ports for interaction with the PS/2 controller.
 */
enum class Ps2Addr {
    // For interacting with the attached devices.
    dev = 0x60,
    // For interacting with the controller itself.
    cont = 0x64
};

/**
    Commands for the controller.
 */
enum class Ps2Command : uint8_t {
    // Device test passed
    dev_pass = 0x00,
    // Read configuration byte
    read_conf = 0x20,
    // Self test passed
    self_pass = 0x55,
    // Write configuration byte
    write_conf = 0x60,
    // Disable second port
    disable_2 = 0xA7,
    // Enable second port
    enable_2 = 0xA8,
    // Test second port
    test_2 = 0xA9,
    // Perform self test
    self_test = 0xAA,
    // Test first port
    test_1 = 0xAB,
    // Disable first port
    disable_1 = 0xAD,
    // Enable first port
    enable_1 = 0xAE,
    // Write to the second PS/2 device
    write_2 = 0xD4,
    // Device acknowledge
    dev_ack = 0xFA,
    // Self test failed
    self_fail = 0xFC,
    // Device reset
    dev_reset = 0xFF
};

/**
    Drive for the PS/2 Controller (keyboard, mouse etc.)
 */
class Ps2Controller {
public:
    /**
        Perform all the required initialisation.
     */
    Ps2Controller();

    /**
        Send a command, checking the controller is ready.

        @param comm Command to send.
     */
    void command(Ps2Command comm);

    /**
        Disable interrupts for both devices.
     */
    void disable_interrupts();

    /**
        Enable interrupts for both devices.
     */
    void enable_interrupts();

    /**
        Flush the data output buffer.
     */
    void flush();

    /**
        Get a response from a command, checking that the controller is ready.

        @return Response from the previous command. Will be 0x00 if the read
                has failed.
     */
    uint8_t get_response();

    /**
        Send the data byte for a command, checking that the controller is ready.

        @param data Data for the previous command.
        @return Whether the write succeeded.
     */
    bool send_data(uint8_t data);

private:
    // Maximum number of attempts when waiting to read or write
    static const unsigned int max_tries = 10;
    // Whether devices 1 and 2 seem to be working
    bool dev_1;
    bool dev_2;
    // Store the configuration byte, so we don't have to fetch it before
    // disableing interrupts (which would raise an interrupt...)
    uint8_t conf;

    /**
        Read the status byte for the controller. Useful for checking whether it
        is ready for input or for a response being read.

        @return Status of the controller. See
                http://wiki.osdev.org/"8042"_PS/2_Controller for details.
     */
    uint8_t status();

    /**
        Using the status, wait util the data port is ready to be read
        from, or the maximum number of tries is exceeded.

        @return Whether the port is ready for reading.
     */
    bool wait_read();

    /**
        Using the status, wait until the data port or command port
        is ready to be written to, or the maximum number of tries is exceeded.
        Must be done for either.

        @return Whether the port is ready for writing.
     */
    bool wait_write();
};

#endif
