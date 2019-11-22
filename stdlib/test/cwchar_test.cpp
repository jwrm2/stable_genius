#include <iostream>

#include "../include/cwchar"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // mbstate_t
    {
        klib::mbstate_t mb1;
        if (mb1.fill[0] != 0 || mb1.fill[1] != 0 || mb1.fill[2] != 0 ||
            mb1.fill[3] != 0 || mb1.fill[4] != 0 || mb1.fill[5] != 0)
        {
            ++fail_count;
            std::cout << "FAILED mbstate_t initialisation\n";
        }
        klib::mbstate_t mb2;
        klib::mbstate_t mb3;
        mb3.fill[0] = 0; mb3.fill[1] = 1; mb3.fill[2] = 2; mb3.fill[3] = 3;
        mb3.fill[4] = 4; mb3.fill[5] = 5;
        if (!(mb1 == mb2))
        {
            ++fail_count;
            std::cout << "FAILED mbstate_t operator== equal\n";
        }
        if (mb1 == mb3)
        {
            ++fail_count;
            std::cout << "FAILED mbstate_t operator== not equal\n";
        }
        if (mb1 != mb2)
        {
            ++fail_count;
            std::cout << "FAILED mbstate_t operator!= equal\n";
        }
        if (!(mb1 != mb3))
        {
            ++fail_count;
            std::cout << "FAILED mbstate_t operator!= not equal\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
