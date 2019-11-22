#include "Pic.h"

#include <stdint.h>

#include "io.h"
#include "InterruptHandler.h"

/******************************************************************************
 ******************************************************************************/

PicDriver::PicDriver(PicMask master_mask, PicMask slave_mask)
{
    // Prepare for initialisation
    outb(static_cast<uint8_t>(PicCommand::init),
         static_cast<uint16_t>(PicAddress::master_command));
    outb(static_cast<uint8_t>(PicCommand::init),
         static_cast<uint16_t>(PicAddress::slave_command));
    // Set the offsets
    outb(static_cast<uint8_t>(InterruptNumber::pic1_start),
         static_cast<uint16_t>(PicAddress::master_data));
    outb(static_cast<uint8_t>(InterruptNumber::pic2_start),
         static_cast<uint16_t>(PicAddress::slave_data));
    // Set up cascading
    outb(static_cast<uint8_t>(PicCommand::master_cascade),
         static_cast<uint16_t>(PicAddress::master_data));
    outb(static_cast<uint8_t>(PicCommand::slave_cascade),
         static_cast<uint16_t>(PicAddress::slave_data));
    // Set to x86 mode
    outb(static_cast<uint8_t>(PicCommand::x86_mode),
         static_cast<uint16_t>(PicAddress::master_data));
    outb(static_cast<uint8_t>(PicCommand::x86_mode),
         static_cast<uint16_t>(PicAddress::slave_data));
    // Set the masks
    set_mask(PicType::master, master_mask);
    set_mask(PicType::slave, slave_mask);   
}

/******************************************************************************/

void PicDriver::acknowledge(PicType id)
{
    switch(id)
    {
    case PicType::slave:
    {
        outb(static_cast<uint8_t>(PicCommand::eoi),
             static_cast<uint16_t>(PicAddress::slave_command));
        outb(static_cast<uint8_t>(PicCommand::eoi),
             static_cast<uint16_t>(PicAddress::master_command));
        break;
    }
    case PicType::master:
    {
        outb(static_cast<uint8_t>(PicCommand::eoi),
             static_cast<uint16_t>(PicAddress::master_command));
        break;
    }
    }
}

/******************************************************************************/

uint16_t PicDriver::get_irr()
{
    outb(static_cast<uint8_t>(PicCommand::read_irr),
         static_cast<uint16_t>(PicAddress::master_command));
    outb(static_cast<uint8_t>(PicCommand::read_irr),
         static_cast<uint16_t>(PicAddress::slave_command));
    uint8_t slave_irr = inb(static_cast<uint16_t>(PicAddress::slave_command));
    uint8_t master_irr = inb(static_cast<uint16_t>(PicAddress::master_command));
    return static_cast<uint16_t>(slave_irr) << 8 |
           static_cast<uint16_t>(master_irr);
}

uint8_t PicDriver::get_irr(PicType id)
{
    switch(id)
    {
    case PicType::master:
    {
    outb(static_cast<uint8_t>(PicCommand::read_irr),
         static_cast<uint16_t>(PicAddress::master_command));
        return inb(static_cast<uint16_t>(PicAddress::master_command));
    }
    case PicType::slave:
    {
    outb(static_cast<uint8_t>(PicCommand::read_irr),
         static_cast<uint16_t>(PicAddress::slave_command));
        return inb(static_cast<uint16_t>(PicAddress::slave_command));
    }
    }
    return 0;
}

/******************************************************************************/

uint16_t PicDriver::get_isr()
{
    outb(static_cast<uint8_t>(PicCommand::read_isr),
         static_cast<uint16_t>(PicAddress::master_command));
    outb(static_cast<uint8_t>(PicCommand::read_isr),
         static_cast<uint16_t>(PicAddress::slave_command));
    uint8_t slave_isr = inb(static_cast<uint16_t>(PicAddress::slave_command));
    uint8_t master_isr = inb(static_cast<uint16_t>(PicAddress::master_command));
    return static_cast<uint16_t>(slave_isr) << 8 |
           static_cast<uint16_t>(master_isr);
}

uint8_t PicDriver::get_isr(PicType id)
{
    switch(id)
    {
    case PicType::master:
    {
    outb(static_cast<uint8_t>(PicCommand::read_isr),
         static_cast<uint16_t>(PicAddress::master_command));
        return inb(static_cast<uint16_t>(PicAddress::master_command));
    }
    case PicType::slave:
    {
    outb(static_cast<uint8_t>(PicCommand::read_isr),
         static_cast<uint16_t>(PicAddress::slave_command));
        return inb(static_cast<uint16_t>(PicAddress::slave_command));
    }
    }
    return 0;
}

/******************************************************************************/

void PicDriver::set_mask(PicType id, PicMask mask)
{
    switch(id)
    {
    case PicType::slave:
    {
        outb(static_cast<uint8_t>(mask),
             static_cast<uint16_t>(PicAddress::slave_data));
        break;
    }
    case PicType::master:
    {
        outb(static_cast<uint8_t>(mask), 
             static_cast<uint16_t>(PicAddress::master_data));
        break;
    }
    }
}

/******************************************************************************
 ******************************************************************************/
