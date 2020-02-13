#include "File.h"

#include <stdint.h>

#include <cstdio>
#include <cstring>
#include <string>

#include "FileSystem.h"
#include "Kernel.h"
#include "Logger.h"
#include "MemoryFileSystem.h"

/******************************************************************************
 ******************************************************************************/

size_t CharacterFile::write(const void* buffer, size_t size, size_t count)
{
    // Do nothing if the file is not open for writing.
    if (!writing)
        return 0;

    // Work out the number of chars to write.
    const char* buf = static_cast<const char*>(buffer);
    size_t n = size * count;

    for (size_t i = 0; i < n; ++i)
        dev.write_char(buf[i]);

    return count;
}

/******************************************************************************/

size_t CharacterFile::read(void* buffer, size_t size, size_t count)
{
    // Do nothing if the file is not open for reading.
    if (!reading)
        return 0;

    // Work out the number of chars to read.
    char* buf = static_cast<char*>(buffer);
    size_t n = size * count;

    klib::string tmp {dev.read_chars(n)};
    klib::memcpy(buf, tmp.c_str(), tmp.size());
    return tmp.size();
}

/******************************************************************************
 ******************************************************************************/

BlockFile::BlockFile(BlockDevice& d, const char* mode) :
    klib::FILE {mode}, dev {d}, current {false}
{
    // Create the buffer.
    buffered = true;
    buffer = new char[dev.sector_size()];
    // No need to read data into the buffer until a read or write occurs.
}

/******************************************************************************/

size_t BlockFile::write(const void* buf, size_t size, size_t count)
{
    // Return immediately if not writing, or not asked to write anything.
    if (!writing || size == 0 || count == 0)
        return 0;

    // Note, appending doesn't make sense for raw block files, so the append
    // flag is ignored.

    // Get the number of bytes to write.
    size_t n = size*count;
    size_t written = 0;

    // Interpret buf as a char*.
    const char* char_buf = static_cast<const char*>(buf);

    // Get current position in buffer.
    size_t buf_pos = static_cast<klib::streamoff>(position) % dev.sector_size();

    // We need to read into the buffer, so we don't stomp on existing data, but
    // only if we're not covering a whole block.
    if (!current && (buf_pos != 0 || n < dev.sector_size()))
    {
        if (dev.read_block(static_cast<klib::streamoff>(position) - buf_pos,
            buffer) != dev.sector_size())
            return written;
        else
            current = true;
    }

    // Write any complete sectors.
    while (written + dev.sector_size() - buf_pos <= n)
    {
        if (static_cast<uint64_t>(position) >= dev.get_size())
        {
            position = klib::fpos_t{-1};
            eof = true;
            return written;
        }

        // We need to write whole sectors.
        size_t ret_val = 0;
        if (buf_pos != 0)
        {
            // We're in the middle of a sector. We need to fill the rest of the
            // buffer from the input, then write the whole sector.
            klib::memcpy(buffer + buf_pos, char_buf + written,
                dev.sector_size() - buf_pos);
            ret_val = dev.write_block(
                position - static_cast<klib::streamoff>(buf_pos), buffer);
        }
        else
            // We're writing a whole sector. No need to copy to the internal
            // buffer, just get it straight from the input.
            ret_val = dev.write_block(position, char_buf + written);

        written += ret_val;
        position += ret_val;
        current = false;
        buf_pos = 0;

        if (ret_val != dev.sector_size())
            return written;
    }

    if (!current && written != n)
    {
        // Update the buffer, so we can modify the active block without
        // destroying the whole block.
        if (dev.read_block(static_cast<klib::streamoff>(position), buffer) !=
            dev.sector_size())
            return written;
        else
            current = true;
    }

    // Write any remaining characters.
    klib::memcpy(buffer + buf_pos, char_buf + written, n - written);
    position += n - written;

    return written;
}

/******************************************************************************/

size_t BlockFile::read(void* buf, size_t size, size_t count)
{
    // Return immediately if not reading, or if not asked to read anything.
    if (!reading || size == 0 || count == 0 || eof)
        return 0;

    // Get the number of bytes to read.
    size_t n = size*count;
    size_t char_read = 0;

    // Interpret buf as a char*.
    char* char_buf = static_cast<char*>(buf);

    // Get current position in buffer.
    size_t buf_pos = static_cast<klib::streamoff>(position) % dev.sector_size();

    // Read any complete sectors.
    while (char_read + dev.sector_size() - buf_pos <= n)
    {
        if (static_cast<uint64_t>(position) >= dev.get_size())
        {
            position = klib::fpos_t{-1};
            eof = true;
            return char_read;
        }

        // If current is false, we need to read in the current block.
        if (!current)
        {
            if (dev.read_block(static_cast<klib::streamoff>(position) - buf_pos,
                buffer) != dev.sector_size())
                return char_read;
            else
                current = true;
        }

        klib::memcpy(char_buf + char_read, buffer + buf_pos,
            dev.sector_size() - buf_pos);
        position += dev.sector_size() - buf_pos;
        char_read += dev.sector_size() - buf_pos;
        current = false;

        buf_pos = 0;
    }

    if (char_read < n)
    {
        // If current is false, we need to read in the current block.
        if (!current)
        {
            if (dev.read_block(static_cast<klib::streamoff>(position) - buf_pos,
                buffer) != dev.sector_size())
                return char_read;
            else
                current = true;
        }
        // Read any remaining characters.
        klib::memcpy(char_buf + char_read, buffer + buf_pos, n - char_read);
        position += n - char_read;
    }

    return char_read;
}

/******************************************************************************/

int BlockFile::flush()
{
    // Return failure immediately if we're not writing.
    if (!writing)
        return EOF;

    // If current is false, then writes can't have been made, so there's nothing
    // to flush.
    if (!current)
        return 0;

    // We just need to overwrite the current sector with the current buffer.
    klib::streamoff buf_pos =
        static_cast<klib::streamoff>(position) % dev.sector_size();
    return (dev.write_block(position - buf_pos, buffer) == dev.sector_size() ?
        0 : EOF);
}

/******************************************************************************/

int BlockFile::seek(long offset, int origin)
{
    // Flush current buffer.
    if(writing && flush() != 0)
        return EOF;

    switch (origin)
    {
    case SEEK_SET:
    {
        // Make sure we don't go before the beginning of the device.
        if (offset < 0)
            offset = 0;
        // Make sure we don't go beyond the end of the device.
        if (static_cast<uint64_t>(offset) > dev.get_size())
        {
            position = klib::fpos_t{-1};
            eof = true;
            return EOF;
        }
        // Test whether we're seeking outside the current buffer.
        klib::streamoff curr_sector = static_cast<klib::streamoff>(position) /
            dev.sector_size();
        position = offset;
        klib::streamoff new_sector = static_cast<klib::streamoff>(position) /
            dev.sector_size();
        if (curr_sector != new_sector)
            current = false;
        eof = false;
        break;
    }
    case SEEK_CUR:
    {
        // Make sure we don't go before the beginning of the device.
        if (offset < 0 && -offset > static_cast<klib::streamoff>(position))
            offset = -static_cast<klib::streamoff>(position);
        // Make sure we don't go beyond the end of the device.
        if (static_cast<uint64_t>(
            static_cast<klib::streamoff>(offset) + position) > dev.get_size())
        {
            position = klib::fpos_t{-1};
            eof = true;
            return EOF;
        }
        // Test whether we're seeking outside the current buffer.
        klib::streamoff curr_sector = static_cast<klib::streamoff>(position) /
            dev.sector_size();
        position += offset;
        klib::streamoff new_sector = static_cast<klib::streamoff>(position) /
            dev.sector_size();
        if (curr_sector != new_sector)
            current = false;
        eof = false;
        break;
    }
    case SEEK_END:
    {
        // Make sure we don't go before the beginning of the device.
        if (offset < 0 && static_cast<uint64_t>(-offset) > dev.get_size())
            offset = -dev.get_size();
        // Make sure we don't go beyond the end of the device.
        if (offset > 0)
        {
            position = klib::fpos_t{-1};
            eof = true;
            return EOF;
        }
        // Test whether we're seeking outside the current buffer.
        klib::streamoff curr_sector = static_cast<klib::streamoff>(position) /
            dev.sector_size();
        position = dev.get_size() - offset;
        klib::streamoff new_sector = static_cast<klib::streamoff>(position) /
            dev.sector_size();
        if (curr_sector != new_sector)
            current = false;
        eof = false;
        break;
    }
    default:
        return EOF;
    }

    return 0;
}

/******************************************************************************
 ******************************************************************************/

DiskFile::DiskFile(const char* m, FileSystem& f, klib::streamoff s) :
    klib::FILE {m}, fs {f}, sz {s}
{
    // Fail if read-only and open for writing.
    if (writing && fs.ro())
    {
        close();
    }
    else
    {
        // Create the buffer.
        buffered = true;
        buffer = new char[fs.block_size()];
    }
}

/******************************************************************************
 ******************************************************************************/

MemoryFile::MemoryFile(void* a, size_t sz, const char* mode,
    MemoryFileSystem& fs) :
    klib::FILE {mode}, addr {a}, file_size {sz}, mfs {fs}
{
    // Fail if read-only and open for writing.
    if (writing && fs.ro())
        close();
}

/******************************************************************************/

int MemoryFile::close()
{
    int ret_val = MemoryFile::flush();

    // Notify the file system to clear up memory if necessary.
    mfs.close(addr);

    ret_val = (klib::FILE::close() == 0 ? ret_val : -1);
    return ret_val;
}

/******************************************************************************/

size_t MemoryFile::write(const void* buf, size_t size, size_t count)
{
    // Do nothing if the file is not open for writing.
    if (!writing)
        return 0;

    // Work out the number of chars to write.
    char* buffer = static_cast<char*>(addr) +
        static_cast<klib::streamoff>(position);
    size_t n = size * count;

    // Check for overflow.
    if (static_cast<klib::streamoff>(position) + n >= file_size)
    {
        void* new_addr = mfs.reallocate_file(addr,
            static_cast<klib::streamoff>(position) + n);
        if (new_addr == nullptr)
            return 0;
        else
            addr = new_addr;
    }

    // Copy data.
    klib::memcpy(buffer, buf, n);
    position += n;
    return count;
}

/******************************************************************************/

size_t MemoryFile::read(void* buf, size_t size, size_t count)
{
    // Do nothing if the file is not open for reading or we've reached EOF.
    if (!reading || size == 0 || count == 0 || eof)
        return 0;

    // Work out the number of chars to read.
    const char* buffer = static_cast<const char*>(addr) +
        static_cast<klib::streamoff>(position);
    size_t n = size * count;
    if (n >= file_size - static_cast<klib::streamoff>(position))
    {
        n = ((file_size - static_cast<klib::streamoff>(position) / size) *
            size);
        eof = true;
    }

    // Copy data.
    klib::memcpy(buf, buffer, n);
    position += n;
    return n / size;
}

/******************************************************************************/

int MemoryFile::seek(long offset, int origin)
{
    switch (origin)
    {
    case SEEK_SET:
    {
        // Make sure we don't go before the beginning of the device.
        if (offset < 0)
            offset = 0;
        // Make sure we don't go beyond the end of the device.
        if (static_cast<uint64_t>(offset) > file_size)
        {
            position = klib::fpos_t{-1};
            eof = true;
            return EOF;
        }
        // Set new position.
        position = offset;
        eof = false;
        break;
    }
    case SEEK_CUR:
    {
        // Make sure we don't go before the beginning of the device.
        if (offset < 0 && -offset > static_cast<klib::streamoff>(position))
            offset = -static_cast<klib::streamoff>(position);
        // Make sure we don't go beyond the end of the device.
        if (static_cast<uint64_t>(
            static_cast<klib::streamoff>(offset) + position) > file_size)
        {
            position = klib::fpos_t{-1};
            eof = true;
            return EOF;
        }
        // Set new position.
        position += offset;
        eof = false;
        break;
    }
    case SEEK_END:
    {
        // Make sure we don't go before the beginning of the device.
        if (offset < 0 && static_cast<uint64_t>(-offset) > file_size)
            offset = - file_size;
        // Make sure we don't go beyond the end of the device.
        if (offset > 0)
        {
            position = klib::fpos_t{-1};
            eof = true;
            return EOF;
        }
        // Test whether we're seeking outside the current buffer.
        // Set new position.
        position = file_size - offset;
        eof = false;
        break;
    }
    default:
        return EOF;
    }

    return 0;
}

/******************************************************************************/

int MemoryFile::truncate()
{
    // Ask the file system to reallocate the size to zero.
    addr = mfs.reallocate_file(addr, 0);

    if (addr == nullptr)
    {
        // Success
        file_size = 0;
        return 0;
    }
    else
        // Failure, shouldn't really be possible.
        return -1;
}

/******************************************************************************
 ******************************************************************************/
