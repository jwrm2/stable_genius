#include "../include/cwchar"

namespace NMSP {

/******************************************************************************
 ******************************************************************************/

bool mbstate_t::operator==(const mbstate_t& other) const
{
    for (size_t i = 0; i < sz; ++i)
        if (fill[i] != other.fill[i])
            return false;

    return true;
}

bool mbstate_t::operator!=(const mbstate_t& other) const
{
    return !(*this == other);
}

/******************************************************************************
 ******************************************************************************/

} // end NMSP namespace
