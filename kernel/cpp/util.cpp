#include "util.h"

#include <stdint.h>

#include <cmath>
#include <string>
#include <vector>

/******************************************************************************
 ******************************************************************************/

klib::string format_bytes(uint64_t bytes)
{
    klib::vector<klib::string> suffixes =
        {"B", "KB", "MB", "GB", "TB", "PB", "EB"};
    klib::string ret_val;
    size_t fct = 1024;
    size_t indx = 0;

    if (bytes < fct)
    {
        ret_val = klib::to_string(bytes) + suffixes[indx];
        return ret_val;
    }

    while (indx + 1 < suffixes.size())
    {
        size_t div = bytes / fct;
        size_t rem = bytes % fct;
        ++indx;

        if (div < fct)
        {
            if (div >= 10)
                ret_val = klib::to_string(div) + suffixes[indx];
            else
            {
                ret_val = klib::to_string(div) + '.';
                ret_val += klib::to_string(static_cast<int>(klib::round(
                    10 * static_cast<double>(rem) / fct)));
                ret_val += suffixes[indx];
            }

            return ret_val;
        }

        bytes = div;
    }

    return ret_val;
}

/******************************************************************************
 ******************************************************************************/
