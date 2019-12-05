#include "Ext.h"

#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <fstream>
#include <istream>
#include <string>
#include <utility>
#include <vector>

#include "File.h"
#include "FileSystem.h"
//#include "Kernel.h"
//#include "Logger.h"

/******************************************************************************
 ******************************************************************************/

FileSystem* create_ext(const klib::string& drv)
{
    FileSystem* fs;

    // Test ext2
    if ((fs = create_ext2(drv)) != nullptr)
        return fs;
    // Other ext versions here.

    return nullptr;
}

/******************************************************************************
 ******************************************************************************/

Ext2SuperBlock::Ext2SuperBlock(klib::istream& in) : val {true}
{
    // Read in the first 84 bytes, which are compulsory.
    in.read(data, compulsory_size);

    // Check the amount read and the signature.
    if (!in || in.gcount() != compulsory_size || signature() != sig)
    {
        val = false;
        return;
    }

    // Check the inode size is at least 128 bytes.
    if (inode_size() < 128)
    {
        val = false;
        return;
    }

    // Check the major version and read the optional data.
    if (major_version() >= 1)
    {
        in.read(data + compulsory_size, data_size - compulsory_size);
        if (!in || in.gcount() != compulsory_size)
            val = false;

        // Skip over any unused space, so the stream is positioned at the end of
        // the superblock reserved space.
        in.ignore(disk_size - data_size);
        if (!in || in.gcount() != disk_size - data_size)
            val = false;
    }
    else
    {
        // Skip over any unused space, so the stream is positioned at the end of
        // the superblock reserved space.
        in.ignore(disk_size - compulsory_size);
        if (!in || in.gcount() != disk_size - compulsory_size)
            val = false;
    }
}

/******************************************************************************/

klib::ostream& Ext2SuperBlock::write(klib::ostream& dest) const
{
    if (!dest)
        return dest;

    if (major_version() >= 1)
        dest.write(data, data_size);
    else
        dest.write(data, compulsory_size);

    return dest;
}

/******************************************************************************/

void Ext2SuperBlock::uuid(const klib::string& u)
{
    if (major_version() >= 1)
        klib::memcpy(data + 104, u.data(), uuid_length);
}

/******************************************************************************/

void Ext2SuperBlock::name(const klib::string& n)
{
    if (major_version() >= 1)
        klib::memcpy(data + 120, n.data(), name_length);
}

/******************************************************************************/

void Ext2SuperBlock::path(const klib::string& p)
{
    if (major_version() >= 1)
    {
        if (p.size() < path_length)
        {
            klib::memcpy(data + 136, n.data(), p.size());
            klib::memset(data + 136 + p.size(), '\0', path_length - p.size());
        }
        else
        {
            klib::memcpy(data + 136, n.data(), path_length - 1);
            data[135 + path_length] = '\0';
        }
    }
}

/******************************************************************************
 ******************************************************************************/

BlockGroupDescriptor::BlockGroupDescriptor(klib::istream& in) : val {true}
{
    // Read in the 18 bytes that actually contain data.
    in.read(data, data_size);

    // Check the amount read and the signature.
    if (!in || in.gcount() != data_size)
    {
        val = false;
        return;
    }

    // Skip over any unused space, so the stream is positioned at the end of
    // the descriptor reserved space.
    in.ignore(disk_size - data_size);
    if (!in || in.gcount() != disk_size - data_size)
        val = false;
}

/******************************************************************************/

klib::ostream& BlockGroupDescriptor::write(klib::ostream& dest) const
{
    if (!dest)
        return dest;

    dest.write(data, data_size);

    return dest;
}

/******************************************************************************/

void BlockGroupDescriptor::dump(klib::ostream& dest) const
{
    dest << klib::hex;
    dest << ' ' << block_map() << ' ' << inode_map() << ' ' << inode_table();

    dest << klib::dec;
    dest << ' ' << unalloc_blocks() << ' ' << unalloc_inodes() << ' ' << dirs();

    dest.flush();
}

/******************************************************************************
 ******************************************************************************/

Ext2Inode::Ext2Inode(klib::istream& in, size_t sz) :
    val {true},
    data{},
    disk_size {sz}
{
    // Read in the 128 bytes that actually contain data.
    in.read(data, data_size);

    // Check the amount read and the signature.
    if (!in || in.gcount() != data_size)
    {
        val = false;
        return;
    }

    // Skip over any unused space, so the stream is positioned at the end of
    // the descriptor reserved space.
    in.ignore(disk_size - data_size);
    if (!in || in.gcount() != disk_size - data_size)
        val = false;
}

/******************************************************************************/

klib::ostream& Ext2Inode::write(klib::ostream& dest)
{
    if (!dest)
        return dest;

    dest.write(data, data_size);

    return dest;
}

/******************************************************************************/

klib::array<uint8_t, os_2_size> Ext2Inode::os_2() const
{
    klib::array<uint8_t, os_2_size> ret_val;
    klib::memcpy(ret_val.data(), data + 116, os_2_size);
    return ret_val;
}

void Ext2Inode::os_2(const klib::array<uint8_t, os_2_size>& v)
{
    klib::memcpy(data + 116, v.data(), os_2_size);
}

/******************************************************************************
 ******************************************************************************/

Ext2File::Ext2File(const char* m, Ext2Inode& in, size_t indx,
    Ext2FileSystem& fs) :
    DiskFile{m, fs, fs.file_size(in)},
    inode{in},
    inode_index{indx},
    ext2fs {fs}
{
    if (mode == "w" || mode == "w+")
        // Truncate the file.
        truncate();
}

/******************************************************************************/

size_t Ext2File::read(void* buf, size_t size, size_t count)
{
    // Do nothing if we shouldn't be reading or we're at EOF.
    if (!reading || eof || size == 0 || count == 0)
        return 0;

    // Flush the current buffer before reading.
    if (writing)
        if(flush() != 0)
            return 0;

    // Work out how many bytes to read.
    klib::streamoff n = size * count;
    char* char_buf = static_cast<char*>(buf);

    // Store the block size.
    size_t bl_sz = fs.block_size();
    // Number of characters read.
    klib::streamoff char_read = 0;

    // Anything left in the current block.
    size_t buf_pos = static_cast<klib::streamoff>(position) % bl_sz;
    if (buf_pos != 0)
    {
        // Work out the number of characters to read.
        size_t read_size = klib::min(
            static_cast<klib::streamoff>(bl_sz - buf_pos),
            static_cast<klib::streamoff>(n));
        read_size = klib::min(static_cast<klib::streamoff>(read_size), sz);
        klib::memcpy(buf, buffer + buf_pos, read_size);
        char_read = read_size;
        position += read_size;
    }

    // Read complete blocks (avoids a copy into the buffer then to the output).
    while (char_read + bl_sz < n &&
        static_cast<klib::streamoff>(position) + bl_sz < sz)
    {
        // Divide by block size to get the block of the file we're in.
        size_t block_no = static_cast<klib::streamoff>(position) / bl_sz;
        // Use the inode to convert that to a real block address.
        size_t block = ext2fs.inode_lookup(inode, block_no);
        if (block == 0)
            // We've run out of file.
            break;
        // Read a whole block.
        fs.read(block * bl_sz, char_buf + char_read, bl_sz);
        char_read += bl_sz;
        position += bl_sz;
    }

    // Final characters.
    if (char_read < n && static_cast<klib::streamoff>(position) < sz)
    {
        // Divide by block size to get the block of the file we're in.
        size_t block_no = static_cast<klib::streamoff>(position) / bl_sz;
        // Use the inode to convert that to a real block address.
        size_t block = ext2fs.inode_lookup(inode, block_no);
        if (block != 0)
        {
            // Work out the number of characters to read.
            size_t read_size = klib::min(static_cast<klib::streamoff>(bl_sz),
                sz - static_cast<klib::streamoff>(position));
            // Read into the buffer.
            fs.read(block * bl_sz, buffer, read_size);
            // Work out the number of characters to copy to the output.
            read_size = klib::min(n - char_read,
                sz - static_cast<klib::streamoff>(position));
            klib::memcpy(char_buf + char_read, buffer, read_size);
            char_read += read_size;
            position += read_size;
        }
    }

    if (static_cast<klib::streamoff>(position) >= sz)
        eof = true;

    return char_read / size;
}

/******************************************************************************/

size_t Ext2File::write(const void* buf, size_t size, size_t count)
{
    // TODO
    (void)buf; (void)size; (void)count;
    return 0;
}

/******************************************************************************/

int Ext2File::flush()
{
    // TODO
    return 0;
}

/******************************************************************************/

int Ext2File::seek(long offset, int origin)
{
    // Flush current buffer.
    if (writing)
        if(flush() != 0)
            return EOF;

    switch (origin)
    {
    case SEEK_SET:
    {
        // Make sure we don't go before the beginning of the file.
        if (offset < 0)
            offset = 0;
        // Make sure we don't go beyond the end of the file.
        if (offset > sz)
        {
            position = klib::fpos_t{-1};
            eof = true;
            return EOF;
        }
        // Test whether we're seeking outside the current buffer. If the buffer
        // position is at the start of the buffer, it hasn't been filled.
        klib::streamoff curr_buf_pos = static_cast<klib::streamoff>(position) %
            fs.block_size();
        klib::streamoff curr_block = static_cast<klib::streamoff>(position) /
            fs.block_size();
        position = offset;
        klib::streamoff new_block = static_cast<klib::streamoff>(position) /
            fs.block_size();
        klib::streamoff new_buf_pos = static_cast<klib::streamoff>(position) %
            fs.block_size();
        if ((curr_buf_pos == 0 || curr_block != new_block) && new_buf_pos != 0)
        {
            // Starting position of the read is rounded down to the block start.
            klib::streamoff read_start = ext2fs.inode_lookup(inode, new_block) *
                fs.block_size();
            // Work out the number of characters to read.
            size_t read_size = klib::min(
                static_cast<klib::streamoff>(fs.block_size()), read_start);
            // Read into the buffer.
            fs.read(read_start, buffer, read_size);
        }
        eof = false;
        break;
    }
    case SEEK_CUR:
    {
        // Make sure we don't go before the beginning of the file.
        if (offset < 0 && -offset > static_cast<klib::streamoff>(position))
            offset = -static_cast<klib::streamoff>(position);
        // Make sure we don't go beyond the end of the device.
        if (offset + static_cast<klib::streamoff>(position) > sz)
        {
            position = klib::fpos_t{-1};
            eof = true;
            return EOF;
        }
        // Test whether we're seeking outside the current buffer. If the buffer
        // position is at the start of the buffer, it hasn't been filled.
        klib::streamoff curr_buf_pos = static_cast<klib::streamoff>(position) %
            fs.block_size();
        klib::streamoff curr_block = static_cast<klib::streamoff>(position) /
            fs.block_size();
        position += offset;
        klib::streamoff new_block = static_cast<klib::streamoff>(position) /
            fs.block_size();
        klib::streamoff new_buf_pos = static_cast<klib::streamoff>(position) %
            fs.block_size();
        if ((curr_buf_pos == 0 || curr_block != new_block) && new_buf_pos != 0)
        {
            // Starting position of the read is rounded down to the block start.
            klib::streamoff read_start = ext2fs.inode_lookup(inode, new_block) *
                fs.block_size();
            // Work out the number of characters to read.
            size_t read_size = klib::min(
                static_cast<klib::streamoff>(fs.block_size()), read_start);
            // Read into the buffer.
            fs.read(read_start, buffer, read_size);
        }
        eof = false;
        break;
    }
    case SEEK_END:
    {
        // Make sure we don't go before the beginning of the device.
        if (offset < 0 && -offset > sz)
            offset = -sz;
        // Make sure we don't go beyond the end of the device.
        if (offset > 0)
        {
            position = klib::fpos_t{-1};
            eof = true;
            return EOF;
        }
        // Test whether we're seeking outside the current buffer. If the buffer
        // position is at the start of the buffer, it hasn't been filled.
        klib::streamoff curr_buf_pos = static_cast<klib::streamoff>(position) %
            fs.block_size();
        klib::streamoff curr_block = static_cast<klib::streamoff>(position) /
            fs.block_size();
        position = sz - offset;
        klib::streamoff new_block = static_cast<klib::streamoff>(position) /
            fs.block_size();
        klib::streamoff new_buf_pos = static_cast<klib::streamoff>(position) %
            fs.block_size();
        if ((curr_buf_pos == 0 || curr_block != new_block) && new_buf_pos != 0)
        {
            // Starting position of the read is rounded down to the block start.
            klib::streamoff read_start = ext2fs.inode_lookup(inode, new_block) *
                fs.block_size();
            // Work out the number of characters to read.
            size_t read_size = klib::min(
                static_cast<klib::streamoff>(fs.block_size()), read_start);
            // Read into the buffer.
            fs.read(read_start, buffer, read_size);
        }
        eof = false;
        break;
    }
    }

    // Return 0 for success.
    return 0;
}

/******************************************************************************/

int Ext2File::truncate()
{
    // Do nothing if we shouldn't be writing or the size is already zero.
    if (!writing)
        return -1;
    if (sz == 0)
        return 0;

    // Store the block size.
    size_t bl_sz = fs.block_size();
    const size_t addr_per_block = bl_sz / sizeof(size_t);

    // Cycle through the blocks in the file and deallocate them in the block
    // table. Wipe the pointers. This doesn't feel very safe if the file is open
    // multiple times.

    // Start with the direct pointers.
    bool finished = false;
    for (size_t i = 0; i < Ext2Inode::no_direct && !finished; ++i)
    {
        finished = truncate_recursive(inode.direct(i), 0);
        inode.direct(0, i);
    }

    // Now the singly indirect pointer.
    if (!finished)
    {
        finished = truncate_recursive(inode.s_indirect(), 1);
        inode.s_indirect(0);
    }

    // The doubly indirect.
    if (!finished)
    {
        finished = truncate_recursive(inode.d_indirect(), 2);
        inode.d_indirect(0);
    }

    // The triply indirect.
    if (!finished)
    {
        finished = truncate_recursive(inode.d_indirect(), 3);
        inode.t_indirect(0);
    }

    // Set the file size to zero.
    sz = 0;

    // Update the inode.
    inode.lower_size(0);
    if (inode.type() == Ext2Inode::file)
        inode.upper_size(0);
    inode.sectors(0);
    ext2fs.update_inode(inode, inode_index);
}

/******************************************************************************/

bool Ext2File::truncate_recursive(size_t bl, size_t depth)
{
    // Don't do anything if the block is zero. Return indicating finished.
    if (bl == 0)
        return true;

    // We can call this with depth 0, for the direct pointers, but we don't need
    // to do any loops.
    if (depth == 0)
    {
        ext2fs.deallocate(bl);
        return false;
    }

    // Store the block size.
    size_t bl_sz = fs.block_size();

    // Create a buffer to read the block.
    unique_ptr<char> buf {new char[bl_sz]};
    bool finished = false;

    // Read the singly indirect block.
    fs.read(bl * bl_sz, buf, bl_sz);
    size_t* buffer_blocks = reinterpret_cast<size_t*>(buf);
    for (size_t i = 0; i < addr_per_block && !finsihed; ++i)
    {
        if (buffer_blocks[i] != 0)
        {
            if (depth == 1)
                ext2fs.deallocate(buffer_blocks[i]);
            else
                finished = truncate_recursive(buffer_blocks[i], depth - 1);
        }
        else
            finished = true;
    }

    return finished;
}

/******************************************************************************
 ******************************************************************************/

Ext2Directory::Ext2Directory(Ext2Inode& in, size_t indx, Ext2FileSystem& fs) :
    inode{in},
    inode_index{indx},
    ext2fs {fs}
{
    // Check that this is actually a directory.
    if (inode.type() != Ext2Inode::directory)
        return;

    // Determine whether the directory has a type field.
    bool type = (fs.get_super_block().required_features &
        static_cast<uint32_t>(
            Ext2FileSystem::required_features::directories_type));

    // Make a file stream for the directory data. We could do this through the
    // VFS, but that creates a lot of extra inode lookups, when we already have
    // the inode.
    Ext2File ifs {"r", inode, inode_index, ext2fs};

    // Process each entry.
    while (true)
    {
        size_t entry_inode;
        if (ifs.read(&entry_inode, sizeof(entry_inode), 1) != 1)
            // Stop if we've run out of file.
            break;

        // Now we read the size of the entry, useful for skipping the unused
        // bytes after the file name.
        uint16_t entry_size;
        if (ifs.read(&entry_size, sizeof(entry_size), 1) != 1)
            // Stop if we've run out of file.
            break;

        // If the inode index is zero, we can now skip over the rest of the
        // entry.
        if (entry_inode == 0)
        {
            size_t skip_length = entry_size - sizeof(entry_inode) -
                sizeof(entry_size);
            if (skip_length != 0)
            {
                char* name_buf = new char[skip_length];
                ifs.read(name_buf, sizeof(char), skip_length);
                delete[] name_buf;
            }
            continue;
        }

        // The name length is either 1 or 2 bytes depending on whether the
        // type field exists. Since the spec seems to say that the maximum file
        // name length is 255 characters, I'm not sure why the second byte is
        // necessary.
        uint8_t name_length_low;
        uint8_t name_length_high;
        if (ifs.read(&name_length_low, sizeof(name_length_low), 1) != 1)
            // Stop if we've run out of file.
            break;
        if (ifs.read(&name_length_high, sizeof(name_length_high), 1) != 1)
            // Stop if we've run out of file.
            break;
        uint16_t name_length;
        if (type)
            name_length = name_length_low;
        else
            name_length = name_length_low +
                (static_cast<uint16_t>(name_length_high) << 8);

        // Now we have the name length. This should be a multiple of 4 with null
        // if necessary. We'll read it into a null terminated buffer and then
        // make a string of it.
        char* name_buf = new char[name_length];
        ifs.read(name_buf, sizeof(char), name_length);
        klib::string name {name_buf, name_length};
        delete[] name_buf;

        // There might be some extra space in the entry that we need to skip.
        size_t skip_length = entry_size - sizeof(entry_inode) -
            sizeof(entry_size) - sizeof(name_length) - name_length;
        if (skip_length != 0)
        {
            name_buf = new char[skip_length];
            ifs.read(name_buf, sizeof(char), skip_length);
            delete[] name_buf;
        }

        // Create the entry.
        contents.emplace_back(entry_inode, entry_size, name_length_low,
            name_length_high, name);
    }
}

/******************************************************************************/

size_t Ext2Directory::lookup(const klib::string& n) const
{
    for (const Entry& p : contents)
        if (p.name == n)
            return p.inode_index;

    return 0;
}

/******************************************************************************/

klib::vector<klib::string> Ext2Directory::ls() const
{
    klib::vector<klib::string> ret_val;
    for (const Entry& p : contents)
        ret_val.push_back(p.name);
    return ret_val;
}

/******************************************************************************
 ******************************************************************************/

Ext2FileSystem::Ext2FileSystem(const klib::string& drv) :
    FileSystem {drv}, val {true}, block_alloc {}
{
    klib::ifstream in {drv_name};
    in.seekg(superblock_loc);
    super_block = Ext2SuperBlock{in};
    in.close();
    if (super_block.signature != signature)
    {
        val = false;
        return;
    }

    if ((super_block.required_features & ~Ext2FileSystem::required_supported)
        != 0)
    {
        val = false;
        return;
    }
    if ((super_block.write_features &
        ~Ext2FileSystem::required_writing_supported) != 0)
        read_only = true;

    // Calculate and consistency check for number of block groups.
    size_t no_bg = super_block.no_blocks / super_block.blocks_per_group;
    no_bg +=
        (super_block.no_blocks % super_block.blocks_per_group == 0 ? 0 : 1);
    size_t temp = super_block.no_inodes / super_block.inodes_per_group;
    temp += (super_block.no_inodes % super_block.inodes_per_group == 0 ? 0 : 1);
    if (no_bg != temp)
    {
        val = false;
        return;
    }

    // Read the block group decriptor table. This starts in the block after the
    // superblock.
    size_t bgdt_off = 0;
    while (superblock_loc + super_block.size > bgdt_off)
        bgdt_off += (1024 << super_block.block_size_shift);
    in = klib::ifstream {drv_name};
    in.seekg(bgdt_off);
    for (size_t i = 0; i < no_bg; ++i)
        bgdt.emplace_back(in);
}

/******************************************************************************/

Directory* Ext2FileSystem::diropen(const klib::string& name)
{
    klib::pair<size_t, Ext2Inode> dir = get_inode(name);
    if (dir.first == 0 || dir.second.get_type() != Ext2Inode::directory)
        return nullptr;

    return new Ext2Directory {dir.second, dir.first, *this};
}

/******************************************************************************/

klib::FILE* Ext2FileSystem::fopen(const klib::string& name, const char* mode)
{
    klib::pair<size_t, Ext2Inode> file = get_inode(name);
    if (file.first != 0)
    {
        // File already exists.
        return new Ext2File{mode, file.second, file.first, *this};
    }
}

/******************************************************************************/

void Ext2FileSystem::rename(const klib::string& f, const klib::string& n)
{
    // TODO
    (void)f; (void)n;
}

/******************************************************************************/

klib::streamoff Ext2FileSystem::file_size(const Ext2Inode& inode) const
{
    klib::streamoff ret_val = 0;

    if ((super_block.write_features & static_cast<uint32_t>(
        required_writing_features::large_file_size)) != 0 &&
        inode.type() == Ext2Inode::file)
    {
        ret_val = inode.upper_size();
        ret_val <<= 32;
    }

    ret_val += inode.lower_size();

    return ret_val;
}

/******************************************************************************/

size_t Ext2FileSystem::inode_lookup(const Ext2Inode& inode, size_t bl) const
{
    const size_t addr_per_block = block_size() / sizeof(bl);

    // First check the direct pointers.
    if (bl < Ext2Inode::no_direct)
        return inode.direct(bl);
    bl -= Ext2Inode::no_direct;

    // Test for out of bounds.
    if (bl >= addr_per_block * (addr_per_block * (addr_per_block + 1) + 1))
        return 0;

    // Pointer address to be filled.
    size_t d_indirect;
    size_t s_indirect;

    // Start by testing the triply indirect.
    if (bl >= addr_per_block * (addr_per_block + 1))
    {
        // Return 0 if the triply indirect pointer is invalid.
        if (inode.t_indirect() == 0)
            return 0;
        // Set up a reader for the block pointed to by the triply indirect
        // pointer.
        klib::ifstream in {drv_name};
        in.seekg(block_to_byte(inode.t_indirect) +
            (bl / (addr_per_block * addr_per_block)) * sizeof(bl));
        // Read the doubly indirect pointer.
        in.read(reinterpret_cast<klib::ifstream::char_type*>(&d_indirect),
            sizeof(d_indirect) / sizeof(klib::ifstream::char_type));
        bl -= addr_per_block * addr_per_block * addr_per_block;
    }
    else
        d_indirect = inode.d_indirect();

    // Next for the doubly indirect.
    if (bl >= addr_per_block &&
        bl < addr_per_block * (addr_per_block + 1))
    {
        // Return 0 if the doubly indirect pointer is invalid.
        if (d_indirect == 0)
            return 0;
        // Set up a reader for the block pointed to by the doubly indirect
        // pointer.
        klib::ifstream in {drv_name};
        in.seekg(block_to_byte(d_indirect) +
            (bl / addr_per_block) * sizeof(bl));
        // Read the singly indirect pointer.
        in.read(reinterpret_cast<klib::ifstream::char_type*>(&s_indirect),
            sizeof(s_indirect)/ sizeof(klib::ifstream::char_type));
        bl -= addr_per_block * addr_per_block;
    }
    else
        s_indirect = inode.s_indirect();

    // Finally resolve the singly indirect.
    if (bl < addr_per_block)
    {
        // Return 0 if the singly indirect pointer is invalid.
        if (s_indirect == 0)
            return 0;
        // Set up a reader for the block pointed to by the singly indirect
        // pointer.
        klib::ifstream in {drv_name};
        in.seekg(block_to_byte(inode.s_indirect) + bl * sizeof(bl));
        // Read the block address.
        size_t ret_val;
        in.read(reinterpret_cast<klib::ifstream::char_type*>(&ret_val),
            sizeof(ret_val) / sizeof(klib::ifstream::char_type));
        return ret_val;
    }

    return 0;
}

/******************************************************************************/

int Ext2FileSystem::update_inode(const Ext2Inode& inode, size_t inode_index)
{
    // Check the index is in the exisitng range.
    if (inode_index > super_block.no_inodes || inode_index == 0)
        return -1;

    // Determine the block group, by dividing by the number of inodes per block
    // group.
    size_t bg = (index - 1) / super_block.inodes_per_group;

    // Determine the offset within the block by modding by the number of inodes
    // per block group.
    size_t offset = (index - 1) % super_block.inodes_per_group;

    // Combine the offset and the block base address to get the inode start.
    uint64_t loc = block_to_byte(bgdt[bg].inode_table) +
        offset * super_block.inode_size;

    // Create a writer for the drive.
    klib::ofstream out {drv_name};
    if (out)
        out.seekg(loc);
    else
        return -1;
    if (out)
        return (inode.write(out, super_block.inode_size) ? 0 : 1);
    else
        return -1;
}

/******************************************************************************/

void Ext2FileSystem::deallocate(size_t bl)
{
    // Do nothing if the block index is larger than the number of blocks.
    if (bl >= super_block.no_blocks)
        return;

    // Find which block group the block is in.
    size_t bl_grp = bl / 
}

/******************************************************************************/

klib::pair<size_t, Ext2Inode> Ext2FileSystem::get_inode(size_t index) const
{
    // Exit if index is greater than the number of inodes present. We can return
    // the default inode, with 0 type, as invalid. Inode indices start at 1. 0
    // is invalid.
    if (index > super_block.no_inodes || index == 0)
        return klib::pair<size_t, Ext2Inode> {0u, Ext2Inode{} };

    // Determine the block group, by dividing by the number of inodes per block
    // group.
    size_t bg = (index - 1) / super_block.inodes_per_group;

    // Determine the offset within the block by modding by the number of inodes
    // per block group.
    size_t offset = (index - 1) % super_block.inodes_per_group;

    // Combine the offset and the block base address to get the inode start.
    uint64_t loc = block_to_byte(bgdt[bg].inode_table) +
        offset * super_block.inode_size;
    klib::ifstream in {drv_name};
    in.seekg(loc);
    return klib::pair<size_t, Ext2Inode> {index, Ext2Inode {in}};
}

/******************************************************************************/

klib::pair<size_t, Ext2Inode> Ext2FileSystem::get_inode(
    const klib::string& name)
{
    // Names need to start with a /. The VFS should have provided us with a
    // name relative to the root of this file system.
    if (name[0] != '/')
        return klib::pair<size_t, Ext2Inode> {0u, Ext2Inode {}};

    // Our place in the string.
    size_t pos;
    // Skip over any /
    for (pos = 0; pos < name.size() && name[pos] == '/'; ++pos) ;
    // Inode of the directory currently being investigated.
    size_t inode_indx = root_inode;
    Ext2Inode dir_inode = get_inode(inode_indx).second;

    while (pos < name.size())
    {
        // Find the next /
        size_t end_pos = name.find('/', pos);
        // Test for npos, which means we're at the end.
        if (end_pos == klib::string::npos)
            end_pos = name.size();
        // Get the substring of the current file or directory to find.
        klib::string current = name.substr(pos, end_pos - pos);
        // Lookup the name in the current directory.
        Ext2Directory dir {dir_inode, inode_indx, *this};
        inode_indx = dir.lookup(current);
        // Test whether the file exists or not.
        if (inode_indx == 0)
            return klib::pair<size_t, Ext2Inode> {0u, Ext2Inode {}};
        // Descend to the new inode.
        dir_inode = get_inode(inode_indx).second;
        // Advance pos and skip over multiple /
        pos = end_pos;
        for (; pos < name.size() && name[pos] == '/'; ++pos) ;
    }

    return klib::pair<size_t, Ext2Inode> {inode_indx, dir_inode};
}

/******************************************************************************/

int Ext2FileSystem::flush_bgdt()
{
    // Locate the start of the BGDT, which is in the block after the super
    // block.
    size_t bgdt_off = 0;
    while (superblock_loc + super_block.size > bgdt_off)
        bgdt_off += (1024 << super_block.block_size_shift);

    // Create a writer and shift to the start of the BGDT.
    klib::ofstream out {drv_name};
    if (!out)
        return -1;
    out.seekg(bgdt_off);
    if (!out)
        return -1;

    // Write each entry. Write them unformatted.
    for (size_t i = 0; i < bgdt.size(); ++i)
    {
        bgdt.write(out);
        if (!out)
            break;
    }

    if (!out)
        return -1;

    return 0;
}

/******************************************************************************/

void Ext2FileSystem::cache_block_alloc(size_t bg_index) const;

/******************************************************************************/

void Ext2FileSystem::flush_block_alloc();

/******************************************************************************
 ******************************************************************************/

Ext2FileSystem* create_ext2(const klib::string& drv)
{
    // As a first check, see if we can find the ext2 signature in the right
    // place. The superblock always begins at byte 1024 and the signature is at
    // byte 56 in the superblock. So, the right place is 1080.
    klib::ifstream in {drv};
    in.seekg(Ext2FileSystem::superblock_loc + Ext2FileSystem::signature_loc);
    uint16_t sig;
    in.read(reinterpret_cast<klib::ifstream::char_type*>(&sig),
        sizeof(sig) / sizeof(klib::ifstream::char_type));
    in.close();
    if (sig != Ext2FileSystem::signature)
        return nullptr;

    // The signature is ok, let's go ahead and construct a file system driver.
    Ext2FileSystem* fs = new Ext2FileSystem {drv};

    // Test whether that worked.
    if (fs->valid())
        return fs;

    delete fs;
    return nullptr;
}

/******************************************************************************
 ******************************************************************************/
