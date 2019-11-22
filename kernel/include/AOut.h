#ifndef AOUT_H
#define AOUT_H

#include <stdint.h>

#include <array>
#include <iterator>

/**
    A symbol table entry.
 */
class nlist {
private:
    // Size of the union. for convenience.
    static constexpr size_t un_size =
        sizeof(sizeof(const char*) > sizeof(int32_t) ? sizeof(const char*) :
        sizeof(int32_t));

public:
    /**
        Size nlist would have if it was definitely packed. The data should look
        like:
        union { const char* name; int32_t strx; };
        uint8_t type;
        int8_t other;
        int16_t desc;
        uint32_t value;
     */
    static constexpr size_t size = un_size + sizeof(uint8_t) + sizeof(int8_t) +
        sizeof(int16_t) + sizeof(uint32_t);

    /**
        Constructor. Copy the data from the source into the array.

        @param d Source to copy data from.
     */
    explicit nlist(const uint32_t* d);

    /**
        Default constructor. Set the data to zero.
     */
    nlist() : data{} {}

    /**
        Extract the name.

        @return Pointer to the name of this symbol.
     */
    const char* name() const
    {
        return reinterpret_cast<const char*>(klib::get<0>(data));
    }

    /**
        Extract the name.

        @return Byte offset of the name in the strings table.
     */
    int32_t strx() const { return static_cast<int32_t>(klib::get<0>(data)); }

    /**
        Extract the type.

        @return The type of this symbol.
     */
    uint8_t type() const
    {
        return
            static_cast<uint8_t>(klib::get<un_size / sizeof(uint32_t)>(data));
    }

    /**
        Extract the other information.

        @return Other information on the nature of this symbol.
     */
    int8_t other() const
    {
        return static_cast<int8_t>(klib::get<un_size / sizeof(uint32_t)>(data)
            >> 8);
    }

    /**
        Extract the debugger information.

        @return Information reserved for debugger use.
     */
    int16_t desc() const
    {
        return static_cast<int16_t>(klib::get<un_size / sizeof(uint32_t)>(data)
            >> 16);
    }

    /**
        Extract the value.

        @return The value of this symbol.
     */
    uint32_t value() const
    {
        return klib::get<un_size / sizeof(uint32_t) + 1>(data);
    }

private:
    // We want a packed struct, but we don't want to use non-standard features.
    // Create an array of the right length instead.
    klib::array<uint32_t, size / sizeof(uint32_t)> data;
};

/**
    Stores a symbol table and the acompanying strings table.
 */
class AOutSymTab {
public:
    /**
        Type of a pointer to a symbol table entry.
     */
    using pointer = nlist*;
    /**
        Type of a const pointer to a symbol table entry.
     */
    using const_pointer = const nlist*;
    /**
        Type of const iterator used to access the symbol list.
     */
    using const_iterator =
        klib::helper::normal_iterator<const_pointer, AOutSymTab>;

    /**
        Construct the table by copying bytes from the provided location.

        @param addr Address to copy from.
        @param tsize Number of bytes in the symbol table.
        @param ssize Number of bytes in the strings table.
     */
    AOutSymTab(const uint32_t* addr, size_t tsize, size_t ssize);

    /**
        Defualt constructor. Construct an invalid table.
     */
    AOutSymTab() :
        val{false},
        inv_nl{},
        sym_ent{0},
        str_len{0},
        nl_start{nullptr},
        str_start{nullptr}
    {}

    /**
        Copy constructor. Take a copy of the symbol and strings table.

        @param other Symbol table to copy.
     */
    AOutSymTab(const AOutSymTab& other);

    /**
        Move constructor. Pirate the symbol and strings table.

        @param other Symbol table to pirate.
     */
    AOutSymTab(AOutSymTab&& other);

    /**
        Copy assignment. Take a copy of the symbol and strings table.

        @param other Symbol table to copy.
        @return Symbol table with the same contents as other.
     */
    AOutSymTab& operator=(const AOutSymTab& other);

    /**
        Move assignment. Pirate the symbol and strings table.

        @param other Symbol table to pirate.
        @return Symbol table with the same contents as other.
     */
    AOutSymTab& operator=(AOutSymTab&& other);

    /**
        Deallocate the memory used for the symbol and strings table.
     */
    ~AOutSymTab();

    /**
        Gets a pointer to the specified offset in the strings table.

        @param off Offset in the table, default 0, or nullptr if index out of
               bounds.
        @return A pointer to the specified offset in the strings table.
     */
    const char* strings(size_t n = 0) const;

    /**
        Gives the number of bytes/characters in the strings table.

        @return Size of the strings table.
     */
    size_t strings_length() const { return str_len; }

    /**
        Gets the specified entry in the symbol table.

        @param n Entry to return, or a blank entry if index out of bounds.
        @return Reference to the specified entry in the symbol table.
     */
    const nlist& symbol(size_t n) const;

    /**
        Gives the number of entries (not the number of bytes) in the symbol
        table.

        @return The number of entries in the symbol table.
     */
    size_t size() const { return sym_ent; }

    /**
        Whether this is a valid symbol table.

        @return True of the table is valid, false otherwise.
     */
    bool valid() const { return val; }

    /**
        Gets the specified entry in the symbol table. Unchecked access.

        @param n Entry to returns.
        @return Reference to the specified entry in the symbol table.
     */
    const nlist& operator[](size_t n) const { return nl_start[n]; }

    /**
        Const iterator pointing to the start of the symbol table.

        @return An iterator to the start of the symbol table.
     */
    const_iterator begin() const { return const_iterator {nl_start}; }

    /**
        Const iterator pointing to the start of the symbol table.

        @return An iterator to the start of the symbol table.
     */
    const_iterator cbegin() const { return const_iterator {nl_start}; }

    /**
        Const iterator pointing to the end of the symbol table.

        @return An iterator to the end of the symbol table.
     */
    const_iterator end() const { return const_iterator {nl_start + sym_ent}; }

    /**
        Const iterator pointing to the end of the symbol table.

        @return An iterator to the end of the symbol table.
     */
    const_iterator cend() const { return const_iterator {nl_start + sym_ent}; }

private:
    // Whether this is a valid table.
    bool val;
    // An invalid nlist entry.
    nlist inv_nl;
    // Number of symbol table entries.
    size_t sym_ent;
    // Number of bytes in the strings table;
    size_t str_len;
    // Pointer to the start of the symbol table;
    nlist* nl_start;
    // Pointer to the start of the strings table;
    char* str_start;

    // Set to a safe invalid state.
    void set_invalid();
};

#endif /* AOUT_H */
