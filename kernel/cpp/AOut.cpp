#include "AOut.h"

#include <cstring>

/******************************************************************************
 ******************************************************************************/

nlist::nlist(const uint32_t* d)
{
    // Check that the size is a multiple of 4.
    static_assert(size % 4 == 0, "Size of nlist is not a multiple of 4.");
    static_assert(un_size % 4 == 0,
        "Size of union in nlist is not a multiple of 4."); 
    klib::memcpy(data.data(), d, size);
}

/******************************************************************************
 ******************************************************************************/

AOutSymTab::AOutSymTab(const uint32_t* addr, size_t tsize, size_t ssize) :
    val{true},
    inv_nl{},
    sym_ent{tsize / nlist::size},
    str_len{ssize},
    nl_start{nullptr},
    str_start{nullptr}
{
    // Allocate space for the symbol table.
    nl_start = new nlist[sym_ent];
    // Sanity check. The first 4 bytes of addr are the size of the table.
    if (static_cast<unsigned long>(*addr) != sym_ent * nlist::size)
    {
        set_invalid();
        return;
    }
    addr += sizeof(unsigned long);
    // Copy the entries.
    for (size_t i = 0; i < sym_ent;
        ++i, addr += (nlist::size / sizeof(uint32_t)))
    {
        nl_start[i] = nlist {addr};   
    }

    // Allocate space for the strings table.
    str_start = new char[ssize];
    // Sanity check. The first 4 bytes are the size of the table.
    if (static_cast<unsigned long>(*addr) != str_len)
    {
        set_invalid();
        return;
    }
    addr += sizeof(unsigned long);
    // Copy the entries.
    klib::memcpy(str_start, addr, ssize);   
}

/******************************************************************************/

AOutSymTab::AOutSymTab(const AOutSymTab& other) :
    val{other.val},
    inv_nl{},
    sym_ent{other.sym_ent},
    str_len{other.str_len}
{
    nl_start = new nlist[sym_ent];
    for (size_t i = 0; i < sym_ent; ++i)
        nl_start[i] = other.nl_start[i];
    str_start = new char[str_len];
    klib::memcpy(str_start, other.str_start, str_len);
}

/******************************************************************************/

AOutSymTab::AOutSymTab(AOutSymTab&& other) :
    val{other.val},
    inv_nl{},
    sym_ent{other.sym_ent},
    str_len{other.str_len},
    nl_start{other.nl_start},
    str_start{other.str_start}
{
    // Set other to invalid (otherwise the destructor will mess up nl_start and
    // str_start).
    other.nl_start = nullptr;
    other.str_start = nullptr;
    other.set_invalid();
}

/******************************************************************************/

AOutSymTab& AOutSymTab::operator=(const AOutSymTab& other)
{
    delete[] nl_start;
    delete[] str_start;

    val = other.val;
    sym_ent = other.sym_ent;
    str_len = other.str_len;

    nl_start = new nlist[sym_ent];
    for (size_t i = 0; i < sym_ent; ++i)
        nl_start[i] = other.nl_start[i];
    str_start = new char[str_len];
    klib::memcpy(str_start, other.str_start, str_len);

    return *this;
}

/******************************************************************************/

AOutSymTab& AOutSymTab::operator=(AOutSymTab&& other)
{
    delete[] nl_start;
    delete[] str_start;

    val = other.val;
    sym_ent = other.sym_ent;
    str_len = other.str_len;
    nl_start = other.nl_start;
    str_start = other.str_start;

    // Set other to invalid (otherwise the destructor will mess up nl_start and
    // str_start).
    other.nl_start = nullptr;
    other.str_start = nullptr;
    other.set_invalid();

    return *this;
}

/******************************************************************************/

AOutSymTab::~AOutSymTab()
{
    delete[] nl_start;
    delete[] str_start;
}

/******************************************************************************/

const char* AOutSymTab::strings(size_t n) const
{
    if (n >= str_len)
        return nullptr;

    return str_start + n;
}

/******************************************************************************/

const nlist& AOutSymTab::symbol(size_t n) const
{
    if (n >= sym_ent)
        return inv_nl;

    return nl_start[n];
}

/******************************************************************************/

void AOutSymTab::set_invalid()
{
    val = false;
    sym_ent = 0;
    str_len = 0;
    delete[] nl_start;
    nl_start = nullptr;
    delete[] str_start;
    str_start = nullptr;
}

/******************************************************************************
 ******************************************************************************/
