#include "Elf.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <istream>
#include <ostream>
#include <string>
#include <typeinfo>

#include "Kernel.h"
#include "Logger.h"
#include "MemoryFileSystem.h"
#include "PageDescriptorTable.h"
#include "paging.h"
#include "util.h"

/******************************************************************************
 ******************************************************************************/

ElfFileHeader::ElfFileHeader(klib::istream& ifs)
{
    ifs.read(reinterpret_cast<klib::istream::char_type*>(data.data()),
        size / sizeof(klib::istream::char_type));
}

/******************************************************************************/

bool ElfFileHeader::check_validity() const
{
    // Check the magic number (the first 4 bytes).
    if (!check_magic())
        return false;

    // Check that it's 32 bit.
    if (get_class() != class_32)
        return false;

    // Check that it's little endian.
    if (get_endian() != little_endian)
        return false;

    // Check that this is ELF version 1.
    if (get_version() != version_1)
        return false;

    // Check that it's targeting UNIX System V.
    if (get_abi() != abi_sysv)
        return false;

    // Check that it's an executable.
    if (get_type() != et_exec)
        return false;

    // Check that the architecture is x86.
    if (get_machine() != machine_x86)
        return false;

    // Check that the second verison number is also 1.
    if (get_version32() != 1)
        return false;

    // Check that the size of the file header is what we expect.
    if (get_ehsize() != 52)
        return false;

    // Check the size of the program headers is what we expect.
    if (get_phentsize() != 32)
        return false;

    // Check that there is at least 1 program header.
    if (get_phnum() == 0)
        return false;

    // Check that the size of the section headers is what we expect.
    if (get_shentsize() != 40)
        return false;

    // We don't need to check the number of section headers or the index of
    // .shstrtab as we don't need those fo loading an executable.
    return true;
}

/******************************************************************************/

void ElfFileHeader::dump(klib::ostream& dest) const
{
    // Field descriptions width
    constexpr size_t desc_width = 37;

    dest << "ELF Header:\n";

    dest.width(desc_width);
    dest << klib::left;
    dest << "  Magic:" << klib::noshowbase << klib::hex;
    for (size_t i = 0; i < 16; ++i)
    {
        uint32_t tmp_val = (data[i / sizeof(uint32_t)] >> ((i % 4) * 8)) & 0xFF;
        dest << tmp_val << (i == 15 ? '\n' : ' ');
    }
    dest << klib::dec << klib::showbase;

    dest.width(desc_width);
    dest << "  Class:";
    switch(get_class())
    {
    case class_32:
        dest << "ELF32\n";
        break;
    case class_64:
        dest << "ELF64\n";
        break;
    default:
        dest << "Unknown\n";
    }

    dest.width(desc_width);
    dest << "  Data:";
    dest << "2's complement, ";
    switch(get_endian())
    {
    case little_endian:
        dest << "little endian\n";
        break;
    case big_endian:
        dest << "big_endian\n";
        break;
    default:
        dest << "Unknown\n";
    }

    dest.width(desc_width);
    dest << "  Version:";
    switch(get_version())
    {
    case version_1:
        dest << "1 (current)\n";
        break;
    default:
        dest << "Unknown\n";
    }

    dest.width(desc_width);
    dest << "  OS/ABI:";
    switch(get_abi())
    {
    case abi_sysv:
        dest << "UNIX - System V\n";
        break;
    case abi_hpux:
        dest << "HP-UX\n";
        break;
    case abi_netbsd:
        dest << "NetBSD\n";
        break;
    case abi_linux:
        dest << "Linux\n";
        break;
    case abi_hurd:
        dest << "GNU Hurd\n";
        break;
    case abi_solaris:
        dest << "Solaris\n";
        break;
    case abi_aix:
        dest << "AIX\n";
        break;
    case abi_irix:
        dest << "IRIX\n";
        break;
    case abi_freebsd:
        dest << "FreeBSD\n";
        break;
    case abi_tru64:
        dest << "Tru64\n";
        break;
    case abi_novell:
        dest << "Novell Modesto\n";
        break;
    case abi_openbsd:
        dest <<"OpenBSD\n";
        break;
    case abi_openvms:
        dest << "OpenVMS\n";
        break;
    case abi_nonstop:
        dest << "NonStop Kernel\n";
        break;
    case abi_aros:
        dest << "Aros\n";
        break;
    case abi_fenix:
        dest << "Fenix OS\n";
        break;
    case abi_cloud:
        dest << "CloudABI\n";
        break;
    default:
        dest << "Unknown\n";
    }

    dest.width(desc_width);
    dest << "  ABI Version:" << static_cast<size_t>(get_abiversion()) << '\n';

    dest.width(desc_width);
    dest << "  Type:";
    switch(get_type())
    {
    case et_none:
        dest << "NONE\n";
        break;
    case et_rel:
        dest << "REL (Relocatable file)\n";
        break;
    case et_exec:
        dest << "EXEC (Executable file)\n";
        break;
    case et_dyn:
        dest << "DYN (Shared object file)\n";
        break;
    case et_core:
        dest << "CORE\n";
        break;
    case et_loos:
        dest << "LOOS\n";
        break;
    case et_hios:
        dest << "HIOS\n";
        break;
    case et_loproc:
        dest << "LOPROC\n";
        break;
    case et_hiproc:
        dest << "HIPROC\n";
        break;
    default:
        dest << "Unknown\n";
    }

    dest.width(desc_width);
    dest << "  Machine:";
    switch(get_machine())
    {
    case machine_unspec:
        dest << "No specific instruction set\n";
        break;
    case machine_sparc:
        dest << "SPARC\n";
        break;
    case machine_x86:
        dest << "x86\n";
        break;
    case machine_mips:
        dest << "MIPS\n";
        break;
    case machine_powerpc:
        dest << "PowerPC\n";
        break;
    case machine_s390:
        dest << "S390\n";
        break;
    case machine_arm:
        dest << "ARM\n";
        break;
    case machine_superh:
        dest << "SuperH\n";
        break;
    case machine_ia64:
        dest << "IA-64\n";
        break;
    case machine_x8664:
        dest << "x86-64\n";
        break;
    case machine_aarch64:
        dest << "AArch64\n";
        break;
    case machine_riscv:
        dest << "RISC-V\n";
        break;
    default:
        dest << "Unknown\n";
    }

    dest.width(desc_width);
    dest << "  Version:" << klib::hex << get_version32() << '\n';

    dest.width(desc_width);
    dest << "  Entry point address:" << get_entry() << '\n';

    dest.width(desc_width);
    dest << "  Start of program headers:" << klib::dec;
    dest << format_bytes(get_phoff()) << '\n';

    dest.width(desc_width);
    dest << "  Start of section headers:" << format_bytes(get_shoff()) << '\n';

    dest.width(desc_width);
    dest << "  Flags:" << klib::hex << get_flags() << '\n';

    dest.width(desc_width);
    dest << "  Size of this header:" << klib::dec;
    dest << format_bytes(get_ehsize()) << '\n';

    dest.width(desc_width);
    dest << "  Size of program headers:";
    dest << format_bytes(get_phentsize()) << '\n';

    dest.width(desc_width);
    dest << "  Number of program headers:" << get_phnum() << '\n';

    dest.width(desc_width);
    dest << "  Size of section headers:";
    dest << format_bytes(get_shentsize()) << '\n';

    dest.width(desc_width);
    dest << "  Number of section headers:" << get_shnum() << '\n';

    dest.width(desc_width);
    dest << "  Section header string table index:" << get_shstrndx() << '\n';

    dest << klib::left;
    dest.flush();
}

/******************************************************************************
 ******************************************************************************/

ElfSectionHeader::ElfSectionHeader(klib::istream& ifs, size_t sz)
{
    ifs.read(reinterpret_cast<klib::istream::char_type*>(data.data()),
        sz / sizeof(klib::istream::char_type));
}

/******************************************************************************/

void ElfSectionHeader::dump(klib::ostream& dest) const
{
    // String representations of the type.
    static const char* types[] = {"NULL", "PROGBITS", "SYMTAB",
        "STRTAB", "RELA", "HASH", "DYNAMIC", "NOTE", "NOBITS", "REL", "SHLIB",
        "DYNSYM", "", "", "INIT_ARRAY", "FINI_ARRAY", "PREINIT_ARRAY", "GROUP",
        "SYMTAB_SHNDX", "NUM"};

    dest.width(type_width);
    dest << klib::left << types[sh_type()] << ' ';

    dest.width(addr_width);
    dest.fill('0');
    dest << klib::internal << sh_addr() << ' ';

    dest.width(offset_width);
    dest << klib::hex << sh_offset() << ' ';

    dest.width(size_width);
    dest << sh_size() << ' ';

    dest.width(ent_width);
    dest << klib::dec << sh_entsize() << ' ';

    dest.width(flag_width);
    dest.fill(' ');
    dest << klib::left << process_flags() << ' ';

    dest.width(link_width);
    dest << sh_link() << ' ';

    dest.width(info_width);
    dest << sh_info() << ' ';

    dest.width(align_width);
    dest << sh_addralign() << ' ';

    dest.flush();
}

/******************************************************************************/

void ElfSectionHeader::set_addr(const void* addr)
{
    uintptr_t vaddr = reinterpret_cast<uintptr_t>(addr);

    klib::get<3>(data) = vaddr;
}

/******************************************************************************/

klib::string ElfSectionHeader::process_flags() const
{
    klib::string s = "";
    if (sh_flags() & SHF_UNDEF)
        s += 'x';
    if (sh_flags() & SHF_WRITE)
        s += 'W';
    if (sh_flags() & SHF_ALLOC)
        s += 'A';
    if (sh_flags() & SHF_EXECINSTR)
        s += 'X';
    if (sh_flags() & SHF_MERGE)
        s += 'M';
    if (sh_flags() & SHF_STRINGS)
        s += 'S';
    if (sh_flags() & SHF_INFO_LINK)
        s += 'I';
    if (sh_flags() & SHF_LINK_ORDER)
        s += 'L';
    if (sh_flags() & SHF_OS_NONCONFORMING)
        s += 'O';
    if (sh_flags() & SHF_GROUP)
        s += 'G';
    if (sh_flags() & SHF_TLS)
        s += 'T';
    if (sh_flags() & SHF_MASKOS)
        s += 'o';
    if (sh_flags() & SHF_MASKPROC)
        s += 'p';
    if (sh_flags() & SHF_ORDERED)
        s += 'R';
    if (sh_flags() & SHF_EXCLUDE)
        s += 'E';
    return s;
}

/******************************************************************************
 ******************************************************************************/

ElfSection::ElfSection(klib::istream& ifs, size_t s) :
    sz {s},
    data {nullptr}
{
    data = new uint32_t[sz / sizeof(uint32_t)];
    ifs.read(reinterpret_cast<klib::istream::char_type*>(data),
        sz / sizeof(klib::istream::char_type));
}

/******************************************************************************/

ElfSection::ElfSection(const ElfSection& other) :
    sz{other.sz},
    data{new uint32_t[sz / sizeof(uint32_t)]}
{
    klib::memcpy(data, other.data, sz);
}

/******************************************************************************/

ElfSection::ElfSection(ElfSection&& other) : sz{other.sz}, data{other.data}
{
    // Set other to invalid (otherwise the destructor will mess up data).
    other.sz = 0;
    other.data = nullptr;
}

/******************************************************************************/

ElfSection& ElfSection::operator=(const ElfSection& other)
{
    delete[] data;

    sz = other.sz;
    data = new uint32_t[sz / sizeof(uint32_t)];
    klib::memcpy(data, other.data, sz);

    return *this;
}

/******************************************************************************/

ElfSection& ElfSection::operator=(ElfSection&& other)
{
    delete[] data;

    sz = other.sz;
    data = other.data;

    // Set other to invalid (otherwise the destructor will mess up data).
    other.sz = 0;
    other.data = nullptr;

    return *this;
}

/******************************************************************************/

ElfSection::~ElfSection()
{
    delete[] data;
}

/******************************************************************************
 ******************************************************************************/

ElfSectionTab::ElfSectionTab(klib::istream& ifs, uint16_t entsize,
    uint16_t num, uint16_t ndx, const klib::string& name, bool k) :
    val{true},
    inv_sh{},
    e_shentsize{entsize},
    e_shnum{num},
    e_shstrndx{ndx},
    file_name{name},
    kernel{k}
{
    // Sanity check. We're assuming only 32 bit architecture for now.
    if (e_shentsize != ElfSectionHeader::size)
    {
        set_invalid();
        return;
    }

    // Create space for the table.
    sh_start = new ElfSectionHeader[e_shnum];
    // Copy the data.
    for (size_t i = 0; i < e_shnum; ++i)
    {
        sh_start[i] = ElfSectionHeader {ifs, e_shentsize};
        if (!ifs)
        {
            val = false;
            return;
        }
    }
}

/******************************************************************************/

ElfSectionTab::ElfSectionTab(const ElfSectionTab& other) :
    val{other.val},
    inv_sh{},
    e_shentsize{other.e_shentsize},
    e_shnum{other.e_shnum},
    e_shstrndx{other.e_shstrndx},
    file_name{other.file_name},
    kernel{other.kernel}
{
    sh_start = new ElfSectionHeader[e_shnum];
    for (size_t i = 0; i < e_shnum; ++i)
        sh_start[i] = other.sh_start[i];
}

/******************************************************************************/

ElfSectionTab::ElfSectionTab(ElfSectionTab&& other) :
    val{other.val},
    inv_sh{},
    e_shentsize{other.e_shentsize},
    e_shnum{other.e_shnum},
    e_shstrndx{other.e_shstrndx},
    sh_start{other.sh_start},
    file_name{other.file_name},
    kernel{other.kernel}
{
    // Set other to invalid (otherwise the destructor will mess up sh_start).
    other.sh_start = nullptr;
    other.set_invalid();
}

/******************************************************************************/

ElfSectionTab& ElfSectionTab::operator=(const ElfSectionTab& other)
{
    delete[] sh_start;

    val = other.val;
    e_shentsize = other.e_shentsize;
    e_shnum = other.e_shnum;
    e_shstrndx = other.e_shstrndx;
    kernel = other.kernel;
    file_name = other.file_name;

    sh_start = new ElfSectionHeader[e_shnum];
    for (size_t i = 0; i < e_shnum; ++i)
        sh_start[i] = other.sh_start[i];

    return *this;
}

/******************************************************************************/

ElfSectionTab& ElfSectionTab::operator=(ElfSectionTab&& other)
{
    delete[] sh_start;

    val = other.val;
    e_shentsize = other.e_shentsize;
    e_shnum = other.e_shnum;
    e_shstrndx = other.e_shstrndx;
    sh_start = other.sh_start;
    kernel = other.kernel;
    file_name = other.file_name;

    // Set other to invalid (otherwise the destructor will mess up sh_start).
    other.sh_start = nullptr;
    other.set_invalid();

    return *this;
}

/******************************************************************************/

ElfSectionTab::~ElfSectionTab()
{
    delete[] sh_start;
}

/******************************************************************************/

void ElfSectionTab::remap_sections(PageDescriptorTable& pdt)
{
    // This function is only for the kernel section headers table.
    if (!kernel)
        return;

    // Since pointer < is undefined unless the pointers are within the same
    // object/array, we strictly must convert to ints first.
    uintptr_t kps =
        reinterpret_cast<uintptr_t>(global_kernel->get_physical_start());
    uintptr_t kpe =
        reinterpret_cast<uintptr_t>(global_kernel->get_physical_end());
    uintptr_t kvs =
        reinterpret_cast<uintptr_t>(global_kernel->get_virtual_start());
    uintptr_t kve =
        reinterpret_cast<uintptr_t>(global_kernel->get_virtual_end());

    for (size_t i = 0; i < e_shnum; ++i)
    {
        // Skip if this is an invalid section, The address would be junk.
        if (sh_start[i].sh_type() == ElfSectionHeader::SHT_NULL)
            continue;
        // No point doing anything if the size is 0.
        size_t sz = sh_start[i].sh_size();
        if (sz == 0)
            continue;
        // Check whether the address is outside the kernel.
        uintptr_t addr = reinterpret_cast<uintptr_t>(sh_start[i].sh_addr());
        if (addr < kvs || addr >= kve)
        {
            // The first (simpler) option is if this section is just an alias
            // for another. In that case, the address will lie within the
            // physical kernel space. This seems to happen for .symtab and
            // .strtab.
            if (kps <= addr && addr < kpe)
            {
                // Just add the kernel virtual base.
                addr += kernel_virtual_base;
                sh_start[i].set_addr(reinterpret_cast<void*>(addr));
            }
            // The second option is if this section has been loaded somewhere
            // else by GRUB. That could be anywhere.
            else
            {
                // Create a virtual memory mapping for the section.
                uint32_t* virt_addr =
                    static_cast<uint32_t*>(pdt.map(sh_start[i].sh_addr(), sz));
                if (virt_addr == nullptr)
                {
                    global_kernel->panic(
                        "No virtual memory for multiboot allocation");
                }
                // Open the section as a file.
                MemoryFileSystem* tmpfs;
                try {
                    tmpfs = dynamic_cast<MemoryFileSystem*>(
                        global_kernel->get_vfs()->lookup("/tmp"));
                }
                catch (klib::bad_cast&)
                {
                    global_kernel->syslog()->warn("ElfSectionTab::remap_sections() Couldn't access tmpfs\n");
                    return;
                }
                klib::string fname {"/remap_section"};
                tmpfs->create_mapping(fname, virt_addr, sz);
                klib::ifstream ifs {};
                ifs.rdbuf()->pubsetbuf(0, 0);
                ifs.open("/tmp" + fname);
                // Make a copy of the data.
                sections.push_back(ElfSection {ifs, sz});
                // Set the address in the section header.
                sh_start[i].set_addr(sections[sections.size() - 1].get_data());
                // Clean up.
                pdt.unmap(virt_addr, sz);
                ifs.close();
                tmpfs->delete_mapping(fname);
            }
        }
    }
}

/******************************************************************************/

void ElfSectionTab::dump(klib::ostream& dest) const
{
    dest << "ELF Section Headers:\n";
    if (!valid())
    {
        dest << "Header table is invalid.\n";
        return;
    }

    // Longest section index (the header uses 2).
    klib::string tmp {klib::to_string(e_shnum - 1)};
    size_t max_digits = klib::max(tmp.size(), static_cast<size_t>(2));

    dest << "  [" << klib::right;
    dest.width(max_digits);
    dest << "Nr] ";

    dest << klib::left;
    dest.width(ElfSectionHeader::name_width);
    dest << "Name" << ' ';

    dest.width(ElfSectionHeader::type_width);
    dest << "Type" << ' ';

    dest.width(ElfSectionHeader::addr_width);
    dest << "Addr" << ' ';

    dest.width(ElfSectionHeader::offset_width);
    dest << "Off" << ' ';

    dest.width(ElfSectionHeader::size_width);
    dest << "Size" << ' ';

    dest.width(ElfSectionHeader::ent_width);
    dest << "ES" << ' ';

    dest.width(ElfSectionHeader::flag_width);
    dest << "Flg" << ' ';

    dest.width(ElfSectionHeader::link_width);
    dest << "Lk" << ' ';

    dest.width(ElfSectionHeader::info_width);
    dest << "Inf" << ' ';

    dest.width(ElfSectionHeader::align_width);
    dest << "Al" << ' ';

    dest << '\n';

    for (size_t i = 0; i < e_shnum; ++i)
    {
        dest << "  [" << klib::right;
        dest.width(max_digits);
        dest << i << "] ";
        dest.width(ElfSectionHeader::name_width);
        dest << klib::left << get_name(i) << ' ';
        sh_start[i].dump(dest);
        dest << '\n';
    }

    dest.flush();
}

/******************************************************************************/

klib::string ElfSectionTab::get_name(size_t n) const
{
    if (n >= e_shnum)
        return "";

    if(sh_start[n].sh_type() == ElfSectionHeader::SHT_NULL)
        return "NULL";

    // If this is the kernel, we should have set the section address of shstrtab
    // to where we copied it from the Multiboot information. If this is a user
    // mode process, the address of shstrtab is probably zero, but we can use
    // the base address and the file offset instead.
    if (kernel)
        return static_cast<const char*>(sh_start[e_shstrndx].sh_addr()) +
            sh_start[n].sh_name();
    else
    {
        klib::ifstream ifs {file_name};
        ifs.seekg(sh_start[n].sh_name() + sh_start[e_shstrndx].sh_offset());
        klib::string tmp;
        // operator>> into a string stops at a 'space' character (as determined
        // by klib::isspace()). Entries in the shstrtab are separated by '\0',
        // which is not a 'space'. Use getline() with '\0' as the delimiter.
        klib::getline(ifs, tmp, '\0');
        return tmp;
    }
}

/******************************************************************************/

const ElfSectionHeader& ElfSectionTab::header(size_t n) const
{
    if (n >= e_shnum)
        return inv_sh;

    return sh_start[n];
}

/******************************************************************************/

void ElfSectionTab::set_invalid()
{
    val = false;
    e_shentsize = 0;
    e_shnum = 0;
    e_shstrndx =0;
    delete[] sh_start;
    sh_start = nullptr;
    file_name = "";
}

/******************************************************************************
 ******************************************************************************/

ElfProgramHeader::ElfProgramHeader(klib::istream& ifs, size_t sz)
{
    ifs.read(reinterpret_cast<klib::istream::char_type*>(data.data()),
        sz / sizeof(klib::istream::char_type));
}

/******************************************************************************/

void ElfProgramHeader::dump(klib::ostream& dest) const
{
    // String representations of the type.
    static const klib::map<pt_type, klib::string> types {
        klib::pair<pt_type, klib::string> {pt_null, "NULL"},
        klib::pair<pt_type, klib::string> {pt_load, "LOAD"},
        klib::pair<pt_type, klib::string> {pt_dynamic, "DYNAMIC"},
        klib::pair<pt_type, klib::string> {pt_interp, "INTERP"},
        klib::pair<pt_type, klib::string> {pt_note, "NOTE"},
        klib::pair<pt_type, klib::string> {pt_shlib, "SHLIB"},
        klib::pair<pt_type, klib::string> {pt_phdr, "PHDR"},
        klib::pair<pt_type, klib::string> {pt_loos, "LOOS"},
        klib::pair<pt_type, klib::string> {pt_hios, "HIOS"},
        klib::pair<pt_type, klib::string> {pt_loproc, "LOPROC"},
        klib::pair<pt_type, klib::string> {pt_hiproc, "HIPROC"}
    };

    dest << "  " << klib::left;
    dest.width(type_width);
    dest << types.find(get_type())->second << ' ';

    dest << klib::hex << klib::internal;
    dest.fill('0');
    dest.width(offset_width);
    dest << get_offset() << ' ';

    dest.width(vaddr_width);
    dest << get_vaddr() << ' ';

    dest.width(paddr_width);
    dest << get_paddr() << ' ';

    dest.width(filesz_width);
    dest << get_filesz() << ' ';

    dest.width(memsz_width);
    dest << get_memsz() << ' ';

    dest << (get_flags() & static_cast<uint32_t>(pt_read) ? 'R' : ' ');
    dest << (get_flags() & static_cast<uint32_t>(pt_write) ? 'W' : ' ');
    dest << (get_flags() & static_cast<uint32_t>(pt_exec) ? 'E' : ' ');
    dest << ' ';
    dest << get_align();

    dest << klib::hex << klib::left;
    dest.fill(' ');
    dest.flush();
}

/******************************************************************************
 ******************************************************************************/

ElfProgramTab::ElfProgramTab(klib::istream& ifs, uint16_t entsize,
    uint16_t num) :
    val{true},
    inv_ph{},
    e_phentsize{entsize},
    e_phnum{num}
{
    // Sanity check. We're assuming only 32 bit architecture for now.
    if (e_phentsize != ElfProgramHeader::size)
    {
        set_invalid();
        return;
    }

    // Create space for the table.
    ph_start = new ElfProgramHeader[e_phnum];
    // Copy the data.
    for (size_t i = 0; i < e_phnum; ++i)
    {
        ph_start[i] = ElfProgramHeader {ifs, e_phentsize};
    }

    // Check that there is at least one loadable section.
    bool loadable = false;
    for (size_t i = 0; i < e_phnum; ++i)
    {
        if (ph_start[i].get_type() == ElfProgramHeader::pt_load)
        {
            loadable = true;
            break;
        }
    }
    if (!loadable)
    {
        set_invalid();
        return;
    }

    // Check that the segments are all correctly aligned.
    for (size_t i = 0; i < e_phnum; ++i)
    {
        if ((ph_start[i].get_align() >> 1) &
            reinterpret_cast<uint32_t>(ph_start[i].get_vaddr()))
        {
            set_invalid();
            return;
        }
    }
}

/******************************************************************************/

ElfProgramTab::ElfProgramTab(const ElfProgramTab& other) :
    val{other.val},
    inv_ph{},
    e_phentsize{other.e_phentsize},
    e_phnum{other.e_phnum}
{
    ph_start = new ElfProgramHeader[e_phnum];
    for (size_t i = 0; i < e_phnum; ++i)
        ph_start[i] = other.ph_start[i];
}

/******************************************************************************/

ElfProgramTab::ElfProgramTab(ElfProgramTab&& other) :
    val{other.val},
    inv_ph{},
    e_phentsize{other.e_phentsize},
    e_phnum{other.e_phnum},
    ph_start{other.ph_start}
{
    // Set other to invalid (otherwise the destructor will mess up ph_start).
    other.ph_start = nullptr;
    other.set_invalid();
}

/******************************************************************************/

ElfProgramTab& ElfProgramTab::operator=(const ElfProgramTab& other)
{
    delete[] ph_start;

    val = other.val;
    e_phentsize = other.e_phentsize;
    e_phnum = other.e_phnum;

    ph_start = new ElfProgramHeader[e_phnum];
    for (size_t i = 0; i < e_phnum; ++i)
        ph_start[i] = other.ph_start[i];

    return *this;
}

/******************************************************************************/

ElfProgramTab& ElfProgramTab::operator=(ElfProgramTab&& other)
{
    delete[] ph_start;

    val = other.val;
    e_phentsize = other.e_phentsize;
    e_phnum = other.e_phnum;
    ph_start = other.ph_start;

    // Set other to invalid (otherwise the destructor will mess up ph_start).
    other.ph_start = nullptr;
    other.set_invalid();

    return *this;
}

/******************************************************************************/

ElfProgramTab::~ElfProgramTab()
{
    delete[] ph_start;
}

/******************************************************************************/

void ElfProgramTab::dump(klib::ostream& dest) const
{
    dest << "ELF Program Headers:\n";
    if (!valid())
    {
        dest << "Header table is invalid.\n";
        return;
    }

    klib::string output {"  Type"};
    size_t tot_length = ElfProgramHeader::type_width + 2;
    for (size_t i = tot_length - output.size(); i > 0; --i)
        output += ' ';

    dest << "  " << klib::left;
    dest.width(ElfProgramHeader::type_width);
    dest << "Type" << ' ';

    dest.width(ElfProgramHeader::offset_width);
    dest << "Offset" << ' ';

    dest.width(ElfProgramHeader::vaddr_width);
    dest << "VirtAddr" << ' ';

    dest.width(ElfProgramHeader::paddr_width);
    dest << "PhysAddr" << ' ';

    dest.width(ElfProgramHeader::filesz_width);
    dest << "FileSize" << ' ';

    dest.width(ElfProgramHeader::memsz_width);
    dest << "MemSize" << ' ';

    dest.width(ElfProgramHeader::flag_width);
    dest << "Flg" << ' ';

    dest.width(ElfProgramHeader::align_width);
    dest << "Align" << ' ';

    dest << '\n';

    for (size_t i = 0; i < e_phnum; ++i)
    {
        ph_start[i].dump(dest);
        dest << '\n';
    }

    dest.flush();
}

/******************************************************************************/

const ElfProgramHeader& ElfProgramTab::header(size_t n) const
{
    if (n >= e_phnum)
        return inv_ph;

    return ph_start[n];
}

/******************************************************************************/

void ElfProgramTab::set_invalid()
{
    val = false;
    e_phentsize = 0;
    e_phnum = 0;
    delete[] ph_start;
    ph_start = nullptr;
}

/******************************************************************************
 ******************************************************************************/

Elf::Elf(const klib::string& name) :
    val {true},
    file_name {name},
    file_hdr {nullptr},
    section_hdrs {nullptr},
    program_hdrs {nullptr}
{
    // Create a stream to read the file.
    klib::ifstream ifs {name};
    if (!ifs)
    {
        val = false;
        return;
    }

    // File header.
    file_hdr = new ElfFileHeader {ifs};
    if (!file_hdr->check_validity())
    {
        global_kernel->syslog()->warn("Invalid ELF program header.\n");
        val = false;
        return;
    }
    if (!ifs)
    {
        val = false;
        return;
    }

    // Section headers.
    ifs.seekg(file_hdr->get_shoff(), klib::ios_base::beg);
    section_hdrs = new ElfSectionTab {
        ifs,
        file_hdr->get_shentsize(),
        file_hdr->get_shnum(),
        file_hdr->get_shstrndx(),
        name,
        false};
    if (!section_hdrs->valid())
    {
        global_kernel->syslog()->warn("Invalid ELF section headers.\n");
        val = false;
        return;
    }
    if (!ifs)
    {
        global_kernel->syslog()->warn("Invalid ELF section headers.\n");
        val = false;
        return;
    }

    // Program headers.
    ifs.seekg(file_hdr->get_phoff(), klib::ios_base::beg);
    program_hdrs = new ElfProgramTab {
        ifs,
        file_hdr->get_phentsize(),
        file_hdr->get_phnum()};
    if (!program_hdrs->valid())
    {
        global_kernel->syslog()->warn("Invalid ELF program headers.\n");
        val = false;
        return;
    }
    if (!ifs)
    {
        global_kernel->syslog()->warn("Invalid ELF program headers.\n");
        val = false;
        return;
    }
}

/******************************************************************************/

Elf::Elf(const Elf& other) : val{other.val}, file_name{other.file_name}
{
    file_hdr = new ElfFileHeader {*other.file_hdr};
    section_hdrs = new ElfSectionTab {*other.section_hdrs};
    program_hdrs = new ElfProgramTab {*other.program_hdrs};
}

/******************************************************************************/

Elf::Elf(Elf&& other) :
    val{other.val},
    file_name {other.file_name},
    file_hdr{other.file_hdr},
    section_hdrs{other.section_hdrs},
    program_hdrs{other.program_hdrs}
{
    other.file_hdr = nullptr;
    other.section_hdrs = nullptr;
    other.program_hdrs = nullptr;
}

/******************************************************************************/

Elf& Elf::operator=(const Elf& other)
{
    val = other.val;
    file_name = other.file_name;

    delete file_hdr;
    delete section_hdrs;
    delete program_hdrs;

    file_hdr = new ElfFileHeader {*other.file_hdr};
    section_hdrs = new ElfSectionTab {*other.section_hdrs};
    program_hdrs = new ElfProgramTab {*other.program_hdrs};

    return *this;
}

/******************************************************************************/

Elf& Elf::operator=(Elf&& other)
{
    val = other.val;
    file_name = other.file_name;

    delete file_hdr;
    delete section_hdrs;
    delete program_hdrs;

    file_hdr = other.file_hdr;
    section_hdrs = other.section_hdrs;
    program_hdrs = other.program_hdrs;

    other.file_hdr = nullptr;
    other.section_hdrs = nullptr;
    other.program_hdrs = nullptr;

    return *this;
}

/******************************************************************************/

Elf::~Elf()
{
    delete file_hdr;
    delete section_hdrs;
    delete program_hdrs;
}

/******************************************************************************/

void Elf::allocate(PageDescriptorTable& pdt) const
{
    // Do nothing if the ELF is not valid.
    if (!valid())
        return;

    // The page configuration must be set to present and user mode. It does need
    // to be writable to copy the data to it. We might consider setting .text
    // and .rodata to unwritable later.
    uint32_t conf = static_cast<uint32_t>(PdeSettings::present) | 
        static_cast<uint32_t>(PdeSettings::writable) |
        static_cast<uint32_t>(PdeSettings::user_access);

    // Cycle through the segments.
    for (size_t i = 0; i < file_hdr->get_phnum(); ++i)
    {
        const ElfProgramHeader& p_hdr = (*program_hdrs)[i];

        // We only load segments of type LOAD.
        if (p_hdr.get_type() != ElfProgramHeader::pt_load)
            continue;

        // Get the start address. Round the start down to a page boundary.
        uintptr_t start = reinterpret_cast<uintptr_t>(p_hdr.get_vaddr());
        start -= start % PageDescriptorTable::page_size;

        // Allocate the pages.
        for (uintptr_t i = start; i < start + p_hdr.get_memsz();
            i += PageDescriptorTable::page_size)
        {
            if (!pdt.allocate(reinterpret_cast<void*>(i), conf))
            {
                global_kernel->panic(
                    "Failed to allocate virtual memory for new process.\n");
                return;
            }
        }
    }
}

/******************************************************************************/

void Elf::deallocate(PageDescriptorTable& pdt) const
{
    // Do nothing if the ELF is not valid.
    if (!valid())
        return;

    // Cycle through the segments.
    for (size_t i = 0; i < file_hdr->get_phnum(); ++i)
    {
        const ElfProgramHeader& p_hdr = (*program_hdrs)[i];

        // We only deallocate segments of type LOAD.
        if (p_hdr.get_type() != ElfProgramHeader::pt_load)
            continue;

        // Get the start address. Round the start down to a page boundary.
        uintptr_t start = reinterpret_cast<uintptr_t>(p_hdr.get_vaddr());
        start -= start % PageDescriptorTable::page_size;

        // Dellocate the pages and physical memory.
        for (uintptr_t i = start; i < start + p_hdr.get_memsz();
            i += PageDescriptorTable::page_size)
            pdt.free(reinterpret_cast<void*>(i), true);
    }
}

/******************************************************************************/

void Elf::dump(klib::ostream& dest) const
{
    file_hdr->dump(dest);
    section_hdrs->dump(dest);
    program_hdrs->dump(dest);
}

/******************************************************************************/

void Elf::load() const
{
    // Do nothing if the ELF is invalid.
    if (!valid())
        return;

    // Open the binary for reading.
    klib::ifstream ifs {file_name};
    if (!ifs)
        return;

    // We are assuming that the virtual mappings are already in place.
    // Cycle through the segments.
    for (size_t i = 0; i < file_hdr->get_phnum(); ++i)
    {
        const ElfProgramHeader& p_hdr = (*program_hdrs)[i];

        // We only load segments of type LOAD.
        if (p_hdr.get_type() != ElfProgramHeader::pt_load)
            continue;

        // First we need to clear bytes up to memsz.
        for (uintptr_t i = 0; i < p_hdr.get_memsz() / sizeof(uint32_t); ++i)
            *(p_hdr.get_vaddr() + i) = 0;

        // Now copy any data from the file.
        ifs.seekg(p_hdr.get_offset(), klib::ios_base::beg);
        ifs.read(reinterpret_cast<klib::istream::char_type*>(p_hdr.get_vaddr()),
            p_hdr.get_filesz() / sizeof(klib::istream::char_type));
    }
}

/******************************************************************************
 ******************************************************************************/
