#ifndef PCI_H
#define PCI_H

#include <stdint.h>

#include <ostream>
#include <string>
#include <vector>

/**
    Represents a PCI device.
 */
class PciDevice {
public:
    /**
        I/O ports for configuration space access.
     */
    enum port : uint16_t {
        /** CONFIG_ADDRESS */
        config_address = 0xCF8,
        /** CONFIG_DATA */
        config_data = 0xCFC
    };

    /**
        Class codes.
     */
    enum class_code : uint8_t {
        /** Device built before class codes. */
        cl_unclassified = 0x0,
        /** Mass storage controller. */
        cl_mass_storage = 0x1,
        /** Network controller. */
        cl_network = 0x2,
        /** Display controller. */
        cl_display = 0x3,
        /** Multimedia device. */
        cl_multimedia = 0x4,
        /** Memory controller. */
        cl_memory = 0x5,
        /** Bridge device. */
        cl_bridge = 0x6,
        /** Simple communication controller. */
        cl_comm = 0x7,
        /** Base system peripheral. */
        cl_peripheral = 0x8,
        /** Input device. */
        cl_input = 0x9,
        /** Docking station. */
        cl_docking = 0xA,
        /** Processor. */
        cl_proc = 0xB,
        /** Serial bus controller. */
        cl_serial = 0xC,
        /** Wireless controller. */
        cl_wireless = 0xD,
        /** Intelligent controller. */
        cl_intelligent = 0xE,
        /** Satellite communication controller. */
        cl_satellite = 0xF,
        /** Encryption controller. */
        cl_encryption = 0x10,
        /** Signal processing controller. */
        cl_signal = 0x11,
        /** Processing accelerator. */
        cl_accel = 0x12,
        /** Non-essential instrumentation. */
        cl_instrument = 0x13,
        /** Co-processor. */
        cl_coproc = 0x40,
        /** Unassigned class. */
        cl_unassigned = 0xFF
    };

    /**
        Subclass codes.
     */
    enum subclass_code : uint8_t {
        /** Generic - other. */
        scl_other = 0x80,
        /** Unclassified - non_VGA. */
        scl_unclassified_non_vga = 0x0,
        /** Unclassified - VGA. */
        scl_unclassified_vga = 0x1,
        /** Mass storage - SCSI bus controller. */
        scl_mass_storage_scsi = 0x0,
        /** Mass storage - IDE controller. */
        scl_mass_storage_ide = 0x1,
        /** Mass storage - floppy disk controller. */
        scl_mass_storage_floppy = 0x2,
        /** Mass storage - IPI bus controller. */
        scl_mass_storage_ipi = 0x3,
        /** Mass storage - RAID controller. */
        scl_mass_storage_raid = 0x4,
        /** Mass storage - ATA controller. */
        scl_mass_storage_ata = 0x5,
        /** Mass storage - serial ATA. */
        scl_mass_storage_sata = 0x6,
        /** Mass storage - serial attached SCSI. */
        scl_mass_storage_sas = 0x7,
        /** Mass storage - non-volatile memory controller. */
        scl_mass_storage_memory = 0x8,
        /** Network - ethernet. */
        scl_network_ethernet = 0x0,
        /** Network - token ring controller. */
        scl_network_token = 0x1,
        /** Network - FDDI controller. */
        scl_network_fddi = 0x2,
        /** Network - ATM controller. */
        scl_network_atm = 0x3,
        /** Network - ISDN controller. */
        scl_network_isdn = 0x4,
        /** Network - WorldFip controller. */
        scl_network_worldfip = 0x5,
        /** Network - PICMG 2.14 Multi Computing. */
        scl_network_picmg = 0x6,
        /** Network - InfiniBand controller. */
        scl_network_infiniband = 0x7,
        /** Network - fabric controller. */
        scl_network_fabric = 0x8,
        /** Display - VGA compatible. */
        scl_display_vga = 0x0,
        /** Display - XGA controller. */
        scl_display_xga = 0x1,
        /** Display - 3D controller (not VGA compatible). */
        scl_display_3d = 0x2,
        /** Multimedia - video controller. */
        scl_multimedia_video = 0x0,
        /** Multimedia - audio controller. */
        scl_multimedia_audio = 0x1,
        /** Multimedia - computer telephony device. */
        scl_multimedia_telephone = 0x2,
        /** Multimedia - audio device. */
        scl_multimedia_audio_device = 0x3,
        /** Memory - RAM controller. */
        scl_memory_ram = 0x0,
        /** Memory - flash controller. */
        scl_memory_flash = 0x1,
        /** Bridge - host bridge. */
        scl_bridge_host = 0x0,
        /** Bridge - ISA bridge. */
        scl_bridge_isa = 0x1,
        /** Bridge - EISA bridge. */
        scl_bridge_eisa = 0x2,
        /** Bridge - MCA bridge. */
        scl_bridge_mca = 0x3,
        /** Bridge - PCI-to-PCI bridge. */
        scl_bridge_pci = 0x4,
        /** Bridge - PCMCIA bridge. */
        scl_bridge_pcmcia = 0x5,
        /** Bridge - NuBus bridge. */
        scl_bridge_nubus = 0x6,
        /** Bridge - CardBus bridge. */
        scl_bridge_cardbus = 0x7,
        /** Bridge - RACEway bridge. */
        scl_bridge_raceway = 0x8,
        /** Bridge - semi-transparent PCI-to-PCI bridge. */
        scl_bridge_semi_pci = 0x9,
        /** Bridge - InfiniBand-to-PCI host bridge. */
        scl_bridge_infiniband = 0xA,
        /** Communication - serial controller. */
        scl_comm_serial = 0x0,
        /** Communication - parallel controller. */
        scl_comm_parallel = 0x1,
        /** Communication - multiport serial controller. */
        scl_comm_multiport = 0x2,
        /** Communication - modem. */
        scl_comm_modem = 0x3,
        /** Communication - IEEE 488.1/2 (GPIB) controller. */
        scl_comm_gpib = 0x4,
        /** Communication - smart card. */
        scl_comm_smart = 0x5,
        /** Peripheral - PIC. */
        scl_peripheral_pic = 0x0,
        /** Peripheral - DMA controller. */
        scl_peripheral_dma = 0x1,
        /** Peripheral - timer. */
        scl_peripheral_timer = 0x2,
        /** Peripheral - RTC controller. */
        scl_peripheral_rtc = 0x3,
        /** Peripheral - PCI hotplug controller. */
        scl_peripheral_hotplug = 0x4,
        /** Peripheral - SD host controller. */
        scl_peripheral_sd = 0x5,
        /** Peripheral - IOMMU. */
        scl_peripheral_iommu = 0x6,
        /** Input - keyboard controller. */
        scl_input_keyboard = 0x0,
        /** Input - digitiser pen. */
        scl_input_pen = 0x1,
        /** Input - mouse controller. */
        scl_input_mouse = 0x2,
        /** Input - scanner controller. */
        scl_input_scanner = 0x3,
        /** Input - gameport controller. */
        scl_input_gameport = 0x4,
        /** Docking - generic. */
        scl_docking_generic = 0x0,
        /** Processor - 386. */
        scl_proc_386 = 0x0,
        /** Processor - 486. */
        scl_proc_486 = 0x1,
        /** Processor - pentium. */
        scl_proc_pentium = 0x2,
        /** Processor - alpha. */
        scl_proc_alpha = 0x10,
        /** Processor - powerPC. */
        scl_proc_powerpc = 0x20,
        /** Processor - MIPS. */
        scl_proc_mips = 0x30,
        /** Processor - co-processor. */
        scl_proc_coproc = 0x40,
        /** Serial - FireWire (IEEE 1394) controller. */
        scl_serial_firewire = 0x0,
        /** Serial - ACCESS bus. */
        scl_serial_access = 0x1,
        /** Serial - SSA. */
        scl_serial_ssa = 0x2,
        /** Serial - USB controller. */
        scl_serial_usb = 0x3,
        /** Serial - fibre channel. */
        scl_serial_fibre = 0x4,
        /** Serial - SMBus. */
        scl_serial_smbus = 0x5,
        /** Serial - InfiniBand. */
        scl_serial_infiniband = 0x6,
        /** Serial - IPMI interface. */
        scl_serial_ipmi = 0x7,
        /** Serial - SERCOS interface. */
        scl_serial_sercos = 0x8,
        /** Serial - CANbus. */
        scl_serial_canbus = 0x9,
        /** Wireless - iRDA compatible controller. */
        scl_wireless_irda = 0x0,
        /** Wireless - consumer IR controller. */
        scl_wireless_ir = 0x1,
        /** Wireless - RF controller. */
        scl_wireless_rf = 0x10,
        /** Wireless - bluetooth controller. */
        scl_wireless_bluetooth = 0x11,
        /** Wireless - broadband controller. */
        scl_wireless_broadband = 0x12,
        /** Wireless - ethernet controller (802.1a). */
        scl_wireless_ethernet_a = 0x20,
        /** Wireless - ethernet controller (802.1b). */
        scl_wireless_ethernet_b = 0x21,
        /** Intelligent - I20. */
        scl_intelligent_i20 = 0x0,
        /** Satellite - TV controller. */
        scl_satellite_tv = 0x1,
        /** Satellite - audio controller. */
        scl_satellite_audio = 0x2,
        /** Satellite - voice controller. */
        scl_satellite_voice = 0x3,
        /** Satellite - data controller. */
        scl_satellite_data = 0x4,
        /** Encryption - network and computing. */
        scl_encryption_network = 0x0,
        /** Encryption - entertainment. */
        scl_encryption_entertainment = 0x10,
        /** Signal - DPIO Module. */
        scl_signal_dpio = 0x0,
        /** Signal - performance counter. */
        scl_signal_performance = 0x1,
        /** Signal - communication synchroniser. */
        scl_signal_synch = 0x10,
        /** Signal - signal processing management. */
        scl_signal_management = 0x20,
        /** Unassigned subclass. */
        scl_unassigned = 0xFF
    };

    /**
        Prog IF codes.
     */
    enum progif_code : uint8_t {
        /** Mass storage - IDE - ISA compatability mode-only controller. */
        p_mass_storage_ide_isa = 0x0,
        /** Mass storage - IDE - PCI native mode-only controller. */
        p_mass_storage_ide_pci = 0x5,
        /** Mass storage - IDE - ISA compatibility mode, supports both channels
            switched to PCI native mode. */
        p_mass_storage_ide_isa_pci = 0xA,
        /** Mass storage - IDE - PCI native mode, supports both channels
            switched to ISA compatibility mode. */
        p_mass_storage_ide_pci_isa = 0xF,
        /** Mass storage - IDE - ISA compatability mode-only controller,
            supports bus mastering. */
        p_mass_storage_ide_isa_master = 0x80,
        /** Mass storage - IDE - PCI native mode-only controller, supports bus
            mastering. */
        p_mass_storage_ide_pci_master = 0x85,
        /** Mass storage - IDE - ISA compatibility mode, supports both channels
            switched to PCI native mode and bus mastering. */
        p_mass_storage_ide_isa_pci_master = 0x8A,
        /** Mass storage - IDE - PCI native mode, supports both channels
            switched to ISA compatibility mode and bus mastering. */
        p_mass_storage_ide_pci_isa_master = 0x8F,
        /** Mass storage - ATA - single DMA. */
        p_mass_storage_ata_single = 0x20,
        /** Mass storage - ATA - chained DMA. */
        p_mass_storage_ata_chained = 0x30,
        /** Mass storage - serial ATA - vendor specific interface. */
        p_mass_storage_sata_vendor = 0x0,
        /** Mass storage - serial ATA - ACHI 1.0. */
        p_mass_storage_sata_achi = 0x1,
        /** Mass storage - serial ATA - serial storage bus. */
        p_mass_storage_sata_serial = 0x2,
        /** Mass storage - serial attached SCSI - SAS. */
        p_mass_storage_sas_sas = 0x0,
        /** Mass storage - serial attached SCSI - serial storage bus. */
        p_mass_storage_sas_serial = 0x1,
        /** Mass storage - memory - NVMHCI. */
        p_mass_storage_memory_nvmhci = 0x1,
        /** Mass storage - memory - NVM Express. */
        p_mass_storage_memory_nvmexpress = 0x2,
        /** Display - VGA - VGA controller. */
        p_display_vga_vga = 0x0,
        /** Display - VGA - 8514 compatible controller. */
        p_display_vga_8514 = 0x1,
        /** Bridge - PCI-to-PCI - normal decode */
        p_bridge_pci_normal = 0x0,
        /** Bridge - PCI-to-PCI - subtractive decode */
        p_bridge_pci_subtractive = 0x1,
        /** Bridge - RACEway - transparent mode */
        p_bridge_raceway_transparent = 0x0,
        /** Bridge - RACEway - endpoint mode */
        p_bridge_raceway_endpoint = 0x1,
        /** Bridge - semi-transparent PCI-to-PCI - primary bus towards host. */
        p_bridge_semi_pci_primary = 0x40,
        /** Bridge - semi-transparent PCI-to-PCI - secondary bus towards host.*/
        p_bridge_semi_pci_secondary = 0x80,
        /** Communication - serial - 8250 compatible (generic XT). */
        p_comm_serial_8520 = 0x0,
        /** Communication - serial - 16450 compatible. */
        p_comm_serial_16450 = 0x1,
        /** Communication - serial - 16550 compatible. */
        p_comm_serial_16550 = 0x2,
        /** Communication - serial - 16650 compatible. */
        p_comm_serial_16650 = 0x3,
        /** Communication - serial - 16750 compatible. */
        p_comm_serial_16750 = 0x4,
        /** Communication - serial - 16850 compatible. */
        p_comm_serial_16850 = 0x5,
        /** Communication - serial - 16950 compatible. */
        p_comm_serial_16950 = 0x6,
        /** Communication - parallel - standard parallel port. */
        p_comm_parallel_standard = 0x0,
        /** Communication - parallel - bidirectional parallel port. */
        p_comm_parallel_bidirectional = 0x1,
        /** Communication - parallel - ECP 1.X compliant parallel port. */
        p_comm_parallel_ecp = 0x2,
        /** Communication - parallel - IEEE 1284 controller. */
        p_comm_parallel_1284_controller = 0x3,
        /** Communication - parallel - IEEE 1284 target device. */
        p_comm_parallel_1284_device = 0x4,
        /** Communication - modem - generic modem. */
        p_comm_modem_generic = 0x0,
        /** Communication - modem - Hayes 16450 compatible interface. */
        p_comm_modem_16450 = 0x1,
        /** Communication - modem - Hayes 16550 compatible interface. */
        p_comm_modem_16550 = 0x2,
        /** Communication - modem - Hayes 16650 compatible interface. */
        p_comm_modem_16650 = 0x3,
        /** Communication - modem - Hayes 16750 compatible interface. */
        p_comm_modem_16750 = 0x4,
        /** Peripheral - PIC - generic 8259 compatible. */
        p_peripheral_pic_generic = 0x0,
        /** Peripheral - PIC - ISA compatible. */
        p_peripheral_pic_isa = 0x1,
        /** Peripheral - PIC - EISA compatible. */
        p_peripheral_pic_eisa = 0x2,
        /** Peripheral - PIC - I/O APIC interrupt controller. */
        p_peripheral_pic_apic = 0x10,
        /** Peripheral - PIC - I/O(x) APIC interrupt controller. */
        p_peripheral_pic_xapic = 0x20,
        /** Peripheral - DMA - generic 8237 compatible. */
        p_peripheral_dma_generic = 0x0,
        /** Peripheral - DMA - ISA compatible. */
        p_peripheral_dma_isa = 0x1,
        /** Peripheral - DMA - EISA compatible. */
        p_peripheral_dma_eisa = 0x2,
        /** Peripheral - timer - generic 8254 compatible. */
        p_peripheral_timer_generic = 0x0,
        /** Peripheral - timer - ISA compatible. */
        p_peripheral_timer_isa = 0x1,
        /** Peripheral - timer - EISA compatible. */
        p_peripheral_timer_eisa = 0x2,
        /** Peripheral - timer - HPET. */
        p_peripheral_timer_hpet = 0x3,
        /** Peripheral - RTC - generic. */
        p_peripheral_rtc_generic = 0x0,
        /** Peripheral - RTC - ISA compatible. */
        p_peripheral_rtc_isa = 0x1,
        /** Input - gameport - generic. */
        p_input_gameport_generic = 0x0,
        /** Input - gameport - extended. */
        p_input_gameport_extended = 0x10,
        /** Serial - FireWire - generic. */
        p_serial_firewire_generic = 0x0,
        /** Serial - FireWire - OHCI. */
        p_serial_firewire_ohci = 0x10,
        /** Serial - USB - UHCI controller. */
        p_serial_usb_uhci = 0x0,
        /** Serial - USB - OHCI controller. */
        p_serial_usb_ohci = 0x10,
        /** Serial - USB - EHCI (USB2) controller. */
        p_serial_usb_ehci = 0x20,
        /** Serial - USB - XHCI (USB3) controller. */
        p_serial_usb_xhci = 0x30,
        /** Serial - USB - unspecified. */
        p_serial_usb_unspecified = 0x80,
        /** Serial - USB - device. */
        p_serial_usb_device = 0xFE,
        /** Serial - IPMI - SMIC. */
        p_serial_ipmi_smic = 0x0,
        /** Serial - IPMI - keyboard controller style. */
        p_serial_ipmi_keyboard = 0x1,
        /** Serial - IPMI - block transfer. */
        p_serial_ipmi_block = 0x2,
        /** Unassigned. */
        p_unassigned = 0xFF
    };

    /** Highest possible bus index. */
    static constexpr uint32_t max_buses = 0xFF;
    /** Highest possible device index. */
    static constexpr uint32_t max_devices = 0x1F;
    /** Highest possible function index. */
    static constexpr uint32_t max_functions = 0x7;

    /**
        Constructor. Sets the bus, device and function numbers, then check for
        existence.

        @param b Bus number.
        @param d Device number.
        @param f Function number.
     */
    PciDevice(uint32_t b, uint32_t d, uint32_t f);

    /**
        Prints device information to the provided stream.

        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

    /**
        Whether the device exists.

        @return True if the device exists, false otherwise.
     */
    bool exists() const { return ex; }

    /**
        Get the bus number.

        @return Bus number.
     */
    uint32_t get_bus() const { return bus_no; }

    /**
        Get the device number.

        @return Device number.
     */
    uint32_t get_device() const { return device_no; }

    /**
        Get the function number.

        @return Function number.
     */
    uint32_t get_function() const { return func_no; }

    /**
        Get a Base Address Register (BAR). There are six BARs for a PCI device.
        Their use depends on the device.

        @param bar BAR to fetch. A number between 0 and 5 inclusive.
        @return Contents of the BAR, or 0 if an invalid bar was specified.
     */
    uint32_t get_bar(size_t bar) const;

    /**
        Get the device class.

        @return Device class.
     */
    class_code get_class() const;

    /**
        Get the header type.

        @return Header type.
     */
    uint8_t get_header_type() const;

    /**
        Get the interrupt line this device will use. This corresponds to the PIC
        IRQ numbers, not the IDT offsets.

        @return PIC IRQ number in use, or 0xFF is not conencted.
     */
    uint8_t get_interrupt_line() const;

    /**
        Writes a location string (BB:DD.F bus:device:function) with all values
        in hex.

        @return Location of this device on the bus.
     */
    klib::string get_location() const;

    /**
        Get the device prog IF.

        @return Device prog IF.
     */
    progif_code get_progif() const;

    /**
        Get the secondary bus number, if this is a PCI-to-PCI bridge. Othrwise
        returns 0.

        @return Secondary bus number or 0.
     */
    uint32_t get_secondary_bus() const;

    /**
        Get the device subclass.

        @return Device subclass.
     */
    subclass_code get_subclass() const;

    /**
        Get the vendor ID.

        @return Vendor ID.
     */
    uint16_t get_vendor() const;

    /**
        Checks whether the device is multifunction.

        @return True if the device is multifunction, false otherwise.
     */
    bool is_multifunction() const;

    /**
        Reads a register from the configuration space. There are 64 registers,
        each with a size of 4 bytes, giving a 256 byte configuration space.
        These must be read as a 4 byte whole. The offset will therefore be
        rounded down to be aligned. The caller may need to offset and mask to
        extract the relevant data.

        @param offset Register to read.
        @return Value of the register at offet for this device, or 0xFFFFFFFF if
                the device does not exist.
     */
    uint32_t read_config(uint8_t offset) const;

    /**
        Set the interrupt line this device will use. This corresponds to the PIC
        IRQ numbers, not the IDT offsets. Depending on the device, this may or
        may not have any actual effect. Try reading the interrupt line back
        after the set.

        @param line PIC IRQ to use.
     */
    void set_interrupt_line(uint8_t line) const;

    /**
        Writes a register in the configuration space. There are 64 registers,
        each with a size of 4 bytes, giving a 256 byte configuration space.
        These must be written as a 4 byte whole. The offset will therefore be
        rounded down to be aligned. The caller may need to first read in the 
        relevant register and then mask to make the necessary adjustments.

        @param offset Register to read.
        @param data Data to set the register to.
     */
    void write_config(uint8_t offset, uint32_t data) const;

protected:
    // Bus number.
    uint32_t bus_no;
    // Device number.
    uint32_t device_no;
    // Function number.
    uint32_t func_no;
    // Whether the device exists. This check is performed by the constructor.
    bool ex;

    // Highest BAR index.
    static constexpr size_t max_bar = 5;
};

/**
    Searches the PCI bus to create a list of the devices that exist.

    @return List of PCI devices.
 */
klib::vector<PciDevice> find_pci();

#endif /* PCI_H */
