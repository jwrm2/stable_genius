#include "../include/cmath"

namespace NMSP {

/******************************************************************************
 ******************************************************************************/

double round(double input)
{
    if (isnan(input))
        return NAN;

    double truncated = trunc(input);

    if (fabs(input - truncated) < 0.5)
        return truncated;
    else
        return truncated + (input < 0.0 ? -1.0 : 1.0);
}

/******************************************************************************
 ******************************************************************************/

double trunc(double input)
{
    if (isnan(input))
        return NAN;

    return static_cast<double>(static_cast<long long int>(input));
}

/******************************************************************************
 ******************************************************************************/

double floor(double input)
{
    if (isnan(input))
        return NAN;

    if (isnan(input))
        return NAN;
    return trunc(input) - (input < 0.0 ? 1 : 0);
}

/******************************************************************************
 ******************************************************************************/

double fabs(double input)
{
    if (isnan(input))
        return NAN;

    return (input < 0.0 ? -input : input);
}

/******************************************************************************
 ******************************************************************************/

double fmod(double input, double mod)
{
    if (isnan(input) || isnan(mod) || mod == 0.)
        return NAN;

    return (input - mod * trunc(input / mod));
}

/******************************************************************************
 ******************************************************************************/

bool isnan(float arg) { return __builtin_isnan(arg); }

bool isnan(double arg) { return __builtin_isnan(arg); }

bool isnan(long double arg) { return __builtin_isnan(arg); }

/******************************************************************************
 ******************************************************************************/

} // NMSP namespace
