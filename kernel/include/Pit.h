#ifndef PIT_H
#define PIT_H

#include <stdint.h>

#include <ostream>

/**
    Class for the programmable interval timer (PIT).
 */
class Pit {
public:
    /**
        Default tick frequency, corresponding to a tick approximately every
        centisecond.
     */
    static constexpr unsigned int default_freq = 100;

    /**
        I/O ports.
     */
    enum io_port : uint16_t {
        /** Channel 0 data */
        channel_0_data = 0x40,
        /** Channel 1 data */
        channel_1_data = 0x41,
        /** Channel 2 data */
        channel_2_data = 0x42,
        /** Mode/Command */
        command_port = 0x43
    };

    /**
        Channel selectors in the configuration command.
     */
    enum channel : uint8_t {
        /** Channel 0 */
        channel_0 = 0x0,
        /** Channel 1 */
        channel_1 = 0x40,
        /** Channel 2 */
        channel_2 = 0x80,
        /** Read-back command */
        read_back = 0xC0
    };

    /**
        Access modes in the configuration command.
     */
    enum access_mode : uint8_t {
        /** Latch count value command */
        latch_count = 0x0,
        /** Low byte only */
        low_byte = 0x10,
        /** High byte only */
        high_byte = 0x20,
        /** Both bytes */
        both_bytes = 0x30
    };

    /**
        Operating modes in the configuration command.
     */
    enum operating_mode : uint8_t {
        /** Interrupt on terminal count */
        mode_0 = 0x0,
        /** Hardware retriggerable one-shot */
        mode_1 = 0x2,
        /** Rate generator */
        mode_2 = 0x4,
        /** Square wave generator */
        mode_3 = 0x6,
        /** Software triggered strobe */
        mode_4 = 0x8,
        /** Hardware triggered strobe */
        mode_5 = 0xA
    };

    /**
        Counting modes in the configuration command.
     */
    enum counting_mode {
        /** Binary mode */
        binary_mode = 0x0,
        /** Four digit BCD mode */
        bcd_mode = 0x1
    };

    /**
        Constructor. Configures the PIT to generate an interrupt at the
        closest possible frequency to the one requested.

        @param freq The requested frequency, in Hz
     */
    explicit Pit(unsigned int freq);

    /**
        Default constructor. Uses the default frequency.
     */
    Pit() : Pit{default_freq} {}

    /**
        Prints the current configuration to the provided stream.

        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

    /**
        Gets the number of complete milliseconds between each PIT interrupt.

        @return PIT period, in milliseconds, rounded down.
     */
    uint32_t period() const { return irq_ms; }

    /**
        Sleeps for at least the number of milliseconds specified. This is a
        busy kernel sleep, not for user processes.

        @param t Time to sleep for, in milliseconds.
     */
    void sleep(unsigned int t);

    /**
        Advances the systme clock by one count.
     */
    void tick();

    /**
        Gets the current system clock.

        @return Time, in complete milliseconds, since the PIT was initialised.
     */
    uint32_t time() const { return timer_ms; }

private:
    // Whole number of milliseconds in the time.
    unsigned int timer_ms;
    // Fractions of milliseconds in the time.
    unsigned int timer_fractions;
    // Whole milliseconds between each IRQ.
    unsigned int irq_ms;
    // Fractions of milliseconds between each IRQ.
    unsigned int irq_fractions;
    // Frequency (in Hz) of IRQ generation.
    unsigned int irq_freq;
    // Frequency divisor stored by the PIT.
    uint16_t reload_value;

    // Inherent frequency of the PIT, in Hz.
    static constexpr unsigned int base_freq = 1193181;
    // Lowest possible frequency (base_freq / 65536)
    static constexpr unsigned int slow_freq = 18;
};

#endif /* PIT_H */
