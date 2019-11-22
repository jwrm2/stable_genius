#include <iostream>

#include "../include/cmath"

using namespace klib;

int main()
{
    size_t fail_count = 0;
    double prec = 0.000001;

    // round
    {
        double d1 = 1.1;
        if (round(d1) != 1.)
        {
            ++fail_count;
            std::cout << "FAILED round(1.1)" << '\n';
        }
        double d2 = 1.5;
        if (round(d2) != 2.)
        {
            ++fail_count;
            std::cout << "FAILED round(1.5)" << '\n';
        }
        double d3 = 1.8;
        if (round(d3) != 2.)
        {
            ++fail_count;
            std::cout << "FAILED round(1.8)" << '\n';
        }
        double d4 = 0.1;
        if (round(d4) != 0.)
        {
            ++fail_count;
            std::cout << "FAILED round(0.1)" << '\n';
        }
        double d5 = 0.5;
        if (round(d5) != 1.)
        {
            ++fail_count;
            std::cout << "FAILED round(0.5)" << '\n';
        }
        double d6 = 0.8;
        if (round(d6) != 1.)
        {
            ++fail_count;
            std::cout << "FAILED round(0.8)" << '\n';
        }
        double d7 = -1.1;
        if (round(d7) != -1.)
        {
            ++fail_count;
            std::cout << "FAILED round(-1.1)" << '\n';
        }
        double d8 = -1.5;
        if (round(d8) != -2.)
        {
            ++fail_count;
            std::cout << "FAILED round(-1.5)" << '\n';
        }
        double d9 = -1.8;
        if (round(d9) != -2.)
        {
            ++fail_count;
            std::cout << "FAILED round(-1.8)" << '\n';
        }
        double d10 = -0.1;
        if (round(d10) != 0.)
        {
            ++fail_count;
            std::cout << "FAILED round(-0.1)" << '\n';
        }
        double d11 = -0.5;
        if (round(d11) != -1.)
        {
            ++fail_count;
            std::cout << "FAILED round(-0.5)" << '\n';
        }
        double d12 = -0.8;
        if (round(d12) != -1.)
        {
            ++fail_count;
            std::cout << "FAILED round(-0.8)" << '\n';
        }
        if (!isnan(round(NAN)))
        {
            ++fail_count;
            std::cout << "FAILED round(NaN)" << '\n';
        }
    }
    // trunc
    {
        double d1 = 1.1;
        if (trunc(d1) != 1.)
        {
            ++fail_count;
            std::cout << "FAILED trunc(1.1)" << '\n';
        }
        double d2 = 1.8;
        if (trunc(d2) != 1.)
        {
            ++fail_count;
            std::cout << "FAILED trunc(1.8)" << '\n';
        }
        double d3 = 0.1;
        if (trunc(d3) != 0.)
        {
            ++fail_count;
            std::cout << "FAILED trunc(0.1)" << '\n';
        }
        double d4 = 0.8;
        if (trunc(d4) != 0.)
        {
            ++fail_count;
            std::cout << "FAILED trunc(0.8)" << '\n';
        }
        double d5 = -1.1;
        if (trunc(d5) != -1.)
        {
            ++fail_count;
            std::cout << "FAILED trunc(-1.1)" << '\n';
        }
        double d6 = -1.8;
        if (trunc(d6) != -1.)
        {
            ++fail_count;
            std::cout << "FAILED trunc(-1.8)" << '\n';
        }
        double d7 = -0.1;
        if (trunc(d7) != 0.)
        {
            ++fail_count;
            std::cout << "FAILED trunc(-0.1)" << '\n';
        }
        double d8 = -0.8;
        if (trunc(d8) != 0.)
        {
            ++fail_count;
            std::cout << "FAILED trunc(-0.8)" << '\n';
        }
        if (!isnan(trunc(NAN)))
        {
            ++fail_count;
            std::cout << "FAILED trunc(NaN)" << '\n';
        }
    }
    // floor
    {
        double d1 = 1.1;
        if (floor(d1) != 1.)
        {
            ++fail_count;
            std::cout << "FAILED floor(1.1)" << '\n';
        }
        double d2 = 1.8;
        if (floor(d2) != 1.)
        {
            ++fail_count;
            std::cout << "FAILED floor(1.8)" << '\n';
        }
        double d3 = 0.1;
        if (floor(d3) != 0.)
        {
            ++fail_count;
            std::cout << "FAILED floor(0.1)" << '\n';
        }
        double d4 = 0.8;
        if (floor(d4) != 0.)
        {
            ++fail_count;
            std::cout << "FAILED floor(0.8)" << '\n';
        }
        double d5 = -1.1;
        if (floor(d5) != -2.)
        {
            ++fail_count;
            std::cout << "FAILED floor(-1.1)" << '\n';
        }
        double d6 = -1.8;
        if (floor(d6) != -2.)
        {
            ++fail_count;
            std::cout << "FAILED floor(-1.8)" << '\n';
        }
        double d7 = -0.1;
        if (floor(d7) != -1.)
        {
            ++fail_count;
            std::cout << "FAILED floor(-0.1)" << '\n';
        }
        double d8 = -0.8;
        if (floor(d8) != -1.)
        {
            ++fail_count;
            std::cout << "FAILED floor(-0.8)" << '\n';
        }
        if (!isnan(floor(NAN)))
        {
            ++fail_count;
            std::cout << "FAILED floor(NaN)" << '\n';
        }
    }
    // fabs
    {
        double d1 = 1.5;
        if (fabs(d1) != 1.5)
        {
            ++fail_count;
            std::cout << "FAILED fabs(1.5)" << '\n';
        }
        double d2 = -1.5;
        if (fabs(d2) != 1.5)
        {
            ++fail_count;
            std::cout << "FAILED fabs(-1.5)" << '\n';
        }
        double d3 = 0.;
        if (fabs(d3) != 0.)
        {
            ++fail_count;
            std::cout << "FAILED fabs(0.)" << '\n';
        }
        if (!isnan(fabs(NAN)))
        {
            ++fail_count;
            std::cout << "FAILED fabs(NaN)" << '\n';
        }
    }
    // fmod
    {
        if (fmod(4.3, 0.3) > 0.1 + prec || fmod(4.3, 0.3) < 0.1 - prec )
        {
            ++fail_count;
            std::cout << "FAILED fmod(4.3, 0.3)" << '\n';
        }
        if (fmod(-4.3, 0.3) > -0.1 + prec || fmod(-4.3, 0.3) < -0.1 - prec)
        {
            ++fail_count;
            std::cout << "FAILED fmod(-4.3, 0.3)" << '\n';
        }
        if (fmod(4.3, -0.3) > 0.1 + prec || fmod(4.3, -0.3) < 0.1 - prec)
        {
            ++fail_count;
            std::cout << "FAILED fmod(4.3, -0.3)" << '\n';
        }
        if (fmod(-4.3, -0.3) > -0.1 + prec || fmod(-4.3, -0.3) < -0.1 - prec)
        {
            ++fail_count;
            std::cout << "FAILED fmod(-4.3, -0.3)" << '\n';
        }
        if (!isnan(fmod(4.3, 0.)))
        {
            ++fail_count;
            std::cout << "FAILED fmod(4.3, 0.)" << '\n';
        }
        if (fmod(0., 0.3) > 0. + prec || fmod(0., 0.3) < 0. - prec)
        {
            ++fail_count;
            std::cout << "FAILED fmod(0., 0.3)" << '\n';
        }
    }
    // isnan
    {
        float f1 = NAN;
        if (!isnan(f1))
        {
            ++fail_count;
            std::cout << "FAILED isnan((float)NaN)" << '\n';
        }
        if (isnan(0.f))
        {
            ++fail_count;
            std::cout << "FAILED isnan(0.f)" << '\n';
        }
        if (isnan(1.f))
        {
            ++fail_count;
            std::cout << "FAILED isnan(1.f)" << '\n';
        }
        double d1 = NAN;
        if (!isnan(d1))
        {
            ++fail_count;
            std::cout << "FAILED isnan((double)NaN)" << '\n';
        }
        if (isnan(0.))
        {
            ++fail_count;
            std::cout << "FAILED isnan(0.)" << '\n';
        }
        if (isnan(1.))
        {
            ++fail_count;
            std::cout << "FAILED isnan(1.)" << '\n';
        }
        long double ld1 = NAN;
        if (!isnan(ld1))
        {
            ++fail_count;
            std::cout << "FAILED isnan((long double)NaN)" << '\n';
        }
        if (isnan(0.l))
        {
            ++fail_count;
            std::cout << "FAILED isnan(0.l)" << '\n';
        }
        if (isnan(1.l))
        {
            ++fail_count;
            std::cout << "FAILED isnan(1.l)" << '\n';
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
