#ifndef IDT_H
#define IDT_H

#include <stddef.h>
#include <stdint.h>

#include <array>

// Forward delcaration
class GdtRegister;

/**
    Types of interrupt handlers.
 */
enum class IdtType : uint16_t {
    // 32 bit task gate
    task_32 = 0x5,
    // 16 bit interrupt gate
    int_16 = 0x6,
    // 16 bit trap gate
    trap_16 = 0x7,
    // 32 bit interrupt gate
    int_32 = 0xE,
    // 32 bit trap gate
    trap_32 = 0xF
};

/**
    A single entry in a Interrupt Descriptor Table (IDT).
 */
class IdtEntry {
public:
    /**
        Create a table entry.

        @param addr Address of the interrupt handler.
        @param cs Selector for interrupt handler. Must be a valid entry in the
                  GDT.
        @param type Type of handler.
        @param priv True for kernel, false for user space
     */
    IdtEntry(void (*addr)(), GdtRegister cs, IdtType type, bool priv);

    /**
        Create an empty (invalid) table entry).
     */
    IdtEntry() : value{0} {}

    /**
        Create a table entry directly from a value. No checking is performed.

        @param data Value of the table entry.
     */
    explicit IdtEntry(uint64_t data) : value{data} {}

    /**
        Gets the data in a format useable by the system.

        @return The entry.
     */
    uint64_t val() const { return value; }

private:
    // Stores the data.
    uint64_t value;
};

/**
    Represents an Interrupt Descriptor Table (IDT).
 */
class Idt {
public:
    /**
        Create an empty table.
     */
    Idt();

    /**
        Sets the value at the specified position. Nothing happens if n is out
        of bounds.

        @param data Value to set.
        @param n Position to set.
     */
    void set(const IdtEntry& data, size_t n);

    /**
        Gets a copy of the value at the sepcified position. Returs a zero value
        if n is out of bounds.

        @param n Position to get.
     */
    IdtEntry inspect(size_t n);

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
        Load the table into the system.
     */
    void load() const;

private:
    // Space for table entries.
    static constexpr size_t max_entries = 255;
    klib::array<uint64_t, max_entries> entries;
};

/**
    Fill the idt with sensible values.

    @param idt Table to fill.
 */
void fill_idt(Idt* idt);

#endif
