#ifndef EXT_H
#define EXT_H

#include <stddef.h>
#include <stdint.h>

#include <array>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "File.h"
#include "FileSystem.h"

// Forward declarations.
class Ext2FileSystem;

/**
    Wrapper function to create a file system known to be ext but with unknown
    version.

    @param drv Full path of the device (probably in /dev).
    @return File system for the disk, or nullptr if it wasn't ext.
 */
FileSystem* create_ext(const klib::string& drv);

/**
    Represents the data in an ext2 superblock.
 */
struct Ext2SuperBlock {
    /**
        Default constructor. Everything becomes zero, blank or the specified
        default for extended attributes.
     */
    Ext2SuperBlock() {}

    /**
        Constructor. Fill the data fields starting from the provided stream.

        @param in Stream to read the data from.
     */
    explicit Ext2SuperBlock(klib::istream& in);

    // Total number of inodes.
    uint32_t no_inodes;
    // Total number of blocks.
    uint32_t no_blocks;
    // Number of blocks reserved for the superuser.
    uint32_t reserved_blocks;
    // Number of unallocated blocks.
    uint32_t unalloc_blocks;
    // Number of unallocated inodes.
    uint32_t unalloc_inodes;
    // Block number of block containing the superblock.
    uint32_t superblock_block;
    // Left shift 1024 by this number of bits to get the block size.
    uint32_t block_size_shift;
    // Left shift 1024 by this number of bits to get the fragment size.
    uint32_t fragment_size_shift;
    // Number of blocks in each block group.
    uint32_t blocks_per_group;
    // Number of fragments in each block group.
    uint32_t fragments_per_group;
    // Number of inodes in each block group.
    uint32_t inodes_per_group;
    // Signature, should match 0xEF53.
    uint16_t signature;
    // Minor version number.
    uint16_t minor_version;
    // Major version number.
    uint32_t major_version;
    // UID that can use reserved blocks.
    uint16_t reserved_uid;
    // GID that can use reserved blocks.
    uint16_t reserved_gid;

    // First non-reserved inode.
    uint32_t first_usable_inode = 11;
    // Size of each inode in bytes.
    uint16_t inode_size = 128;
    // Block group this superblock is part of, if a backup copy.
    uint16_t backup_block = 0;
    // Optional features present.
    uint32_t optional_features = 0;
    // Features present that must be supported for reads and writes.
    uint32_t required_features = 0;
    // Features present that must be supported for writes.
    uint32_t write_features = 0;
    // File system ID (UUID).
    klib::string uuid;
    static constexpr size_t uuid_length = 16;
    // Volume name.
    klib::string name;
    static constexpr size_t name_length = 16;

    // Superblock always has 1024 bytes reserved for it.
    static constexpr size_t size = 1024;
};

/**
    Contains the information of a block group descriptor.
 */
struct BlockGroupDescriptor {
    /**
        Default constructor. Everything becomes zero.
     */
    BlockGroupDescriptor() {}

    /**
        Constructor. Fill the data fields starting from the provided stream.

        @param in Stream to read the data from.
     */
    explicit BlockGroupDescriptor(klib::istream& in);

    /**
        Print the contents to the provided stream.

        @param dest Stream to print information to.
     */
    void dump(klib::ostream& dest) const;

    // Block address of block usage bitmap.
    uint32_t block_map;
    // Block address of inode usage bitmap.
    uint32_t inode_map;
    // Starting block address of inode table.
    uint32_t inode_table;
    // Number of unallocated blocks in group.
    uint16_t unalloc_blocks;
    // Number of unallocated inodes in group.
    uint16_t unalloc_inodes;
    // Number of directories in group.
    uint16_t dirs;

private:
    // Number of unused bytes trailing a block group descriptor before reaching
    // the next entry in the table.
    static constexpr size_t unused_space = 14;
};

/**
    Ext2 inode data.
 */
struct Ext2Inode {
    /**
        Values for the type field.
     */
    enum Type : uint16_t {
        /** First in first out pipe. */
        fifo = 0x1000,
        /** Character device. */
        char_dev = 0x2000,
        /** Directory. */
        directory = 0x4000,
        /** Block device. */
        block_dev = 0x6000,
        /** Regular file. */
        file = 0x8000,
        /** Symbolic link. */
        sym_link = 0xA000,
        /** Unix socket. */
        socket = 0xC000
    };

    /**
        Default constructor. Everything becomes zero.
     */
    Ext2Inode() = default;

    /**
        Constructor. Fill the data fields starting from the provided stream.

        @param in Stream to read the data from.
     */
    explicit Ext2Inode(klib::istream& in);

    /**
        Process the type field to get the type of file.
     */
    Type get_type() const { return static_cast<Type>(type & 0xF000); }

    // Types and permissions.
    uint16_t type;
    // User ID.
    uint16_t uid;
    // Lower 32 bits of file size in bytes.
    uint32_t lower_size;
    // Last access time.
    uint32_t access_time;
    // Creation time.
    uint32_t creation_time;
    // Last modification time.
    uint32_t mod_time;
    // Deletion time.
    uint32_t del_time;
    // Group id.
    uint16_t gid;
    // Count of hard links.
    uint16_t hard_links;
    // Number of disk sectors (not blocks) used, excluding inode sector.
    uint32_t sectors;
    // Flags.
    uint32_t flags;
    // First OS specific value.
    uint32_t os_1;
    // Array of direct block pointers.
    static constexpr size_t no_direct = 12;
    klib::array<uint32_t, no_direct> direct;
    // Singly indirect pointer.
    uint32_t s_indirect;
    // Doubly indirect pointer.
    uint32_t d_indirect;
    // Triply indirect pointer.
    uint32_t t_indirect;
    // Generation number (for NFS).
    uint32_t gen_no;
    // File ACL (reserved in older versions).
    uint32_t file_acl;
    // Directory ACL or upper 32 bits of file size.
    uint32_t upper_size;
    // Block address of fragment.
    uint32_t frag_addr;
    // Second OS specific value.
    static constexpr size_t os_2_size = 12;
    klib::array<uint8_t, os_2_size> os_2;

    // Size of data fields on disk.
    static constexpr size_t size = 128;
};

/**
    Represents an ext2 file. Inherits from the abstract base File. Stores a
    pointer to the file system for the reads and writes, as well as an inode for
    looking up block addresses.
 */
class Ext2File : public DiskFile {
public:
    /**
        Constructor. Calls the File constructor with the mode and size, and sets
        the inode and file system pointer.

        @param m C-style mode to open the file in.
        @param in Inode for the file.
        @param indx Index in the file system of that inode.
        @param fs File system the file belongs to.
     */
    Ext2File(const char* m, Ext2Inode& in, size_t indx, Ext2FileSystem& fs);

    /**
        Closes the handle, freeing any resources used. Further accesses will
        fail.

        @return 0 if successful, otherwise EoF.
     */
    virtual int close() override
    {
        inode_index = 0;
        return DiskFile::close();
    }

    /**
        Writes to the file.

        @param buffer Location of the first object to be written.
        @param size Size of each object.
        @param count Number of objects to be written.
        @return Number of objects actually written.
     */
    virtual size_t write(const void* buffer, size_t size, size_t count)
        override;

    /**
        Reads from the file.

        @param buffer Location of the first object to be read into.
        @param size Size of each object.
        @param count Number of objects to be read.
        @return Number of objects actually read.
     */
    virtual size_t read(void* buffer, size_t size, size_t count) override;

    /**
        Writes any unwritten data from the output buffer to the underlying
        device. Implements fflush.

        @param stream File to flush.
        @return 0 on success, otherwise EoF.
     */
    virtual int flush() override;

    /**
        Gets the current file position indicator. Implements fgetpos.

        @param pos Pointer to a position indicator that will be set.
        @return 0 on success, nonzero otherwise.
     */
    virtual int getpos(klib::fpos_t* pos) override
    {
        if (pos == nullptr)
            return EOF;

        *pos = position;
        return 0;
    }

    /**
        Changes the current file position indicator to that specified by the
        offset from the origin. Implements fseek.

        @param offset Number of characters to shift relative to the origin.
        @param origin Base position for the shift. May be the beginning, current
               or end position.
        @return 0 on success, nonzero otherwise.
     */
    virtual int seek(long offset, int origin) override;

    /**
        Empties the contents of a file. Goes through each block pointer and
        deallocates the block before wiping the pointer. Sets the file size to
        zero.

        @return 0 on success, -1 on failure.
     */
    virtual int truncate() override;

protected:
    // Inode for this file.
    Ext2Inode inode;
    // Inode index. Necessary for editing the inode.
    size_t inode_index;
    // File system for ext2fs specific operations.
    Ext2FileSystem& ext2fs;
};

/**
    Stores the contents of an ext2 directory as a map from name to inode.
 */
class Ext2Directory : public Directory {
public:
    /**
        Constructor. Given an inode and a file system, process the inode as
        a directory, creating a mapping from names to inode indices.

        @param In to process.
        @param indx Index in the file system of that inode.
        @param fs File system to work on.
     */
    Ext2Directory(Ext2Inode& in, size_t indx, Ext2FileSystem& fs);

    /**
        Virtual destructor. Calls close() to free any resources.
     */
    virtual ~Ext2Directory() { close(); }

    /**
        Closes the handle, freeing any resources used. Further accesses will
        fail.
     */
    virtual void close() override { contents.clear(); };

    /**
        Returns the inode index of the file or directory matching the provided
        string, or 0 if it does not exist in this directory.

        @param n Name to look up.
        @return Inode index matching n, or 0 if it doesn't exist.
     */
    size_t lookup(const klib::string& n) const;

    /**
        Returns a list of the contents of the directory.

        @return Contents of the directory.
     */
    virtual klib::vector<klib::string> ls() const override;

protected:
    // Struct for the data in a single entry.
    struct Entry {
        // Inode index.
        uint32_t inode_index;
        // Total size of entry.
        uint16_t size;
        // Name length lower byte.
        uint8_t name_length_low;
        // Name length upper byte or type id.
        uint8_t name_length_high;
        // Name.
        klib::string name;
    };

    // Inode for this directory.
    Ext2Inode inode;
    // Inode index. Necessary for editing the inode.
    size_t inode_index;
    // File system for ext2fs specific operations.
    Ext2FileSystem& ext2fs;
    // Vector of the entries.
    klib::vector<Entry> contents;
};

/**
    Implementation of the abstract FileSystem base for ext2 file systems.
 */
class Ext2FileSystem : public FileSystem {
public:
    /**
        Features among the required features set.
     */
    enum class required_features {
        /** Compression is used. */
        compression = 0x1,
        /** Directory entries have type field. */
        directories_type = 0x2,
        /** File needs to replay its journal. */
        journal_replay = 0x4,
        /** File system uses a journal device. */
        journal_device = 0x8
    };

    /**
        Features from the required feature set currently supported. Bits set
        correspond to the required features in the superblock. And that value
        with the complement of this and if it's not zero, we don't support it.
     */
    static constexpr uint32_t required_supported =
        static_cast<uint32_t>(required_features::directories_type);

    /**
        Features among the required for writing features set.
     */
    enum class required_writing_features {
        /** Sparse superblocks and group decriptor tables. */
        sparse = 0x1,
        /** File system uses 64-bit file size. */
        large_file_size = 0x2,
        /** Directory contents are stored in the form of a binary tree. */
        directory_tree = 0x4
    };

    /**
        Features from the required for writing feature set currently supported.
        Bits set correspond to the required for writing features in the
        superblock. And that value with the complement of this and if it's not
        zero, we don't support it.
     */
    static constexpr uint32_t required_writing_supported =
        static_cast<uint32_t>(required_writing_features::large_file_size);

    /**
        Constructor. Creates an ext2 driver for the given device or partition.

        @param drv /dev name of the device.
     */
    Ext2FileSystem(const klib::string& drv);

    /**
        Opens a directory, returning a pointer to the directory handle. nullptr
        is returned on errors, such as the directory not existing, or the name
        referring to a file.

        @param name Full path name from the root directory of the file system.
        @return Directory pointer. nullptr on error.
     */
    virtual Directory* diropen(const klib::string& name) override;

    /**
        Opens a file, returning a pointer to the file handle. nullptr is
        returned on errors, such as the file not existing when reading is
        requested, asking for write when the file system is read only, or if the
        file is a directory.

        @param name Full path name from the root directory of the file system.
        @param mode Mode in which to open the file.
        @return File pointer. nullptr on error.
     */
    virtual klib::FILE* fopen(const klib::string& name, const char* mode)
        override;

    /**
        Rename the given file to the new given name.

        @param f File to rename.
        @param n New name for the file.
     */
    virtual void rename(const klib::string& f, const klib::string& n) override;

    /**
        Get the block size of the file system.

        @return Block size.
     */
    size_t block_size() const override
    {
        return 1024 << super_block.block_size_shift;
    }

    /**
        Given an inode, return the file size. This depends on the type of the
        file and the optional features specified in the superblock.

        @param inode Inode to check.
        @return Size of the file described by inode.
     */
    klib::streamoff file_size(const Ext2Inode& inode) const;

    /**
        Get the super block.

        @return Reference to the superblock.
     */
    const Ext2SuperBlock& get_super_block() const { return super_block; }

    /**
        Given an inode for a file and a block index in the file, get the block
        address in the file system.

        @param inode Inode to use.
        @param bl Block index in the file.
        @return Block address in the file system.
     */
    size_t inode_lookup(const Ext2Inode& inode, size_t bl) const;

    /**
        Whether the file system is valid. Errors in the superblock or the device
        not actually having an ext2 file system will make this false on
        construction.

        @return Whether the device seems to have a valid ext2 file system.
     */
    bool valid() const { return val; }

    /**
        Byte offset from the start of the device to find the superblock at.
     */
    static constexpr size_t superblock_loc = 1024;

    /**
        Byte offset from the start of the superblock to find the ext2 signature
        at.
     */
    static constexpr size_t signature_loc = 56;

    /**
        Value of the ext2 signature. Helps to confirm a device actually has an
        ext2 file system.
     */
    static constexpr uint16_t signature = 0xEF53;

protected:
    // Whether this seems to be a valid ext2 file systems.
    bool val;
    // The contents of the superblock.
    Ext2SuperBlock super_block;
    // Table of block group descriptors.
    klib::vector<BlockGroupDescriptor> bgdt;
    // Inode index for the root directory.
    static constexpr size_t root_inode = 2;

    /**
        Turn a block address into a byte offset.

        @param ba Block address.
        @return Byte offset of the start of the block.
     */
    uint64_t block_to_byte(size_t ba) const
    {
        return ba * (1024 << super_block.block_size_shift);
    }

    /**
        Get the block address a byte offset belongs to.

        @param off Byte offset to test.
        @return Block address that contains the byte.
     */
    size_t byte_to_block(uint64_t off) const
    {
        return
            static_cast<size_t>(off / (1024 << super_block.block_size_shift));
    }

    // Retrieve inode data, given its numerical address or file or directory
    // name.
    klib::pair<size_t, Ext2Inode> get_inode(size_t index) const;
    klib::pair<size_t, Ext2Inode> get_inode(const klib::string& name);

};

/**
    Wrapper function to create an ext2 file system, or return nullptr if the
    disk does not have an ext2 file sytsem.

    @param drv Full path of the device (probably in /dev).
    @return File system for the disk, or nullptr if it wasn't ext.
 */
Ext2FileSystem* create_ext2(const klib::string& drv);
#endif /* EXT_H */
