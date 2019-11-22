#include "Device.h"

#include <map>
#include <string>
#include <utility>

#include "Kernel.h"
#include "Logger.h"
#include "util.h"

/******************************************************************************
 ******************************************************************************/

void BlockDevice::dump(klib::ostream& dest) const
{
    dest << desc;
    dest << " size is " << format_bytes(size) << '\n';
    dest.flush();
}

/******************************************************************************
 ******************************************************************************/

void print_DiskIoError(klib::ostream& dest, DiskIoError err)
{
    static const klib::map<DiskIoError, klib::string> err_name {
        klib::pair<DiskIoError, klib::string>
            {DiskIoError::success,
            "Success"},
        klib::pair<DiskIoError, klib::string>
            {DiskIoError::bad_alignment,
            "Offset not a multiple of sector size"},
        klib::pair<DiskIoError, klib::string>
            {DiskIoError::bad_size,
            "Size too large or not a multiple of sector size"},
        klib::pair<DiskIoError, klib::string>
            {DiskIoError::read_only,
            "Cannot write to a read-only device"},
        klib::pair<DiskIoError, klib::string>
            {DiskIoError::hardware_fault,
            "Controller reports hardware fault"},
        klib::pair<DiskIoError, klib::string>
            {DiskIoError::no_device,
            "Device does not exist"},
        klib::pair<DiskIoError, klib::string>
            {DiskIoError::bad_driver,
            "Wrong driver for device"}
    };

    dest << "Disk I/O error: " << err_name.find(err)->second << '\n';
    dest.flush();
}

/******************************************************************************
 ******************************************************************************/
