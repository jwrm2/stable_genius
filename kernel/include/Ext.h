#ifndef EXT_H
#define EXT_H

#include <stddef.h>
#include <stdint.h>

#include <array>
#include <ios>
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
    Features among the required features set.
 */
enum class ext2_required_features : uint32_t {
    /** None. */
    none = 0x0,
    /** Compression is used. */
    compression = 0x1,
    /** Directory entries have type field. */
    directories_type = 0x2,
    /** File needs to replay its journal. */
    journal_replay = 0x4,
    /** File system uses a journal device. */
    journal_device = 0x8
};

namespace klib {
    template<>
    struct BitmaskEnable<ext2_required_features> : public true_type {};
}

/**
    Features among the required for writing features set.
 */
enum class ext2_required_writing_features : uint32_t {
    /** None. */
    none = 0x0,
    /** Sparse superblocks and group descriptor tables. */
    sparse = 0x1,
    /** File system uses 64-bit file size. */
    large_file_size = 0x2,
    /** Directory contents are stored in the form of a binary tree. */
    directory_tree = 0x4
};

namespace klib {
    template<>
    struct BitmaskEnable<ext2_required_writing_features> : public true_type {};
} // end klib namespace

/**
    Optional features.
 */
enum class ext2_optional_features : uint32_t {
    /** None. */
    none = 0x0,
    /** Preallocate blocks when creating a new directory. */
    preallocate = 0x1,
    /** AFS server inodes exist. */
    afs_server = 0x2,
    /** File system has a journal (ie is actually Ext3). */
    journal = 0x4,
    /** Inodes have extended attributes/ */
    extended_inodes = 0x8,
    /** File system can resize to a bigger size. */
    resize = 0x10,
    /** Directories use a hash index. */
    directory_hash = 0x20,
};

namespace klib {
    template<>
    struct BitmaskEnable<ext2_optional_features> : public true_type {};
} // end klib namespace

using klib::operator|;
using klib::operator&;
using klib::operator^;
using klib::operator~;
using klib::operator|=;
using klib::operator&=;
using klib::operator^=;

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

    /**
        Write the data unformatted to the provided stream, at the current
        position in the stream. The data sent to the disk is adjusted to
        indicate the provided block address in the superblock_no field. The data
        in memory is unaffected and continues to be correct for the primary
        superblock.

        @param dest The stream to write to.
        @param bl Block address to write to. The position data is written to is
               determined by the position of the dest stream, but one of the
               superblock fields is the block address it's occupying.
        @return The stream after writing.
     */
    klib::ostream& write(klib::ostream& dest, size_t bl) const;

    /**
        Gets or sets the total number of inodes.

        @param no Number of inodes to set.
        @return Total number of inodes.
     */
    uint32_t no_inodes() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data());
    }
    void no_inodes(uint32_t no)
    {
        *reinterpret_cast<uint32_t*>(data.data()) = no;
    }

    /**
        Gets or sets the total number of blocks.

        @param no Number of blocks to set.
        @return Total number of blocks.
     */
    uint32_t no_blocks() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 4);
    }
    void no_blocks(uint32_t no)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 4) = no;
    }

    /**
        Gets or sets the total number of blocks reserved for the superuser.

        @param res Number of reserved blocks to set.
        @return Number of reserved blocks.
     */
    uint32_t reserved_blocks() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 8);
    }
    void reserved_blocks(uint32_t res)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 8) = res;
    }

    /**
        Gets or sets the total number of unallocated blocks.

        @param unalloc Number of unallocated blocks to set.
        @return Number of unallocated blocks.
     */
    uint32_t unalloc_blocks() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 12);
    }
    void unalloc_blocks(uint32_t unalloc)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 12) = unalloc;
    }

    /**
        Gets or sets the total number of unallocated inodes.

        @param unalloc Number of unallocated inodes to set.
        @return Number of unallocated inodes.
     */
    uint32_t unalloc_inodes() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 16);
    }
    void unalloc_inodes(uint32_t unalloc)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 16) = unalloc;
    }

    /**
        Gets or sets the block number of the block containing the superblock.

        @param sb Block number of the block containing the superblock to set.
        @return Block number of the block containing the superblock.
     */
    uint32_t superblock_no() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 20);
    }
    void superblock_no(uint32_t sb)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 20) = sb;
    }

    /**
        Log base 2 of the block size - 10. Left shift 1024 by this number to get
        the block size.

        @param sh Block size shift to set.
        @return Block size shift.
     */
    uint32_t block_size_shift() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 24);
    }
    void block_size_shift(uint32_t sh)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 24) = sh;
    }

    /**
        Log base 2 of the fragment size - 10. Left shift 1024 by this number to
        get the fragment size.

        @param sh Fragment size shift to set.
        @return Fragment size shift.
     */
    uint32_t fragment_size_shift() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 28);
    }
    void fragment_size_shift(uint32_t sh)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 28) = sh;
    }

    /**
        Number of blocks in each block group.

        @param bpg Number of blocks in each block group to set.
        @return Number of blocks in each block group.
     */
    uint32_t blocks_per_group() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 32);
    }
    void blocks_per_group(uint32_t bpg)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 32) = bpg;
    }

    /**
        Number of fragments in each block group.

        @param fpg Number of fragments in each block group to set.
        @return Number of fragments in each block group.
     */
    uint32_t fragments_per_group() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 36);
    }
    void fragments_per_group(uint32_t fpg)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 36) = fpg;
    }

    /**
        Number of inodes in each block group.

        @param ipg Number of inodes in each block group to set.
        @return Number of inodes in each block group.
     */
    uint32_t inodes_per_group() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 40);
    }
    void inodes_per_group(uint32_t ipg)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 40) = ipg;
    }

    /**
        Last mount time (in POSIX time).

        @param t Last mount time to set.
        @return Last mount time.
     */
    uint32_t last_mount() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 44);
    }
    void last_mount(uint32_t t)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 44) = t;
    }

    /**
        Last written time (in POSIX time).

        @param t Last written time to set.
        @return Last written time.
     */
    uint32_t last_write() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 48);
    }
    void last_write(uint32_t t)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 48) = t;
    }

    /**
        Number of mounts since last consistency check.

        @param n Number of mounts to set.
        @return Number of mounts.
     */
    uint16_t mounts_since_check() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data() + 52);
    }
    void mounts_since_check(uint16_t n)
    {
        *reinterpret_cast<uint16_t*>(data.data() + 52) = n;
    }

    /**
        Number of mounts allowed before consistency check.

        @param n Number of mounts to set.
        @return Number of mounts.
     */
    uint16_t mounts_before_check() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data() + 54);
    }
    void mounts_before_check(uint16_t n)
    {
        *reinterpret_cast<uint16_t*>(data.data() + 54) = n;
    }

    /**
        Ext2 signature. Should be 0xef53.

        @param sig Signature to set.
        @return Signature.
     */
    uint16_t signature() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data() + 56);
    }
    void signature(uint16_t sig)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 56) = sig;
    }

    /**
        File system state. 1 means clean, 2 means there are errors.

        @param st State to set.
        @return State.
     */
    enum state_t : uint16_t {
        /** No errors in the file system */
        clean = 1,
        /** Errors present in the file system. */
        errors = 2
    };

    state_t state() const
    {
        return *reinterpret_cast<const state_t*>(data.data() + 58);
    }
    void state(state_t st)
    {
        *reinterpret_cast<state_t*>(data.data() + 58) = st;
    }

    /**
        Action on error. 1 means ignore, 2 means remount read only, 3 means
        panic.

        @param ac Action to set.
        @return Action on error.
     */
    enum error_action_t : uint16_t {
        /** Ignore errors and carry on. */
        ignore = 1,
        /** Remount the file system read-only. */
        remount = 2,
        /** Cause a kernel panic. */
        panic = 3
    };

    uint16_t error_action() const
    {
        return *reinterpret_cast<const error_action_t*>(data.data() + 60);
    }
    void error_action(error_action_t ac)
    {
        *reinterpret_cast<error_action_t*>(data.data() + 60) = ac;
    }

    /**
        Minor version number.

        @param ver Minor version number to set.
        @return Minor version number.
     */
    uint16_t minor_version() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data() + 62);
    }
    void minor_version(uint16_t ver)
    {
        *reinterpret_cast<uint16_t*>(data.data() + 62) = ver;
    }

    /**
        Last consistency check time (in POSIX time).

        @param t Last consistency check time to set.
        @return Last consistency check time.
     */
    uint32_t last_check() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 64);
    }
    void last_check(uint32_t t)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 64) = t;
    }

    /**
        Interval between forced consistency checks (in POSIX time).

        @param t Check interval  to set.
        @return Check interval.
     */
    uint32_t check_interval() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 68);
    }
    void check_interval(uint32_t t)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 68) = t;
    }

    /**
        ID of the operating system that created this file system. 0 is Linux,
        1 is GNU HURD, 2 is MASIX, 3 is FreeBSD, 4 is other.

        @param op Operating system ID to set.
        @return Operating system ID.
     */
    enum opid_t : uint32_t {
        /** Linux */
        linux = 0,
        /** GNU HURD */
        hurd = 1,
        /** MASIX */
        masix = 3,
        /** FreeBSD */
        freebsd = 4,
        /** Other OS */
        other = 5
    };

    opid_t opid() const
    {
        return *reinterpret_cast<const opid_t*>(data.data() + 72);
    }
    void opid(opid_t op)
    {
        *reinterpret_cast<opid_t*>(data.data() + 72) = op;
    }

    /**
        Major version number.

        @param ver Major version number to set.
        @return Major version number.
     */
    uint32_t major_version() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 76);
    }
    void major_version(uint32_t ver)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 76) = ver;
    }

    /**
        UID of the user that can use reserved blocks.

        @param uid Reserved block user to set.
        @return Reserved block user.
     */
    uint16_t reserved_uid() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data() + 80);
    }
    void reserved_uid(uint16_t uid)
    {
        *reinterpret_cast<uint16_t*>(data.data() + 80) = uid;
    }

    /**
        GID of the group that can use reserved blocks.

        @param gid Reserved block group to set.
        @return Reserved block group.
     */
    uint16_t reserved_gid() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data() + 82);
    }
    void reserved_gid(uint16_t gid)
    {
        *reinterpret_cast<uint16_t*>(data.data() + 82) = gid;
    }

    /**
        First non-reserved inode. Variable and stored in the superblock if the
        major version is 1 or higher. Otherwise fixed to 11.

        @param no First non-reserved inode number to set. Silently fails if the
                  major version is less than 1.
        @return First non-reserved inode number.
     */
    uint32_t first_inode() const
    {
        if (major_version() >= 1)
            return *reinterpret_cast<const uint32_t*>(data.data() + 84);
        else
            return f_inode;
    }
    void first_inode(uint32_t no)
    {
        if (major_version() >= 1)
            *reinterpret_cast<uint32_t*>(data.data() + 84) = no;
    }

    /**
        Size of each inode. Variable and stored in the superblock if the major
        version is 1 or higher. Otherwise fixed to 128.

        @param sz Size of an inode to set. Silently fails if the major version
               is less than 1.
        @return Size of an inode.
     */
    uint16_t inode_size() const
    {
        if (major_version() >= 1)
            return *reinterpret_cast<const uint16_t*>(data.data() + 88);
        else
            return inode_sz;
    }
    void inode_size(uint16_t sz)
    {
        if (major_version() >= 1)
            *reinterpret_cast<uint16_t*>(data.data() + 88) = sz;
    }

    /**
        Block group this superblock is part of, if it's a backup. For major
        version 1 or higher only.

        @param blg Block group number to set.
        @return Block group number.
     */
    uint16_t backup_group() const
    {
        if (major_version() >= 1)
            return *reinterpret_cast<const uint16_t*>(data.data() + 90);
        else
            return 0;
    }
    void backup_group(uint16_t blg)
    {
        if (major_version() >= 1)
            *reinterpret_cast<uint16_t*>(data.data() + 90) = blg;
    }

    /**
        Optional features in use in the file system. The implementation does
        not need to support these for reading or writing. For major version 1 or
        higher only.

        @param feat Optional features to set.
        @return Optional features.
     */
    ext2_optional_features optional_features() const
    {
        if (major_version() >= 1)
            return *reinterpret_cast<const ext2_optional_features*>(
                data.data() + 92);
        else
            return ext2_optional_features::none;
    }
    void optional_features(ext2_optional_features feat)
    {
        if (major_version() >= 1)
            *reinterpret_cast<ext2_optional_features*>(data.data() + 92) = feat;
    }

    /**
        Required features in use in the file system. The implementation must
        support these for reading or writing. For major version 1 or higher
        only.

        @param feat Required features to set.
        @return Required features.
     */
    ext2_required_features required_features() const
    {
        if (major_version() >= 1)
            return *reinterpret_cast<const ext2_required_features*>(
                data.data() + 96);
        else
            return ext2_required_features::none;
    }
    void required_features(ext2_required_features feat)
    {
        if (major_version() >= 1)
            *reinterpret_cast<ext2_required_features*>(data.data() + 96) = feat;
    }

    /**
        Required writing features in use in the file system. The implementation
        does not need to support these to mount the file system read only, but
        must for mounting read-write. For major version 1 or higher only.

        @param feat Required writing features to set.
        @return Required writing features.
     */
    ext2_required_writing_features required_writing_features() const
    {
        if (major_version() >= 1)
            return *reinterpret_cast<const ext2_required_writing_features*>(
                data.data() + 100);
        else
            return ext2_required_writing_features::none;
    }
    void required_writing_features(ext2_required_writing_features feat)
    {
        if (major_version() >= 1)
            *reinterpret_cast<ext2_required_writing_features*>(
                data.data() + 100) = feat;
    }

    /**
        File system UUID. For major version 1 or higher only.

        @param u UUID to set.
        @return UUID.
     */
    klib::string uuid() const
    {
        if (major_version() >= 1)
            return klib::string {
                reinterpret_cast<const char*>(data.data() + 104), uuid_length};
        else
            return klib::string {};
    }
    void uuid(const klib::string& u);

    /**
        Volume name. For major version 1 or higher only.

        @param n Name to set.
        @return Name.
     */
    klib::string name() const
    {
        if (major_version() >= 1)
            return klib::string {
                reinterpret_cast<const char*>(data.data() + 120), name_length};
        else
            return klib::string {};
    }
    void name(const klib::string& n);

    /**
        Last mounted path. On disk, this is stored with a null terminator. For
        major version 1 or higher only.

        @param p Path to set.
        @return Path.
     */
    klib::string path() const
    {
        if (major_version() >= 1)
            return klib::string {
                reinterpret_cast<const char*>(data.data()) + 136};
        else
            return klib::string {};
    }
    void path(const klib::string& p);

    // TODO There are other fields, but they all relate to additional features.

    /**
        Whether the file system seems to be valid.

        @return True if the file system seems to be valid, false otherwise.
     */
    bool valid() const { return val; }

    /**
        Size of the data in the superblock. 236 for major version 1 or higher,
        84 for earlier versions.

        @return Size of the data in the superblock.
     */
    size_t size() const
    {
        if (major_version() >= 1)
            return data_size;
        else
            return compulsory_size;
    }   

protected:
    // Whether the data seems to be valid.
    bool val;
    // Size of data fields.
    static constexpr size_t compulsory_size = 84;
    static constexpr size_t data_size = 236;
    // Total size reserved for superblock.
    static constexpr size_t disk_size = 1024;
    // Actual store for the data.
    klib::array<uint8_t, data_size> data;

    // Signature value to compare with.
    static constexpr uint16_t sig = 0xef53;
    // Fixed first non-reserved inode, for major version less than 1.
    static constexpr uint32_t f_inode = 11;
    // Fixed size of inodes, for major version less than 1.
    static constexpr uint16_t inode_sz = 128;
    // Length of the file system ID (UUID).
    static constexpr size_t uuid_length = 16;
    // Length of the volume name.
    static constexpr size_t name_length = 16;
    // Length of the path last mounted as.
    static constexpr size_t path_length = 64;
    // Location of the field indicating the block number.
    static constexpr size_t block_number_field = 20;
    // Size of the field indicating the block number.
    static constexpr size_t block_number_size = 4;
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
        Write the data unformatted to the provided stream.

        @param dest The stream to write to.
        @return The stream after writing.
     */
    klib::ostream& write(klib::ostream& dest) const;

    /**
        Print the contents to the provided stream in a human readable format.

        @param dest Stream to print information to.
     */
    void dump(klib::ostream& dest) const;

    /**
        Block address of the block usage bitmap.

        @param bl Block address of the block usage bitmap to set.
        @return Block address of the block usage bitmap. 
     */
    uint32_t block_map() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data());
    }
    void block_map(uint32_t bl)
    {
        *reinterpret_cast<uint32_t*>(data.data()) = bl;
    }

    /**
        Block address of the inode usage bitmap.

        @param bl Block address of the inode usage bitmap to set.
        @return Block address of the inode usage bitmap. 
     */
    uint32_t inode_map() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 4);
    }
    void inode_map(uint32_t bl)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 4) = bl;
    }

    /**
        Block address of the start of the inode table.

        @param bl Block address of the start of the inode table to set.
        @return Block address of the start of the inode table.
     */
    uint32_t inode_table() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 8);
    }
    void inode_table(uint32_t bl)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 8) = bl;
    }

    /**
        Number of unallocated blocks in the block group.

        @param n Number of unallocated blocks to set.
        @return Number of unallocated blocks.
     */
    uint16_t unalloc_blocks() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data() + 12);
    }
    void unalloc_blocks(uint16_t n)
    {
        *reinterpret_cast<uint16_t*>(data.data() + 12) = n;
    }

    /**
        Number of unallocated inodes in the block group.

        @param n Number of unallocated inodes to set.
        @return Number of unallocated inodes.
     */
    uint16_t unalloc_inodes() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data() + 14);
    }
    void unalloc_inodes(uint16_t n)
    {
        *reinterpret_cast<uint16_t*>(data.data() + 14) = n;
    }

    /**
        Number of directory inodes in the block group.

        @param n Number of directories to set.
        @return Number of directories.
     */
    uint16_t dirs() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data() + 16);
    }
    void dirs(uint16_t n)
    {
        *reinterpret_cast<uint16_t*>(data.data() + 16) = n;
    }

    /**
        Whether the record seems to be valid.

        @return True if the record seems to be valid, false otherwise.
     */
    bool valid() const { return val; }

protected:
    // Whether we seem to have a valid record.
    bool val;
    // Size of used data.
    static constexpr size_t data_size = 18;
    // Total size reserved for block group descriptor.
    static constexpr size_t disk_size = 32;
    // Actual store for the data.
    klib::array<uint8_t, data_size> data;
};

/**
    Ext2 inode data.
 */
struct Ext2Inode {
    /**
        Default constructor. Everything becomes zero. We assume 128 bytes
        on disk.
     */
    Ext2Inode() : val {false}, data {}, disk_size {128} {}

    /**
        Constructor. Fill the data fields starting from the provided stream.

        @param in Stream to read the data from.
        @param sz Size of the inode on disk, read from the superblock.
     */
    explicit Ext2Inode(klib::istream& in, size_t sz);

    /**
        Write the inode data to a stream. Zero padding is added if the
        superblock indicates more than 128 bytes.

        @param dest Stream to write to.
        @return dest after writing.
     */
    klib::ostream& write(klib::ostream& dest) const;

    /**
        Values for the type field.
     */
    enum type_t : uint16_t {
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
        Type of file.

        @param t Type of file to set.
        @return Type of file.
     */
    type_t type() const
    {
        return static_cast<type_t>(
            *reinterpret_cast<const uint16_t*>(data.data()) & 0xF000);
    }
    void type(type_t t)
    {
        *reinterpret_cast<uint16_t*>(data.data()) =
            (*reinterpret_cast<uint16_t*>(data.data()) & 0x0FFF) |
            static_cast<uint16_t>(t);
    }

    /**
        Permissions of the file. Only the lowest 12 bits can be set. The highest
        4 bits are the file type and will be unset in the value to set.

        @param p Permissions to set (& 0x0FFF).
        @return Permissions.
     */
    uint16_t permissions() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data()) & 0x0FFF;
    }
    void permissions(uint16_t p)
    {
        *reinterpret_cast<uint16_t*>(data.data()) =
            (*reinterpret_cast<uint16_t*>(data.data()) & 0xF000) | (p & 0x0FFF);
    }

    /**
        User ID of the file owner.

        @param u User ID to set.
        @return User ID.
     */
    uint16_t uid() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data() + 2);
    }
    void uid(uint16_t u)
    {
        *reinterpret_cast<uint16_t*>(data.data() + 2) = u;
    }

    /**
        Lower 4 bytes of the file size.

        @param lsz Lower size to set.
        @return Lower size.
     */
    uint32_t lower_size() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 4);
    }
    void lower_size(uint32_t lsz)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 4) = lsz;
    }

    /**
        Last access time (in POSIX time).

        @param t Last access time to set.
        @return Last access time.
     */
    uint32_t access_time() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 8);
    }
    void access_time(uint32_t t)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 8) = t;
    }

    /**
        Creation time (in POSIX time).

        @param t Creation time to set.
        @return Creation time.
     */
    uint32_t creation_time() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 12);
    }
    void creation_time(uint32_t t)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 12) = t;
    }

    /**
        Modification time (in POSIX time).

        @param t Modification time to set.
        @return Modification time.
     */
    uint32_t modification_time() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 16);
    }
    void modification_time(uint32_t t)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 16) = t;
    }

    /**
        Deletion time (in POSIX time).

        @param t Deletion time to set.
        @return Deletion time.
     */
    uint32_t deletion_time() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 20);
    }
    void deletion_time(uint32_t t)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 20) = t;
    }

    /**
        Group ID of the file owner.

        @param g Group ID to set.
        @return Group ID.
     */
    uint16_t gid() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data() + 24);
    }
    void gid(uint16_t g)
    {
        *reinterpret_cast<uint16_t*>(data.data() + 24) = g;
    }

    /**
        Number of hard links to the file.

        @param n Number of hard links to set to set.
        @return Number of hard links.
     */
    uint16_t hard_links() const
    {
        return *reinterpret_cast<const uint16_t*>(data.data() + 26);
    }
    void hard_links(uint16_t n)
    {
        *reinterpret_cast<uint16_t*>(data.data() + 26) = n;
    }

    /**
        Number of disk sectors used by the file data.

        @param n Number of disk sectors to set.
        @return Number of disk sectors.
     */
    uint32_t sectors() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 30);
    }
    void sectors(uint32_t n)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 30) = n;
    }

    /**
        Values of the flags field.
     */
    enum flags_t : uint32_t {
        /** Secure deletion (unused). */
        secure_del = 0x00000001,
        /** Keep copy on deletion (unused). */
        del_copy = 0x00000002,
        /** File compression (unused). */
        compression = 0x00000004,
        /** Synchronous updating of the disk. */
        sync = 0x00000008,
        /** Immutable file. */
        immutable = 0x00000010,
        /** Only appending is permitted. */
        append_only = 0x00000020,
        /** File not included by dump command. */
        not_dump = 0x00000040,
        /** Do not update last access time. */
        no_last_access = 0x00000080,
        /** Hash indexed directory. */
        hash_dir = 0x00010000,
        /** AFS directory. */
        afs_dir = 0x00020000,
        /** Journal file data. */
        journal_data = 0x00040000
    };

    /**
        Flags.

        @param f Flags to set.
        @return Flags.
     */
    flags_t flags() const
    {
        return *reinterpret_cast<const flags_t*>(data.data() + 32);
    }
    void sectors(flags_t f)
    {
        *reinterpret_cast<flags_t*>(data.data() + 32) = f;
    }

    /**
        First OS specific value.

        @param v OS specific value to set.
        @return OS specific value.
     */
    uint32_t os_1() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 36);
    }
    void os_1(uint32_t v)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 36) = v;
    }


    /**
        Number of direct block pointers.
     */
    static constexpr size_t no_direct = 12;

    /**
        Direct block pointer, specified by index.

        @param bl Block pointer value to set. Does nothing if n is out of
               bounds.
        @param n Index of pointer.
        @return Block pointer value. 0 if out of bounds.
     */
    uint32_t direct(size_t n) const
    {
        if (n >= no_direct)
            return 0;
        return *reinterpret_cast<const uint32_t*>(data.data() + 40 + 4*n);
    }
    void direct(uint32_t bl, size_t n)
    {
        if (n >= no_direct)
            return;
        *reinterpret_cast<uint32_t*>(data.data() + 40 + 4*n) = bl;
    }

    /**
        Singly indirect pointer.

        @param v Singly indirect pointer to set.
        @return Singly indirect pointer.
     */
    uint32_t s_indirect() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 88);
    }
    void s_indirect(uint32_t v)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 88) = v;
    }

    /**
        Doubly indirect pointer.

        @param v Doubly indirect pointer to set.
        @return Doubly indirect pointer.
     */
    uint32_t d_indirect() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 92);
    }
    void d_indirect(uint32_t v)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 92) = v;
    }

    /**
        Triply indirect pointer.

        @param v Triply indirect pointer to set.
        @return Triply indirect pointer.
     */
    uint32_t t_indirect() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 96);
    }
    void t_indirect(uint32_t v)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 96) = v;
    }

    /**
        Generation number. Used for NFS.

        @param n Generation number to set.
        @return Generation number.
     */
    uint32_t gen_no() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 100);
    }
    void gen_no(uint32_t v)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 100) = v;
    }

    /**
        Extended attribute block, file ACL.

        @param acl File ACL to set to set.
        @return File ACL.
     */
    uint32_t file_acl() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 104);
    }
    void file_acl(uint32_t acl)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 104) = acl;
    }

    /**
        Extended attribute block, directory ACL.

        @param acl Directory ACL to set to set. Does nothing if not a directory.
        @return Directory ACL. 0 if not a directory.
     */
    uint32_t directory_acl() const
    {
        if (type() != directory)
            return 0;
        return *reinterpret_cast<const uint32_t*>(data.data() + 108);
    }
    void directory_acl(uint32_t acl)
    {
        if (type() != directory)
            return;
        *reinterpret_cast<uint32_t*>(data.data() + 108) = acl;
    }

    /**
        Upper 4 bytes of the file size, if not a directory.

        @param usz Upper size to set. Does nothing if a directory.
        @return Upper size. 0 if a directory.
     */
    uint32_t upper_size() const
    {
        if (type() == directory)
            return 0;
        return *reinterpret_cast<const uint32_t*>(data.data() + 108);
    }
    void upper_size(uint32_t usz)
    {
        if (type() == directory)
            return;
        *reinterpret_cast<uint32_t*>(data.data() + 108) = usz;
    }

    /**
        Block address of fragment.

        @param bl Block address of fragment to set.
        @return Block address of fragment.
     */
    uint32_t frag_addr() const
    {
        return *reinterpret_cast<const uint32_t*>(data.data() + 112);
    }
    void frag_addr(uint32_t bl)
    {
        *reinterpret_cast<uint32_t*>(data.data() + 112) = bl;
    }

    /**
        Size, in bytes, of the second OS specific value.
     */
    static constexpr size_t os_2_size = 12;

    /**
        Second OS specific value.

        @param v OS specific value to set.
        @return OS specific value.
     */
    klib::array<uint8_t, os_2_size> os_2() const;
    void os_2(const klib::array<uint8_t, os_2_size>& v);

    /**
        Whether the record seems to be valid.

        @param v Set whether the record is valid.
        @return True if the record seems to be valid, false otherwise.
     */
    bool valid() const { return val; }
    void valid(bool v) { val = v; }

protected:
    // Whether we seem to have a valid record.
    bool val;
    // Size of used data.
    static constexpr size_t data_size = 128;
    // Actual store for the data.
    klib::array<uint8_t, data_size> data;
    // Size of the inode on disk. Read from the superblock.
    size_t disk_size;
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
        the inode index and file system pointer.

        @param m C-style mode to open the file in.
        @param indx Index in the file system of the inode.
        @param fs File system the file belongs to.
     */
    Ext2File(const char* m, size_t indx, Ext2FileSystem& fs);

    /**
        Closes the handle, freeing any resources used. Further accesses will
        fail. Flushes the inode record and deletes it from the file system
        cache.

        @return 0 if successful, otherwise EoF.
     */
    virtual int close() override;

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
    // Inode index. Necessary for editing the inode.
    size_t inode_index;

private:
    /**
        Called by truncate. Reads the block pointed top as a list of blocks,
        either to deallocate, or read as another list of blocks, according to
        the depth.

        @param bl The block containing the list of blocks to read.
        @param depth Depth left to recurse. 0 for a direct pointer, 1 for a
               singly indirect, etc.
        @return True if we reached a 0, indicating end of file.
     */
    bool truncate_recursive(size_t bl, size_t depth);
};

/**
    Stores the contents of an ext2 directory as a map from name to inode.
 */
class Ext2Directory : public Directory {
public:
    /**
        Constructor. Given an inode index and a file system, process the inode
        as a directory, creating a mapping from names to inode indices.

        @param indx Index in the file system of that inode.
        @param fs File system to work on.
        @param w Whether the directory is open for writing. Defaults to false.
     */
    Ext2Directory(size_t indx, Ext2FileSystem& fs, bool w = false);

    /**
        Virtual destructor. Calls close() to free any resources.
     */
    virtual ~Ext2Directory() { close(); }

    /**
        Closes the handle, freeing any resources used. Further accesses will
        fail.
     */
    virtual void close() override;

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

    /**
        Values for the type field.
     */
    enum type_t : uint8_t {
        /** Unknown type. */
        unknown = 0x0,
        /** Regular file. */
        file = 0x1,
        /** Directory. */
        directory = 0x2,
        /** Character device. */
        char_dev = 0x3,
        /** Block device. */
        block_dev = 0x4,
        /** FIFO pipe. */
        fifo = 0x5,
        /** Unix socket. */
        socket = 0x6,
        /** Symbolic link. */
        sym_link = 0x7
    };

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

    // Inode index. Necessary for editing the inode.
    size_t inode_index;
    // File system the directory resides on.
    Ext2FileSystem& ext2fs;
    // Vector of the entries.
    klib::vector<Entry> contents;
};

/**
    Implementation of the abstract FileSystem base for ext2 file systems.
 */
class Ext2FileSystem : public FileSystem {
protected:
    // Keep a cache of block allocation tables. Means we can deallocate lots of
    // blocks without updating each table multiple times. The key is which block
    // group the table is for. The data is the allocation table, with a boolean
    // indicating whether it's been modified.
    klib::map<size_t, klib::pair<klib::vector<char>, bool>> block_alloc;

    // Keep a cache of inode allocation tables. The key is which block group the
    // table is for. The data is the allocation table, with a boolean indicating
    // whether it's been modified.
    klib::map<size_t, klib::pair<klib::vector<char>, bool>> inode_alloc;

    // Keep a cache of inodes for files undergoing changes. Keep them centrally
    // for the file system so that files open multiple times can be kept in sync
    // without repeated disk writes and reads. The boolean is used to keep track
    // of whether the inode has been modified.
    klib::map<size_t, klib::pair<Ext2Inode, bool>> inodes;

    // The contents of the superblock, along with a flag for whether it's
    // been modified.
    klib::pair<Ext2SuperBlock, bool> super_block;

    // Table of block group descriptors, along with a flag for each indicating
    // whether it has been modified.
    klib::vector<klib::pair<BlockGroupDescriptor, bool>> bgdt;

public:
    /**
        Features from the required feature set currently supported. Bits set
        correspond to the required features in the superblock. And that value
        with the complement of this and if it's not zero, we don't support it.
     */
    static constexpr ext2_required_features required_supported =
        ext2_required_features::directories_type;

    /**
        Features from the required for writing feature set currently supported.
        Bits set correspond to the required for writing features in the
        superblock. And that value with the complement of this and if it's not
        zero, we don't support it.
     */
    static constexpr ext2_required_writing_features required_writing_supported =
        ext2_required_writing_features::large_file_size |
        ext2_required_writing_features::sparse;

    /**
        Constructor. Creates an ext2 driver for the given device or partition.

        @param drv /dev name of the device.
     */
    Ext2FileSystem(const klib::string& drv);

    /**
        Opens a directory, returning a pointer to the directory handle. nullptr
        is returned on errors, such as the directory not existing, or the name
        referring to a file. The directory inode will be cached.

        @param name Full path name from the root directory of the file system.
        @return Directory pointer. nullptr on error.
     */
    virtual Directory* diropen(const klib::string& name) override;

    /**
        Opens a file, returning a pointer to the file handle. nullptr is
        returned on errors, such as the file not existing when reading is
        requested, asking for write when the file system is read only, or if the
        file is a directory. The file inode will be cached.

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
        return 1024 << super_block.first.block_size_shift();
    }

    /**
        Given an inode index, return the file size. This depends on the type of
        the file and the optional features specified in the superblock. The
        inode will be cached if it isn't already.

        @param inode_index Inode index to check.
        @return Size of the file described by inode.
     */
    klib::streamoff file_size(size_t inode_index);

    /**
        Calls the provided functor on the superblock. Access through this
        method allows the file system to keep track of whether the superblock
        has been modified and therefore whether it needs to be written back to
        disk on the next flush call. Member methods of Ext2FileSystem are
        permitted to call const functions of the superblock directly. Non-member
        methods and member methods that need to modify the superblock must
        always use this method.

        @param mod Whether the function call will modify the superblock. When
               flushed, the superblock will not be written back to disk unless
               a superblock call with mod set to true has been made. I guess
               it might be possible to deduce this from whether the functor is
               const or not, but I haven't worked out how to do that.
        @param f Functor to call on the superblock.
        @param args Arguments to pass to the functor.
        @return Whatever the functor returns.
     */
    template <typename F, typename... Args>
    auto superblock_call(bool mod, F f, Args&&... args) ->
        decltype(f(super_block.first, klib::forward<Args>(args)...))
    {
        // Set the inode to changed, if necessary.
        if (mod)
            super_block.second = true;
        // It is still possible to fail here, if we get interrupted and then
        // some other thread accessing the same inode closes the file. I think
        // that's sufficiently unlikely that I'm not considering it. 
        return f(super_block.first, klib::forward<Args>(args)...);
    }

    /**
        Calls the provided functor on the block group descriptor with the given
        index. Access through this method allows the file system to keep track
        of whether the descriptor has been modified and therefore whether it
        needs to be written back to disk on the next flush call. Member methods
        of Ext2FileSystem are permitted to call const functions of the
        descriptor directly. Non-member methods and member methods that need to
        modify the descriptor must always use this method.

        @param blg_index Index of the descriptor in the BGDT.
        @param mod Whether the function call will modify the descriptor. When
               flushed, the descriptor will not be written back to disk unless
               a superblock call with mod set to true has been made. I guess
               it might be possible to deduce this from whether the functor is
               const or not, but I haven't worked out how to do that.
        @param f Functor to call on the descriptor.
        @param args Arguments to pass to the functor.
        @return Whatever the functor returns.
     */
    template <typename F, typename... Args>
    auto bgdt_call(size_t blg_index, bool mod, F f, Args&&... args) ->
        decltype(f(bgdt[blg_index].first, klib::forward<Args>(args)...))
    {
        // Set the inode to changed, if necessary.
        if (mod)
            bgdt[blg_index].second = true;
        // It is still possible to fail here, if we get interrupted and then
        // some other thread accessing the same inode closes the file. I think
        // that's sufficiently unlikely that I'm not considering it. 
        return f(bgdt[blg_index].first, klib::forward<Args>(args)...);
    }

    /**
        Given an inode for a file and a block index in the file, get the block
        address in the file system. The inode will be cached.

        @param inode_index Inode index to use. Will be cached if it wasn't
                already.
        @param bl Block index in the file.
        @return Block address in the file system.
     */
    size_t inode_lookup(size_t inode_index, size_t bl);

    /**
        Given an inode for a file and a block index in the file, set the block
        address in the file system. The inode will be cached.

        @param inode_index Inode index to use. Will be cached if it wasn't
                already.
        @param bl_index Block index in the file.
        @param bl_addr Block address in the file system.
        @return 0 on success, -1 on failure.
     */
    int inode_set(size_t inode_index, size_t bl_index, size_t bl_addr);

    /**
        Looks up the inode corresponding to the inode index and calls the
        provided functor on it. The functor is expected to be of the form
        ret_type operator()(Ext2Inode& in, Args&&... args)
        { return in.func(args...); } where func is one of the inode functions.
        This provides a way to access and change the properties of a cached
        inode.

        @param inode_index Index of the inode.
        @param mod Whether the function call will modify the inode. When
               flushed, the inode information will not be written back to disk
               unless an inode call with mod set to true has been made. I guess
               it might be possible to deduce this from whether the type of the
               arguments of F has a const Ext2Inode& or an Ext2Inode, but that
               sounds difficult.
        @param f Functor to call on the inode.
        @param args Arguments to pass to the functor.
        @return Whatever the functor returns.
     */
    template <typename F, typename... Args>
    auto inode_call(size_t inode_index, bool mod, F f, Args&&... args) ->
        decltype(f(inodes[inode_index].first, klib::forward<Args>(args)...))
    {
        cache_inode(inode_index);
        // Set the inode to changed, if necessary.
        if (mod)
            inodes[inode_index].second = true;
        // It is still possible to fail here, if we get interrupted and then
        // some other thread accessing the same inode closes the file. I think
        // that's sufficiently unlikely that I'm not considering it. 
        return f(inodes[inode_index].first, klib::forward<Args>(args)...);
    }

    /**
        Writes the cached inode with the matching inode index back to disk, but
        only if it's been modified. Optionally deletes the cache record.

        @param inode_index Index of the inode to flush. 0 is not a valid inode
               index, so is used to indicate flushing all cached inodes.
        @param free Whether to remove the record from the inode cache. Defaults
               to false.
        @return 0 on success, -1 on failure.
     */
    int flush_inode(size_t inode_index, bool free = false);

    /**
        Writes all the metadata back to disk, including the superblock, the
        BGDT and any cached allocation tables.

        @return 0 on success, -1 on failure.
     */
    int flush_metadata();

    /**
        Allocate a new block. Looks through the block allocation tables to find
        an unused block. If it finds one, update the block allocation table and
        the inode. Does not flush metadata back to disk. Fails if no blocks are
        free. A block within the same block group as the inode will be allocated
        if possible. The search will begin at the last existing allocated block
        in the file, unless that is not in the same block group as the inode, in
        which case that block group will be searched first.

        @param inode_index Inode to find a new block for.
        @param bl_index Block index the new block will be in the file. Because
               of the way the write works, the inode file size may be out of
               step, so we can't use that to get this information. We could
               follow the pointers in the inode, but that would be tedious.
        @param indirect_block If true, this block will be used for storing
               block pointers to be folloowed from the inode indirect pointers.
               In this case, the bl_index is ignored. Defualts to false, meaning
               the block will be used for storing file data.
        @return Block number on success, or 0 on failure (which is an invalid
                block).
     */
    size_t allocate_new_block(size_t inode_index, size_t bl_index,
        bool indirect_block = false);

    /**
        Deallocate a block, setting it to unused in the block group usage table.
        Updates the entry for number of unused blocks in the block group
        descriptor table and superblock too. Does not flush the information
        back to the disk. Does not change any inodes.

        @param bl Block to deallocate.
        @return 0 on success, -1 on failure.
     */
    int deallocate_block(size_t bl);

    /**
        Gets or sets a bit in a cached block allocation table.

        @param bg_index Index of the block group to access.
        @param index Block index within the group.
        @param alloc Whether the block should be allocated or not.
        @return Whether the bit is allocated.
     */
    void access_block_alloc(size_t bg_index, size_t index, bool alloc);
    bool access_block_alloc(size_t bg_index, size_t index);

    /**
        Allocate a new inode. Looks through the inode allocation tables to find
        an unused inode. If it finds one, update the inode allocation table. The
        inode data structure is initialised in an invalid state. Data must be
        entered via inode_call, then the state set to valid. The inode is put in
        the cache and is not written to disk until flush_inode is called on it.
        The search begins in the same block group as the parent directory, if
        provided.

        @param inode_index Inode of the parent directory for the new file. The
               search for a free inode begins here. Defualts to 1.
        @return Index of the new inode on success. 0 (whihc is not a valid inode
                index) on failure.
     */
    size_t allocate_new_inode(size_t inode_index = 1);

    /**
        Deallocate an inode, setting it to unused in the block group usage table.
        Updates the entry for number of unused inodes in the block group
        descriptor table and superblock too. Does not flush the information
        back to the disk. Does not change any directory entries.

        @param indx Inode to deallocate.
        @return 0 on success, -1 on failure.
     */
    int deallocate_inode(size_t indx);

    /**
        Gets or sets a bit in a cached inode allocation table.

        @param bg_index Index of the block group to access.
        @param index Inode index within the group.
        @param alloc Whether the inode should be allocated or not.
        @return Whether the bit is allocated.
     */
    void access_inode_alloc(size_t bg_index, size_t index, bool alloc);
    bool access_inode_alloc(size_t bg_index, size_t index);

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
    // Whether this seems to be a valid ext2 file system.
    bool val;
    // Inode index for the root directory.
    static constexpr size_t root_inode = 2;

    /**
        Turn a block address into a byte offset.

        @param ba Block address.
        @return Byte offset of the start of the block.
     */
    uint64_t block_to_byte(size_t ba) const
    {
        return ba * block_size();
    }

    /**
        Get the block address a byte offset belongs to.

        @param off Byte offset to test.
        @return Block address that contains the byte.
     */
    size_t byte_to_block(uint64_t off) const
    {
        return
            static_cast<size_t>(off / block_size());
    }

    // Writes the superblock back to the disk, after changes
    // in memory have been made. Return 0 on success, -1 on failure.
    int flush_superblock();

    // Writes the block group descriptor table back to the disk, after changes
    // in memory have been made. Return 0 on success, -1 on failure.
    int flush_bgdt();

    // Caches the block allocation table for a particular block group, by
    // reading it from the disk. Return 0 on success, -1 on failure.
    int cache_block_alloc(size_t bg_index);

    // Flush the cache of block allocation tables, writing them back to the
    // disk. Return 0 on success, -1 on failure.
    int flush_block_alloc();

    // Caches the inode allocation table for a particular block group, by
    // reading it from the disk. Return 0 on success, -1 on failure.
    int cache_inode_alloc(size_t bg_index);

    // Flush the cache of inode allocation tables, writing them back to the
    // disk. Return 0 on success, -1 on failure.
    int flush_inode_alloc();

    // Caches the a particular inode, by reading it from the disk. Return 0 on
    // success, -1 on failure.
    int cache_inode(size_t inode_index);

    // Lookup the inode corresponding to a file name and get back the index.
    // Inodes for the file and its parent directories are all cached. 0 is not
    // a valid inode index and is used for failure.
    size_t get_inode_index(const klib::string& name);

    // Determines whether the block group with index bg should contain backup
    // copies of the superblock and BDGT, if the sparse backups feature is in
    // use/
    bool backup_group(size_t bg) const;
};

/**
    Wrapper function to create an ext2 file system, or return nullptr if the
    disk does not have an ext2 file sytsem.

    @param drv Full path of the device (probably in /dev).
    @return File system for the disk, or nullptr if it wasn't ext.
 */
Ext2FileSystem* create_ext2(const klib::string& drv);
#endif /* EXT_H */
