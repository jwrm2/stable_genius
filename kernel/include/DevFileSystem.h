#ifndef DEV_FILE_SYSTEM_H
#define DEV_FILE_SYSTEM_H

#include <map>
#include <string>
#include <vector>

#include "FileSystem.h"

// Forward declarations
class Device;
enum class DeviceType;
class IdeController;

/**
    Implementation of FileSystem for the special dev file system. This maintains
    a list of physical devices such as serial ports and disks, as well as
    allowing them to be opened as files for reading and writing them.
 */
class DevFileSystem : public FileSystem {
public:
    /**
        Constructor. Creates empty lists of devices and sets the underlying
        device to non-existant.
     */
    DevFileSystem();

    /**
        Destructor. Called on shutdown. Calls the destructors for all physical
        device drivers, so should ensure all shutdown operations are performed.
     */
    virtual ~DevFileSystem();

    /**
        Gets the driver for the device with the given name.

        @param n Name of the device.
        @return Driver for the device, or nullptr if it doesn't exist.
     */
    Device* get_device_driver(const klib::string& name) const;

    /**
        Gets the full list of device drivers. Read only.

        @return Reference to the full list of device drivers.
     */
    const klib::map<klib::string, Device*>& get_drivers() const
    {
        return device_drivers;
    } 

    /**
        Opens a directory, returning a pointer to the directory handle. nullptr
        is returned on errors, such as the directory not existing, or the name
        referring to a file. Since this is a virtual file system with no
        underlying physical device, directory listings are created on-the-fly as
        required.

        @param name Full path name from the root directory of the file system.
        @return Directory pointer. nullptr on error.
     */
    virtual Directory* diropen(const klib::string& name) override;

    /**
        Opens a file, returning a pointer to the file handle. nullptr is
        returned on errors, such as the file not existing. This is used to open
        physical devices for raw reading and writing.

        @param name Full path name from the root directory of the file system.
        @param mode C-style mode in which to open the file.
        @return File pointer. nullptr on error.
     */
    virtual klib::FILE* fopen(const klib::string& name, const char* mode)
        override;

    /**
        Get the block size of the file system. Since this is a virtual FS, it
        doesn't have blocks and the size is therefore 1 byte.

        @return Block size.
     */
    virtual size_t block_size() const override { return 1; }

    /**
        Removing a device file doesn't make sense. Always fails without doing
        anything. Use unmount instead to disconnect a device. 

        @param name Full path name from the root directory of the file system.
        @return -1, indicating failure.
     */
    virtual int remove(const klib::string&) override { return -1; }

    /**
        The dev names are set by standards and renames are not permitted. This
        function does nothing.

        @param f File to rename.
        @param n New name for the file.
     */
    virtual void rename(const klib::string&, const klib::string&) override {}

    /**
        Given a standard dev name of a block device, returns the driver for the
        device.

        @param n Name to look up.
        @return Driver for the device, or nullptr if it doesn't exist.
     */

    /**
        Performs a search for serial ports and adds them to the device list.
        TODO for now this will only add COM1.
     */
    void add_serial();

    /**
        Adds drivers for the devices attached to the provided list of IDE
        controllers, then searches those which are hard drives for partitions
        and adds those too.

        @param ides List of IDE controllers. The list won't be changed, but
                    references will be passed on to the partition drivers, which
                    may end up calling non-const member functions.
     */
    void add_ide(klib::vector<IdeController>& ides);

    /**
        Performs a search for TTY devices and adds them to the device list.
        TODO for now this will only add one TTY with the keyboard as input and
        the VGA as output.
     */
    void add_tty();

protected:
    // List of the device drivers. They are keyed by standard Linux names,
    // eg. /dev/sda or /dev/sr1.
    klib::map<klib::string, Device*> device_drivers;
};

/**
    Gets the name for a new entry for a device following naming conventions.
    Eg. if two hard drives already exist a third is sdc.

    @param t Type of device.
    @param m Map to check for current devices.
    @return Name to use for the new device.
*/
klib::string get_new_device_name(DeviceType t,
    const klib::map<klib::string, Device*>& m);

/**
    Given a device type, returns the string name corresponding to the standard
    dev prefix for the type.

    @param t Device type to check.
    @return dev string prefix for the type.
 */
klib::string type_to_prefix(DeviceType t);

/**
    Given a device type, checks whether the device is indexed by a number or a
    letter.

    @param t Device type to check.
    @return True for letter indexing, false for number indexing.
 */
bool alpha_index(DeviceType t);
// 

#endif /* DEV_FILE_SYSTEM_H */
