#include "Gdt.h"

#include "Kernel.h"
#include "Logger.h"

/******************************************************************************
 ******************************************************************************/

GdtEntry::GdtEntry(uint32_t base,
                   uint32_t limit,
                   bool gran,
                   bool sz,
                   bool priv,
                   bool tss,
                   bool exec,
                   bool dir,
                   bool perm,
                   bool acc)
    : value{0}, ok{false}
{
    // Checks
    if (limit > 0x000FFFFF)
        return;

    ok = true;

    // See http://wiki.osdev.org/Global_Descriptor_Table for data layout.
    value |= static_cast<uint64_t>(limit & 0x0000FFFF);
    value |= static_cast<uint64_t>(limit & 0x00FF0000) << 32;
    value |= static_cast<uint64_t>(base & 0x00FFFFFF) << 16;
    value |= static_cast<uint64_t>(base & 0xFF000000) << 32;

    // Access byte
    value |= 0x1ll << 47; // Present
    if (!priv) // Privilege
        value |= 0x3ll << 45;
    if (!tss)
        value |= 0x1ll << 44; // Set for not TSS or LDT, unset for those.
    if (exec) // Executable
        value |= 0x1ll << 43;
    if (dir) // Direction/Conforming
        value |= 0x1ll << 42;
    if (perm) // Read/writable
        value |= 0x1ll << 41;
    if (acc) // Accessed
        value |= 0x1ll << 40;

    // Flags
    if (gran) // Granularity (1B or 4K blocks)
        value |= 0x1ll << 55;
    if (sz) // Size (16 bit or 32 bit)
        value |= 0x1ll << 54;
}

/******************************************************************************
 ******************************************************************************/

GdtRegister::GdtRegister(size_t index, bool priv, bool local)
{
    value = index * 8;
    if (local)
        value |= 1 << 2;
    if (!priv)
        value |= 3;
}

/******************************************************************************
 ******************************************************************************/

Gdt::Gdt() : entries{}, user_cs{0}, user_ds{0}
{
    // We always need a null entry at the start.
    entries.push_back(GdtEntry{}.val());
}

/******************************************************************************/

bool Gdt::push_back(const GdtEntry& data)
{
    if (!data.valid())
        return false;

    entries.push_back(data.val());
    return true;
}

/******************************************************************************/

bool Gdt::erase(size_t n)
{
    // We can't delete the null entry at the start.
    if (n == 0 || n >= entries.size())
        return false;

    entries.erase(entries.begin() + n);
    return true;
}

/******************************************************************************/

GdtEntry Gdt::inspect(size_t n) const
{
    if (n >= entries.size())
        return GdtEntry{};
    else
        return GdtEntry{entries[n]};
}

/******************************************************************************/

bool Gdt::set_kernel_mode_cs(size_t n)
{
    if (n == 0 || n >= entries.size())
        return false;

    kernel_cs = n;
    return true;
}

/******************************************************************************/

bool Gdt::set_kernel_mode_ds(size_t n)
{
    if (n == 0 || n >= entries.size())
        return false;

    kernel_ds = n;
    return true;
}

/******************************************************************************/

bool Gdt::set_user_mode_cs(size_t n)
{
    if (n == 0 || n >= entries.size())
        return false;

    user_cs = n;
    return true;
}

/******************************************************************************/

bool Gdt::set_user_mode_ds(size_t n)
{
    if (n == 0 || n >= entries.size())
        return false;

    user_ds = n;
    return true;
}

/******************************************************************************/

bool Gdt::set_user_mode_tss(size_t n)
{
    if (n == 0 || n >= entries.size())
        return false;

    user_tss = n;
    return true;
}

/******************************************************************************/

bool Gdt::load() const
{
    if (kernel_cs == 0 || kernel_ds == 0 || user_tss == 0)
        return false;

    GdtRegister cs {kernel_cs, true, false};
    GdtRegister ds {kernel_ds, true, false};

    load_gdt(reinterpret_cast<uint32_t>(entries.data()),
        static_cast<uint16_t>(entries.size() * 8));
    reset_segments(cs.val(), ds.val());

    global_kernel->syslog()->info("Loading segment selector %X to TSS\n",
        static_cast<uint32_t>(user_mode_tss().val()));
    load_tss(user_mode_tss().val());

    return true;
}

/******************************************************************************
 ******************************************************************************/

Tss::Tss(const void* esp, const GdtRegister& ss)
{
    for (size_t i = 0; i < tss_size / sizeof(uint32_t); ++i)
        data[i] = 0;

    klib::get<1>(data) = reinterpret_cast<uint32_t>(esp);
    klib::get<2>(data) = ss.val();
}

/******************************************************************************
 ******************************************************************************/
