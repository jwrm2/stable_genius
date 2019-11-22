#include "Pit.h"

#include <stdint.h>

#include <ostream>
#include <string>

#include "io.h"

/******************************************************************************
 ******************************************************************************/

Pit::Pit(unsigned int freq)
{
    // Test for too low or too high frequencies.
    if (freq > base_freq)
        reload_value = 2; // 1 is not allowed for some modes.
    else if (freq <= slow_freq)
        reload_value = 0 - 1;
    // Calculate closest reload value.
    else
        reload_value = (base_freq / freq) + (base_freq % freq > freq / 2);

    // Get the actual frequency.
    irq_freq = base_freq / reload_value +
        (base_freq % reload_value > reload_value / 2);

    // Work out the IRQ millisecond values.
    irq_ms = reload_value * 1000 / base_freq;
    uint64_t temp = ((reload_value * 1000) % base_freq) * (1ll << 32);
    irq_fractions = static_cast<uint32_t>(temp / base_freq);

    // Make sure the counters are at zero.
    timer_ms = 0;
    timer_fractions = 0;

    // Configure the hardware.
    // Strictly we need to disable interrupts here, but we'll assume that we're
    // in kernel configuration and they are already disabled.
    outb(channel_0 | both_bytes | mode_2 | binary_mode, command_port);
    outb(static_cast<uint8_t>(reload_value), channel_0_data);
    outb(static_cast<uint8_t>(reload_value >> 8), channel_0_data);
}

/******************************************************************************/

void Pit::dump(klib::ostream& dest) const
{
    dest << "PIT configuration:\n";
    dest << "  Reload value: " << reload_value << '\n';
    dest << "  Frequency (Hz): " << irq_freq << '\n';
    dest << "  Whole milliseconds per tick: " << irq_ms << '\n';
    dest << "  Fractions of a millisecond per tick (out of 2^32): "
         << irq_fractions << '\n';
    dest << "  Current whole milliseconds: " << timer_ms << '\n';
    dest << "  Current fractions of a millisecond (out of 2^32): "
         << timer_fractions << '\n';

    dest.flush();
}

/******************************************************************************/

void Pit::sleep(unsigned int t)
{
    uint32_t end = t + timer_ms;
    uint32_t start_fractions = timer_fractions;

    while ((timer_ms == end && timer_fractions < start_fractions) ||
        timer_ms < end)
    {
        // This seems to be necessary, to stop compiler optimisations messing
        // with the loop.
        asm("nop");
    }
}

/******************************************************************************/

void Pit::tick()
{
    timer_fractions += irq_fractions;
    timer_ms += irq_ms;

    // Check for carry from the fractions.
    timer_ms += (timer_fractions < irq_fractions);
}

/******************************************************************************
 ******************************************************************************/
