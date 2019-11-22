#include "Ide.h"

#include <cstdio>
#include <ostream>
#include <string>

#include "DevFileSystem.h"
#include "FileSystem.h"
#include "InterruptHandler.h"
#include "io.h"
#include "Kernel.h"
#include "Logger.h"
#include "Pci.h"
#include "SignalManager.h"
#include "util.h"

/******************************************************************************
 ******************************************************************************/

IdeController::IdeController(uint32_t b, uint32_t d, uint32_t f) :
    PciDevice{b, d, f}
{
    configure();
}

IdeController::IdeController(const PciDevice& other) :
    PciDevice{other.get_bus(), other.get_device(), other.get_function()}
{
    configure();
}

/******************************************************************************/

void IdeController::dump(klib::ostream& dest) const
{
    klib::string tmp;

    dest << "IDE controller at " << get_location() << " on the PCI bus.\n";
    dest << "  Driver located at address " << this << '\n';
    dest << "  " << (serial ? "Serial" : "Parallel") << " controller\n";

    dest << "  First interrupt at offset " << klib::hex;
    dest << static_cast<uint32_t>(int1) << klib::dec << '\n';
    if (!serial)
    {
        dest << "  Second interrupt at offset " << klib::hex;
        dest << static_cast<uint32_t>(int2) << klib::dec << '\n';
    }

    for (size_t cha = 0; cha < 2; ++cha)
    {
        klib::string cha_name {cha == 0 ? "Primary" : "Secondary"};

        dest << "  " << cha_name << " channel port at " << klib::hex << channels[cha].base << '\n';
        dest << "  " << cha_name << " control channel port at " << channels[cha].ctrl << '\n';
        dest << "  " << cha_name << " BMIDE base port at " << channels[cha].bmide << '\n';
        dest << "  " << cha_name << " interrupts disabled: " << klib::boolalpha << channels[cha].nien << '\n';
        dest << klib::noboolalpha << klib::dec;
    }

    for (unsigned int cha = 0; cha < 2; ++cha)
    {
        for (unsigned int ra = 0; ra < 2; ++ra)
        {
            unsigned int dev = 2 * cha + ra;

            if (!devs[dev].exists)
                continue;

            klib::string cha_str
                {devs[dev].cha == channel::primary ? "Primary" : "Secondary"};
            klib::string ra_str
                {devs[dev].ra == rank::master ? "Master" : "Slave"};
            klib::string type_str
                {devs[dev].type == interface_type::ata ? "PATA" : "PATAPI"};

            dest << "  " << cha_str << " channel: " << ra_str << " device\n";
            dest << "    " << type_str << " interface\n";
            dest << "    ";
            dest << format_bytes(static_cast<uint64_t>(devs[dev].size) *
                static_cast<uint64_t>(devs[dev].sector_size)) << '\n';
            dest << "    " << devs[dev].model << '\n'; 
        }
    }

    dest.flush();
}

/******************************************************************************/

DiskIoError IdeController::ata_read(channel cha, rank ra, uint64_t off,
    void* addr, size_t sz)
{
    size_t dev = 2 * static_cast<uint8_t>(cha) + static_cast<uint8_t>(ra);
    address_mode mode;

    // Prepare mode and address.
    DiskIoError ret_val = prepare_io(cha, ra, off, sz, mode);
    if (ret_val != DiskIoError::success)
        return ret_val;

    // Send the command to the command register.
    if (mode == address_mode::lba48)
        ide_write(cha, register_offset::command,
            static_cast<uint8_t>(commands::read_pio_ext));
    else
        ide_write(cha, register_offset::command,
            static_cast<uint8_t>(commands::read_pio));

    // Do the actual read.
    char* v_addr = static_cast<char*>(addr);
    uint8_t no_sects = sz / devs[dev].sector_size;
    for (size_t i = 0; i < no_sects; ++i)
    {
        polling_response perr = polling(static_cast<channel>(cha), true);
        if(perr != polling_response::no_error)
            return DiskIoError::hardware_fault;

        insw(channels[static_cast<uint8_t>(cha)].base +
            static_cast<uint16_t>(register_offset::data) - low_byte_adjust,
            v_addr, devs[dev].sector_size / 2);

        v_addr += devs[dev].sector_size;
    }

    // Wait to make sure everything is finished.
    polling(static_cast<channel>(cha), false);

    return DiskIoError::success;
}

/******************************************************************************/

DiskIoError IdeController::ata_write(channel cha, rank ra, uint64_t off,
    const void* addr, size_t sz)
{
    size_t dev = 2 * static_cast<uint8_t>(cha) + static_cast<uint8_t>(ra);
    address_mode mode;

    // Prepare mode and address.
    DiskIoError ret_val = prepare_io(cha, ra, off, sz, mode);
    if (ret_val != DiskIoError::success)
        return ret_val;

    // Send the command to the command register.
    if (mode == address_mode::lba48)
        ide_write(cha, register_offset::command,
            static_cast<uint8_t>(commands::write_pio_ext));
    else
        ide_write(cha, register_offset::command,
            static_cast<uint8_t>(commands::write_pio));

    // Make sure we're ready.
    polling_response perr = polling(static_cast<channel>(cha), true);
    if (perr == polling_response::write_protected)
        return DiskIoError::read_only;
    else if(perr != polling_response::no_error)
        return DiskIoError::hardware_fault;

    // Do the actual write.
    const char* v_addr = static_cast<const char*>(addr);
    uint8_t no_sects = sz / devs[dev].sector_size;
    for (size_t i = 0; i < no_sects; ++i)
    {
        outsw(channels[static_cast<uint8_t>(cha)].base +
            static_cast<uint16_t>(register_offset::data) - low_byte_adjust,
            v_addr, devs[dev].sector_size / 2);

        v_addr = v_addr + devs[dev].sector_size;

        // Wait for ready again.
        polling(static_cast<channel>(cha), false);
    }

    // Cache flush.
    if (mode == address_mode::lba48)
        ide_write(cha, register_offset::command,
            static_cast<uint8_t>(commands::cache_flush_ext));
    else
        ide_write(cha, register_offset::command,
            static_cast<uint8_t>(commands::cache_flush));

    // Wait to make sure everything is finished.
    polling(static_cast<channel>(cha), false);

    return DiskIoError::success;
}

/******************************************************************************/

void IdeController::configure()
{
    // If the device is not an IDE controller, we'll set ex to false to indicate
    // that. It could still be a valid PCI device.
    if (get_class() != cl_mass_storage ||
        get_subclass() != scl_mass_storage_ide)
        ex = false;

    // Don't do anything if the PCI device does not exist.
    if (!ex)
        return;

    configure_irq();
    configure_bar();
    configure_devices();
}

/******************************************************************************/

void IdeController::configure_bar()
{
    // Read the ports.
    uint16_t primary_addr = static_cast<uint16_t>(get_bar(0));
    uint16_t primary_control_addr = static_cast<uint16_t>(get_bar(1)); 
    uint16_t secondary_addr = static_cast<uint16_t>(get_bar(2));
    uint16_t secondary_control_addr = static_cast<uint16_t>(get_bar(3));
    uint16_t bmide_addr = static_cast<uint16_t>(get_bar(4));

    // I/O port addresses must be 4 byte aligned. Bit 1 is reserved.
    // If I/O and the value is 0x0, use the defaults instead.
    if ((primary_addr & 0xFFFC) == 0)
        primary_addr = static_cast<uint16_t>(default_addr::primary_addr);
    if ((primary_control_addr & 0xFFFC) == 0)
        primary_control_addr =
            static_cast<uint16_t>(default_addr::primary_control_addr);
    if ((secondary_addr & 0xFFFC) == 0)
        secondary_addr = static_cast<uint16_t>(default_addr::secondary_addr);
    if ((secondary_control_addr & 0xFFFC) == 0)
        secondary_control_addr =
            static_cast<uint16_t>(default_addr::secondary_control_addr);

    // Set the addresses and set the interrupts disable (nien) to true.
    channels[static_cast<uint8_t>(channel::primary)] = ChannelRegisters {
        primary_addr, primary_control_addr, bmide_addr, true, rank::unknown
    };
    channels[static_cast<uint8_t>(channel::secondary)] = ChannelRegisters {
        secondary_addr, secondary_control_addr,
        static_cast<uint16_t>(bmide_addr + 8), true, rank::unknown
    };

    // Disable interrupts.
    ide_write(channel::primary, register_offset::control,
        static_cast<uint8_t>(control_bits::nien));
    ide_write(channel::secondary, register_offset::control,
        static_cast<uint8_t>(control_bits::nien));
    irq_invoked = false;
}

/******************************************************************************/

void IdeController::configure_irq()
{
    // First, we'll try setting the interrupt line to a dummy value.
    set_interrupt_line(0xFE);
    // Read it back to see if we managed to set it.
    uint8_t irq = get_interrupt_line();

    if (irq == 0xFE)
    {
        // We managed to set the interrupt line. Therefore this is a serial IDE
        // controller.
        serial = true;
        // Set the IRQ to a sensible value. Serial IDE only uses 1 IRQ.
        set_interrupt_line(static_cast<uint8_t>(InterruptNumber::ata1) -
            static_cast<uint8_t>(InterruptNumber::pic2_start) + 8);
        int1 = InterruptNumber::ata1;
        int2 = static_cast<InterruptNumber>(0xFF);
    }
    else
    {
        // We could not set the interrupt line. Therefore this is a parallel IDE
        // controller.
        serial = false;
        int1 = InterruptNumber::ata1;
        int2 = InterruptNumber::ata2;
    }

    irq_expected = false;
    irq_invoked = false;
}

/******************************************************************************/

void IdeController::configure_devices()
{
    // Cycle over primary and secondary channels, and master and slave per
    // channel.
    for (size_t cha = 0; cha < 2; ++cha)
    {
        // Set the channel active device to unknown.
        channels[cha].dev = rank::unknown;

        for (size_t ra = 0; ra < 2; ++ra)
        {
            // Index for accessing the devs array.
            size_t dev = cha * 2 + ra;
            // Set device to not present to start with.
            devs[dev].exists = false;
            devs[dev].cha = static_cast<channel>(cha);
            devs[dev].ra = static_cast<rank>(ra);
            devs[dev].model = "";

            // Select drive within channel.
            polling_response perr = 
                switch_device(static_cast<channel>(cha), static_cast<rank>(ra));

            // If we have an error here, there's probably no device.
            if (perr == polling_response::device_fault ||
                perr == polling_response::error)
            {
                print_error(dev, perr);
                continue;
            }

            // Clear the necessary ports.
            ide_write(static_cast<channel>(cha), register_offset::seccount0, 0);
            ide_write(static_cast<channel>(cha), register_offset::lba0, 0);
            ide_write(static_cast<channel>(cha), register_offset::lba1, 0);
            ide_write(static_cast<channel>(cha), register_offset::lba2, 0);

            // Send the identify command.
            ide_write(static_cast<channel>(cha), register_offset::command,
                static_cast<uint8_t>(commands::identify));

            // Read the status. A zero indicates no device.
            if (ide_read(static_cast<channel>(cha), register_offset::status)
                == 0)
                continue;

            // Wait for the device to be not busy.
            perr = polling(static_cast<channel>(cha), true);

            // Test for ATAPI if an error is indicated.
            if (perr == polling_response::error)
            {
                // ATAPI sets some magic values on the addressing registers.
                uint8_t lba_mid = ide_read(static_cast<channel>(cha),
                    register_offset::lba1);
                uint8_t lba_high = ide_read(static_cast<channel>(cha),
                    register_offset::lba2);

                if (lba_mid == 0x14 && lba_high == 0xEB)
                    devs[dev].type = interface_type::atapi;
                else
                    // Unknown type, we'll assume it's unusable.
                    continue;

                // We need to send identify packet instead.
                ide_write(static_cast<channel>(cha), register_offset::command,
                    static_cast<uint8_t>(commands::identify_packet));
                perr = polling(static_cast<channel>(cha), true);

                if (perr != polling_response::no_error)
                    continue;

            }
            else if (perr == polling_response::no_error)
                // No error, so we have a valid ATA device.
                devs[dev].type = interface_type::ata;
            else
                // Assume some other error means the device is unusable.
                continue;

            // Now we can read the identification space.
            char ident_buf[ident_size];
            insw(channels[cha].base +
                static_cast<uint16_t>(register_offset::data) - low_byte_adjust,
                ident_buf, ident_size / 2);

            if (devs[dev].type == interface_type::ata)
                read_identify(static_cast<channel>(cha), static_cast<rank>(ra),
                    ident_buf);

            else if (devs[dev].type == interface_type::atapi)
                read_identify_packet(static_cast<channel>(cha),
                    static_cast<rank>(ra), ident_buf);
        }
    }
}

/******************************************************************************/

void IdeController::add_drivers(klib::map<klib::string, Device*>& drvs)
{
    for (size_t cha = 0; cha < 2; ++cha)
    {
        for (size_t ra = 0; ra < 2; ++ra)
        {
            size_t dev = 2 * cha + ra;

            if (!devs[dev].exists)
                continue;

            if (devs[dev].type == interface_type::ata)
            {
                // Make real disk driver.
                PataDriver* drv = new PataDriver{*this,
                        static_cast<channel>(cha),
                        static_cast<rank>(ra),
                        "PATA disk",
                        devs[dev].sector_size,
                        FileSystemType::none,
                        static_cast<uint64_t>(devs[dev].size) *
                        static_cast<uint64_t>(devs[dev].sector_size)};
                klib::string name {get_new_device_name(DeviceType::ata_disk,
                    global_kernel->get_vfs()->get_dev()->get_drivers())};

                // Add to the list of disks.
                drvs[name] = drv;
            }
        }
    }   
}

/******************************************************************************/

void IdeController::read_identify(channel cha, rank ra, char* ident_buf)
{
    size_t dev = 2 * static_cast<uint8_t>(cha) + static_cast<uint8_t>(ra);

    // Set parameters from the identification space.
    devs[dev].exists = true;
    devs[dev].type = interface_type::ata;
    devs[dev].signature = *reinterpret_cast<uint16_t*>(ident_buf +
        static_cast<uint16_t>(ident_offset::device_type));
    devs[dev].features = *reinterpret_cast<uint16_t*>(ident_buf +
        static_cast<uint16_t>(ident_offset::capabilities));
    devs[dev].command_sets = *reinterpret_cast<uint32_t*>(ident_buf +
        static_cast<uint16_t>(ident_offset::commandsets));

    // Check LBA and DMA support.
    devs[dev].lba_support = devs[dev].features &
        static_cast<uint16_t>(capabilities_bits::lba);
    devs[dev].dma_support = devs[dev].features &
        static_cast<uint16_t>(capabilities_bits::dma);

    // Sector size. This is 512 unless bit 12 of the sector properties
    // is set. Multiply by 2 as the size is given in words (16 bits).
    if (*reinterpret_cast<uint16_t*>(ident_buf +
        static_cast<uint16_t>(ident_offset::sector_properties)) &
        static_cast<uint16_t>(sector_properties_bits::size_specified))
        devs[dev].sector_size = *reinterpret_cast<uint16_t*>(ident_buf +
            static_cast<uint16_t>(ident_offset::sector_size)) * 2;
    else
        devs[dev].sector_size = default_sector_size;

    // Get the device size.
    if (devs[dev].command_sets &
        static_cast<uint32_t>(command_set_bits::extended_address))
    {
        // 48-bit addressing
        devs[dev].size = *reinterpret_cast<uint64_t*>(ident_buf +
            static_cast<uint16_t>(ident_offset::max_lba_ext));
    }
    else
    {
        // CHS or 28-bit addressing
        devs[dev].size = *reinterpret_cast<uint64_t*>(ident_buf +
            static_cast<uint16_t>(ident_offset::max_lba));
    }

    // Get the model name. Complicated by the bytes in each word being
    // in the wrong order. This field is 40 characters long, padded with
    // spaces.
    devs[dev].model = klib::string {};
    for (size_t i = 0; i < model_length / 2; ++i)
    {
        devs[dev].model += ident_buf[2 * i + 1 +
            static_cast<uint16_t>(ident_offset::model)];
        devs[dev].model += ident_buf[2 * i +
            static_cast<uint16_t>(ident_offset::model)];
    }
    // Delete trailing spaces.
    auto it = --devs[dev].model.end();
    while (*it == ' ')
        --it;
    devs[dev].model.erase(++it, devs[dev].model.end());
}

/******************************************************************************/

void IdeController::read_identify_packet(channel cha, rank ra, char* ident_buf)
{
    size_t dev = 2 * static_cast<uint8_t>(cha) + static_cast<uint8_t>(ra);

    // Set parameters from the identification space.
    devs[dev].exists = true;
    devs[dev].signature = *reinterpret_cast<uint16_t*>(ident_buf +
        static_cast<uint16_t>(ident_offset::device_type));
    devs[dev].features = *reinterpret_cast<uint16_t*>(ident_buf +
        static_cast<uint16_t>(ident_offset::capabilities));
    devs[dev].command_sets = *reinterpret_cast<uint32_t*>(ident_buf +
        static_cast<uint16_t>(ident_offset::commandsets));

    // LBA always supported for ATAPI.
    devs[dev].lba_support = true;
    // Check DMA support.
    devs[dev].dma_support = devs[dev].features &
        static_cast<uint16_t>(capabilities_bits::dma);

    // TODO read sector size and number of sectors.
    devs[dev].size = 0;
    devs[dev].sector_size = 0;

    // Get the model name. Complicated by the bytes in each word being
    // in the wrong order. This field is 40 characters long, padded with
    // spaces.
    devs[dev].model = klib::string {};
    for (size_t i = 0; i < model_length / 2; ++i)
    {
        devs[dev].model += ident_buf[2 * i + 1 +
            static_cast<uint16_t>(ident_offset::model)];
        devs[dev].model += ident_buf[2 * i +
            static_cast<uint16_t>(ident_offset::model)];
    }
    // Delete trailing spaces.
    auto it = --devs[dev].model.end();
    while (*it == ' ')
        --it;
    devs[dev].model.erase(++it, devs[dev].model.end());
}

/******************************************************************************/

uint8_t IdeController::ide_read(channel cha, register_offset reg) const
{
    uint8_t result = 0;
    uint16_t reg_val = static_cast<uint16_t>(reg);

    // For certain registers, we're using the high byte. Set 0x80 in the control
    // register to request this.
    if (register_offset::seccount1 <= reg && reg <= register_offset::lba5)
        ide_write(cha, register_offset::control,
            static_cast<uint8_t>(control_bits::hob) +
            (channels[static_cast<size_t>(cha)].nien << 1));

    // Read from the appropriate register.
    if (reg <= register_offset::status)
        result = inb(channels[static_cast<size_t>(cha)].base + reg_val -
            low_byte_adjust);
    else if (reg <= register_offset::lba5)
        result = inb(channels[static_cast<size_t>(cha)].base + reg_val -
            high_byte_adjust);
    else if (reg <= register_offset::devaddress)
        result = inb(channels[static_cast<size_t>(cha)].ctrl + reg_val -
            control_adjust);
    else if (reg <= register_offset::bmide_top)
        result = inb(channels[static_cast<size_t>(cha)].bmide + reg_val -
            bmide_adjust);

    // Reset the control register.
    if (register_offset::seccount1 <= reg && reg <= register_offset::lba5)
        ide_write(cha, register_offset::control,
            (channels[static_cast<size_t>(cha)].nien << 1));

    return result;
}

/******************************************************************************/

void IdeController::ide_write(channel cha, register_offset reg, uint8_t data)
    const
{
    uint16_t reg_val = static_cast<size_t>(reg);

    // For certain registers, we're using the high byte. Set 0x80 in the control
    // register to request this.
    if (register_offset::seccount1 <= reg && reg <= register_offset::lba5)
        ide_write(cha, register_offset::control,
            static_cast<uint8_t>(control_bits::hob) +
            (channels[static_cast<size_t>(cha)].nien << 1));

    // Write to the appropriate register.
    if (reg <= register_offset::status)
        outb(data, channels[static_cast<size_t>(cha)].base + reg_val -
            low_byte_adjust);
    else if (reg <= register_offset::lba5)
        outb(data, channels[static_cast<size_t>(cha)].base + reg_val -
            high_byte_adjust);
    else if (reg <= register_offset::devaddress)
        outb(data, channels[static_cast<size_t>(cha)].ctrl + reg_val -
            control_adjust);
    else if (reg <= register_offset::bmide_top)
        outb(data, channels[static_cast<size_t>(cha)].bmide + reg_val -
            bmide_adjust);

    // Reset the control register.
    if (register_offset::seccount1 <= reg && reg <= register_offset::lba5)
        ide_write(cha, register_offset::control,
            (channels[static_cast<size_t>(cha)].nien << 1));

}

/******************************************************************************/

IdeController::polling_response IdeController::polling(channel cha,
    bool advanced_check) const
{
    // Delay for 400ns by reading the alternate status register 4 times.
//    for (size_t i = 4; i != 0; --i)
//        ide_read(cha, register_offset::altstatus);

    // Wait for BSY to be cleared.
    uint8_t state;
    while ((state = ide_read(cha, register_offset::status)) &
        static_cast<uint8_t>(status_bits::bsy)) ;

    if (advanced_check)
    {
        if (state & static_cast<uint8_t>(status_bits::err))
            return polling_response::error;

        if (state & static_cast<uint8_t>(status_bits::df))
            return polling_response::device_fault;

        if ((state & static_cast<uint8_t>(status_bits::drdy)) == 0)
            return polling_response::not_ready;

        if ((state & static_cast<uint8_t>(status_bits::drq)) == 0)
            return polling_response::data_not_ready;
    }

    return polling_response::no_error;
}

/******************************************************************************/

DiskIoError IdeController::prepare_io(channel cha, rank ra, uint64_t off,
    size_t sz, IdeController::address_mode& mode)
{
    size_t dev = 2 * static_cast<uint8_t>(cha) + static_cast<uint8_t>(ra);

    // Basic checks.
    if (!devs[dev].exists)
        return DiskIoError::no_device;
    if (devs[dev].type != interface_type::ata)
        return DiskIoError::bad_driver;
    if (sz % devs[dev].sector_size != 0 ||
        sz / devs[dev].sector_size > max_sectors ||
        (off - 1 + sz) / devs[dev].sector_size >= devs[dev].size)
        return DiskIoError::bad_size;
    if (off % devs[dev].sector_size != 0)
        return DiskIoError::bad_alignment;

    // Switch to the correct device on the bus.
    polling_response perr = switch_device(cha, ra);
    if (perr != polling_response::no_error)
        return DiskIoError::hardware_fault;

    // TODO DMA support.

    // Disable interrupts to prevent interference.
    ide_write(cha, register_offset::control,
        static_cast<uint8_t>(control_bits::nien));

    // Decide whether to use LBA or CHS.
    uint8_t lba[6];
    uint8_t head;
    uint64_t sect = off / devs[dev].sector_size;
    uint8_t no_sect = sz / devs[dev].sector_size;
    if (devs[dev].lba_support)
    {
        if (sect >= (1 << 28))
        {
            // Address is too large for LBA28, so must be using LBA48
            mode = address_mode::lba48;
            lba[0] = (sect & 0x0000000000FF);
            lba[1] = (sect & 0x00000000FF00) >> 8;
            lba[2] = (sect & 0x000000FF0000) >> 16;
            lba[3] = (sect & 0x0000FF000000) >> 24;
            lba[4] = (sect & 0x00FF00000000) >> 32;
            lba[5] = (sect & 0xFF0000000000) >> 40;
            // Head is unused in LBA48.
            head = 0;
        }
        else
        {
            // Device supports LBA28 and the address is small enough to use it.
            mode = address_mode::lba28;
            lba[0] = (sect & 0x000000FF);
            lba[1] = (sect & 0x0000FF00) >> 8;
            lba[2] = (sect & 0x00FF0000) >> 16;
            // Higher addresses unused for LBA28.
            lba[3] = 0;
            lba[4] = 0;
            lba[5] = 0;
            // Head contains the upper 4 bits.
            head = (sect & 0x0F000000) >> 24;
        }
    }
    else
    {
        // CHS addressing.
        mode = address_mode::chs;
        uint8_t s = (sect % sectors_per_track) + 1;
        uint16_t c = (sect + 1 - s) / (sectors_per_track * heads_per_cylinder);
        uint8_t h = ((sect + 1  - s) / sectors_per_track) % heads_per_cylinder;
        lba[0] = s;
        lba[1] = (c & 0xFF);
        lba[2] = (c & 0xFF00) >> 8;
        // Higher addresses are unused for CHS.
        lba[3] = 0;
        lba[4] = 0;
        lba[5] = 0;
        head = h & 0xF;
    }

    // Make sure BSY is clear.
    perr = polling(static_cast<channel>(cha), false);
    if (perr != polling_response::no_error)
        return DiskIoError::hardware_fault;

    // Write the HDDEVSEL register.
    ide_write(static_cast<channel>(cha), register_offset::hddevsel,
        static_cast<uint8_t>(hddevsel_bits::req1) |
        static_cast<uint8_t>(hddevsel_bits::req2) |
        (ra == rank::slave ? static_cast<uint8_t>(hddevsel_bits::slave) : 0) |
        (mode == address_mode::chs ?
            0 : static_cast<uint8_t>(hddevsel_bits::lba)) |
        head);

    // Load up the rest of the address and the number of sectors.
    if (mode == address_mode::lba48)
    {
        ide_write(static_cast<channel>(cha), register_offset::seccount1, 0);
        ide_write(static_cast<channel>(cha), register_offset::lba3, lba[3]);
        ide_write(static_cast<channel>(cha), register_offset::lba4, lba[4]);
        ide_write(static_cast<channel>(cha), register_offset::lba5, lba[5]);
    }
    ide_write(static_cast<channel>(cha), register_offset::seccount0, no_sect);
    ide_write(static_cast<channel>(cha), register_offset::lba0, lba[0]);
    ide_write(static_cast<channel>(cha), register_offset::lba1, lba[1]);
    ide_write(static_cast<channel>(cha), register_offset::lba2, lba[2]);

    return DiskIoError::success;
}

/******************************************************************************/

void IdeController::print_error(size_t drive,
    IdeController::polling_response err) const
{
    if (err == polling_response::no_error)
        return;

    global_kernel->syslog()->error("IDE controller at %s on PCI bus:\n",
        get_location().c_str());

    klib::string cha
        {devs[drive].cha == channel::primary ? "Primary" : "Secondary"};
    klib::string ra
        {devs[drive].ra == rank::master ? "Master" : "Slave"};

    global_kernel->syslog()->error("  [%s, %s] %s\n", cha.c_str(), ra.c_str(),
        devs[drive].model.c_str());

    uint8_t st = ide_read(devs[drive].cha, register_offset::error);
    switch(err)
    {
    case polling_response::no_error:
        break;
    case polling_response::device_fault:
        global_kernel->syslog()->error("  Device fault.\n");
        break;
    case polling_response::error:
        if (st & static_cast<uint8_t>(error_bits::amnf))
            global_kernel->syslog()->error("  No address mark found.\n");
        if (st & static_cast<uint8_t>(error_bits::tk0nf))
            global_kernel->syslog()->error("  Track 0 not found.\n");
        if (st & static_cast<uint8_t>(error_bits::abrt))
            global_kernel->syslog()->error("  Command aborted.\n");
        if (st & static_cast<uint8_t>(error_bits::mcr))
            global_kernel->syslog()->error("  Media change request.\n");
        if (st & static_cast<uint8_t>(error_bits::idnf))
            global_kernel->syslog()->error("  No ID mark found.\n");
        if (st & static_cast<uint8_t>(error_bits::mc))
            global_kernel->syslog()->error("  Media changed.\n");
        if (st & static_cast<uint8_t>(error_bits::unc))
            global_kernel->syslog()->error("  Uncorrectable data.\n");
        if (st & static_cast<uint8_t>(error_bits::bbk))
            global_kernel->syslog()->error("  Bad sectors.\n");
        break;
    case polling_response::not_ready:
        global_kernel->syslog()->error("  Device not ready.\n");
        break;
    case polling_response::data_not_ready:
        global_kernel->syslog()->error("  Data not ready.\n");
        break;
    case polling_response::write_protected:
        global_kernel->syslog()->error("  Device is write protected.\n");
        break;
    }
}

/******************************************************************************/

IdeController::polling_response IdeController::switch_device(channel cha,
    rank ra)
{
    size_t dev = 2 * static_cast<size_t>(cha) + static_cast<size_t>(ra);

    // Do nothing if we're already on the device.
    if (channels[dev].dev == ra)
        return polling_response::no_error;

    // There are some required bits. The drive is bit 4.
    ide_write(static_cast<channel>(cha), register_offset::hddevsel,
        static_cast<uint8_t>(hddevsel_bits::req1) |
        static_cast<uint8_t>(hddevsel_bits::req2) |
        (ra == rank::slave ? static_cast<uint8_t>(hddevsel_bits::slave) : 0));

    // Poll until done.
    polling_response perr = polling(cha, true);

    // Data not ready is not an error here, as we're not moving data.
    if (perr == polling_response::data_not_ready)
        perr = polling_response::no_error;

    // No error. Change the active device. 
    if (perr == polling_response::no_error)
        channels[static_cast<size_t>(cha)].dev = ra;
    // An error. Who know's what happened to the active device?
    else
        channels[static_cast<size_t>(cha)].dev = rank::unknown;

    return perr;
}

/******************************************************************************
 ******************************************************************************/

size_t PataDriver::read_block(uint64_t off, void* addr)
{
    if (off > size || off % s_sz != 0)
        return 0;

    DiskIoError ret_val = cont.ata_read(cha, ra, off, addr, s_sz);

    return (ret_val == DiskIoError::success ? s_sz : 0);
}

/******************************************************************************/

size_t PataDriver::write_block(uint64_t off, const void* addr)
{
   if (off > size || off % s_sz != 0)
        return 0;

    DiskIoError ret_val = cont.ata_write(cha, ra, off, addr, s_sz);

    return (ret_val == DiskIoError::success ? s_sz : 0);
}

/******************************************************************************/

PollType PataDriver::poll_check(PollType cond) const
{
    PollType ret_val = PollType::pollnone;

    // For now, the device is always ready, since we haven't done DMA for
    // asynchronous writing. We do have a busy flag in preparation.
    if ((cond & PollType::pollin) != PollType::pollnone && !busy)
        ret_val |= PollType::pollin;
    if ((cond & PollType::pollout) != PollType::pollnone && !busy)
        ret_val |= PollType::pollout;

    return ret_val;
}

/******************************************************************************
 ******************************************************************************/
