#include "Serial.h"

#include <stddef.h>

#include "Device.h"
#include "io.h"
#include "SignalManager.h"

/******************************************************************************
 ******************************************************************************/

Serial::Serial(SerialAddress port,
               uint16_t d,
               uint8_t ic,
               uint8_t lc,
               uint8_t bc,
               uint8_t mc) :
    CharacterDevice {DeviceType::serial_port},
    data_port{static_cast<uint16_t>(static_cast<uint16_t>(port) +
        static_cast<uint16_t>(PortOffset::data))},
    interrupt_port{static_cast<uint16_t>(static_cast<uint16_t>(data_port) + 
        static_cast<uint16_t>(PortOffset::interrupt))},
    fifo_command_port{static_cast<uint16_t>(static_cast<uint16_t>(data_port) +
        static_cast<uint16_t>(PortOffset::fifo))},
    line_command_port{static_cast<uint16_t>(static_cast<uint16_t>(data_port) +
        static_cast<uint16_t>(PortOffset::line))},
    modem_command_port{static_cast<uint16_t>(static_cast<uint16_t>(data_port) +
        static_cast<uint16_t>(PortOffset::modem))},
    line_status_port{static_cast<uint16_t>(static_cast<uint16_t>(data_port) +
        static_cast<uint16_t>(PortOffset::line_status))},
    divisor{d},
    interrupt_conf{ic},
    line_conf{lc},
    buffer_conf{bc},
    modem_conf{mc}
{
    set_divisor(d);
    set_interrupt_conf(ic),
    set_line_conf(lc);
    set_buffer_conf(bc);
    set_modem_conf(mc);
}

/******************************************************************************/

void Serial::set_divisor(uint16_t d)
{
    divisor = d;

    // Set DLAB (bit 7 of line configuration byte) to prepare for receipt.
    outb(0x80 | line_conf, line_command_port);
    // Send high byte.
    outb(static_cast<uint8_t>(divisor >> 8 & 0x00FF), interrupt_port);
    // Send low byte.
    outb(static_cast<uint8_t>(divisor & 0x00FF), data_port);
    // Unset DLAB
    line_conf &= 0x7F;
    outb(line_conf, line_command_port);
}

/******************************************************************************/

void Serial::set_interrupt_conf(uint8_t ic)
{
    interrupt_conf = ic;
    outb(ic, interrupt_port);
}

/******************************************************************************/

void Serial::set_line_conf(uint8_t lc)
{
    line_conf = lc;
    outb(lc, line_command_port);
}

/******************************************************************************/

void Serial::set_buffer_conf(uint8_t bc)
{
    buffer_conf = bc;
    outb(bc, fifo_command_port);
}

/******************************************************************************/

void Serial::set_modem_conf(uint8_t mc)
{
    modem_conf = mc;
    outb(mc, modem_command_port);
}

/******************************************************************************/

void Serial::write_char(char c)
{
    // Wait until the buffer is empty.
    flush();
    // Send data.
    outb(static_cast<uint8_t>(c), data_port);
}

/******************************************************************************/

int Serial::flush()
{
    // Wait until the buffer is empty.
    while (!transmit_buffer_empty()) ;
    return 0;
}

/******************************************************************************/

int Serial::close()
{
    // Wait until the buffer is empty.
    return flush();
}

/******************************************************************************/

PollType Serial::poll_check(PollType cond) const
{
    PollType ret_val = PollType::pollnone;

    // Check if we're ready to write by looking at the transmit buffer.
    if ((cond & PollType::pollout) != PollType::pollnone &&
        transmit_buffer_empty())
        ret_val |= PollType::pollout;
    // Check if we're ready to read by looking at the receive buffer.
    if ((cond & PollType::pollin) != PollType::pollnone &&
        receive_buffer_full())
        ret_val |= PollType::pollin;

    return ret_val;
}

/******************************************************************************/

bool Serial::transmit_buffer_empty() const
{
    // The buffer is empty if bit 5 of the line status register is set.
    uint8_t data = inb(line_status_port);
    return (data & 0x20);
}

/******************************************************************************/

bool Serial::receive_buffer_full() const
{
    // There is data to read if bit 0 of the line status register is set.
    uint8_t data = inb(line_status_port);
    return (data & 0x1);
}

/******************************************************************************
 ******************************************************************************/
