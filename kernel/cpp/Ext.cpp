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
#include "Kernel.h"
#include "Logger.h"

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
    in.read(reinterpret_cast<char*>(data.data()), compulsory_size);

    // Check the amount read and the signature.
    if (!in || in.gcount() != compulsory_size || signature() != sig)
    {
        val = false;
        return;
    }

    // Check the major version and read the optional data.
    if (major_version() >= 1)
    {
        in.read(reinterpret_cast<char*>(data.data()) + compulsory_size,
            data_size - compulsory_size);
        if (!in || in.gcount() != data_size - compulsory_size)
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

    // Check the inode size is at least 128 bytes.
    if (inode_size() < 128)
    {
        val = false;
        return;
    }
}

/******************************************************************************/

klib::ostream& Ext2SuperBlock::write(klib::ostream& dest, size_t bl) const
{
    if (!dest)
        return dest;

    // Write the first 20 bytes (before the field indicating the block number).
    dest.write(reinterpret_cast<const char*>(data.data()), block_number_field);

    // Write the block number field.
    dest.write(reinterpret_cast<const char*>(&bl), block_number_size);

    // Write the rest of the data.
    const char* data_offset = reinterpret_cast<const char*>(data.data()) +
        block_number_field + block_number_size;
    if (major_version() >= 1)
        dest.write(data_offset,
            data_size - block_number_field - block_number_size);
    else
        dest.write(data_offset,
            compulsory_size - block_number_field - block_number_size);

    return dest;
}

/******************************************************************************/

void Ext2SuperBlock::uuid(const klib::string& u)
{
    if (major_version() >= 1)
        klib::memcpy(reinterpret_cast<char*>(data.data() + 104),
            u.data(), uuid_length);
}

/******************************************************************************/

void Ext2SuperBlock::name(const klib::string& n)
{
    if (major_version() >= 1)
        klib::memcpy(data.data() + 120, n.data(), name_length);
}

/******************************************************************************/

void Ext2SuperBlock::path(const klib::string& p)
{
    if (major_version() >= 1)
    {
        if (p.size() < path_length)
        {
            klib::memcpy(data.data() + 136, p.data(), p.size());
            klib::memset(data.data() + 136 + p.size(), '\0',
                path_length - p.size());
        }
        else
        {
            klib::memcpy(data.data() + 136, p.data(), path_length - 1);
            data[135 + path_length] = '\0';
        }
    }
}

/******************************************************************************
 ******************************************************************************/

BlockGroupDescriptor::BlockGroupDescriptor(klib::istream& in) : val {true}
{
    // Read in the 18 bytes that actually contain data.
    in.read(reinterpret_cast<char*>(data.data()), data_size);

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

    dest.write(reinterpret_cast<const char*>(data.data()), data_size);

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
    in.read(reinterpret_cast<char*>(data.data()), data_size);

    // Check the amount read and the signature.
    if (!in || in.gcount() != data_size)
    {
        val = false;
        return;
    }

    // Skip over any unused space, so the stream is positioned at the end of
    // the reserved space.
    in.ignore(disk_size - data_size);
    if (!in || in.gcount() != disk_size - data_size)
        val = false;
}

/******************************************************************************/

klib::ostream& Ext2Inode::write(klib::ostream& dest) const
{
    if (!dest)
        return dest;

    dest.write(reinterpret_cast<const char*>(data.data()), data_size);

    return dest;
}

/******************************************************************************/

klib::array<uint8_t, Ext2Inode::os_2_size> Ext2Inode::os_2() const
{
    klib::array<uint8_t, os_2_size> ret_val;
    klib::memcpy(ret_val.data(), data.data() + 116, os_2_size);
    return ret_val;
}

void Ext2Inode::os_2(const klib::array<uint8_t, os_2_size>& v)
{
    klib::memcpy(data.data() + 116, v.data(), os_2_size);
}

/******************************************************************************
 ******************************************************************************/

Ext2File::Ext2File(const char* m, size_t indx, Ext2FileSystem& fs) :
    DiskFile{m, fs, fs.file_size(indx)},
    inode_index{indx}
{}

/******************************************************************************/

int Ext2File::close()
{
    // Call the parent close, which will flush the writing buffer, including
    // flushing the inode, but only if it was open for writing. It will also set
    // writing to false, so we'll take a copy of that to determine whether we'll
    // we'll need to flush metadata.
    bool temp_writing = writing;
    int ret_val = DiskFile::close();

    Ext2FileSystem& ext2fs = dynamic_cast<Ext2FileSystem&>(fs);
    if (temp_writing)
    {
        // Open for writing, so we need to flush the metadata.
        ret_val = (ext2fs.flush_metadata() == -1 ? -1 : ret_val);
    }
    else
        // Open for reading. We still need to call the inode flush (which wasn't
        // done by the parent close) to remove it from the cache.
        ret_val = (ext2fs.flush_inode(inode_index, true) == -1 ? -1 : ret_val);

    inode_index = 0;
    return ret_val;
}

/******************************************************************************/

size_t Ext2File::read(void* buf, size_t size, size_t count)
{
    // Turn the file system reference into a specific ext2fs reference.
    Ext2FileSystem& ext2fs = dynamic_cast<Ext2FileSystem&>(fs);

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
        // Use the inode index to convert that to a real block address.
        size_t block = ext2fs.inode_lookup(inode_index, block_no);
        if (block == 0)
            // We've run out of file.
            break;
        // Read a whole block.
        size_t gcount = fs.read(block * bl_sz, char_buf + char_read, bl_sz);
        char_read += gcount;
        position += gcount;

        if (gcount != bl_sz)
            return char_read / size;
    }

    // Final characters.
    if (char_read < n && static_cast<klib::streamoff>(position) < sz)
    {
        // Divide by block size to get the block of the file we're in.
        size_t block_no = static_cast<klib::streamoff>(position) / bl_sz;
        // Use the inode index to convert that to a real block address.
        size_t block = ext2fs.inode_lookup(inode_index, block_no);
        if (block != 0)
        {
            // Work out the number of characters to read.
            size_t read_size = klib::min(static_cast<klib::streamoff>(bl_sz),
                sz - static_cast<klib::streamoff>(position));
            // Read into the buffer.
            size_t gcount = fs.read(block * bl_sz, buffer, read_size);
            // Work out the number of characters to copy to the output.
            read_size = klib::min(n - char_read,
                sz - static_cast<klib::streamoff>(position));
            read_size = klib::min(read_size, gcount);
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
    // Turn the file system reference into a specific ext2fs reference.
    Ext2FileSystem& ext2fs = dynamic_cast<Ext2FileSystem&>(fs);

    // Do nothing if we shouldn't be writing.
    if (!writing || size == 0 || count == 0)
        return 0;

    // Work out how many bytes to read.
    klib::streamoff n = size * count;
    const char* char_buf = static_cast<const char*>(buf);

    // Store the block size.
    size_t bl_sz = fs.block_size();
    // Number of characters written.
    klib::streamoff char_written = 0;

    // Fill the current buffer.
    size_t buf_pos = static_cast<klib::streamoff>(position) % bl_sz;
    if (buf_pos != 0)
    {
        // Work out the number of characters to write.
        size_t write_size = klib::min(
            static_cast<klib::streamoff>(bl_sz - buf_pos),
            static_cast<klib::streamoff>(n));
        klib::memcpy(buffer + buf_pos, char_buf, write_size);
        char_written = write_size;
        position += write_size;

        // Divide by block size to get the block of the file we're in.
        size_t block_index = static_cast<klib::streamoff>(position) / bl_sz;
        // Use the inode index to convert that to a real block address.
        size_t block_addr = ext2fs.inode_lookup(inode_index, block_index);
        if (block_addr == 0)
            // We've run out of file, allocate a new block. Not strictly
            // necessary, but it helps avoid a nasty situation where no blocks
            // would be available, but the write reports completion.
            block_addr = ext2fs.allocate_new_block(inode_index, block_index);

        // If we've filled the buffer, write it.
        if (static_cast<klib::streamoff>(position) % bl_sz == 0 &&
            block_addr != 0)
                fs.write(block_addr * bl_sz, buffer, bl_sz);
    }

    // Write complete blocks (avoids a copy into the buffer then to the disk).
    while (char_written + bl_sz < n)
    {
        // Divide by block size to get the block of the file we're in.
        size_t block_index = static_cast<klib::streamoff>(position) / bl_sz;
        // Use the inode index to convert that to a real block address.
        size_t block_addr = ext2fs.inode_lookup(inode_index, block_index);
        if (block_addr == 0)
            // We've run out of file, allocate a new block.
            block_addr = ext2fs.allocate_new_block(inode_index, block_index);
        if (block_addr == 0)
            // Allocation failed. Bail.
            break;
        // Write a whole block.
        fs.write(block_addr * bl_sz, char_buf + char_written, bl_sz);
        char_written += bl_sz;
        position += bl_sz;
    }

    // Write final characters into the buffer.
    if (char_written < n)
    {
        // Divide by block size to get the block of the file we're in.
        size_t block_index = static_cast<klib::streamoff>(position) / bl_sz;
        // Use the inode index to convert that to a real block address.
        size_t block_addr = ext2fs.inode_lookup(inode_index, block_index);
        if (block_addr == 0)
            // We've run out of file, allocate a new block. Not strictly
            // necessary, but it helps avoid a nasty situation where no blocks
            // would be available, but the write reports completion.
            block_addr = ext2fs.allocate_new_block(inode_index, block_index);
        if (block_addr != 0)
        {
            // Work out the number of characters to write.
            size_t write_size = n - char_written;
            // Write into the buffer.
            klib::memcpy(buffer, char_buf + char_written, write_size);
            char_written += write_size;
            position += write_size;
        }
    }

    // Update file size.
    if (position > sz)
    {
        sz = position;
        ext2fs.inode_call(inode_index, true,
            [] (Ext2Inode& in, size_t lsz)
            { in.lower_size(lsz); }, static_cast<size_t>(sz));
        if (ext2fs.inode_call(inode_index, false,
            [] (Ext2Inode& in) { return in.type(); }) == Ext2Inode::file &&
            (ext2fs.superblock_call(false, [] (Ext2SuperBlock& sb)
            { return sb.required_writing_features(); }) &
            ext2_required_writing_features::large_file_size) !=
            ext2_required_writing_features::none)
            ext2fs.inode_call(inode_index, true,
                [] (Ext2Inode& in, size_t usz)
                { in.upper_size(usz); }, static_cast<size_t>(sz >> 32));
    }

    return char_written / size;
}

/******************************************************************************/

int Ext2File::flush()
{
    if (writing)
    {
        Ext2FileSystem& ext2fs = dynamic_cast<Ext2FileSystem&>(fs);

        // Flush the writing buffer.
        size_t bl_sz = fs.block_size();
        size_t buf_pos = static_cast<klib::streamoff>(position) % bl_sz;
        if (buf_pos != 0)
        {
            // Divide by block size to get the block of the file we're in.
            size_t block_index = static_cast<klib::streamoff>(position) / bl_sz;
            // Use the inode index to convert that to a real block address.
            size_t block_addr = ext2fs.inode_lookup(inode_index, block_index);
            if (block_addr == 0)
                // We've run out of file, allocate a new block. I don't think
                // this should happen here, but it doesn't really matter if it
                // does.
                block_addr =
                    ext2fs.allocate_new_block(inode_index, block_index);
            if (block_addr != 0)
                fs.write(block_addr * bl_sz, buffer, buf_pos);

            // No need to update file position and size, that was taken care of
            // when the data was put into the buffer.
        }

        // Flush filesystem metadata.
        ext2fs.flush_inode(inode_index);
        ext2fs.flush_metadata();
    }

    return 0;
}

/******************************************************************************/

int Ext2File::seek(long offset, int origin)
{
    // Turn the file system reference into a specific ext2fs reference.
    Ext2FileSystem& ext2fs = dynamic_cast<Ext2FileSystem&>(fs);

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
            klib::streamoff read_start =
                ext2fs.inode_lookup(inode_index, new_block) * fs.block_size();
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
            klib::streamoff read_start =
                ext2fs.inode_lookup(inode_index, new_block) * fs.block_size();
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
            klib::streamoff read_start =
                ext2fs.inode_lookup(inode_index, new_block) * fs.block_size();
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
    // Turn the file system reference into a specific ext2fs reference.
    Ext2FileSystem& ext2fs = dynamic_cast<Ext2FileSystem&>(fs);

    // Do nothing if we shouldn't be writing or the size is already zero.
    if (!writing)
        return -1;
    if (sz == 0)
        return 0;

    // Cycle through the blocks in the file and deallocate them in the block
    // table. Wipe the pointers.

    // Start with the direct pointers.
    bool finished = false;
    for (size_t i = 0; i < Ext2Inode::no_direct && !finished; ++i)
    {
        finished = truncate_recursive(ext2fs.inode_call(inode_index, false,
            [] (Ext2Inode& in, size_t n) { return in.direct(n); }, i), 0);
        ext2fs.inode_call(inode_index, true,
        [] (Ext2Inode& in, size_t bl, size_t n) { in.direct(bl, n); }, 0, i);
    }

    // Now the singly indirect pointer.
    if (!finished)
    {
        finished = truncate_recursive(ext2fs.inode_call(inode_index, false,
            [] (Ext2Inode& in) { return in.s_indirect(); }), 1);
        ext2fs.inode_call(inode_index, true,
        [] (Ext2Inode& in, size_t v) { in.s_indirect(v); }, 0);
    }

    // The doubly indirect.
    if (!finished)
    {
        finished = truncate_recursive(ext2fs.inode_call(inode_index, false,
            [] (Ext2Inode& in) { return in.d_indirect(); }), 2);
        ext2fs.inode_call(inode_index, true,
        [] (Ext2Inode& in, size_t v) { in.d_indirect(v); }, 0);
    }

    // The triply indirect.
    if (!finished)
    {
        finished = truncate_recursive(ext2fs.inode_call(inode_index, false,
            [] (Ext2Inode& in) { return in.t_indirect(); }), 3);
        ext2fs.inode_call(inode_index, true,
        [] (Ext2Inode& in, size_t v) { in.t_indirect(v); }, 0);
    }

    // Set the file size to zero.
    sz = 0;

    // Update the inode.
    ext2fs.inode_call(inode_index, true,
    [] (Ext2Inode& in, size_t lsz) { in.lower_size(lsz); }, 0);
    if (ext2fs.inode_call(inode_index, false,
        [] (Ext2Inode& in) { return in.type(); }) == Ext2Inode::file &&
        (ext2fs.superblock_call(false, [] (Ext2SuperBlock& sb)
        { return sb.required_writing_features(); }) &
        ext2_required_writing_features::large_file_size) !=
        ext2_required_writing_features::none)
        ext2fs.inode_call(inode_index, true,
            [] (Ext2Inode& in, size_t usz) { in.upper_size(usz); }, 0);

    return 0;
}

/******************************************************************************/

bool Ext2File::truncate_recursive(size_t bl, size_t depth)
{
    // Don't do anything if the block is zero. Return indicating finished.
    if (bl == 0)
        return true;

    // Turn the file system reference into a specific ext2fs reference.
    Ext2FileSystem& ext2fs = dynamic_cast<Ext2FileSystem&>(fs);

    // We can call this with depth 0, for the direct pointers, but we don't need
    // to do any loops.
    if (depth == 0)
    {
        ext2fs.deallocate_block(bl);
        return false;
    }

    // Store the block size.
    size_t bl_sz = fs.block_size();
    const size_t addr_per_block = bl_sz / sizeof(size_t);

    // Create a buffer to read the block.
    char* buf = new char[bl_sz];
    bool finished = false;

    // Read the current indirect block.
    fs.read(bl * bl_sz, buf, bl_sz);
    size_t* buffer_blocks = reinterpret_cast<size_t*>(buf);
    for (size_t i = 0; i < addr_per_block && !finished; ++i)
    {
        if (buffer_blocks[i] != 0)
        {
            if (depth == 1)
                ext2fs.deallocate_block(buffer_blocks[i]);
            else
                finished = truncate_recursive(buffer_blocks[i], depth - 1);
        }
        else
            finished = true;
    }

    delete[] buf;

    // Free the block storing the indirect information.
    ext2fs.deallocate_block(bl);

    return finished;
}

/******************************************************************************
 ******************************************************************************/

Ext2Directory::Ext2Directory(size_t indx, Ext2FileSystem& fs, bool w) :
    Directory {w},
    inode_index{indx},
    ext2fs {fs},
    contents {},
    edited {false}
{
    // Check that this is actually a directory.
    if (ext2fs.inode_call(inode_index, false,
        [] (Ext2Inode& in) { return in.type(); }) != Ext2Inode::directory)
        return;

    // Determine whether the directory has a type field.
    has_type = (ext2fs.superblock_call(false, [] (Ext2SuperBlock& sb)
            { return sb.required_features(); }) &        
            ext2_required_features::directories_type) !=
            ext2_required_features::none;

    // Make a file stream for the directory data. We could do this through the
    // VFS, but that creates a lot of extra inode lookups, when we already have
    // the inode index.
    Ext2File ifs {"r", inode_index, ext2fs};

    // Process each entry.
    while (true)
    {
        size_t entry_inode;
        if (ifs.read(&entry_inode, sizeof(entry_inode), 1) != 1)
            // Stop if we've run out of file.
            break;

        // Now we read the size of the entry, used for skipping the unused bytes
        // after the file name.
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
        if (has_type)
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

int Ext2Directory::flush()
{
    // Do nothing if no edits have been made.
    if (!writing || !edited)
        return 0;

    // Open the directory as a file, which will trucnate the current data.
    Ext2File f {"w", inode_index, ext2fs};

    // Keep track of our file position, so that we don't have to keep asking the
    // file where we are.
    size_t f_pos = 0;

    // Stash the file system block size. Directory entries are not allowed to
    // span multiple blocks.
    size_t bl_sz = ext2fs.block_size();

    // Loop over the entries.
    for (auto it = contents.begin(); it != contents.end(); ++it)
    {
        // Check that we're about to write 4 byte aligned. This is just a sanity
        // check, as the previous entry should have been written so it is.
        if (f_pos % 4 != 0)
        {
            global_kernel->syslog()->warn("Ext2Directory::flush directory data not 4 byte aligned after write.\n");
            return -1;
        }

        // Adjust the entry if the size is not a multiple of 4.
        if (it->size % 4 != 0)
            it->size += 4 - it->size % 4;

        // Check that we're not about to cross a block boundary. This is just a
        // sanity check, as the previous entry should have been written to avoid
        // this.
        if (f_pos / bl_sz != (f_pos + it->size) / bl_sz)
        {
            global_kernel->syslog()->warn("Ext2Directory::flush entry will cross block boundary.\n");
            return -1;
        }

        // Note: the largest possible value for the entry size is 65535, which
        // is much larger than the typical block size (1024). We have no
        // mechanism to deal with that here, instead trusting that all Ext2
        // writers will not allow the writing of a stupidly large entry.

        // Adjust the entry size to the end of the current block if the next
        // entry will cross a block boundary.
        auto it_next = it + 1;
        if ((f_pos + it->size) / bl_sz !=
            (f_pos + it->size + it_next->size) / bl_sz)
            it->size += bl_sz - f_pos % bl_sz;

        // Write the data.
        f.write(&it->inode_index, sizeof(it->inode_index), 1);
        f.write(&it->size, sizeof(it->size), 1);
        f.write(&it->name_length_low, sizeof(it->name_length_low), 1);
        f.write(&it->name_length_high, sizeof(it->name_length_high), 1);
        f.write(it->name.data(), sizeof(decltype(it->name)::value_type),
            it->name.size());
        // Pad the rest of the entry with zeroes.
        size_t no_zeroes = it->size - sizeof(it->inode_index) -
            sizeof(it->size) - sizeof(it->name_length_low) -
            sizeof(it->name_length_high) - it->name.size();
        klib::vector<char> zeroes (no_zeroes, '\0');
        f.write(zeroes.data(), sizeof(decltype(zeroes)::value_type),
            zeroes.size());

        // Update the file position.
        f_pos += it->size;
    }

    // The file destructor is called on return, which will flush the inode and
    // file system metadata.
    edited = false;
    return 0;
}

/******************************************************************************/

int Ext2Directory::close()
{
    int ret_val = 0;

    if (writing)
        // If we've opened the directory for writing, make sure changes are sent
        // back to the disk. This will also flush the file system meta data.
        ret_val = flush();

    // Even if we didn't open for writing, we need to tell the file system
    // to flush the inode, to wipe it from the cache.
    ret_val = (ret_val == -1 ? -1 : ext2fs.flush_inode(inode_index, true));
    contents.clear();

    return ret_val;
};

/******************************************************************************/

int Ext2Directory::new_entry(size_t indx, const klib::string& name,
    Ext2Directory::type_t type)
{
    // Fail if the directory is not open for writing.
    if (!writing)
        return -1;

    // Fail if the name is empty.
    if (name == "")
        return -1;

    // Calulcate the name length.
    uint8_t name_length_low = static_cast<uint8_t>(name.size());
    uint8_t name_length_high;
    if (!has_type)
        name_length_high = static_cast<uint8_t>(name.size() >> 8);
    else
        name_length_high = static_cast<uint8_t>(type);

    // Now the size of the entry, which gets rounded up to a multiple of 4.
    uint16_t size = sizeof(inode_index) + sizeof(uint16_t) +
        sizeof(name_length_low) + sizeof(name_length_high) + name.size();
    size += 4 - size % 4;
    // Since we require directory entries to not span multiple blocks, fail if
    // the size is larger than a block.
    if (size > ext2fs.block_size())
        return -1;

    // This method does not check the inode number or allocate any blocks for
    // it. Nor does it worry about allocating new blocks for the directory data
    // or writing that data back to the disk.
    edited = true;
    contents.emplace_back(indx, size, name_length_low, name_length_high,
        name);

    return 0;
}

/******************************************************************************/

int Ext2Directory::delete_entry(size_t indx)
{
    // Fail if the directory is not open for writing.
    if (!writing)
        return -1;

    // Search for an entry matching the inode index.
    for (auto it = contents.begin(); it != contents.end(); ++it)
    {
        if (it->inode_index == indx)
        {
            // Match. Erase the entry.
            contents.erase(it);
            edited = true;
            return 0;
        }
    }

    // Search completed without a match. Failure.
    return -1;
}

int Ext2Directory::delete_entry(const klib::string& name)
{
    // Fail if the directory is not open for writing.
    if (!writing)
        return -1;

    // Search for an entry matching the name.
    for (auto it = contents.begin(); it != contents.end(); ++it)
    {
        if (it->name == name)
        {
            // Match. Erase the entry.
            contents.erase(it);
            edited = true;
            return 0;
        }
    }

    // Search completed without a match. Failure.
    return -1;
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
    FileSystem {drv}, val {true}
{
    klib::ifstream in {drv_name};
    in.seekg(superblock_loc);
    super_block = klib::pair<Ext2SuperBlock, bool> {Ext2SuperBlock{in}, false};
    in.close();
    if (!super_block.first.valid() ||
        super_block.first.signature() != signature)
    {
        val = false;
        return;
    }

    if ((super_block.first.required_features() &
        ~Ext2FileSystem::required_supported) != ext2_required_features::none)
    {
        val = false;
        return;
    }
    if ((super_block.first.required_writing_features() &
        ~Ext2FileSystem::required_writing_supported) !=
        ext2_required_writing_features::none)
    {
        read_only = true;
    }

    // Calculate and consistency check for number of block groups.
    size_t no_bg = 1 + (super_block.first.no_blocks() - 1) /
        super_block.first.blocks_per_group();
    size_t temp = 1 + (super_block.first.no_inodes() - 1) /
        super_block.first.inodes_per_group();
    if (no_bg != temp)
    {
        val = false;
        return;
    }

    // Read the block group decriptor table. This starts in the block after the
    // superblock.
    size_t block_addr = (block_size() == 1024 ? 2 : 1);
    in = klib::ifstream {drv_name};
    in.seekg(block_addr * block_size());
    for (size_t i = 0; i < no_bg; ++i)
        bgdt.emplace_back(klib::pair<BlockGroupDescriptor, bool> {in, false});
}

/******************************************************************************/

Directory* Ext2FileSystem::diropen(const klib::string& name)
{
    size_t dir = get_inode_index(name);
    if (dir == 0 || inode_call(dir, false,
        [] (Ext2Inode& in) { return in.type(); }) != Ext2Inode::directory)
        return nullptr;

    return new Ext2Directory {dir, *this};
}

/******************************************************************************/

klib::FILE* Ext2FileSystem::fopen(const klib::string& name, const char* mode)
{
    size_t file = get_inode_index(name);
    if (file != 0)
    {
        // File already exists.
        return new Ext2File {mode, file, *this};
    }

    // If the file does not exist, opening in "r" or "r+" is an error.
    if (klib::strcmp(mode, "r") == 0 || klib::strcmp(mode, "r+") == 0)
        return nullptr;

    // Otherwise we need to create a new file. Start by getting the directory
    // under which to make it.
    size_t last_slash = name.find_last_of('/'); 
    klib::string dir_name {name.substr(0, last_slash)};
    klib::string file_name {name.substr(last_slash + 1)};
    size_t dir = get_inode_index(dir_name);

    if (dir == 0)
        // The inode lookup failed, probably because the parent directory does
        // not exist. This is an error.
        return nullptr;

    // Create a new inode for the file.
    size_t new_inode_index = allocate_new_inode(dir);
    if (new_inode_index == 0)
        // We failed to create a new inode, probably because the file system is
        // full. This is an error.
        return nullptr;

    // Add starting data to the new inode. Every field was default initialised
    // to zero.
    inode_call(new_inode_index, true, [] (Ext2Inode& in, Ext2Inode::type_t t)
        { in.type(t); }, Ext2Inode::file);
    inode_call(new_inode_index, true, [] (Ext2Inode& in, uint16_t n)
        { in.hard_links(n); }, 1);
    inode_call(new_inode_index, true, [] (Ext2Inode& in, bool v)
        { in.valid(v); }, true);

    // Open the directory and add the new entry. The directory will be closed on
    // return, which will flush the file system metadata.
    Ext2Directory ext2dir {dir, *this, true};
    int ret_val = ext2dir.new_entry(new_inode_index, file_name);

    global_kernel->syslog()->info("Ext2FileSystem::fopen added new inode to directory, ret_val = %d\n", ret_val);

    // Adding the file to the directory could still fail, if for example a new
    // block needs to be allocated and we've run out of room.
    if (ret_val == 0)
        // Success. Return the new file.
        return new Ext2File {mode, new_inode_index, *this};

    // If we've got here without returning, we've failed.
    return nullptr;
}

/******************************************************************************/

void Ext2FileSystem::rename(const klib::string& f, const klib::string& n)
{
    // TODO
    (void)f; (void)n;
}

/******************************************************************************/

klib::streamoff Ext2FileSystem::file_size(size_t inode_index)
{
    klib::streamoff ret_val = 0;

    if ((super_block.first.required_writing_features() & 
        ext2_required_writing_features::large_file_size) !=
        ext2_required_writing_features::none &&
        inode_call(inode_index, false, [] (Ext2Inode& in) { return in.type(); })
        == Ext2Inode::file)
    {
        ret_val = inode_call(inode_index, false, [] (Ext2Inode& in)
            { return in.upper_size(); });
        ret_val <<= 32;
    }

    ret_val += inode_call(inode_index, false, [] (Ext2Inode& in)
            { return in.lower_size(); });

    return ret_val;
}

/******************************************************************************/

size_t Ext2FileSystem::inode_lookup(size_t inode_index, size_t bl)
{
    const size_t addr_per_block = block_size() / sizeof(bl);

    // First check the direct pointers.
    if (bl < Ext2Inode::no_direct)
        return inode_call(inode_index, false, [] (Ext2Inode& in, size_t n)
            { return in.direct(n); }, bl);
    bl -= Ext2Inode::no_direct;

    // Test for out of bounds.
    if (bl >= addr_per_block * (addr_per_block * (addr_per_block + 1) + 1))
        return 0;

    // Pointer address to be filled.
    size_t t_indirect = 0;
    size_t d_indirect = 0;
    size_t s_indirect = 0;

    // Start by testing the triply indirect.
    if (bl >= addr_per_block * (addr_per_block + 1))
    {
        // Get rid of the sinlgy and double indirect blocks from the index.
        bl -= addr_per_block * (addr_per_block + 1);

        // Fetch the triply indirect pointer value.
        size_t t_indirect = inode_call(inode_index, false,
            [](Ext2Inode& in) { return in.t_indirect(); });
        // Return 0 if the triply indirect pointer is invalid.
        if (t_indirect == 0)
            return 0;
        // Set up a reader for the block pointed to by the triply indirect
        // pointer.
        klib::ifstream in {drv_name};
        in.seekg(block_to_byte(t_indirect) + bl /
            (addr_per_block * addr_per_block) * sizeof(bl));
        // Read the doubly indirect pointer.
        in.read(reinterpret_cast<klib::ifstream::char_type*>(&d_indirect),
            sizeof(d_indirect) / sizeof(klib::ifstream::char_type));
        bl %= (addr_per_block * addr_per_block);
    }
    else
        d_indirect = inode_call(inode_index, false,
            [](Ext2Inode& in) { return in.d_indirect(); });

    // Next for the doubly indirect.
    if ((bl >= addr_per_block &&
        bl < addr_per_block * (addr_per_block + 1)) || t_indirect != 0)
    {
        // If we didn't have a triply indirect, get rid of the singly indirect
        // blocks from the index.
        if (t_indirect == 0)
            bl -= addr_per_block;

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
        bl %= addr_per_block;
    }
    else
        s_indirect = inode_call(inode_index, false,
            [](Ext2Inode& in) { return in.s_indirect(); });

    // Finally resolve the singly indirect.
    // Return 0 if the singly indirect pointer is invalid.
    if (s_indirect == 0)
        return 0;
    // Set up a reader for the block pointed to by the singly indirect
    // pointer.
    klib::ifstream in {drv_name};
    in.seekg(block_to_byte(s_indirect) + bl * sizeof(bl));
    // Read the block address.
    size_t ret_val;
    in.read(reinterpret_cast<klib::ifstream::char_type*>(&ret_val),
        sizeof(ret_val) / sizeof(klib::ifstream::char_type));
    return ret_val;

    return 0;
}

/******************************************************************************/

int Ext2FileSystem::inode_set(size_t inode_index, size_t bl_index,
    size_t bl_addr)
{
    // Fail immediately if the file system is read only.
    if (ro())
        return -1;

    const size_t addr_per_block = block_size() / sizeof(bl_index);

    // First check the direct pointers.
    if (bl_index < Ext2Inode::no_direct)
    {
        inode_call(inode_index, true, [] (Ext2Inode& in, size_t bl, size_t n)
            { in.direct(bl, n); }, bl_addr, bl_index);
        return 0;
    }
    bl_index -= Ext2Inode::no_direct;


    // Test for out of bounds.
    if (bl_index >=
        addr_per_block * (addr_per_block * (addr_per_block + 1) + 1))
        return -1;

    // Pointer address to be filled.
    size_t t_indirect = 0;
    size_t d_indirect = 0;
    size_t s_indirect = 0;

    // Save the intermediate indices in case we need to allocate new pointers.
    size_t d_index = 0;
    size_t s_index = 0;

    // Start by testing the triply indirect.
    if (bl_index >= addr_per_block * (addr_per_block + 1))
    {
        // Get rid of the sinlgy and double indirect blocks from the index.
        bl_index -= addr_per_block * (addr_per_block + 1);

        // Fetch the triply indirect pointer value.
        t_indirect = inode_call(inode_index, false,
            [](Ext2Inode& in) { return in.t_indirect(); });
        if (t_indirect == 0)
        {
            // We need to allocate a new block of doubly-indirect pointers.
            t_indirect = allocate_new_block(inode_index, 0, true);
            if (t_indirect == 0)
                // The allocation failed. Return failure.
                return -1;
            // Update the inode record with the new indirect block.
            inode_call(inode_index, true,
                [](Ext2Inode& in, size_t t) { in.t_indirect(t); }, t_indirect);
            // Write zeroes to the new block so that we don't accidentally
            // hijack other file's blocks.
            // Set up a writer for the new block.
            klib::ofstream out {drv_name};
            out.seekp(block_to_byte(t_indirect));
            if (!out)
                return -1;
            // Write the zeroes.
            for (size_t i = 0;
                i < block_size() / sizeof(klib::ofstream::char_type); ++i)
                out.put('\0');
            if (!out)
                return -1;
        }
        // Set up a reader for the block pointed to by the triply indirect
        // pointer.
        klib::ifstream in {drv_name};
        d_index = bl_index / (addr_per_block * addr_per_block);
        in.seekg(block_to_byte(t_indirect) + d_index * sizeof(bl_index));
        // Read the doubly indirect pointer.
        in.read(reinterpret_cast<klib::ifstream::char_type*>(&d_indirect),
            sizeof(d_indirect) / sizeof(klib::ifstream::char_type));
        bl_index %= (addr_per_block * addr_per_block);
    }
    else
        d_indirect = inode_call(inode_index, false,
            [](Ext2Inode& in) { return in.d_indirect(); });

    // Next for the doubly indirect.
    if ((bl_index >= addr_per_block &&
        bl_index < addr_per_block * (addr_per_block + 1)) || t_indirect != 0)
    {
        // If we didn't have a triply indirect, get rid of the singly indirect
        // blocks from the index.
        if (t_indirect == 0)
            bl_index -= addr_per_block;

        if (d_indirect == 0)
        {
            // We need to allocate a new block of singly-indirect pointers.
            d_indirect = allocate_new_block(inode_index, 0, true);
            if (d_indirect == 0)
                // The allocation failed. Return failure.
                return -1;
            if (t_indirect == 0)
                // Update the inode record with the new indirect block.
                inode_call(inode_index, true, [](Ext2Inode& in, size_t d)
                    { in.d_indirect(d); }, d_indirect);
            else
            {
                // Update the triply indirect block with the new doubly indirect
                // pointer.
                // Set up a writer for the device.
                klib::ofstream out {drv_name};
                // Offset to the correct block.
                out.seekp(block_to_byte(t_indirect) +
                    d_index * sizeof(bl_index));
                if (!out)
                    // The write failed. Return failure.
                    return -1;
            }
            // Write zeroes to the new block so that we don't accidentally
            // hijack other file's blocks.
            // Set up a writer for the new block.
            klib::ofstream out {drv_name};
            out.seekp(block_to_byte(d_indirect));
            if (!out)
                return -1;
            // Write the zeroes.
            for (size_t i = 0;
                i < block_size() / sizeof(klib::ofstream::char_type); ++i)
                out.put('\0');
            if (!out)
                return -1;
        }
        // Set up a reader for the block pointed to by the doubly indirect
        // pointer.
        klib::ifstream in {drv_name};
        size_t s_index = bl_index / addr_per_block;
        in.seekg(block_to_byte(d_indirect) + s_index * sizeof(bl_index));
        // Read the singly indirect pointer.
        in.read(reinterpret_cast<klib::ifstream::char_type*>(&s_indirect),
            sizeof(s_indirect)/ sizeof(klib::ifstream::char_type));
        bl_index %= addr_per_block;
    }
    else
        s_indirect = inode_call(inode_index, false,
            [](Ext2Inode& in) { return in.s_indirect(); });

    // Finally resolve the singly indirect.
    if (s_indirect == 0)
    {
        // We need to allocate a new block of direct pointers.
        s_indirect = allocate_new_block(inode_index, 0, true);
        if (s_indirect == 0)
            // The allocation failed. Return failure.
            return -1;
        if (d_indirect == 0)
            // Update the inode record with the new indirect block.
            inode_call(inode_index, true, [](Ext2Inode& in, size_t s)
                { in.s_indirect(s); }, s_indirect);
        else
        {
            // Update the dobly indirect block with the new singly indirect
            // pointer.
            // Set up a writer for the device.
            klib::ofstream out {drv_name};
            // Offset to the correct block.
            out.seekp(block_to_byte(d_indirect) +
                s_index * sizeof(bl_index));
            if (!out)
                // The write failed. Return failure.
                return -1;
        }
        // Write zeroes to the new block so that we don't accidentally
        // hijack other file's blocks.
        // Set up a writer for the new block.
        klib::ofstream out {drv_name};
        out.seekp(block_to_byte(s_indirect));
        if (!out)
            return -1;
        // Write the zeroes.
        for (size_t i = 0;
            i < block_size() / sizeof(klib::ofstream::char_type); ++i)
            out.put('\0');
        if (!out)
            return -1;
    }
    // Set up a writer for the block pointed to by the singly indirect
    // pointer.
    klib::ofstream out {drv_name};
    out.seekp(block_to_byte(s_indirect) + bl_index * sizeof(bl_index));
    if (!out)
        return -1;
    // Write the block address.
    out.write(reinterpret_cast<klib::ofstream::char_type*>(&bl_addr),
        sizeof(bl_addr) / sizeof(klib::ofstream::char_type));
    if (!out)
        return -1;

    return 0;
}

/******************************************************************************/

int Ext2FileSystem::flush_inode(size_t inode_index, bool free)
{
    // Immediately suceed if the inode is not cached. This hopefully means
    // it's already been freed by another process.
    if (inodes.count(inode_index) == 0)
        return 0;

    bool success = true;

    // We only need to write back to disk if the inode has been modified.
    if (inodes[inode_index].second)
    {
        // Determine the block group, by dividing by the number of inodes per
        // block group.
        size_t bg = (inode_index - 1) / super_block.first.inodes_per_group();

        // Determine the offset within the block by modding by the number of
        // inodes per block group.
        size_t offset = (inode_index - 1) %
            super_block.first.inodes_per_group();

        // Combine the offset and the block base address to get the inode start.
        uint64_t loc = block_to_byte(bgdt[bg].first.inode_table()) +
            offset * super_block.first.inode_size();

        // Create a writer for the drive.
        klib::ofstream out {drv_name};
        out.seekp(loc);
        if (!out)
            return -1;

        // Do the write. There is a small possibility that another process will
        // have already flushed the inode, in which case the element in inodes
        // won't exist. We use bounds checked access and catch the exception to
        // deal with that. It counts as a success, as the work is already done.
        if (out)
            try {
                success = inodes.at(inode_index).first.write(out).good();
            }
            catch (klib::out_of_range&)
            {}
        else
            return -1;
    }

    if (success && free)
        // Delete the entry from the cached inodes.
        inodes.erase(inode_index);
    else if (success)
        // Set the inode to unmodified.
        inodes[inode_index].second = false;

    return (success ? 0 : -1);
}

/******************************************************************************/

void Ext2FileSystem::access_block_alloc(size_t bg_index, size_t index,
    bool alloc)
{
    // Cache the table.
    if (cache_block_alloc(bg_index) != 0)
        return;

    // Set the table to modified.
    block_alloc[bg_index].second = true;

    // Edit the data.
    if (alloc)
        block_alloc[bg_index].first[index / 8] |= (1 << (index%8));
    else
        block_alloc[bg_index].first[index / 8] &= ~(1 << (index%8));
}


bool Ext2FileSystem::access_block_alloc(size_t bg_index, size_t index)
{
    // Cache the table.
    if (cache_block_alloc(bg_index) != 0)
        return true;

    // Read the data.
    return block_alloc[bg_index].first[index / 8] & (1 << (index%8));
}

/******************************************************************************/

void Ext2FileSystem::access_inode_alloc(size_t bg_index, size_t index,
    bool alloc)
{
    // Cache the table.
    if (cache_inode_alloc(bg_index) != 0)
        return;

    // Set the table to modified.
    inode_alloc[bg_index].second = true;

    // Edit the data.
    if (alloc)
        inode_alloc[bg_index].first[index / 8] |= (1 << (index%8));
    else
        inode_alloc[bg_index].first[index / 8] &= ~(1 << (index%8));
}


bool Ext2FileSystem::access_inode_alloc(size_t bg_index, size_t index)
{
    // Cache the table.
    if (cache_inode_alloc(bg_index) != 0)
        return true;

    // Read the data.
    return inode_alloc[bg_index].first[index / 8] & (1 << (index%8));
}

/******************************************************************************/

int Ext2FileSystem::flush_metadata()
{
    return (flush_superblock() == 0) && (flush_bgdt() == 0) &&
        (flush_block_alloc() == 0) && (flush_inode_alloc() == 0) ? 0 : -1;
}

/******************************************************************************/

size_t Ext2FileSystem::allocate_new_block(size_t inode_index, size_t bl_index,
    bool indirect_block)
{
    // Fail immediately if the superblock says the file system is full or the
    // file system is read only.
    if (ro() || super_block.first.unalloc_blocks() == 0)
        return 0;

    // Find the block group the inode is in.
    size_t inode_bl_grp = (inode_index - 1) /
        super_block.first.inodes_per_group();

    size_t search_start = 0;
    size_t search_start_bl_grp = 0;
    if (bl_index != 0 && !indirect_block)
    {
        // Use the provided bl_index (which is for the new block), to work out
        // the location of the previous block. The conditional + 1 is because
        // the zeroth block does not count as part of the zeroth block group if
        // it only contains the boot block. 
        search_start = inode_lookup(inode_index, bl_index - 1);
        search_start_bl_grp = (bl_index + (block_size() == 1024 ? 1 : 0)) /
            super_block.first.blocks_per_group();
    }

    // If we find a free block, new_block gets set to the index within the block
    // group and new_block_group gets set to the block group.
    bool found = false;
    size_t new_block = 0;
    size_t new_block_group = 0;
    if (search_start_bl_grp == inode_bl_grp &&
        bgdt[inode_bl_grp].first.unalloc_blocks() != 0)
    {
        // The two block groups match. We start searching at the previous
        // allocated block.
        for (new_block = search_start + 1;
            new_block < super_block.first.blocks_per_group(); ++new_block)
        {
            if (!access_block_alloc(inode_bl_grp, new_block))
            {
                found = true;
                new_block_group = inode_bl_grp;
                break;
            }
        }
        // If we haven't found a free one, wrap to the start of the block group.
        if (!found)
        {
            for (new_block = 0; new_block < search_start; ++new_block)
            {
                if (!access_block_alloc(inode_bl_grp, new_block))
                {
                    found = true;
                    new_block_group = inode_bl_grp;
                    break;
                }
            }

        }
    }
    else
    {
        // The two block groups do not match. Search the block group of the
        // inode first.
        if (bgdt[inode_bl_grp].first.unalloc_blocks() != 0)
        {
            for (new_block = 0;
                new_block < super_block.first.blocks_per_group(); ++new_block)
            {
                if (!access_block_alloc(inode_bl_grp, new_block))
                {
                    found = true;
                    new_block_group = inode_bl_grp;
                    break;
                }
            }
        }
        // If we haven't found a free one, search the rest of the group the last
        // allocated block was in.
        if (bgdt[search_start_bl_grp].first.unalloc_blocks() != 0)
        {
            for (new_block = search_start + 1;
                new_block < super_block.first.blocks_per_group(); ++new_block)
            {
                if (!access_block_alloc(search_start_bl_grp, new_block))
                {
                    found = true;
                    new_block_group = search_start_bl_grp;
                    break;
                }
            }
            // If we still haven't found a free one, wrap to the start of the
            // block group.
            for (new_block = 0; new_block < search_start; ++new_block)
            {
                if (!access_block_alloc(search_start_bl_grp, new_block))
                {
                    found = true;
                    new_block_group = search_start_bl_grp;
                    break;
                }
            }
        }
    }

    // If we've got here without finding a free block, start searching the rest
    // of the block groups.
    if (!found)
    {
        size_t no_bg = 1 + (super_block.first.no_blocks() - 1) /
            super_block.first.blocks_per_group();
        for (size_t bg = 0; bg < no_bg; ++bg)
        {
            // Skip if we've already searched it.
            if (bg == inode_bl_grp || bg == search_start_bl_grp)
                continue;

            // Skip if the block group is full.
            if (bgdt[bg].first.unalloc_blocks() == 0)
                continue;

            // Search the table for a free block.
            for (new_block = 0;
                new_block < super_block.first.blocks_per_group(); ++new_block)
            {
                if (!access_block_alloc(bg, new_block))
                {
                    found = true;
                    new_block_group = bg;
                    break;
                }
            }
        }
    }

    if (!found)
        // We've tried everything and failed to find an unallocated block. Bail.
        return 0;

    // Calculate the actual block address (new block is the index within the
    // block group. The conditional + 1 is because the zeroth block does not
    // count as part of the zeroth block group if it only contains the boot
    // block.
    size_t block_addr = new_block_group * super_block.first.blocks_per_group() +
        new_block + (block_size() == 1024 ? 1 : 0);

    // Sanity check that the block found is within the file system.
    if (block_addr >= super_block.first.no_blocks())
    {
        global_kernel->syslog()->warn(
            "Ext2FileSystem::allocate block to allocate is out of bounds\n");
        return 0;
    }

    // We've found something. Update the allocation table, group descriptor and
    // superblock. We must do this before setting the inode, as the inode might
    // need more blocks for indirect pointers.
    access_block_alloc(new_block_group, new_block, true);
    bgdt_call(new_block_group, true, [] (BlockGroupDescriptor& bgd)
        { bgd.unalloc_blocks(bgd.unalloc_blocks() - 1); });
    superblock_call(true, [] (Ext2SuperBlock& sb)
        { sb.unalloc_blocks(sb.unalloc_blocks() - 1); });

    // Set the inode pointer. It is possible to fail this, if the set requires
    // additional indirect blocks that there is not space for. We don't do this
    // for an indirect block allocation, as the indirect pointers will be set by
    // inode_set().
    if (!indirect_block && inode_set(inode_index, bl_index, block_addr) == -1)
    {
        // The inode set failed. Change the allocation back to unused.
        access_block_alloc(new_block_group, new_block, false);
    bgdt_call(new_block_group, true, [] (BlockGroupDescriptor& bgd)
        { bgd.unalloc_blocks(bgd.unalloc_blocks() + 1); });
        superblock_call(true, [] (Ext2SuperBlock& sb)
            { sb.unalloc_blocks(sb.unalloc_blocks() + 1); });
        return 0;
    }

    return block_addr;
}

/******************************************************************************/

int Ext2FileSystem::deallocate_block(size_t bl)
{
    // Do nothing if the block index is larger than the number of blocks.
    if (bl >= super_block.first.no_blocks())
        return -1;

    // Find which block group the block is in, and the index in the group. The
    // conditional + 1 is because the zeroth block does not count as part of the
    // zeroth block group if it only contains the boot block. 
    size_t bl_grp = (bl + (block_size() == 1024 ? 1 : 0)) /
        super_block.first.blocks_per_group();
    size_t bl_indx = (bl + (block_size() == 1024 ? 1 : 0)) %
        super_block.first.blocks_per_group();

    // Check whether it was already deallocated.
    bool change = access_block_alloc(bl_grp, bl_indx);

    if (change)
    {
        // Set the block to unalloctaed in the bitmap.
        access_block_alloc(bl_grp, bl_indx, false);
        // Increase the number of unallocated blocks in the Block Descriptor.
        bgdt_call(bl_grp, true, [] (BlockGroupDescriptor& bgd)
            { bgd.unalloc_blocks(bgd.unalloc_blocks() - 1); });
        // Increase the number of unallocated blocks in the Superblock.
        superblock_call(true, [] (Ext2SuperBlock& sb)
            { sb.unalloc_blocks(sb.unalloc_blocks() + 1); });
    }

    return 0;
}

/******************************************************************************/

size_t Ext2FileSystem::allocate_new_inode(size_t inode_index)
{
    // Fail immediately if the superblock says the file system is full or the
    // file system is read only.
    if (ro() || super_block.first.unalloc_inodes() == 0)
        return 0;

    // Determine which block group the parent inode is in, remembering that
    // inode 0 does not exist.
    size_t ipg = super_block.first.inodes_per_group();
    size_t no_bg = 1 + (super_block.first.no_inodes() - 1) / ipg;
    size_t inode_bl_grp = (inode_index - 1) / ipg;
        
    // found is set to true when we find something. new_index becomes the offset
    // within the group of the new inode. new_index_group becomes the group of
    // the new inode.
    bool found = false;
    size_t new_index = 0;
    size_t new_block_group = inode_bl_grp;

    // Loop over the block groups from the search start to the end, then from 0
    // to the search start.
    while (!found)
    {
        // Skip if there are no free inodes in this group.
        if (bgdt[new_block_group].first.unalloc_inodes() == 0)
            continue;

        // Search the allocation table. Start the search at the first
        // non-reserved inode for the first group, or 0 for other groups.
        new_index =
            (new_block_group == 0 ? super_block.first.first_inode() : 0);
        for (; new_index < ipg; ++new_index)
        {
            if (!access_inode_alloc(new_block_group, new_index))
            {
                found = true;
                break;
            }
        }

        // If we found something, we need to break before changing the group.
        if (found)
        {
            global_kernel->syslog()->info("Ext2FileSystem::allocate_new_inode free inode at index %u of group %u\n", new_index, new_block_group);
            break;
        }

        ++new_block_group;
        new_block_group %= no_bg;
        if (new_block_group == inode_bl_grp)
            break;
    }

    // Return failure if we didn't find anything.
    if (!found)
        return 0;

    // We've found something. Update the allocation table, group descriptor and
    // superblock.
    access_inode_alloc(new_block_group, new_index, true);
    bgdt_call(new_block_group, true, [] (BlockGroupDescriptor& bgd)
        { bgd.unalloc_inodes(bgd.unalloc_inodes() - 1); });
    superblock_call(true, [] (Ext2SuperBlock& sb)
        { sb.unalloc_inodes(sb.unalloc_inodes() - 1); });

    // Work out the index. Remember to add 1, since 0 is not a valid inode.
    size_t ret_val = new_block_group * ipg + new_index + 1;

    // Default initialise an inode and add it to the cache table. The inode is
    // created in an invalid state. It is up to the caller to set initial data
    // appropriately.
    inodes.emplace(ret_val, klib::pair<Ext2Inode, bool> {Ext2Inode{}, false});

    return ret_val;
}

/******************************************************************************/

int Ext2FileSystem::deallocate_inode(size_t indx)
{
    // Do nothing if the inode index is larger than the number of inodes.
    if (indx >= super_block.first.no_inodes())
        return -1;

    // Find which block group the block is in, and the index in the group. The
    // -1 is because the zroth inode does ot exist.
    size_t bl_grp = (indx - 1) / super_block.first.inodes_per_group();
    size_t bl_indx = (indx - 1) % super_block.first.inodes_per_group();
    // Check whether it was already deallocated.
    bool change = access_inode_alloc(bl_grp, bl_indx);

    if (change)
    {
        // Open the file in "w" mode, which truncates it and ensures all the
        // file's data blocks get deallocated.
        Ext2File f {"w", indx, *this};
        f.close();

        // Set the inode to unalloctaed in the bitmap.
        access_inode_alloc(bl_grp, bl_indx, false);
        // Increase the number of unallocated inodes in the Block Descriptor.
        bgdt_call(bl_grp, true, [] (BlockGroupDescriptor& bgd)
            { bgd.unalloc_inodes(bgd.unalloc_blocks() - 1); });
        // Increase the number of unallocated blocks in the Superblock.
        superblock_call(true, [] (Ext2SuperBlock& sb)
            { sb.unalloc_inodes(sb.unalloc_inodes() + 1); });
    }

    return 0;
}

/******************************************************************************/

int Ext2FileSystem::flush_superblock()
{
    // Whether to write the superblock. If it hasn't been modified, we don't
    // need to write it.
    if (!super_block.second)
        return 0;

    // Create a writer.
    klib::ofstream out {drv_name};
    if (!out)
        return -1;

    // Take a copy of the superblock. That way, if it gets updated halfway
    // through this call, it won't get written in a different state to different
    // backup copies.
    Ext2SuperBlock sb_copy {super_block.first};
    super_block.second = false;

    // Cycle over the block groups.
    for (size_t i = 0;
        i < 1 + (sb_copy.no_blocks() - 1) /  sb_copy.blocks_per_group();
        ++i)
    {
        // If using the sparse feature, backups are only present for i = 0, 1
        // and powers of 3, 5 and 7. Skip this block groups if using sparse and
        // the block group doesn't match.
        if ((sb_copy.required_writing_features() &
            ext2_required_writing_features::sparse) !=
            ext2_required_writing_features::none && !backup_group(i))
            continue;

        size_t block_addr = 0;
        if (i == 0)
            // The first superblock is always located at 1024 bytes.
            out.seekp(superblock_loc);
        else
        {
            // The superblock backup is the first block of the block group. The
            // conditional +1 comes from the presence of the boot block before
            // the start of the first block group. If the block size is 1024,
            // that's a whole block. If the block size is anything else (which
            // must be larger) the first superblock is still in block 0 and
            // block 0 counts as part of the first block group.
            block_addr = i * sb_copy.blocks_per_group() +
                (block_size() == 1024 ? 1 : 0);
            out.seekp(block_addr * block_size());
        }
        if (!out)
            return -1;

        // Write the data. Provide the block number, for editing the superblock
        // address.
        sb_copy.write(out, block_addr);
        if (!out)
            return -1;
    }

    return 0;
}

/******************************************************************************/

int Ext2FileSystem::flush_bgdt()
{
    // Create a writer.
    klib::ofstream out {drv_name};
    if (!out)
        return -1;

    // Cycle over the block groups.
    for (size_t i = 0;
        i < 1 + (super_block.first.no_blocks() - 1) /
        super_block.first.blocks_per_group(); ++i)
    {
        // If using the sparse feature, backups are only present for i = 0, 1
        // and powers of 3, 5 and 7. Skip this block groups if using sparse and
        // the block group doesn't match.
        if ((super_block.first.required_writing_features() &
            ext2_required_writing_features::sparse) !=
            ext2_required_writing_features::none && !backup_group(i))
            continue;

        // Locate the start of the BGDT, which is in the block after the
        // superblock. The conditional +1 comes from the presence of the boot
        // block before the start of the first block group. If the block size
        // is 1024, that's a whole block. If the block size is anything else
        // (which must be larger) the first superblock is still in block 0 and
        // sblock 0 counts as part of the first block group.
        size_t block_addr = i * super_block.first.blocks_per_group() + 1 +
            (block_size() == 1024 ? 1 : 0);

        // Shift the stream to the right location.
        out.seekp(block_addr * block_size());
        if (!out)
            return -1;

        // Write each entry. Write them unformatted.
        for (size_t j = 0; j < bgdt.size(); ++j)
        {
            // Skip this entry if it has not been modified.
            if (!bgdt[j].second)
                continue;

            bgdt[j].first.write(out);
            if (!out)
                return -1;
        }
    }

    return 0;
}

/******************************************************************************/

int Ext2FileSystem::cache_block_alloc(size_t bg_index)
{
    // Check whether the block allocation table is already cached.
    if (block_alloc.count(bg_index) != 0)
        return 0;

    // Get the block address of the allocation table.
    size_t block_addr = bgdt[bg_index].first.block_map();

    // Create a reader and shift to the start of the allocation table.
    klib::ifstream in {drv_name};
    if (!in)
        return -1;
    in.seekg(block_addr * block_size());
    if (!in)
        return -1;

    // Create a cache entry of the right size.
    klib::pair<typename klib::map<size_t,
        klib::pair<klib::vector<char>, bool>>::iterator, bool> p
        = block_alloc.emplace(bg_index, klib::pair<klib::vector<char>, bool> {
        klib::vector<char> (block_size()), false});
    // p.second is a bool indicating whether the emplacement succeeded. If it
    // did, p.first is an iterator to the new element.
    if (!p.second)
        return -1;

    // Read the data into the new element.
    in.read(p.first->second.first.data(), block_size());

    // Check for success.
    if (!in || in.gcount() != block_size())
    {
        block_alloc.erase(p.first);
        return -1;
    }

    return 0;
}

/******************************************************************************/

int Ext2FileSystem::flush_block_alloc()
{
    // Cycle through the cached block allocation tables.
    auto it = block_alloc.begin();
    while (it != block_alloc.end()) 
    {
        // Only write if the table has been modified.
        if (it->second.second)
        {
            // Create a writer.
            klib::ofstream out {drv_name};
            if (!out)
            {
                ++it;
                continue;
            }
            // Get the block address of the start of the table.
            size_t block_addr = bgdt[it->first].first.block_map();
            // Shift the stream to the start of the table.
            out.seekp(block_addr * block_size());
            if (!out)
            {
                ++it;
                continue;
            }

            // Do the write.
            out.write(it->second.first.data(), block_size());
            if (!out)
            {
                ++it;
                continue;
            }
        }
        // Erase the entry.
        it = block_alloc.erase(it);
    }

    // Check whether we managed to flush all the allocation tables.
    return (block_alloc.empty() ? 0 : -1);
}

/******************************************************************************/

int Ext2FileSystem::cache_inode_alloc(size_t bg_index)
{
    // Check whether the inode allocation table is already cached.
    if (inode_alloc.count(bg_index) != 0)
        return 0;

    // Get the block address of the allocation table.
    size_t block_addr = bgdt[bg_index].first.inode_map();

    // Create a reader and shift to the start of the allocation table.
    klib::ifstream in {drv_name};
    if (!in)
        return -1;
    in.seekg(block_addr * block_size());
    if (!in)
        return -1;

    // Create a cache entry of the right size.
    klib::pair<typename klib::map<size_t,
        klib::pair<klib::vector<char>, bool>>::iterator, bool> p
        = inode_alloc.emplace(bg_index, klib::pair<klib::vector<char>, bool> {
        klib::vector<char> (block_size()), false});
    // p.second is a bool indicating whether the emplacement succeeded. If it
    // did, p.first is an iterator to the new element.
    if (!p.second)
        return -1;

    // Read the data into the new element.
    // Check the position in the file.
    in.read(inode_alloc[bg_index].first.data(), block_size());


    // Check for success.
    if (!in || in.gcount() != block_size())
    {
        global_kernel->syslog()->warn("Ext2FileSystem::cache_inode_alloc failed to read inode allocation table\n");
        inode_alloc.erase(p.first);
        return -1;
    }

    return 0;
}

/******************************************************************************/

int Ext2FileSystem::flush_inode_alloc()
{
    // Cycle through the cached inode allocation tables.
    auto it = inode_alloc.begin();
    while (it != inode_alloc.end()) 
    {
        // Only write if the table has been modified.
        if (it->second.second)
        {
            // Create a writer.
            klib::ofstream out {drv_name};
            if (!out)
            {
                ++it;
                continue;
            }
            // Get the block address of the start of the table.
            size_t block_addr = bgdt[it->first].first.inode_map();
            // Shift the stream to the start of the table.
            out.seekp(block_addr * block_size());
            if (!out)
            {
                ++it;
                continue;
            }

            // Do the write.
            out.write(it->second.first.data(), block_size());
            if (!out)
            {
                ++it;
                continue;
            }
        }
        // Erase the entry.
        it = inode_alloc.erase(it);
    }

    // Check whether we managed to flush all the allocation tables.
    return (inode_alloc.empty() ? 0 : -1);
}

/******************************************************************************/

int Ext2FileSystem::cache_inode(size_t index)
{
    // Exit with failure if index is greater than the number of inodes present.
    if (index > super_block.first.no_inodes() || index == 0)
        return -1;

    // Exit with success if the inode is already cached.
    if (inodes.count(index) != 0)
    {
        return 0;
    }

    // Determine the block group, by dividing by the number of inodes per block
    // group.
    size_t bg = (index - 1) / super_block.first.inodes_per_group();

    // Determine the offset within the block by modding by the number of inodes
    // per block group.
    size_t offset = (index - 1) % super_block.first.inodes_per_group();

    // Combine the offset and the block base address to get the inode start.
    uint64_t loc = block_to_byte(bgdt[bg].first.inode_table()) +
        offset * super_block.first.inode_size();
    klib::ifstream in {drv_name};
    in.seekg(loc);
    klib::pair<klib::map<size_t, klib::pair<Ext2Inode, bool>>::iterator,
        bool> p = inodes.emplace(index, klib::pair<Ext2Inode, bool>
        {Ext2Inode {in, super_block.first.inode_size()}, false});
    // p.second is a bool indicating whether the emplacement succeeded. If it
    // did, p.first is an iterator to the new element.
    return (p.second ? 0 : -1);
}

/******************************************************************************/

size_t Ext2FileSystem::get_inode_index(const klib::string& name)
{
    // Names need to start with a /. The VFS should have provided us with a
    // name relative to the root of this file system.
    if (name[0] != '/')
        return 0;

    // Our place in the string.
    size_t pos;
    // Skip over any /
    for (pos = 0; pos < name.size() && name[pos] == '/'; ++pos) ;
    // Inode of the directory currently being investigated.
    size_t inode_indx = root_inode;

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
        Ext2Directory dir {inode_indx, *this};
        inode_indx = dir.lookup(current);
        // Test whether the file exists or not.
        if (inode_indx == 0)
            return 0;
        // Advance pos and skip over multiple /
        pos = end_pos;
        for (; pos < name.size() && name[pos] == '/'; ++pos) ;
    }

    return inode_indx;
}

/******************************************************************************/

bool Ext2FileSystem::backup_group(size_t bg) const
{
    // 0 and 1 are backup blocks.
    if (bg == 0 || bg == 1)
        return true;

    // Powers of 3 are backup groups.
    size_t n = bg;
    while (n % 3 == 0)
        n /= 3;
    if (n == 1)
        return true;

    // Powers of 5 are backup groups.
    n = bg;
    while (n % 5 == 0)
        n /= 5;
    if (n == 1)
        return true;

    // Powers of 7 are backup groups.
    n = bg;
    while (n % 7 == 0)
        n /= 7;
    if (n == 1)
        return true;

    // If we've got here, all the matches have failed and it's not a backup
    // group.
    return false;
}

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
