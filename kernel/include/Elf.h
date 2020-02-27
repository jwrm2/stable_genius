#ifndef ELF_H
#define ELF_H

#include <stddef.h>
#include <stdint.h>

#include <array>
#include <istream>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>

// Forward declarations
class PageDescriptorTable;

/**
    An ELF file header.
 */
class ElfFileHeader {
public:
    /**
        Size the header would have if it was definitely packed. The data should
        look like:
        uint32_t[4] e_ident;
        uint16_t e_type;
        uint16_t e_machine;
        uint32_t e_version;
        uint32_t e_entry;
        uint32_t e_phoff;
        uint32_t e_shoff;
        uint32_t e_flags;
        uint16_t e_ehsize;
        uint16_t e_phentsize;
        uint16_t e_phnum;
        uint16_t e_shentsize;
        uint16_t e_shnum;
        uint16_t e_shstrndx;
     */
    static constexpr size_t size = 52;

    /**
        Constructor. Just copy the data from source into this array.

        @param ifs Input stream to get data from. Must already be positioned at
               the start of the header.
     */
    explicit ElfFileHeader(klib::istream& ifs);

    /**
        Default constuctor, Make a blank entry, which will be considered invalid
        as the magic number won't match.
     */
    ElfFileHeader() : data{} {}

    /**
        Possible values for the class byte.
     */
    enum ei_class : uint8_t {
        /** 32 bit */
        class_32 = 1,
        /** 64 bit */
        class_64 = 2
    };

    /**
        Possible values for the endian byte.
     */
    enum ei_data : uint8_t {
        /** Little endian */
        little_endian = 1,
        /** Big endian */
        big_endian = 2
    };

    /**
        Possible values for the version byte.
     */
    enum ei_version : uint8_t {
        /** Version 1 */
        version_1 = 1
    };

    /**
        Possible values for the OS ABI byte.
     */
    enum ei_osabi : uint8_t {
        /** UNIX System V */
        abi_sysv = 0x00,
        /** HP-UX */
        abi_hpux = 0x01,
        /** NetBSD */
        abi_netbsd = 0x02,
        /** Linux */
        abi_linux = 0x03,
        /** GNU Hurd */
        abi_hurd = 0x04,
        /** Solaris */
        abi_solaris = 0x06,
        /** AIX */
        abi_aix = 0x07,
        /** IRIX */
        abi_irix = 0x08,
        /** FreeBSD */
        abi_freebsd = 0x09,
        /** Tru64 */
        abi_tru64 = 0x0A,
        /** Novell Modesto */
        abi_novell = 0x0B,
        /** OpenBSD */
        abi_openbsd = 0x0C,
        /** OpenVMS */
        abi_openvms = 0x0D,
        /** NonStop Kernel */
        abi_nonstop = 0x0E,
        /** AROS */
        abi_aros = 0x0F,
        /** Fenix OS */
        abi_fenix = 0x10,
        /** CloudABI */
        abi_cloud = 0x11
    };

    /**
        Possible values of the file type.
     */
    enum ei_type : uint16_t {
        /** None */
        et_none = 0x0,
        /** Relocatable */
        et_rel = 0x1,
        /** Executable */
        et_exec = 0x2,
        /** Sharod object library */
        et_dyn = 0x3,
        /** ??? */
        et_core = 0x4,
        /** ??? */
        et_loos = 0xfe00,
        /** ??? */
        et_hios = 0xfeff,
        /** ??? */
        et_loproc = 0xff00,
        /** ??? */
        et_hiproc = 0xffff
    };

    enum ei_machine : uint16_t {
        /** Unspecified */
        machine_unspec = 0x0,
        /** SPARC */
        machine_sparc = 0x2,
        /** x86 */
        machine_x86 = 0x3,
        /** MIPS */
        machine_mips = 0x8,
        /** PowerPC */
        machine_powerpc = 0x14,
        /** S390 */
        machine_s390 = 0x16,
        /** ARM */
        machine_arm = 0x28,
        /** SuperH */
        machine_superh = 0x2a,
        /** IA-64 */
        machine_ia64 = 0x32,
        /** x68-64 */
        machine_x8664 = 0x3e,
        /** AArch64 */
        machine_aarch64 = 0xb7,
        /** RISC-V */
        machine_riscv = 0xF3
    };

    /**
        Checks whether the magic number indicates a valid ELF file.

        @param Whether the magic number matches.
     */
    bool check_magic() const { return (klib::get<0>(data) == magic); }

    /**
        Returns the class, whether this is 32 bit or 64 bit.

        @return class_32 for 32 bit, class_64 for 64 bit.
     */
    ei_class get_class() const
    {
        return static_cast<ei_class>(klib::get<1>(data));
    }

    /**
        Returns the endianness, big or little.

        @return little_endian or big_endian.
     */
    ei_data get_endian() const
    {
        return static_cast<ei_data>(klib::get<1>(data) >> 8);
    }

    /**
        Returns the ELF version.

        @return version_1, nothing else is defined.
     */
    ei_version get_version() const
    {
        return static_cast<ei_version>(klib::get<1>(data) >> 16); 
    }

    /**
        Returns the target ABI.

        @return A recognised abi.
     */
    ei_osabi get_abi() const
    {
        return static_cast<ei_osabi>(klib::get<1>(data) >> 24);
    }

    /**
        Returns ABI version. Interpretation is dependent on the ABI.

        @return ABI version.
     */
    uint8_t get_abiversion() const
    {
        return static_cast<uint8_t>(klib::get<2>(data));
    }

    /**
        Gets the type of file (executable, dynamic library etc.)

        @return File type.
     */
    ei_type get_type() const
    {
        return static_cast<ei_type>(klib::get<4>(data));
    }

    /**
        Gets the target instruction set architecture.

        @return Architecture type.
     */
    ei_machine get_machine() const
    {
        return static_cast<ei_machine>(klib::get<4>(data) >> 16);
    }

    /**
        The version again?

        @return Version number
     */
    uint32_t get_version32() const
    {
        return klib::get<5>(data);
    }

    /**
        The virtual address of the memory entry point.

        @return Entry point address.
     */
    uintptr_t get_entry() const
    {
        return klib::get<6>(data);
    }

    /**
        The offset of the start of the program headers from the start of the
        file.

        @return Program header offset.
     */
    uintptr_t get_phoff() const
    {
        return klib::get<7>(data);
    }

    /**
        The offset of the start of the section headers from the start of the
        file.

        @return Section header offset.
     */
    uintptr_t get_shoff() const
    {
        return klib::get<8>(data);
    }

    /**
        The flags. Interpretation depends on the target architecture.

        @return Architecture flags.
     */
    uint32_t get_flags() const
    {
        return klib::get<9>(data);
    }

    /**
        The size of the file header, We've already assumed it's 52 bytes...

        @return Size of this header.
     */
    uint16_t get_ehsize() const
    {
        return static_cast<uint16_t>(klib::get<10>(data));
    }

    /**
        The size of a program header, 32 bytes for 32 bit architecture.

        @return Size of a program header.
     */
    uint16_t get_phentsize() const
    {
        return static_cast<uint16_t>(klib::get<10>(data) >> 16);
    }

    /**
        The number of program headers.

        @return Number of program headers.
     */
    uint16_t get_phnum() const
    {
        return static_cast<uint16_t>(klib::get<11>(data));
    }

    /**
        The size of a section header, 40 bytes for 32 bit architecture.

        @return Size of a program header.
     */
    uint16_t get_shentsize() const
    {
        return static_cast<uint16_t>(klib::get<11>(data) >> 16);
    }

    /**
        The number of section headers.

        @return Number of section headers.
     */
    uint16_t get_shnum() const
    {
        return static_cast<uint16_t>(klib::get<12>(data));
    }

    /**
        The index of the section header with the section names.

        @return Index of .shstrtab.
     */
    uint16_t get_shstrndx() const
    {
        return static_cast<uint16_t>(klib::get<12>(data) >> 16);
    }

    /**
        Checks various fields for whether the ELF is suitable for loading by
        this kernel.

        @return. Whether the kernel can load the file.
     */
    bool check_validity() const;

    /**
        Prints a summary of the header to the provided Stream.

        @dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

private:
    // Actual data.
    klib::array<uint32_t, size / sizeof(uint32_t)> data;
    // Magic number.
    static constexpr uint32_t magic = 0x464C457F;
};   

/**
    A section header in an ELF file.
 */
class ElfSectionHeader {
public:
    /**
        Size the header would have if it was definitely packed. The data should
        look like:
        int32_t sh_name;
        uint32_t sh_type;
        uint32_t sh_flags;
        uintptr_t sh_addr;
        int32_t sh_offset;
        uint32_t sh_size;
        uint32_t sh_link;
        uint32_t sh_info;
        uint32_t sh_addralign;
        uint32_t sh_entsize;
     */
    static constexpr size_t size = 2 * sizeof(int32_t) + 7 * sizeof(uint32_t) +
        sizeof(uintptr_t);

    /**
        Some types of header.
     */
    enum sh_types : uint32_t {
        /** Blank/invalid entry. The first entry must always be this. */
        SHT_NULL = 0x0,
        /** Program data. */
        SHT_PROGBITS = 0x1,
        /** Symbol table. */
        SHT_SYMTAB = 0x2,
        /** String table. */
        SHT_STRTAB = 0x3,
        /** Relocation entries with addends. */
        SHT_RELA = 0x4,
        /** Symbol hash table. */
        SHT_HASH = 0x5,
        /** Dynamic linking information. */
        SHT_DYNAMIC = 0x6,
        /** Notes. */
        SHT_NOTE = 0x7,
        /** Program space with no data (bss). */
        SHT_NOBITS = 0x8,
        /** Relocation entries without addends. */
        SHT_REL = 0x9,
        /** Reserved. */
        SHT_SHLIB = 0xA,
        /** Dynamic linker symbol table. */
        SHT_DYNSYM = 0xB,
        /** Array of contructors. */
        SHT_INIT_ARRAY = 0xE,
        /** Array of destructors. */
        SHT_FINI_ARRAY = 0xF,
        /** Array of pre-constructors. */
        SHT_PREINIT_ARRAY = 0x10,
        /** Section group */
        SHT_GROUP = 0x11,
        /** Extended section indices. */
        SHT_SYMTAB_SHNDX = 0x12,
        /** Number of defined types. */
        SHT_NUM = 0x13,
        /** Start OS-specific. */
        SHT_LOOS = 0x60000000
    };

    /**
        Meanings of flags in the flags value.
     */
    enum flags : uint32_t {
        /** Writeable. */
        SHF_WRITE = 0x1,
        /** Occupies memory during execution. */
        SHF_ALLOC = 0x2,
        /** Executable. */
        SHF_EXECINSTR = 0x4,
        /** Might be merged. */
        SHF_MERGE = 0x10,
        /** Contains null terminates strings. */
        SHF_STRINGS = 0x20,
        /** sh_info contains SHT index. */
        SHF_INFO_LINK = 0x40,
        /** Preserve order after combining. */
        SHF_LINK_ORDER = 0x80,
        /** Non-standard OS specific handling required. */
        SHF_OS_NONCONFORMING = 0x100,
        /** Section is member of a group. */
        SHF_GROUP = 0x200,
        /** Section holds thread local data. */
        SHF_TLS = 0x400,
        /** OS specific. */
        SHF_MASKOS = 0x0ff00000,
        /** Processor specific. */
        SHF_MASKPROC = 0xf0000000,
        /** Special ordering requirement (Solaris). */
        SHF_ORDERED = 0x4000000,
        /** Section is excluded unless referenced or allocated (Solaris). */
        SHF_EXCLUDE = 0x8000000,
        /** Everything else. */
        SHF_UNDEF = 0xf3fff808
    };

    /**
        Field widths for dumping.
     */
    static constexpr size_t name_width = 18;
    static constexpr size_t type_width = 16;
    static constexpr size_t addr_width = 11;
    static constexpr size_t offset_width = 11;
    static constexpr size_t size_width = 11;
    static constexpr size_t ent_width = 5;
    static constexpr size_t flag_width = 4;
    static constexpr size_t link_width = 3;
    static constexpr size_t info_width = 4;
    static constexpr size_t align_width = 4;

    /**
        Constructor. Copy the data from the source into the array.

        @param ifs Input stream to get data from. Must already be positioned at
               the start of the header.
        @param sz Number of bytes in the section header.
     */
    ElfSectionHeader(klib::istream& ifs, size_t sz);

    /**
        Default constructor. Set the data to zero. As long as the type is zero,
        it counts as invalid.
     */
    ElfSectionHeader() : data{} {}

    /**
        Dumps information on the header to the specified Stream.

        @param dest Stream to write to.
     */
    void dump(klib::ostream& dest) const;

    /**
        Extract the location of the name in .shstrtab. Actual attempts to access
        .shstrtab for the kernel should only be made after calling
        ElfSectionTab::remap_sections() to fix physical addresses.

        @return Byte offset of the name in the .shstrtab section.
     */
    int32_t sh_name() const { return static_cast<int32_t>(klib::get<0>(data)); }

    /**
        Extract the type.

        @return Flag identifying the type of this header.
     */
    sh_types sh_type() const
    {
        return static_cast<sh_types>(klib::get<1>(data));
    }

    /**
        Extract the flags.

        @return Flag identifying the attributes of the section.
     */
    uint32_t sh_flags() const
    {
        return klib::get<2>(data);
    }

    /**
        Extract the address. Will be the virtual address if the section is
        part of the program proper. If the section was something added by the
        linker (eg. .symtab) it's probably the physical address for the kernel
        or 0x0 for a user mode process.

        @return Address of the start of the section.
     */
    const void* sh_addr() const
    {
        return reinterpret_cast<void*>(klib::get<3>(data));
    }

    /**
        Extract the offest.

        @return Offset of the section in the file image.
     */
    int32_t sh_offset() const
    {
        return static_cast<int32_t>(klib::get<4>(data));
    }

    /**
        Extract the size.

        @return Size in bytes of the section.
     */
    uint32_t sh_size() const { return klib::get<5>(data); }

    /**
        Extract the link section. The meaning depends on the section type.

        @return Index of an associated section.
     */
    uint32_t sh_link() const { return klib::get<6>(data); }

    /**
        Extract the info. The meaning depends on the section type.

        @return Extra information about the section.
     */
    uint32_t sh_info() const { return klib::get<7>(data); }

    /**
        Extract the required section alignment. Must be a power of two.

        @return Required section alignment.
     */
    uint32_t sh_addralign() const { return klib::get<8>(data); }

    /**
        Extract the entry size, for fixed-size sections. Otherwise zero.

        @return Size of fixed-size entries.
     */
    uint32_t sh_entsize() const { return klib::get<9>(data); }

    /**
        Set the virtual address. This is necessary to fiddle address for
        sections that weren't loaded within the kernel by GRUB.

        @param addr Address to set. Should be virtual, but this is unchecked.
     */
    void set_addr(const void* addr);

private:
    // We want a packed struct, but don't want to use non-standard features. Use
    // an array of the right length instead.
    klib::array<uint32_t, size / sizeof(uint32_t)> data;

    // Add the flag characters to the provided string.
    klib::string process_flags() const;
};

/**
    The actual data in an ELF section.
 */
class ElfSection {
public:
    /**
        Constructor. Copies the data from the provided address.

        @param ifs Input stream to get data from. Must already be positioned at
               the start of the section.
        @param s Number of bytes in the section.
     */
    ElfSection(klib::istream& ifs, size_t s);

    /**
        Default constructor. Sets size to zero and data location to nullptr.
     */
    ElfSection() : sz{0}, data{nullptr} {}

    /**
        Copy constructor. Makes a copy of the data.

        @param other Another ElfSection to copy from.
     */
    ElfSection(const ElfSection& other);

    /**
        Move constructor. Pirates the data.

        @param other Another ElfSection to pirate from.
     */
    ElfSection(ElfSection&& other);

    /**
        Copy assignment. Makes a copy of the data.

        @param other Another ElfSection to copy from.
     */
    ElfSection& operator=(const ElfSection& other);

    /**
        Move assignemnt. Pirates the data.

        @param other Another ElfSection to pirate from.
     */
    ElfSection& operator=(ElfSection&& other);

    /**
        Destructor. Frees the data.
     */
    ~ElfSection();

    /**
        Get the location of the data.

        @return Data location.
     */
    const uint32_t* get_data() const { return data; }

    /**
        Get the size, in bytes, of the section.

        @return Section size
     */
    size_t size() const { return sz; }
private:
    // Size of the section.
    size_t sz;
    // Location of the data.
    uint32_t* data;
};

/**
    Table of section headers.
 */
class ElfSectionTab {
public:
    /**
        Type of a pointer to a section table entry.
     */
    using pointer = ElfSectionHeader*;
    /**
        Type of a const pointer to a section table entry.
     */
    using const_pointer = const ElfSectionHeader*;
    /**
        Type of const iterator used to access the header list.
     */
    using const_iterator =
        klib::helper::normal_iterator<const_pointer, ElfSectionTab>;

    /**
        Construct the table by copying bytes from the provided location.

        @param ifs Input stream to get data from. Must already be positioned at
               the start of the header.
        @param entsize Size of each entry.
        @param num Number of entries.
        @param ndx Index of the section with the section names.
        @param name File name of the binary. Required for reading shstrndx.
        @param k True for the kernel, false for a user mode process.
     */
    ElfSectionTab(klib::istream& ifs,
              uint16_t entsize,
              uint16_t num,
              uint16_t ndx,
              const klib::string& name = "",
              bool k = true);

    /**
        Default constructor. Make an invalid table.
     */
    ElfSectionTab() :
        val{false},
        inv_sh{},
        e_shentsize{0},
        e_shnum{0},
        e_shstrndx{0},
        sh_start{nullptr},
        file_name{""},
        kernel{false}
    {}

    /**
        Copy constructor. Take a copy of the header table.

        @param other Symbol table to copy.
     */
    ElfSectionTab(const ElfSectionTab& other);

    /**
        Move constructor. Pirate the header table.

        @param other Header table to pirate.
     */
    ElfSectionTab(ElfSectionTab&& other);

    /**
        Copy assignment. Take a copy of the header table.

        @param other Header table to copy.
        @return Header table with the same contents as other.
     */
    ElfSectionTab& operator=(const ElfSectionTab& other);

    /**
        Move assignment. Pirate the header table.

        @param other Header table to pirate.
        @return Header table with the same contents as other.
     */
    ElfSectionTab& operator=(ElfSectionTab&& other);

    /**
        Deallocate the memory used for the header table.
     */
    ~ElfSectionTab();

    /**
        Fix any sections that have virtual addresses not inside the kernel. This
        happens when a section that is not normally part of an executable gets
        loaded by GRUB (which seems to load all sections). Either the section is
        really just an alias for another one, in which case the physical address
        will lie within the kernel physical address and we can just add the 
        virtual base, or it really is somewhere else, in which case we copy it
        onto the heap and fiddle the address in its section header.

        @param pdt Page Descriptor Table for creating virtual mappings to the
                   physical location of the section.
     */
    void remap_sections(PageDescriptorTable& pdt);

    /**
        Dumps information on the whole table to the specified Stream.

        @param dest Stream to write to.
     */
    void dump(klib::ostream& dest) const;

    /**
        Get the name of a section by looking up in .shstrtab. Unlikely to work
        for the kernel if remap_sections() hasn't been run.

        @param n Index of the section to fetch the name for.
        @return Name of the section as recorded in .shstrtab. "NULL" if the
                the section type is SHT_NULL. Empty string if the index is out
                of range.
     */
    klib::string get_name(size_t n) const;

    /**
        Gives the size in bytes of a single entry.

        @return Size of an entry.
     */
    size_t entsize() const { return e_shentsize; }

    /**
        Gets the specified entry in the table.

        @param n Header to return, or a blank header if index out of bounds.
        @return Reference to the specified entry in the table.
     */
    const ElfSectionHeader& header(size_t n) const;

    /**
        Gives the number of entries in the table.

        @return The number of entries in the table.
     */
    size_t size() const { return e_shnum; }

    /**
        Gives the index of the section header with the names of the sections.

        @return Index of the section header with the names of the sections.
     */
    size_t strndx() const { return e_shstrndx; }

    /**
        Whether this is a valid header table.

        @return True of the table is valid, false otherwise.
     */
    bool valid() const { return val; }

    /**
        Gets the specified Header in the table. Unchecked access.

        @param n Header to return, or a blank entry if index out of bounds.
        @return Reference to the specified entry in the table.
     */
    const ElfSectionHeader& operator[](size_t n) const { return sh_start[n]; }

    /**
        Const iterator pointing to the start of the table.

        @return An iterator to the start of the table.
     */
    const_iterator begin() const { return const_iterator {sh_start}; }

    /**
        Const iterator pointing to the start of the table.

        @return An iterator to the start of the table.
     */
    const_iterator cbegin() const { return const_iterator {sh_start}; }

    /**
        Const iterator pointing to the end of the table.

        @return An iterator to the end of the table.
     */
    const_iterator end() const { return const_iterator {sh_start + e_shnum}; }

    /**
        Const iterator pointing to the end of the table.

        @return An iterator to the end of the table.
     */
    const_iterator cend() const { return const_iterator {sh_start + e_shnum}; }

private:
    // Whether the table is valid.
    bool val;
    // An invalid section header (as long as the type is 0, it's invalid).
    ElfSectionHeader inv_sh;
    // Size of a single entry in the table.
    uint16_t e_shentsize;
    // Number of entries in the table.
    uint16_t e_shnum;
    // Index of the entry that contains the section names.
    uint16_t e_shstrndx;
    // List of entries.
    ElfSectionHeader* sh_start;
    // List of any sections it was necessary to copy.
    klib::vector<ElfSection> sections;
    // File name of the binary. Empty for the kernel.
    klib::string file_name;
    // Whether this table is for the kernel or a user mode process.
    bool kernel;

    // Set to a safe invalid state.
    void set_invalid();
};

/**
    A program header of an ELF.
 */
class ElfProgramHeader {
public:
    /**
        Size of the header if it was packed. The actual data should look like:
        uint32_t p_type;
        uint32_t p_offset;
        uint32_t p_vaddr;
        uint32_t p_paddr;
        uint32_t p_filesz;
        uint32_t p_memsz;
        uint32_t p_flags;
        uint32_t p_align;
     */
    static constexpr size_t size = 32;

    /**
        Types of segment as specified in p_type.
     */
    enum pt_type : uint32_t {
        /** Ignore this entry. */
        pt_null = 0x0,
        /** Load this entry by first clearing p_memsz bytes at p_vaddr, then
            copy p_filesz bytes to p_vaddr. */
        pt_load = 0x1,
        /** Requires dynamic linking. */
        pt_dynamic = 0x2,
        /** Contains a file path to an executable to use as an interpreter for
            the following section. */
        pt_interp = 0x3,
        /** Comment segment. */
        pt_note = 0x4,
        /** ??? */
        pt_shlib = 0x5,
        /** Contains the program headers. */
        pt_phdr = 0x6,
        /** Architecture specific information. */
        pt_loos = 0x60000000,
        /** Architecture specific information. */
        pt_hios = 0x6FFFFFFF,
        /** Architecture specific information. */
        pt_loproc = 0x70000000,
        /** Architecture specific information. */
        pt_hiproc = 0x7FFFFFFF
    };

    /**
        Meanings of flags in p_flags.
     */
    enum pt_flags : uint32_t {
        /** Executable */
        pt_exec = 0x1,
        /** Writable */
        pt_write = 0x2,
        /** Readable */
        pt_read = 0x4
    };

    /**
        Field widths for dumping.
     */
    static constexpr size_t type_width = 15;
    static constexpr size_t offset_width = 11;
    static constexpr size_t vaddr_width = 11;
    static constexpr size_t paddr_width = 11;
    static constexpr size_t filesz_width = 11;
    static constexpr size_t memsz_width = 11;
    static constexpr size_t flag_width = 4;
    static constexpr size_t align_width = 6;

    /**
        Constructor. Read the data from the provided address into this nice
        structure.

        @param ifs Input stream to get data from. Must already be positioned at
               the start of the header.
        @param sz Size of the header in bytes.
     */
    explicit ElfProgramHeader(klib::istream& ifs, size_t sz);

    /**
        Default constructor. Set the data to zero. As long as the p_types is 0,
        the section won't be loaded.
     */
    ElfProgramHeader() : data{} {}

    /**
        Get the segment type.

        @return Segment type.
     */
    pt_type get_type() const
    {
        return static_cast<pt_type>(klib::get<0>(data));
    }

    /**
        Get the segment offset in the file.

        @return Segment offset.
     */
    uint32_t get_offset() const
    {
        return klib::get<1>(data);
    }

    /**
        Get the virtual address at which to load the segment.

        @return Segment virtual address.
     */
    uint32_t* get_vaddr() const
    {
        return reinterpret_cast<uint32_t*>(klib::get<2>(data));
    }

    /**
        Get the physical address at which to load the segment. This will be
        ignored for user mode processes.

        @return Segment physical address.
     */
    uint32_t* get_paddr() const
    {
        return reinterpret_cast<uint32_t*>(klib::get<3>(data));
    }

    /**
        Get the size of the segment in the file.

        @return Segment size in file.
     */
    uint32_t get_filesz() const
    {
        return klib::get<4>(data);
    }

    /**
        Get the size of the segment in memory. This may be larger than the file
        size because of, for example, a .bss section.

        @return Segment size in memory.
     */
    uint32_t get_memsz() const
    {
        return klib::get<5>(data);
    }

    /**
        Get the flags (rwx) for the segment.

        @return Segment flags.
     */
    uint32_t get_flags() const
    {
        return klib::get<6>(data);
    }

    /**
        Get the required segment alignment. 0 or 1 mean no alignment, otherwise
        it should be a power of 2. If p_vaddr does not have this alignment, the
        header is invalid.

        @return Segment alignment.
     */
    uint32_t get_align() const
    {
        return klib::get<7>(data);
    }

    /**
        Print information in this header to the provided stream.

        @param dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

private:
    // Store for the actual data, as a byte array so we don't have to worry
    // about packing.
    klib::array<uint32_t, size / sizeof(uint32_t)> data;
};

/**
    Table of program headers.
 */
class ElfProgramTab {
public:
    /**
        Type of a pointer to a program table entry.
     */
    using pointer = ElfProgramHeader*;
    /**
        Type of a const pointer to a program table entry.
     */
    using const_pointer = const ElfProgramHeader*;
    /**
        Type of const iterator used to access the header list.
     */
    using const_iterator =
        klib::helper::normal_iterator<const_pointer, ElfProgramTab>;

    /**
        Construct the table by copying bytes from the provided location.

        @param ifs Input stream to get data from. Must already be positioned at
               the start of the header.
        @param entsize Size in bytes of each entry.
        @param num Number of entries.
     */
    ElfProgramTab(klib::istream& ifs, uint16_t entsize, uint16_t num);

    /**
        Default constructor. Make an invalid table.
     */
    ElfProgramTab() :
        val{false},
        inv_ph{},
        e_phentsize{0},
        e_phnum{0}
    {}

    /**
        Copy constructor. Take a copy of the header table.

        @param other Header table to copy.
     */
    ElfProgramTab(const ElfProgramTab& other);

    /**
        Move constructor. Pirate the header table.

        @param other Header table to pirate.
     */
    ElfProgramTab(ElfProgramTab&& other);

    /**
        Copy assignment. Take a copy of the header table.

        @param other Header table to copy.
        @return Header table with the same contents as other.
     */
    ElfProgramTab& operator=(const ElfProgramTab& other);

    /**
        Move assignment. Pirate the header table.

        @param other Header table to pirate.
        @return Header table with the same contents as other.
     */
    ElfProgramTab& operator=(ElfProgramTab&& other);

    /**
        Deallocate the memory used for the header table.
     */
    ~ElfProgramTab();

    /**
        Dumps information on the whole table to the specified Stream.

        @param dest Stream to write to.
     */
    void dump(klib::ostream& dest) const;

    /**
        Gives the size in bytes of a single entry.

        @return Size of an entry.
     */
    size_t entsize() const { return e_phentsize; }

    /**
        Gets the specified entry in the table.

        @param n Header to return, or a blank header if index out of bounds.
        @return Reference to the specified entry in the table.
     */
    const ElfProgramHeader& header(size_t n) const;

    /**
        Gives the number of entries in the table.

        @return The number of entries in the table.
     */
    size_t size() const { return e_phnum; }

    /**
        Whether this is a valid header table.

        @return True of the table is valid, false otherwise.
     */
    bool valid() const { return val; }

    /**
        Gets the specified Header in the table. Unchecked access.

        @param n Header to return.
        @return Reference to the specified entry in the table.
     */
    const ElfProgramHeader& operator[](size_t n) const { return ph_start[n]; }

    /**
        Const iterator pointing to the start of the table.

        @return An iterator to the start of the table.
     */
    const_iterator begin() const { return const_iterator {ph_start}; }

    /**
        Const iterator pointing to the start of the table.

        @return An iterator to the start of the table.
     */
    const_iterator cbegin() const { return const_iterator {ph_start}; }

    /**
        Const iterator pointing to the end of the table.

        @return An iterator to the end of the table.
     */
    const_iterator end() const { return const_iterator {ph_start + e_phnum}; }

    /**
        Const iterator pointing to the end of the table.

        @return An iterator to the end of the table.
     */
    const_iterator cend() const { return const_iterator {ph_start + e_phnum}; }

private:
    // Whether the table is valid.
    bool val;
    // An invalid program header (as long as the type is 0, it's invalid).
    ElfProgramHeader inv_ph;
    // Size of a single entry in the table.
    uint16_t e_phentsize;
    // Number of entries in the table.
    uint16_t e_phnum;
    // List of entries.
    ElfProgramHeader* ph_start;

    // Set to a safe invalid state.
    void set_invalid();
};

/**
    An ELF file.
 */
class Elf {
public:
    /**
        Default constructor. Makes a set of blank invalid headers.
     */
    Elf() :
        val {false},
        file_name {""},
        file_hdr {nullptr},
        section_hdrs {nullptr},
        program_hdrs {nullptr}
    {}

    /**
        Constructor. Read in all the headers given the file name.

        @param name File name to read.
     */
    explicit Elf(const klib::string& name);

    /**
        Copy constructor. Make a copy of the headers in the other ELF file.

        @param other ELF file to make a copy of.
     */
    Elf(const Elf& other);

    /**
        Move constructor. Pirate the headers in the other ELF file.

        @param other ELF file to pirate from.
     */
    Elf(Elf&& other);

    /**
        Copy assignemnt. Make a copy of the headers in the other ELF file.

        @param other ELF file to make a copy of.
        @return Reference to this ELF file.
     */
    Elf& operator=(const Elf& other);

    /**
        Move assignment. Pirate the headers in the other ELF file.

        @param other ELF file to pirate from.
        @return Reference to this ELF file.
     */
    Elf& operator=(Elf&& other);

    /**
        Desctructor. Frees the file header, section headers table and program
        headers table.
     */
    ~Elf();

    /**
        Allocates all the pages required to cover the segments listed in the
        program header table.

        @param pdt Page Descriptor Table for the allocations.
     */
    void allocate(PageDescriptorTable& pdt) const;

    /**
        Deallocates all the pages required to cover the segments listed in the
        program header table.

        @param pdt Page Descriptor Table for the deallocations.
     */
    void deallocate(PageDescriptorTable& pdt) const;

    /**
        Print information on the file, program and section headers to the
        provided stream.

        @dest Stream to print to.
     */
    void dump(klib::ostream& dest) const;

    /**
        Reads through the section headers to find the .bss section and then
        returns the first location after that section as loaded in memory. This
        gives the end of the programme data and therefore the start of the heap.

        @return Programme break point.
     */
    uintptr_t* get_break_point() const;

    /**
        Gives the file header.

        @return The file header.
     */
    const ElfFileHeader& get_file_header() const { return *file_hdr; }

    /**
        Gives the section headers table.

        @return The section headers table.
     */
    const ElfSectionTab& get_section_table() const { return *section_hdrs; }

    /**
        Gives the program headers table.

        @return The program headers table.
     */
    const ElfProgramTab& get_program_table() const { return *program_hdrs; }

    /**
        Load the program segments into memory at their specified locations.
     */
    void load() const;

    /**
        Whether this seems to be a valid ELF file that the kernel can load.

        @return Whether the file is valid.
     */
    bool valid() const { return val; }

private:
    // Whether this seems to be a valid ELF file that the kernel can load.
    bool val;
    // File name of the binary. We need this for loading the binary later.
    klib::string file_name;
    // File header.
    ElfFileHeader* file_hdr;
    // Section header table.
    ElfSectionTab* section_hdrs;
    // Program header table.
    ElfProgramTab* program_hdrs;
};
#endif /* ELF_H */
