#include "Pci.h"

#include <map>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "io.h"

/******************************************************************************
 ******************************************************************************/

PciDevice::PciDevice(uint32_t b, uint32_t d, uint32_t f) :
    bus_no{b}, device_no{d}, func_no{f}, ex{true}
{
    if (bus_no > max_buses || device_no > max_devices ||
        func_no > max_functions)
    {
        ex = false;
        return;
    }

    // Try to get the vendor number, to check whether the device exists. A
    // return of 0xFFFF means the device does not exist, as no vendor has that
    // ID.
    if (get_vendor() == 0xFFFF)
        ex = false;
}

/******************************************************************************/

void PciDevice::dump(klib::ostream& dest) const
{
    // Class names.
    static const klib::map<PciDevice::class_code, klib::string> class_names {
        klib::pair<class_code, klib::string>
            {cl_unclassified, "Unclassified device"},
        klib::pair<class_code, klib::string>
            {cl_mass_storage, "Mass storage controller"},
        klib::pair<class_code, klib::string>
            {cl_network, "Network controller"},
        klib::pair<class_code, klib::string>
            {cl_display, "Display Controller"},
        klib::pair<class_code, klib::string>
            {cl_multimedia, "Multimedia controller"},
        klib::pair<class_code, klib::string>
            {cl_memory, "Memory controller"},
        klib::pair<class_code, klib::string>
            {cl_bridge, "Bridge device"},
        klib::pair<class_code, klib::string>
            {cl_comm, "Simple communication controller"},
        klib::pair<class_code, klib::string>
            {cl_peripheral, "Base system peripheral"},
        klib::pair<class_code, klib::string>
            {cl_input, "Input device controller"},
        klib::pair<class_code, klib::string>
            {cl_docking, "Docking station"},
        klib::pair<class_code, klib::string>
            {cl_proc, "Processor"},
        klib::pair<class_code, klib::string>
            {cl_serial, "Serial bus controller"},
        klib::pair<class_code, klib::string>
            {cl_wireless, "Wireless controller"},
        klib::pair<class_code, klib::string>
            {cl_intelligent, "Intelligent controller"},
        klib::pair<class_code, klib::string>
            {cl_satellite, "Satellite communication controller"},
        klib::pair<class_code, klib::string>
            {cl_encryption, "Encryption controller"},
        klib::pair<class_code, klib::string>
            {cl_signal, "Signal processing controller"},
        klib::pair<class_code, klib::string>
            {cl_accel, "Processing accelerator"},
        klib::pair<class_code, klib::string>
            {cl_instrument, "Non-essential instrumentation"},
        klib::pair<class_code, klib::string>
            {cl_coproc, "Co-processor"},
        klib::pair<class_code, klib::string>
            {cl_unassigned, "Unassigned class"}};

    // Sub-class names.
    static const
        klib::map<klib::pair<class_code, subclass_code>,
        klib::string>
        subclass_names {
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_unclassified, scl_unclassified_non_vga},
            "Non-VGA compatible device"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_unclassified, scl_unclassified_vga},
            "VGA compatible device"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_mass_storage, scl_mass_storage_scsi},
            "SCSI bus controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_mass_storage, scl_mass_storage_ide},
            "IDE controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_mass_storage, scl_mass_storage_floppy},
            "Floppy disk controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_mass_storage, scl_mass_storage_ipi},
            "IPI bus controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_mass_storage, scl_mass_storage_raid},
            "RAID controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_mass_storage, scl_mass_storage_ata},
            "ATA controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_mass_storage, scl_mass_storage_sata},
            "Serial ATA controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_mass_storage, scl_mass_storage_sas},
            "Serial attached SCSI controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_mass_storage, scl_mass_storage_memory},
            "Non-volatile memory controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_mass_storage, scl_other},
            "Other"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_network, scl_network_ethernet},
            "Ethernet controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_network, scl_network_token},
            "Token ring controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_network, scl_network_fddi},
            "FDDI controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_network, scl_network_atm},
            "ATM controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_network, scl_network_isdn},
            "ISDN controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_network, scl_network_worldfip},
            "WorldFip controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_network, scl_network_picmg},
            "PICMG 2.14 Multi Computing"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_network, scl_network_infiniband},
            "InfiniBand controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_network, scl_network_fabric},
            "Fabric controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_network, scl_other},
            "Other"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_display, scl_display_vga},
            "VGA compatible controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_display, scl_display_xga},
            "XGA compatible controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_display, scl_display_3d},
            "3D contoller (not VGA compatible)"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_display, scl_other},
            "Other"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_multimedia, scl_multimedia_video},
            "Multimedia video controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_multimedia, scl_multimedia_audio},
            "Multimedia audio controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_multimedia, scl_multimedia_telephone},
            "Computer telephony device"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_multimedia, scl_multimedia_audio_device},
            "Audio device"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_multimedia, scl_other},
            "Other"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_memory, scl_memory_ram},
            "RAM controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_memory, scl_memory_flash},
            "Flash controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_memory, scl_other},
            "Other"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_bridge, scl_bridge_host},
            "Host bridge"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_bridge, scl_bridge_isa},
            "ISA bridge"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_bridge, scl_bridge_eisa},
            "EISA bridge"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_bridge, scl_bridge_mca},
            "MCA bridge"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_bridge, scl_bridge_pci},
            "PCI-to-PCI bridge"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_bridge, scl_bridge_pcmcia},
            "PCMCIA bridge"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_bridge, scl_bridge_nubus},
            "NuBus bridge"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_bridge, scl_bridge_cardbus},
            "CardBus bridge"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_bridge, scl_bridge_raceway},
            "RACEway bridge"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_bridge, scl_bridge_semi_pci},
            "PCI-to-PCI bridge"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_bridge, scl_bridge_infiniband},
            "InfiniBand-to-PCI host bridge"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_bridge, scl_other},
            "Other"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_comm, scl_comm_serial},
            "Serial controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_comm, scl_comm_parallel},
            "Parallel controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_comm, scl_comm_multiport},
            "Multiport Serial controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_comm, scl_comm_modem},
            "Modem"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_comm, scl_comm_gpib},
            "IEEE 488.1/2 (GPIB) Controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_comm, scl_comm_smart},
            "Smart Card"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_comm, scl_other},
            "Other"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_peripheral, scl_peripheral_pic},
            "PIC"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_peripheral, scl_peripheral_dma},
            "DMA controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_peripheral, scl_peripheral_timer},
            "Timer"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_peripheral, scl_peripheral_rtc},
            "RTC controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_peripheral, scl_peripheral_hotplug},
            "PCI hot-plug controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_peripheral, scl_peripheral_sd},
            "SD host controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_peripheral, scl_peripheral_iommu},
            "IOMMU controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_peripheral, scl_other},
            "Other"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_input, scl_input_keyboard},
            "Keyboard controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_input, scl_input_pen},
            "Digitiser pen"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_input, scl_input_mouse},
            "Mouse controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_input, scl_input_scanner},
            "Scanner controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_input, scl_input_gameport},
            "Gameport controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_input, scl_other},
            "Other"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_docking, scl_docking_generic},
            "Generic"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_docking, scl_other},
            "Other"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_proc, scl_proc_386},
            "386"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_proc, scl_proc_486},
            "486"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_proc, scl_proc_pentium},
            "Pentium"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_proc, scl_proc_alpha},
            "Alpha"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_proc, scl_proc_powerpc},
            "PowerPC"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_proc, scl_proc_mips},
            "MIPS"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_proc, scl_proc_coproc},
            "Co-processor"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_serial, scl_serial_firewire},
            "FireWire (IEEE 1394) contoller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_serial, scl_serial_access},
            "ACCESS bus"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_serial, scl_serial_ssa},
            "SSA"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_serial, scl_serial_usb},
            "USB controller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_serial, scl_serial_fibre},
            "Fibre channel"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_serial, scl_serial_smbus},
            "SMBus"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_serial, scl_serial_infiniband},
            "InfiniBand"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_serial, scl_serial_ipmi},
            "IPMI interface"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_serial, scl_serial_sercos},
            "SERCOS interface (IEC 61491)"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_serial, scl_serial_canbus},
            "CANbus"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_wireless, scl_wireless_irda},
            "iRDA compatible contoller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_wireless, scl_wireless_ir},
            "Consumer IR contoller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_wireless, scl_wireless_rf},
            "RF contoller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_wireless, scl_wireless_bluetooth},
            "Bluetooth contoller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_wireless, scl_wireless_ethernet_a},
            "Ethernet contoller (802.1a)"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_wireless, scl_wireless_ethernet_b},
            "Ethernet contoller (802.1b)"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_wireless, scl_other},
            "Other"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_intelligent, scl_intelligent_i20},
            "I20"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_satellite, scl_satellite_tv},
            "Satellite TV contoller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_satellite, scl_satellite_audio},
            "Satellite audio contoller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_satellite, scl_satellite_voice},
            "Satellite voice contoller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_satellite, scl_satellite_data},
            "Satellite data contoller"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_encryption, scl_encryption_network},
            "Network and computing encryption/decryption"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_encryption, scl_encryption_entertainment},
            "Entertainment encryption/decryption"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_encryption, scl_other},
            "Other"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_signal, scl_signal_dpio},
            "DPIO Modules"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_signal, scl_signal_performance},
            "Performance counters"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_signal, scl_signal_synch},
            "Communication synchroniser"},
        klib::pair<klib::pair<class_code, subclass_code>, klib::string>
            {klib::pair<class_code, subclass_code>
            {cl_signal, scl_signal_management},
            "Signal processing management"}
    };

    // Prog IF names. I really don't fancy implementing tuple, so use nested
    // pairs instead.
    static const klib::map<klib::pair<klib::pair<class_code, subclass_code>,
        progif_code>, klib::string> progif_names {
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_ide}, p_mass_storage_ide_isa},
            "ISA compatability mode-only controller"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_ide}, p_mass_storage_ide_pci},
            "PCI native mode-only controller"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_ide}, p_mass_storage_ide_isa_pci},
            "ISA Compatibility mode controller, supports both channels switched to PCI native mode"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_ide}, p_mass_storage_ide_pci_isa},
            "PCI native mode controller, supports both channels switched to ISA compatibility mode"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_ide}, p_mass_storage_ide_isa_master},
            "ISA Compatibility mode-only controller, supports bus mastering"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_ide}, p_mass_storage_ide_pci_master},
            "PCI native mode-only controller, supports bus mastering"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_ide}, p_mass_storage_ide_isa_pci_master},
            "ISA Compatibility mode controller, supports both channels switched to PCI native mode, supports bus mastering"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_ide}, p_mass_storage_ide_pci_isa_master},
            "PCI native mode controller, supports both channels switched to ISA compatibility mode, supports bus mastering"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_ata}, p_mass_storage_ata_single},
            "Single DMA"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_ata}, p_mass_storage_ata_chained},
            "Chained DMA"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_sata}, p_mass_storage_sata_vendor},
            "Vendor specific interface"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_sata}, p_mass_storage_sata_achi},
            "ACHI 1.0"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_sata}, p_mass_storage_sata_serial},
            "Serial storage bus"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_sas}, p_mass_storage_sas_sas},
            "SAS"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_sas}, p_mass_storage_sas_serial},
            "Serial storage bus"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_memory}, p_mass_storage_memory_nvmhci},
            "NVMHCI"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_mass_storage, scl_mass_storage_memory}, p_mass_storage_memory_nvmexpress},
            "NVM Express"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_display, scl_display_vga}, p_display_vga_vga},
            "VGA controller"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_display, scl_display_vga}, p_display_vga_8514},
            "8514 compatible controller"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_bridge, scl_bridge_pci}, p_bridge_pci_normal},
            "Normal decode"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_bridge, scl_bridge_pci}, p_bridge_pci_subtractive},
            "Subtractive decode"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_bridge, scl_bridge_raceway}, p_bridge_raceway_transparent},
            "Transparent mode"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_bridge, scl_bridge_raceway}, p_bridge_raceway_endpoint},
            "Endpoint mode"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_bridge, scl_bridge_semi_pci}, p_bridge_semi_pci_primary},
            "Semi-transparent, primary bus towards host CPU"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_bridge, scl_bridge_semi_pci}, p_bridge_semi_pci_secondary},
            "Semi-transparent, secondary bus towards host CPU"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_serial}, p_comm_serial_8520},
            "8520 compatible (Generic XT)"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_serial}, p_comm_serial_16450},
            "16450 compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_serial}, p_comm_serial_16550},
            "16550 compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_serial}, p_comm_serial_16650},
            "16650 compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_serial}, p_comm_serial_16750},
            "16750 compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_serial}, p_comm_serial_16850},
            "16850 compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_serial}, p_comm_serial_16950},
            "16950 compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_parallel}, p_comm_parallel_standard},
            "Standard parallel port"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_parallel}, p_comm_parallel_bidirectional},
            "Bi-directional parallel port"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_parallel}, p_comm_parallel_ecp},
            "ECP 1.X compliant parallel port"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_parallel}, p_comm_parallel_1284_controller},
            "IEEE 1284 controller"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_parallel}, p_comm_parallel_1284_device},
            "IEEE 1284 target device"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_modem}, p_comm_modem_generic},
            "Generic modem"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_modem}, p_comm_modem_16450},
            "Hayes 16450 compatible interface"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_modem}, p_comm_modem_16550},
            "Hayes 16550 compatible interface"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_modem}, p_comm_modem_16650},
            "Hayes 16650 compatible interface"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_comm, scl_comm_modem}, p_comm_modem_16750},
            "Hayes 16750 compatible interface"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_pic}, p_peripheral_pic_generic},
            "Generic 8529 compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_pic}, p_peripheral_pic_isa},
            "ISA compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_pic}, p_peripheral_pic_eisa},
            "EISA compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_pic}, p_peripheral_pic_apic},
            "I/O APIC interrupt controller"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_pic}, p_peripheral_pic_xapic},
            "I/O(x) APIC interrupt controller"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_dma}, p_peripheral_dma_generic},
            "Generic 8237 compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_dma}, p_peripheral_dma_isa},
            "ISA compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_dma}, p_peripheral_dma_eisa},
            "EISA compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_timer}, p_peripheral_timer_generic},
            "Generic 8254 compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_timer}, p_peripheral_timer_isa},
            "ISA compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_timer}, p_peripheral_timer_eisa},
            "EISA compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_timer}, p_peripheral_timer_hpet},
            "HPET compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_rtc}, p_peripheral_rtc_generic},
            "Generic"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_peripheral, scl_peripheral_rtc}, p_peripheral_rtc_isa},
            "ISA compatible"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_input, scl_input_gameport}, p_input_gameport_generic},
            "Generic"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_input, scl_input_gameport}, p_input_gameport_extended},
            "Extended"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_serial, scl_serial_firewire}, p_serial_firewire_generic},
            "Generic"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_serial, scl_serial_firewire}, p_serial_firewire_ohci},
            "OHCI"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_serial, scl_serial_usb}, p_serial_usb_uhci},
            "UHCI"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_serial, scl_serial_usb}, p_serial_usb_ohci},
            "OHCI"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_serial, scl_serial_usb}, p_serial_usb_ehci},
            "EHCI (USB2)"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_serial, scl_serial_usb}, p_serial_usb_xhci},
            "XHCI (USB3)"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_serial, scl_serial_usb}, p_serial_usb_unspecified},
            "Unspecified"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_serial, scl_serial_usb}, p_serial_usb_device},
            "Device"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_serial, scl_serial_ipmi}, p_serial_ipmi_smic},
            "SMIC"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_serial, scl_serial_ipmi}, p_serial_ipmi_keyboard},
            "Keyboard controller style"},
        klib::pair<klib::pair<klib::pair<class_code, subclass_code>,
            progif_code>, klib::string> {
            klib::pair<klib::pair<class_code, subclass_code>, progif_code> {
            klib::pair<class_code, subclass_code> {
            cl_serial, scl_serial_ipmi}, p_serial_ipmi_block},
            "Block transfer"}
    };

    dest << "  " << get_location() << ' ';

    if (!ex)
    {
        dest << "No device\n";
        return;
    }

    // Class, subclass and progif.
    class_code cl = get_class();
    subclass_code scl = get_subclass();
    progif_code p = get_progif();

    auto it1 = class_names.find(cl);
    if (it1 != class_names.end())
        dest << it1->second;

    auto it2 =
        subclass_names.find(klib::pair<class_code, subclass_code>{cl, scl});
    if (it2 != subclass_names.end())
        dest << "; " << it2->second;

    auto it3 = progif_names.find(
        klib::pair<klib::pair<class_code, subclass_code>, progif_code>
        {klib::pair<class_code, subclass_code>{cl, scl}, p});
    if (it3 != progif_names.end())
        dest << "; " << it3->second;

    dest << '\n';

    dest.flush();
}

/******************************************************************************/

uint32_t PciDevice::get_bar(size_t bar) const
{
    if (!ex || bar > max_bar)
        return 0x0;

    // The bars begin at the fifth register ad are consecutive. Each one is a
    // full register.
    uint32_t reg = read_config(0x10 + 4 * bar);
    return reg;
}

/******************************************************************************/

PciDevice::class_code PciDevice::get_class() const
{
    if (!ex)
        return cl_unassigned;

    // The class is the highest byte of the third register.
    uint32_t reg = read_config(0x8);
    return static_cast<class_code>(reg >> 24);
}

/******************************************************************************/

uint8_t PciDevice::get_header_type() const
{
    if (!ex)
        return 0x00;

    // The header type is the second highest byte of the fourth register.
    uint32_t reg = read_config(0xC);
    return static_cast<uint8_t>((reg >> 16) & 0xFF);
}

/******************************************************************************/

uint8_t PciDevice::get_interrupt_line() const
{
    if (!ex)
        // 0xFF is used to indicate no connection.
        return 0xFF;

    // The interrupt line is the lowest byte of the sixteenth register.
    uint32_t reg = read_config(0x3C);
    return static_cast<uint8_t>(reg & 0xFF);
}

/******************************************************************************/

klib::string PciDevice::get_location() const
{
    // Bus, device and function
    klib::string ret_val;
    klib::string tmp;
    klib::helper::strprintf(tmp, "%X", bus_no);
    tmp.erase(0, 2);
    if (tmp.size() == 1)
        tmp = '0' + tmp;
    ret_val = tmp + ':';

    klib::helper::strprintf(tmp, "%X", device_no);
    tmp.erase(0, 2);
    if (tmp.size() == 1)
        tmp = '0' + tmp;
    ret_val += tmp + '.';

    klib::helper::strprintf(tmp, "%X", func_no);
    tmp.erase(0, 2);
    ret_val += tmp;

    return ret_val;
}

/******************************************************************************/

PciDevice::progif_code PciDevice::get_progif() const
{
    if (!ex)
        return p_unassigned;

    // The subclass is the second lowest byte of the third register.
    uint32_t reg = read_config(0x8);
    return static_cast<progif_code>((reg >> 8) & 0xFF);
}

/******************************************************************************/

uint32_t PciDevice::get_secondary_bus() const
{
    if (!ex || get_class() != cl_bridge || get_subclass() != scl_bridge_pci)
        return 0;

    // The secondary bus is second lowest byte of the 7th register.
    uint32_t reg = read_config(0x18);
    return (reg >> 8) & 0xFF;
}

/******************************************************************************/

PciDevice::subclass_code PciDevice::get_subclass() const
{
    if (!ex)
        return scl_unassigned;

    // The subclass is the second highest byte of the third register.
    uint32_t reg = read_config(0x8);
    return static_cast<subclass_code>((reg >> 16) & 0xFF);
}

/******************************************************************************/

uint16_t PciDevice::get_vendor() const
{
    if (!ex)
        return 0xFFFF;

    // The vendor ID is the lower two bytes of the first register.
    uint32_t reg = read_config(0);
    return static_cast<uint16_t>(reg);
}

/******************************************************************************/

bool PciDevice::is_multifunction() const
{
    // Fetch the header type.
    uint8_t header_type = get_header_type();
    // The device is multifunction if the 7th bit is set.
    return ((header_type >> 7) == 1);
}

/******************************************************************************/

uint32_t PciDevice::read_config(uint8_t offset) const
{
    if (!ex)
        return 0xFFFFFFFF;

    // Round offset down to be 4 byte aligned.
    offset &= 0xFC;

    // Compose the address. Bus is bits 23-16, device 15-11, function 10-8 and
    // offset 7-0 (although 1-0 will be 0). 31 must be set and 30-24 unset.
    uint32_t address = (bus_no << 16) | (device_no << 11) | (func_no << 8) |
        offset | 0x80000000;
    address &= 0x80FFFFFF;

    // Send the configuration.
    outl(address, static_cast<uint16_t>(config_address));

    // Read data and return.
    return inl(static_cast<uint16_t>(config_data));
}

/******************************************************************************/

void PciDevice::set_interrupt_line(uint8_t line) const
{
    // The interrupt line is the lowest byte of the sixteenth register.
    uint32_t data = read_config(0x3C);
    data &= 0xFFFFFF00;
    data &= line;
    write_config(0x3C, data);
}

/******************************************************************************/

void PciDevice::write_config(uint8_t offset, uint32_t data) const
{
    if (!ex)
        return;

    // Round offset down to be 4 byte aligned.
    offset &= 0xFC;

    // Compose the address. Bus is bits 23-16, device 15-11, function 10-8 and
    // offset 7-0 (although 1-0 will be 0). 31 must be set and 30-24 unset.
    uint32_t address = (bus_no << 16) | (device_no << 11) | (func_no << 8) |
        offset | 0x80000000;
    address &= 0x80FFFFFF;

    // Send the configuration.
    outl(address, static_cast<uint16_t>(config_address));

    // Write data.
    outl(data, static_cast<uint16_t>(config_data));
}

/******************************************************************************
 ******************************************************************************/

static void scan_bus(uint32_t bus, klib::vector<PciDevice>& devices)
{
    // Do nothing if the bus number is too high.
    if (bus > PciDevice::max_buses)
        return;

    // Scan the indicated bus for devices.
    for (uint32_t d = 0; d < PciDevice::max_devices; ++d)
    {
        PciDevice temp {bus, d, 0};
        if (temp.exists())
        {
            // Add the device to the list.
            devices.push_back(temp);
            // Check if this is a PCI-to-PCI bridge.
            if (temp.get_class() == PciDevice::cl_bridge &&
                temp.get_subclass() == PciDevice::scl_bridge_pci)
                // Check the secondary bus.
                scan_bus(temp.get_secondary_bus(), devices);

            // Check if this is a multifunction device.
            if (temp.is_multifunction())
            {
                // Scan additional functions.
                for (uint32_t f = 1; f < PciDevice::max_functions; ++f)
                {
                    PciDevice temp2 {bus, d, f};
                    if (temp2.exists())
                    {
                        // Add the device to the list.
                        devices.push_back(temp2);
                        // Check if this is a PCI-to-PCI bridge.
                        if (temp2.get_class() == PciDevice::cl_bridge &&
                            temp2.get_subclass() == PciDevice::scl_bridge_pci)
                            // Check the secondary bus.
                            scan_bus(temp2.get_secondary_bus(), devices);
                    }
                }
            }
        }
    }
}

/******************************************************************************
 ******************************************************************************/

klib::vector<PciDevice> find_pci()
{
    klib::vector<PciDevice> devices;

    // The device at (0, 0, 0) should be the PCI host controller.
    PciDevice temp {0, 0, 0};
    if (temp.exists() == false)
        // No host controller. Return.
        return devices;

    // Work out how many buses the host controller is managing.
    uint32_t no_buses = 1;
    if (temp.is_multifunction())
    {
        for (PciDevice temp2 {0, 0, no_buses}; temp2.exists();
            ++no_buses, temp2 = PciDevice {0, 0, no_buses})
        {
            devices.push_back(temp2);
        }
    }

    for (uint32_t i = 0; i < no_buses; ++i)
        scan_bus(i, devices);

    return devices;
}

/******************************************************************************
 ******************************************************************************/
