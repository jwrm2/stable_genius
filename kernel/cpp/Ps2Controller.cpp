#include "Ps2Controller.h"

#include "io.h"
#include "Kernel.h"
#include "Logger.h"

/******************************************************************************
 ******************************************************************************/

Ps2Controller::Ps2Controller() : dev_1{true}, dev_2{true}
{
    // Disbale devices so they don't interfere with initialisation.
    command(Ps2Command::disable_1);
    command(Ps2Command::disable_2);

    // Make sure the output buffer is empty
    flush();

    // Disable interrupts (for the moment) and keyboard translation
    command(Ps2Command::read_conf);
    conf = get_response();
    if (!(conf & 0x20))
        dev_2 = false;
    conf &= 0xBC;
    command(Ps2Command::write_conf);
    send_data(conf);

    // Perform self test
    command(Ps2Command::self_test);
    uint8_t resp = get_response();
    switch (resp)
    {
    case static_cast<uint8_t>(Ps2Command::self_pass):
        break;
    case static_cast<uint8_t>(Ps2Command::self_fail):
        global_kernel->syslog()->error("PS/2 controller self test failed\n");
        return;
    default:
        global_kernel->syslog()->error("Unexpected response from PS/2 controller self test\n");
        return;
    }

    // Determine if the second port exists (but only is we don't already know)
    if (!dev_2)
    {
        command(Ps2Command::enable_2);
        command(Ps2Command::read_conf);
        conf = get_response();
        if (conf & 0x20)
            dev_2 = false;
        command(Ps2Command::disable_2);
    }

    // Test the ports
    command(Ps2Command::test_1);
    resp = get_response();
    if (resp != static_cast<uint8_t>(Ps2Command::dev_pass))
    {
        global_kernel->syslog()->warn("PS/2 device 1 test failed\n");
        dev_1 = false;
    }
    if (dev_2)
    {
        command(Ps2Command::test_2);
        resp = get_response();
        if (resp != static_cast<uint8_t>(Ps2Command::dev_pass))
        {
            global_kernel->syslog()->warn("PS/2 device 2 test failed\n");
            dev_2 = false;
        }
    }

    if (!dev_1 && !dev_2)
    {
        global_kernel->syslog()->error("No devices for the PS/2 controller\n");
        return;
    }

    // Enable devices
    if (dev_1)
        command(Ps2Command::enable_1);
    if (dev_2)
        command(Ps2Command::enable_2);

    // Update configuration byte store, changed by enabling devices
    command(Ps2Command::read_conf);
    conf = get_response();

    // Reset devices
    flush();
    if(dev_1)
    {
        send_data(static_cast<uint8_t>(Ps2Command::dev_reset));
        resp = get_response();
        if (resp != static_cast<uint8_t>(Ps2Command::dev_ack))
            global_kernel->syslog()->warn("PS/2 device 1 reset failed\n");
        flush();
    }
    if(dev_2)
    {
        command(Ps2Command::write_2);
        send_data(static_cast<uint8_t>(Ps2Command::dev_reset));
        resp = get_response();
        if (resp != static_cast<uint8_t>(Ps2Command::dev_ack))
            global_kernel->syslog()->warn("PS/2 device 2 reset failed\n");
        flush();
    }

    // Leave interrupts disabled until the end of kernel initialisation
}

/******************************************************************************/

void Ps2Controller::command(Ps2Command data)
{
    wait_write();
    outb(static_cast<uint8_t>(data), static_cast<uint16_t>(Ps2Addr::cont));
}

/******************************************************************************/

void Ps2Controller::disable_interrupts()
{
    if (dev_1)
        conf &= 0xFE;
    if (dev_2)
        conf &= 0xFD;
    command(Ps2Command::write_conf);
    send_data(conf);
}

/******************************************************************************/

void Ps2Controller::enable_interrupts()
{
    if (dev_1)
        conf |= 0x01;
    if (dev_2)
        conf |= 0x02;
    command(Ps2Command::write_conf);
    send_data(conf);
}

/******************************************************************************/

void Ps2Controller::flush()
{
    while (status() & 0x01)
        inb(static_cast<uint16_t>(Ps2Addr::dev));
}

/******************************************************************************/

uint8_t Ps2Controller::get_response()
{
    if (wait_read())
        return inb(static_cast<uint16_t>(Ps2Addr::dev));
    else
        return 0x00;
}

/******************************************************************************/

bool Ps2Controller::send_data(uint8_t data)
{
    if (wait_write())
    {
        outb(data, static_cast<uint16_t>(Ps2Addr::dev));
        return true;
    }
    else
        return false;
}

/******************************************************************************/

uint8_t Ps2Controller::status()
{
    return inb(static_cast<uint16_t>(Ps2Addr::cont));
}

/******************************************************************************/

bool Ps2Controller::wait_read()
{
    for (unsigned int i = 0; i < max_tries; ++i)
        if (status() & 0x01)
            return true;

    return false;
}

/******************************************************************************/

bool Ps2Controller::wait_write()
{
    while (status() & 0x02) ;

    for (unsigned int i = 0; i < max_tries; ++i)
        if (!(status() & 0x02))
            return true;

    return false;    
}

/******************************************************************************
 ******************************************************************************/
