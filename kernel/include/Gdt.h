#ifndef GDT_H
#define GDT_H

#include <stddef.h>
#include <stdint.h>

#include <array>
#include <vector>

/**
    A single entry in a Global Descriptor Table (GDT). Mistakes in setting this
    are liable to cause the computer to restart.
 */
class GdtEntry {
public:
    /**
        Create a table entry.

        @param base Start address of the segment.
        @param limit Size of the segment. This is in bytes if granularity is
                     false, or in 4K if granularity is true. The maximum value
                     is 2^20 - 1 (1048575). Mulitply that by 4K gets you the
                     4GB limit.
        @param sz True for 32-bit mode, false for 16-bit mode.
        @param priv True for kernel, false for user space.
        @param tss Whethert his segment is for a TSS (or LDT).
        @param gran True for 4K block size, false for 1B block size.
        @param exec Excecutable, true for code segment, false for data segment.
        @param dir Direction/Conforming, normally set to false.
        @param perm Permissions. True makes code segments readable and data
                    segments writable.
        @param acc Whether the segment has been accessed. Special meaning for
               TSS segments.
     */
    GdtEntry(uint32_t base,
             uint32_t limit,
             bool gran,
             bool sz,
             bool priv,
             bool tss,
             bool exec,
             bool dir,
             bool perm,
             bool acc);

    /**
        Create a table entry directly from a value. No checking is performed.

        @param data Value of the table entry.
     */
    explicit GdtEntry(uint64_t data) : value{data}, ok{true} {}

    /**
        Create an invalid table entry.
     */
    GdtEntry() : value{0}, ok{false} {}

    /**
        Gets the data in a format useable by the system.

        @return The entry.
     */
    uint64_t val() const { return value; }

    /**
        Checks whether this is a valid entry.

        @return Whether sensible values were provided to generate the entry.
     */
    bool valid() const { return ok; }

private:
    // Stores the data.
    uint64_t value;
    // Stores results of checks done in constructor.
    bool ok;
};

/**
    Register entry for selecting a table entry. Mistakes in setting this are
    liable to cause the computer to restart.
 */
class GdtRegister {
public:
    /**
        Create a register entry.

        @param index Entry in the table. Usually 1 for the kernel code segment,
               2 for the kernel data segment, 3 for the user code segment, 4 for
               the user data segment and 5 for the TSS.
        @param priv True for kernel, false for user space.
        @param local True for an LDT entry, false for a GDT entry.
     */
    GdtRegister(size_t index, bool priv, bool local);

    /**
        Gets the data in a format useable by the system.

        @return The entry.
     */
    uint16_t val() const { return value; }

    /**
        Gets the index number.

        @return Entry specified.
     */
    size_t index() const { return value >> 3; }

private:
    // Stores the data.
    uint16_t value;
};

/**
    Represents a Global Descriptor Table (GDT). Mistakes in setting this are
    liable to cause the computer to restart.
 */
class Gdt {
public:
    /**
        Create the null descriptor entry. Set all other entries to zero.
     */
    Gdt();

    /**
        Add a table row to the end.

        @param data Row entry.
        @return Whether the operation succeeded.
     */
    bool push_back(const GdtEntry& data);

    /**
        Delete the table entry at the specified position. Nothing happens if
        there is no entry at that position. Later entries will be bumped up
        to fill the space. The null descriptor entry (at zero) cannot be erased.

        @param n Position to erase.
        @return Whether the operation succeeded.
     */
    bool erase(size_t n);

    /**
        Get the entry at a specified position. Read only access.

        @param n Position to read.
        @return The entry at the specified position. Returns an invalid entry if
                no entry was found at the position.
     */
    GdtEntry inspect(size_t n) const;

    /**
        Get the entry at a specified position. Completely unchecked access: use
        with caution.

        @param n Position to read.
        @return The entry at the specified position. Returns an invalid entry if
                no entry was found at the position.
     */
    uint64_t& operator[](size_t n) { return entries[n]; }
    const uint64_t& operator[](size_t n) const { return entries[n]; }

    /**
        Get the current size of the table.

        @return Number of entries in the current table.
     */
    size_t size() const { return entries.size(); }

    /**
        Returns the GDT register corresponding to the kernel mode code segment.

        @return GDT register value for the kernel mode code segment.
     */
    GdtRegister kernel_mode_cs() const
    {
        return GdtRegister {kernel_cs, true, false};
    }

    /**
        Sets the kernel mode code segment to the index provided, or does nothing
        if the index is out of bounds.

        @return Whether the operation suceeded.
     */
    bool set_kernel_mode_cs(size_t n);

    /**
        Returns the GDT register corresponding to the kernel mode data segment.

        @return GDT register value for the kernel mode data segment.
     */
    GdtRegister kernel_mode_ds() const
    {
        return GdtRegister {kernel_ds, true, false};
    }

    /**
        Sets the kernel mode data segment to the index provided, or does nothing
        if the index is out of bounds.

        @return Whether the operation suceeded.
     */
    bool set_kernel_mode_ds(size_t n);

    /**
        Returns the GDT register corresponding to the user mode code segment.

        @return GDT register value for the user mode code segment.
     */
    GdtRegister user_mode_cs() const
    {
        return GdtRegister {user_cs, false, false};
    }

    /**
        Sets the user mode code segment to the index provided, or does nothing
        if the index is out of bounds.

        @return Whether the operation suceeded.
     */
    bool set_user_mode_cs(size_t n);

    /**
        Returns the GDT register corresponding to the user mode data segment.

        @return GDT register value for the user mode data segment.
     */
    GdtRegister user_mode_ds() const
    {
        return GdtRegister {user_ds, false, false};
    }

    /**
        Sets the user mode data segment to the index provided, or does nothing
        if the index is out of bounds.

        @return Whether the operation suceeded.
     */
    bool set_user_mode_ds(size_t n);

    /**
        Returns the GDT register corresponding to the task state segment.

        @return GDT register value for the task state segment.
     */
    GdtRegister user_mode_tss() const
    {
        return GdtRegister {user_tss, false, false};
    }

    /**
        Sets the task state segment to the index provided, or does nothing
        if the index is out of bounds.

        @return Whether the operation suceeded.
     */
    bool set_user_mode_tss(size_t n);

    /**
        Load the table into the system. The currently stored kernel mode cs and
        ds values are used.

        @return Whether the operation succeeded.
     */
    bool load() const; 

private:
    // Space for table entries. We're very much relying on vector having
    // contiguous storage. The standard does guarantee that, so we just have to
    // hope klib is compliant.
    klib::vector<uint64_t> entries;

    // Indices of useful segments that other things might need to know.
    size_t kernel_cs;
    size_t kernel_ds;
    size_t user_cs;
    size_t user_ds;
    size_t user_tss;
};

/**
    Represents a Task State Segment. This contains a lot of unused information
    also the SS and ESP values that will be used when switching to kernel mode
    from user mode by interrupt.
 */
class Tss {
public:
    /**
        Constructor. Sets the ESP and SS to the provided values, and everything
        else to zero. SS should probably be the kernel data segment.

        @param esp Stack pointer for use on interrupts.
        @param ss Stack segment selector for use on interrupt. Should probably
               be the kernel data segment.
     */
    Tss(const void* esp, const GdtRegister& ss);

    /**
        Gets the address of the start of the data. Needed when creating the
        GDT entry for the TSS.

        @return Address of the data.
     */
    const uint32_t* base() const { return data.data(); }

    /**
        Gets the current esp value, the stack that will be loaded on interrupt.

        @return Current TSS esp value.
     */
    const void* get_esp() const
    {
        return reinterpret_cast<const void*>(klib::get<1>(data));
    }

    /**
        Sets the esp value, the stack that will be loaded on interrupt.

        @param esp The TSS esp value to set.
     */
    void set_esp(const void* esp)
    {
        klib::get<1>(data) = reinterpret_cast<uint32_t>(esp);
    }

    /**
        Size, in bytes, of the TSS data. Needed when creating the GDT entry for
        the TSS.
     */
    static constexpr size_t tss_size = 104;

private:
    // The actual data.
    klib::array<uint32_t, tss_size/sizeof(uint32_t)> data;
};

/**
    Assembly code to load the Global Descriptor Table.

    @param addr Address of the table to load.
    @param size of the table.
 */
extern "C"
void load_gdt(uint32_t addr, uint16_t sz);

/**
    Assembly code to reset the segment registers after loading the GDT.

    @param cs Value to load into code segment register.
    @param ds Value to load into data segment registers.
 */
extern "C"
void reset_segments(uint16_t cs, uint16_t ds);

/**
    Assembly code to load the task register.

    @param tss Segment selector for the TSS. This should have the index and
           privilege level set.
 */
extern "C"
void load_tss(uint16_t tss);

#endif
