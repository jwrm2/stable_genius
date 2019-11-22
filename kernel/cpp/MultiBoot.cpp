#include "MultiBoot.h"

#include <array>
#include <cstring>
#include <fstream>
#include <limits>
#include <new>
#include <ostream>
#include <typeinfo>

#include "FileSystem.h"
#include "Kernel.h"
#include "Logger.h"
#include "PageDescriptorTable.h"
#include "PageFrameAllocator.h"
#include "util.h"

/******************************************************************************
 ******************************************************************************/

// Fetch a string given the physical address of its start.

klib::string getstring(const char* phys_addr, PageDescriptorTable& pdt)
{
    // This is complicated: we need to map the pointer to a virtual address
    // and make sure that the whole string is included in the mapping. We
    // don't know the length of the string until we've finished reading it.
    // We'll just have to start reading and increase the mapping if
    // necessary. What a pain.
    size_t alloc_size = PageDescriptorTable::page_size;
    const char* virt_addr = static_cast<char*>(pdt.map(phys_addr, alloc_size));
    if (virt_addr == nullptr)
        global_kernel->panic("No virtual memory for multiboot allocation");

    size_t size = 0;
    const char* current = virt_addr;
    while (*current != 0)
    {
        ++size;
        if (size == alloc_size)
        {
            pdt.unmap(virt_addr, alloc_size);
            alloc_size += PageDescriptorTable::page_size;
            virt_addr = static_cast<const char*>(
                pdt.map(phys_addr, alloc_size)
            );
            if (virt_addr == nullptr)
                global_kernel->panic(
                    "No virtual memory for multiboot allocation");
            current = virt_addr + size;
        }
        else
            ++current;
    }
    
    klib::string ret_val {virt_addr};
    pdt.unmap(virt_addr, alloc_size);

    return ret_val;
}

/******************************************************************************
 ******************************************************************************/

MultiBootDev::MultiBootDev(const uint32_t* start) : val{true}
{
    uint32_t data = *start;
    p3 = static_cast<uint8_t>(data);
    p2 = static_cast<uint8_t>(data >> 8);
    p1 = static_cast<uint8_t>(data >> 16);
    drv = static_cast<uint8_t>(data >> 24);

    if ((p2 == 0xFF && p3 != 0xFF) ||
        (p1 == 0xFF && (p2 != 0xFF || p3 != 0xFF)))
        val = false;
}

/******************************************************************************
 ******************************************************************************/

MultiBootModule::MultiBootModule(const uint32_t* start,
    PageDescriptorTable& pdt) :
    val{true}
{
    // Get the physical start and end.
    const uint32_t* phys_start = reinterpret_cast<const uint32_t*>(*start);
    const uint32_t* phys_end = reinterpret_cast<const uint32_t*>(*(start + 1));
    ptrdiff_t sz = (phys_end - phys_start) * sizeof(uint32_t);

    // Create a virtual mapping for the physical address the module is currently
    // occupying.
    const uint32_t* virt_start = static_cast<const uint32_t*>(
        pdt.map(phys_start, sz));

    // Create heap space for the module and copy it across. This means we can
    // later call the module without messing about with physical memory.
    uint32_t* dest = new uint32_t[sz / sizeof(uint32_t)];
    klib::memcpy(dest, virt_start, sz);

    // Unmap the physical memory and set the new virtual start and end.
    pdt.unmap(virt_start, sz);

    mstart = dest;
    mend = dest + sz / sizeof(uint32_t);

    // A string may be associated with the module. It may be empty.
    // Get the physical location of the string and fetch.
    const char* phys_addr =
        reinterpret_cast<const char*>(*(start + 2));
    str = getstring(phys_addr, pdt);
}

/******************************************************************************/

MultiBootModule::MultiBootModule(const MultiBootModule& other) :
    val{other.val}, str{other.str}, mstart{nullptr}, mend{nullptr}
{
    if (other.mstart)
    {
        ptrdiff_t sz = other.mod_size();
        uint32_t* dest = new uint32_t[sz / sizeof(uint32_t)];
        klib::memcpy(dest, other.mstart, sz);

        mstart = dest;
        mend = dest + sz / sizeof(uint32_t);
    }
}

/******************************************************************************/

MultiBootModule::MultiBootModule(MultiBootModule&& other) :
    val{other.val}, str{other.str}, mstart{other.mstart}, mend{other.mend}
{
    // Set other to invalid (otherwise the destructor will mess up the data).
    other.mstart = nullptr;
    other.mend = nullptr;
}

/******************************************************************************/

MultiBootModule& MultiBootModule::operator=(const MultiBootModule& other)
{
    val = other.val;
    str = other.str;

    delete[] static_cast<uint32_t*>(mstart);

    if (other.mstart)
    {
        ptrdiff_t sz = other.mod_size();
        uint32_t* dest = new uint32_t[sz / sizeof(uint32_t)];
        klib::memcpy(dest, other.mstart, sz);

        mstart = dest;
        mend = dest + sz / sizeof(uint32_t);
    }
    else
    {
        mstart = nullptr;
        mend = nullptr;
    }

    return *this;
}

/******************************************************************************/

MultiBootModule& MultiBootModule::operator=(MultiBootModule&& other)
{
    val = other.val;
    str = other.str;

    delete[] static_cast<uint32_t*>(mstart);

    mstart = other.mstart;
    mend = other.mend;

    // Set other to invalid (otherwise the destructor will mess up the data).
    other.mstart = nullptr;
    other.mend = nullptr;

    return *this;
}

/******************************************************************************
 ******************************************************************************/

klib::array<const char* const, MultiBootMap::field_number>
    MultiBootMap::field_headers = {"Index", "Address", "Length", "Type"};

klib::array<const size_t, MultiBootMap::field_number>
    MultiBootMap::field_widths = {6, 11, 11, 4};

/******************************************************************************/

MultiBootMap::MultiBootMap(const uint32_t* start) : val{true}
{
    // The multiboot spec reserves 8 bytes for each of these fields. GRUB on
    // 32 bit x86 seems to use the first word and leaves the second blank.
    addr = reinterpret_cast<void*>(*start);
    len = *(start + 2);
    uint32_t t = *(start + 4);
    switch (t)
    {
        case 1: case 3: case 4: case 5:
            mtype = static_cast<MemType>(t);
            break;
        default:
            mtype = MemType::reserved;
    }
}

/******************************************************************************/

void MultiBootMap::dump(klib::ostream& dest) const
{
    dest.width(field_widths[1]);
    dest << klib::hex << addr << ' ';
    dest.width(field_widths[2]);
    dest << format_bytes(len) << ' ';
    dest.width(field_widths[3]);
    dest << static_cast<uint32_t>(mtype);
    dest << klib::dec;

    dest.flush();
}

/******************************************************************************
 ******************************************************************************/

MultiBootDrive::MultiBootDrive(const uint32_t* start) : val{true}
{
    sz = *start;
    no = static_cast<uint8_t>(*(start + 1));
    uint8_t m = static_cast<uint8_t>(*(start + 1) >> 8);
    switch (m)
    {
        case 0:
            mode = AccessMode::chs;
            break;
        case 1:
            mode = AccessMode::lba;
            break;
        default:
            mode = AccessMode::invalid;
            val = false;
            return;
    }
    cyl = static_cast<uint16_t>(*(start + 1) >> 16);
    head = static_cast<uint8_t>(*(start + 2));
    sect = static_cast<uint8_t>(*(start + 2) >> 16);

    // Get the list of io ports. The data is sequential, and zero terminated.
    uintptr_t offset = 5;
    for (uint16_t p;
        (p = static_cast<uint16_t>(*(start + offset / 2) >>
            (offset % 2) * 16)) != 0;
        ++offset)
        ioports.push_back(p);
}

/******************************************************************************
 ******************************************************************************/

MultiBootRom::MultiBootRom(const uint32_t* start) : val{true}
{
    // Get the number of bytes to read.
    uint16_t sz = static_cast<uint16_t>(*start);

    // The size can be zero, indicating the data could not be generated.
    if (sz == 0)
    {
        val = false;
        return;
    }

    // Read the data into the data vector.
    for (uint16_t i = 0; i < sz / sizeof(uint32_t); ++i)
        data.push_back(*(start++));
}

/******************************************************************************
 ******************************************************************************/

MultiBootApm::MultiBootApm(const uint32_t* start) : val{true}
{
    ver = static_cast<uint16_t>(*start);
    code_seg = static_cast<uint16_t>(*start >> 16);
    off = reinterpret_cast<void*>(*(start + 1));
    code_seg_16 = static_cast<uint16_t>(*(start + 2));
    data_seg = static_cast<uint16_t>(*(start + 2) >> 16);
    flag = static_cast<uint16_t>(*(start + 3));
    code_seg_len = static_cast<uint16_t>(*(start + 3) >> 16);
    code_seg_16_len = static_cast<uint16_t>(*(start + 4));
    data_seg_len = static_cast<uint16_t>(*(start + 4) >> 16);
}

/******************************************************************************/

void MultiBootApm::dump(klib::ostream& dest) const
{
    if (!valid())
    {
        dest << "MultiBootApm table is invalid\n";
        return;
    }

    dest << "Dumping MultiBoot APM information\n";
    dest << "  Version number: " << ver << '\n';
    dest << "  32-bit code segment: " << klib::hex << code_seg << '\n';
    dest << "  Offset of entry point: " << off << '\n';
    dest << "  16-bit code segment: " << code_seg_16 << '\n';
    dest << "  16-bit data segment: " << data_seg << '\n';
    dest << "  Flags: " << flag << '\n';
    dest << "  Length of 32-bit code segment: " << code_seg_len << '\n';
    dest << "  Length of 16-bit code segment: " << code_seg_16_len << '\n';
    dest << "  Length of 16-bit data segment: " << data_seg_len << '\n';
    dest << klib::dec;

    dest.flush();
}

/******************************************************************************
 ******************************************************************************/

MultiBootVbe::MultiBootVbe(const uint32_t* start, PageDescriptorTable& pdt) :
    val{true}
{
    // Fetch control info.
    const void* phys_addr = reinterpret_cast<void*>(*start);
    global_kernel->syslog()->info("VBE control info at physical address %p\n", phys_addr);
    const uint32_t* virt_addr =
        static_cast<uint32_t*>(pdt.map(phys_addr, VbeCntrl::size));
    cntrl = VbeCntrl {virt_addr, pdt};
    pdt.unmap(virt_addr, VbeCntrl::size);

    // Fetch mode info.
    phys_addr = reinterpret_cast<void*>(*(start + 1));
    global_kernel->syslog()->info("VBE mode info at physical address %p\n", phys_addr);
    virt_addr = static_cast<uint32_t*>(pdt.map(phys_addr, VbeMode::size));
    minfo = VbeMode {virt_addr};
    pdt.unmap(virt_addr, VbeMode::size);

    m = static_cast<uint16_t>(*(start + 2));
    int_seg = static_cast<uint16_t>(*(start + 2) >> 16);
    int_off = static_cast<uint16_t>(*(start + 3));
    int_len = static_cast<uint16_t>(*(start + 3) >> 16);
}

/******************************************************************************/

void MultiBootVbe::dump(klib::ostream& dest) const
{
    if (!valid())
    {
        dest << "MultiBootVbe table is invalid\n";
        return;
    }

    dest << "Dumping MultiBoot VBE information\n";
    dest << "  Current mode: " << klib::hex << m << '\n';
    dest << "  Interface segment: " << int_seg << '\n';
    dest << "  Interface offset: " << int_off << '\n';
    dest << "  Interface length: " << int_len << '\n';
    dest << klib::dec;

    // Dump the control information.
    cntrl.dump(dest);

    // Dump the mode information.
    minfo.dump(dest);
}

/******************************************************************************
 ******************************************************************************/

void MultiBootColor::dump(klib::ostream& dest) const
{
    uint32_t val = (red_value() << 16) + (green_value() << 8) + blue_value();
    dest << "rgb " << klib::hex << val << klib::dec;

    dest.flush();
}

/******************************************************************************
 ******************************************************************************/

MultiBootIndex::MultiBootIndex(const uint8_t* start, PageDescriptorTable& pdt):
    val{true}
{
    // Note: due to the way the MultiBoot information is set out, the start
    // pointer is unlikely to be 4 byte aligned.

    // Number of entries in the palette.
    size_t num = start[4] + (start[5] << 8);

    // Fetch the colours.
    const void* phys_addr = reinterpret_cast<void*>(
        start[0] + (start[1] << 8) + (start[2] << 16) + (start[3] << 24)
    );
    global_kernel->syslog()->info("Indexed colour palette at physical address %p\n", phys_addr);
    const uint8_t* virt_addr =
        static_cast<uint8_t*>(pdt.map(phys_addr, num * MultiBootColor::size));
    if (virt_addr == nullptr)
    {
        global_kernel->panic(
            "No virtual memory for multiboot allocation");
    }
    for (size_t i = 0; i < num; ++i)
        cols.push_back(MultiBootColor {virt_addr + i * MultiBootColor::size});
    // Add an entry at the end for invalid.
    cols.push_back(MultiBootColor {});
    // Clean up memory.
    pdt.unmap(phys_addr, num * MultiBootColor::size);
}

/******************************************************************************/

void MultiBootIndex::dump(klib::ostream& dest) const
{
    dest << "Dumping Colour Index Palette containing " << cols.size() - 1 <<
        "entries:\n";

    for (size_t i = 0; i < cols.size() - 1; ++i)
    {
        dest << "  " << i << ": ";
        cols[i].dump(dest);
        dest << '\n';
    }

    dest.flush();
}

/******************************************************************************
 ******************************************************************************/

MultiBootRgb::MultiBootRgb(const uint8_t* start) : val{true}
{
    for (size_t i = 0; i < size; ++i)
        data[i]= start[i];
}

/******************************************************************************/

void MultiBootRgb::dump(klib::ostream& dest) const
{
    dest << "Dumping RGB colour info:\n" << klib::hex;
    dest << "  Red field position: " << red_field_position() << '\n';
    dest << "  Green field position: " << green_field_position() << '\n';
    dest << "  Blue field position: " << blue_field_position() << '\n';
    dest << "  Red mask size: " << red_mask_size() << '\n';
    dest << "  Green mask size: " << green_mask_size() << '\n';
    dest << "  Blue mask size: " << blue_mask_size() << '\n';

    dest.flush();
}

/******************************************************************************
 ******************************************************************************/

MultiBootFrameBuffer::MultiBootFrameBuffer(const uint32_t* start,
    PageDescriptorTable& pdt) : val{true}, index{}, rgb{}
{
    add = reinterpret_cast<void*>(*start);
    p = *(start + 2);
    w = *(start + 3);
    h = *(start + 4);
    b = static_cast<uint8_t>(*(start + 5));
    uint8_t t = static_cast<uint8_t>(*(start + 5) >> 8);
    switch (t)
    {
    case 0:
        ty = ColorType::indexed;
        index = MultiBootIndex{reinterpret_cast<const uint8_t*>(start) + 22,
            pdt};
        break;
    case 1:
        ty = ColorType::rgb;
        rgb = MultiBootRgb{reinterpret_cast<const uint8_t*>(start) + 22};
        break;
    case 2:
        ty = ColorType::text;
        break;
    default:
        ty = ColorType::invalid;
        val = false;
    }
}

/******************************************************************************/

void MultiBootFrameBuffer::dump(klib::ostream& dest) const
{
    dest << "Dumping FrameBuffer information:\n";

    dest << "  FrameBuffer address: " << add << '\n';
    dest << "  FrameBuffer pitch: " << p << '\n';
    dest << "  FrameBuffer width: " << w << '\n';
    dest << "  FrameBuffer height: " << h << '\n';
    dest << "  Bits per pixel: " << static_cast<size_t>(b) << '\n';

    switch (ty)
    {
    case ColorType::indexed:
        dest << "  Indexed colour\n";
        index.dump(dest);
        break;
    case ColorType::rgb:
        dest << "  RGB colour\n";
        rgb.dump(dest);
        break;
    case ColorType::text:
        dest << "  Text mode\n";
        break;
    default:
        dest << "  Unknown colour type\n";
    }

    dest.flush();
}

/******************************************************************************
 ******************************************************************************/

MultiBootInfo::MultiBootInfo(const void* phys_start) : val{true}
{
    // Create a virtual mapping for the requested address and get the virtual
    // address.
    PageDescriptorTable& pdt = *(global_kernel->get_pdt());
    const uint32_t* const start =
        static_cast<const uint32_t*>(pdt.map(phys_start, mb_size));

    if (start == nullptr)
    {
        global_kernel->syslog()->error(
            "Unable to generate virtual mapping for multiboot infomation\n"
        );
        return;
    }

    fl = *start;
    global_kernel->syslog()->info("Multiboot flags: %X\n", fl);

    if (flags(Flag::memory))
    {
        mlow = *(start + 1);
        mupper = *(start + 2);
        global_kernel->syslog()->info("mlow = %X; mupper = %X\n", mlow, mupper);
    }

    if (flags(Flag::device))
    {
        dev = MultiBootDev{start + 3};
        if (!dev.valid())
        {
            val = false;
            return;
        }
        global_kernel->syslog()->info("Boot device: drive = %X, part1 = %X, part2 = %X, part3 = %X\n", dev.drive(), dev.part1(), dev.part2(), dev.part3());
    }

    if (flags(Flag::cmdline))
    {
        const char* phys_addr = reinterpret_cast<const char*>(*(start + 4));
        global_kernel->syslog()->info("cmdline is at physical address %p\n", phys_addr);
        cmd = getstring(phys_addr, pdt);
        global_kernel->syslog()->info("Command line contents: %s\n", cmd.c_str());
    }


    if (flags(Flag::modules))
    {
        // Number of modules and address of the first module.
        mcount = *(start + 5);
        const uint32_t* phys_addr = reinterpret_cast<const uint32_t*>(
            *(start + 6));

        // We need to create a mapping spanning all the module information
        // records. They start at mfirst and each is 16 bytes long.
        const uint32_t* virt_addr = reinterpret_cast<const uint32_t*>(
            pdt.map(phys_addr, mcount * MultiBootModule::size));
        if (virt_addr == nullptr)
            global_kernel->panic("No virtual memory for multiboot allocation");

        // Now create each record and add it to the list.
        for (size_t i = 0; i < mcount; ++i)
            modules.push_back( MultiBootModule{virt_addr +
                (MultiBootModule::size * i) / sizeof(uint32_t), pdt});

        // Clean up the virtual mapping.
        pdt.unmap(virt_addr, mcount * MultiBootModule::size);

        for (size_t i = 0; i < mcount; ++i)
        {
            if (modules[i].valid())
                global_kernel->syslog()->info("Module %u loaded from %p to %p; module string is: %s.\n",
                    i, modules[i].mod_start(), modules[i].mod_end(), modules[i].mod_string().c_str());
            else
                global_kernel->syslog()->warn("Module %u is invalid\n", i);
        }

        // Add an extra module so we can return an invalid one if necessary.
        modules.push_back(MultiBootModule{});
    }
    else
    {
        // We still need an invalid module at the end of the list.
        modules.push_back(MultiBootModule{});
    }

    if (flags(Flag::aout) && flags(Flag::elf))
    {
        val = false;
        return;
    }

    if (flags(Flag::aout))
    {
        // Get the size (in bytes, I hope) of the symbol table.
        size_t tsize = *(start + 7);
        // Get the size (in bytes, I hope) of the strings table.
        size_t ssize = *(start + 8);
        // Get the address of the start of the table.
        void* phys_addr = reinterpret_cast<void*>(*(start + 9));

        // We can work out the total size from the size of the symbol table plus
        // the size of the strings table plus two more longs for storing the
        // sizes.
        size_t tot_size = ssize + tsize + 2 * sizeof(unsigned long);

        // Create a virtual mapping for the data.
        const uint32_t* const virt_addr =
            static_cast<const uint32_t*>(pdt.map(phys_addr, tot_size));
        if (virt_addr == nullptr)
            global_kernel->panic("No virtual memory for multiboot allocation");

        // Copy the data onto the heap.
        aout = AOutSymTab{start + 7, tsize, ssize};

        // Clean up memory.
        pdt.unmap(virt_addr, tot_size);

        if (aout.valid())
            global_kernel->syslog()->info("a.out symbol table loaded, %u entries, length of strings is %u.\n", aout.size(), aout.strings_length());
        else
            global_kernel->syslog()->warn("a.out symbol table is invalid.\n");
    }
    else
        aout = AOutSymTab{};

    if (flags(Flag::elf))
    {
        // Oddly, these three have 4 bytes in the Multiboot header, but only
        // two in the ELF file header.
        uint32_t num = *(start + 7);
        uint32_t entsize = *(start + 8);
        uint32_t ndx = *(start + 10);
        if (num > klib::numeric_limits<uint16_t>::max() ||
            entsize > klib::numeric_limits<uint16_t>::max() ||
            ndx > klib::numeric_limits<uint16_t>::max())
        {
            global_kernel->syslog()->warn(
                "Multiboot ELF file header values are out of range.\n");
            global_kernel->syslog()->warn("ELF num = %u\n", num);
            global_kernel->syslog()->warn("ELF entsize = %u\n", entsize);
            global_kernel->syslog()->warn("ELF ndx = %u\n", ndx);
            elf = ElfSectionTab {};
        }
        else
        {
            // Make a mapping for the header table.
            const void* phys_addr =
                reinterpret_cast<void*>(*(start + 9));
            uint32_t* virt_addr =
                static_cast<uint32_t*>(pdt.map(phys_addr, num * entsize));
            if (virt_addr == nullptr)
            {
                global_kernel->panic(
                    "No virtual memory for multiboot allocation");
            }
            // Open the location as a file.
            MemoryFileSystem* tmpfs;
            try {
                tmpfs = dynamic_cast<MemoryFileSystem*>(
                    global_kernel->get_vfs()->lookup("/tmp"));
            }
            catch (klib::bad_cast&)
            {
                global_kernel->syslog()->warn("MultiBootInfo() Couldn't access tmpfs\n");
                return;
            }
            klib::string fname {"/kernel_section_table"};
            tmpfs->create_mapping(fname, virt_addr, num * entsize);
            klib::ifstream ifs {"/tmp" + fname};

            elf = ElfSectionTab {ifs, static_cast<uint16_t>(entsize),
                static_cast<uint16_t>(num), static_cast<uint16_t>(ndx)};

            // Clean up memory.
            pdt.unmap(virt_addr, num * entsize);

            // It seems GRUB loads the .shstrtab section physically somewhere
            // after the kernel. Since this section isn't really part of the
            // program, I couldn't get the linker to give me the location, nor
            // even set the end of the kernel to after the section. Therefore
            // we'll just have to copy the names out manually.
            // Make a mapping for this section.
            const ElfSectionHeader& strndx = elf.header(elf.strndx());
            phys_addr = strndx.sh_addr();
            size_t size = strndx.sh_size();
            const char* strndx_addr =
                static_cast<char*>(pdt.map(phys_addr, size));
            if (strndx_addr == nullptr)
            {
                global_kernel->panic(
                    "No virtual memory for multiboot allocation");
            }

            // Sort out any sections with physical addresses.
            elf.remap_sections(pdt);

            // Clean up memory.
            pdt.unmap(strndx_addr, size);
        }

        if (elf.valid())
            elf.dump(*global_kernel->syslog()->stream());
        else
            global_kernel->syslog()->warn("ELF header table is invalid.\n");
    }

    if (flags(Flag::map))
    {
        // Length of the area holding the map.
        uint32_t length = *(start + 11);
        // Start of the map. Although the Multiboot spec says the size is at -4,
        // this address is for the first size.
        const void* phys_addr = reinterpret_cast<void*>(*(start + 12));

        // Create a virtual mapping.
        const uint32_t* virt_addr = 
            static_cast<uint32_t*>(pdt.map(phys_addr, length));
        if (virt_addr == nullptr)
        {
            global_kernel->panic(
                "No virtual memory for multiboot allocation");
        }

        global_kernel->syslog()->info("Reading memory map of length %u from %p\n", length, virt_addr);

        uint32_t offset = 0;
        while (offset < length / sizeof(uint32_t))
        {
            // Size comes first, takes up four bytes.
            map.push_back(MultiBootMap {virt_addr + offset + 1});
            // Size does not include it's own 4 bytes.
            offset += *(virt_addr + offset) / sizeof(uint32_t) + 1;
        }
        // Add an invalid entry at the end.
        map.push_back(MultiBootMap {});

        // Clean up memory.
        pdt.unmap(virt_addr, length);

        // Pass the information to the Page Frame Allocator, so that segments
        // that are not available won't be used.
        pfa_read_map(map);

        // Dump map.
        klib::ostream& out = *global_kernel->syslog()->stream();
        out << "Dumping memory map:\n";

        for (size_t i = 0; i < MultiBootMap::field_number; ++i)
        {
            out.width(MultiBootMap::field_widths[i]);
            out << MultiBootMap::field_headers[i] << ' ';
        }
        out << '\n';

        for (size_t i = 0; i < map.size() - 1; ++i)
        {
            out.width(MultiBootMap::field_widths[0]);
            out << i << ' ';         
            map[i].dump(out);
            out << '\n';
        }
        pfa_dump_status(out);
    }
    else
    {
        // We still need an invalid entry at the end of the list.
        map.push_back(MultiBootMap{});
    }

    if (flags(Flag::drives))
    {
        // Get the size in bytes.
        uint32_t dlength = *(start + 13);
        if (dlength != 0)
        {
            // Physical address of the data.
            const void* phys_addr =
                reinterpret_cast<void*>(*(start + 14));
            // Create a mapping for the physical address.
            const uint32_t* virt_addr = 
                static_cast<uint32_t*>(pdt.map(phys_addr, dlength));

            if (virt_addr == nullptr)
            {
                global_kernel->panic(
                    "No virtual memory for multiboot allocation");
            }

            global_kernel->syslog()->info("Reading drive info of length %u from %p\n", dlength, virt_addr);

            for (uint32_t offset = 0; offset < dlength;
                offset += drvs[drvs.size() - 1].size())
            {
                if (offset % sizeof(uint32_t) != 0)
                {
                    global_kernel->syslog()->warn("Multiboot drives map is not 4 byte aligned\n");
                    break;
                }
                drvs.push_back(MultiBootDrive{virt_addr +
                    offset / sizeof(uint32_t)});
            }

            // Add an invalid entry at the end.
            drvs.push_back(MultiBootDrive{});

            // Clean up memory.
            pdt.unmap(virt_addr, dlength);
        }
    }
    else
    {
        // We still need an invalid drive at the end of the list.
        drvs.push_back(MultiBootDrive{});
    }

    if (flags(Flag::rom))
    {
        // Get the physical address of the start of the data.
        const void* phys_addr =
                reinterpret_cast<void*>(*(start + 15));

        // Create a virtual mapping. We need to do this before reading the size.
        const uint32_t* virt_addr = 
            static_cast<uint32_t*>(pdt.map(phys_addr, sizeof(uint16_t)));
        if (virt_addr == nullptr)
        {
            global_kernel->panic(
                "No virtual memory for multiboot allocation");
        }

        // Read the size. Unmap and remap.
        uint16_t sz = static_cast<uint16_t>(*virt_addr);
        pdt.unmap(virt_addr, sizeof(uint16_t));
        virt_addr = static_cast<uint32_t*>(pdt.map(phys_addr, sz));
        if (virt_addr == nullptr)
        {
            global_kernel->panic(
                "No virtual memory for multiboot allocation");
        }

        // Copy the data.
        rom = MultiBootRom(virt_addr);

        // Clean up memory.
        pdt.unmap(virt_addr, sz);       
    }

    if (flags(Flag::name))
    {
        const char* phys_addr =
            reinterpret_cast<const char*>(*(start + 16));
        global_kernel->syslog()->info("Boot loader name is at physical address %p\n", phys_addr);
        name = getstring(phys_addr, pdt);
        global_kernel->syslog()->info("Boot loader name: %s\n", name.c_str());
    }

    if (flags(Flag::apm))
    {
        // Get the physical address of the start of the data.
        const void* phys_addr =
                reinterpret_cast<void*>(*(start + 17));

        // Create a virtual mapping.
        const uint32_t* virt_addr = 
            static_cast<uint32_t*>(pdt.map(phys_addr, MultiBootApm::size));
        if (virt_addr == nullptr)
        {
            global_kernel->panic(
                "No virtual memory for multiboot allocation");
        }

        // Read the data.
        apm = MultiBootApm {virt_addr};

        // Dump the data.
        apm.dump(*global_kernel->syslog()->stream());

        // Clean up memory.
        pdt.unmap(virt_addr, MultiBootApm::size);
    }

    if (flags(Flag::vbe))
    {
        // Get the data.
        vbe_table = MultiBootVbe {start + 18, pdt};

        // Dump the data.
        vbe_table.dump(*global_kernel->syslog()->stream());
    }

    if (flags(Flag::fb))
    {
        fb = MultiBootFrameBuffer{start + 22, pdt};

        if (fb.valid())
            fb.dump(*(global_kernel->syslog()->stream()));

        else
            global_kernel->syslog()->warn("Invalid frame buffer multiboot information.\n");
    }

    // Free the virtual mapping we created.
    pdt.unmap(start, mb_size);
}

/******************************************************************************/

bool MultiBootInfo::flags(MultiBootInfo::Flag f) const
{
    switch (f)
    {
    case MultiBootInfo::Flag::memory:
        return fl & 0x00000001;
    case MultiBootInfo::Flag::device:
        return fl & 0x00000002;
    case MultiBootInfo::Flag::cmdline:
        return fl & 0x00000004;
    case MultiBootInfo::Flag::modules:
        return fl & 0x00000008;
    case MultiBootInfo::Flag::aout:
        return fl & 0x00000010;
    case MultiBootInfo::Flag::elf:
        return fl & 0x00000020;
    case MultiBootInfo::Flag::map:
        return fl & 0x00000040;
    case MultiBootInfo::Flag::drives:
        return fl & 0x00000080;
    case MultiBootInfo::Flag::rom:
        return fl & 0x00000100;
    case MultiBootInfo::Flag::name:
        return fl & 0x00000200;
    case MultiBootInfo::Flag::apm:
        return fl & 0x00000400;
    case MultiBootInfo::Flag::vbe:
        return fl & 0x00000800;
    case MultiBootInfo::Flag::fb:
        return fl & 0x00001000;
    default:
        return false;
    }
}

/******************************************************************************/

const MultiBootModule& MultiBootInfo::mods(size_t n) const
{
    if (n >= mcount)
        // On set up, we stored an invalid module at the end.
        return modules[mcount];
    else
        return modules[n];
}

/******************************************************************************/

const MultiBootMap& MultiBootInfo::mmap(size_t n) const
{
    if (n >= map.size())
        // On set up, we stored an invalid module at the end.
        return map[map.size() - 1];
    else
        return map[n];
}

/******************************************************************************/

const MultiBootDrive& MultiBootInfo::drive(size_t n) const
{
    if (n >= drvs.size())
        // On set up, we stored an invalid drive at the end.
        return drvs[drvs.size() - 1];
    else
        return drvs[n];
}

/******************************************************************************
 ******************************************************************************/
