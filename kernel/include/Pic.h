#ifndef PIC_H
#define PIC_H

#include <stdint.h>

/**
    Identify which PIC.
 */
enum class PicType {
    master, slave
};

/**
    IO port address for the PICs.
 */
enum class PicAddress : uint16_t {
    master_command = 0x20, master_data = 0x21, slave_command = 0xA0,
    slave_data = 0xA1
};

/**
    Values for the command register.
 */
enum class PicCommand : uint8_t {
    // Initialise, PIC then waits for three values sent to the data port
    init = 0x11,
    // End of Interrupt aka Acknowledge.
    eoi = 0x20,
    // Cascade setting for the master controller
    master_cascade = 0x04,
    // Cascade setting for the slave controller
    slave_cascade = 0x02,
    // x86 mode
    x86_mode = 0x01,
    // Read Interrupt Request Register
    read_irr = 0x0A,
    // Read In Service Register
    read_isr = 0x0B
};

/**
    Values for setting the interrupt mask.
 */
enum class PicMask : uint8_t {
    // Programmable interrupt timer
    master_pit = 0x01,
    // Keyboard
    master_keyboard = 0x02,
    // Cascade, ie interrupts from the slave
    master_cascade = 0x04,
    // Serial port 2
    master_com2 = 0x08,
    // Serial port 1
    master_com1 = 0x10,
    // LPT2 (parallel)
    master_lpt2 = 0x20,
    // Floppy drive
    master_floppy = 0x40,
    // LPT1 (printer)
    master_lpt1 = 0x80,
    // CMOS real time clock
    slave_rtc = 0x01,
    // Advanced configuration and power interface
    slave_acpi = 0x02,
    // Peripherals
    slave_peri2 = 0x04,
    // Peripherals
    slave_peri1 = 0x08,
    // Mouse
    slave_mouse = 0x10,
    // Coprocessor
    slave_coproc = 0x20,
    // Primary ATA
    slave_ata1 = 0x40,
    // Secondary ATA
    slave_ata2 = 0x80,
    // Enable all
    enable = 0x00,
    // Disable all
    disable = 0xFF
};

/**
    Driver for the Programmable Interrupt Controllers.
 */
class PicDriver {
public:
    /**
        Initialise the Programmable Interrupt Controller.

        @param master_mask Interrupt mask for the master.
        @param slave_mask Interrupt mask for the slave.
     */
    PicDriver (PicMask master_mask, PicMask slave_mask);

    /**
        Send an acknowledgement to allow for further interrupts.

        @param id Send to master or slave.
     */
    void acknowledge(PicType id);

    /**
        Read the interrupt request registers from both controllers.

        @return The slave IRR in the high byte and the master IRR in the low
                byte.
     */
    uint16_t get_irr();

    /**
        Read the interrupt request register from one controller.

        @param id Which controller to read.
        @return The IRR of the specified controller.
     */
    uint8_t get_irr(PicType id);

    /**
        Read the in service registers from both controllers.

        @return The slave ISR in the high byte and the master ISR in the low
                byte.
     */
    uint16_t get_isr();

    /**
        Read the in service register from one controller.

        @param id Which controller to read.
        @return The ISR of the specified controller.
     */
    uint8_t get_isr(PicType id);

    /**
        Set the mask, to enable or disable specific interrupts.

        @param id Whether to set the master or slave mask.
        @param mask New mask to set.
     */
    void set_mask(PicType id, PicMask mask);
};

#endif
