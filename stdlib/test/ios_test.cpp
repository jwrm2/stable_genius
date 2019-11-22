#include <iostream>

#include "../include/ios"

#include "../include/cwchar"
#include "../include/type_traits"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // streamoff
    if (!is_same<streamoff, long long>::value)
    {
        ++fail_count;
        std::cout << "FAILED streamoff type\n";
    }
    // fpos
    {
        // constructor
        fpos<klib::mbstate_t> fp1 {1};
        klib::mbstate_t mb1 = fp1.state();
        if (static_cast<streamoff>(fp1) != 1 || mb1.fill[0] != 0 ||
            mb1.fill[1] != 0 || mb1.fill[2] != 0 || mb1.fill[3] != 0 ||
            mb1.fill[4] != 0 || mb1.fill[5] != 0)
        {
            ++fail_count;
            std::cout << "FAILED fpos constructor\n";
        }
        // state get
        fpos<klib::mbstate_t> fp2 {1};
        klib::mbstate_t mb2 = fp2.state();
        if (mb2.fill[0] != 0 || mb2.fill[1] != 0 || mb2.fill[2] != 0 ||
            mb2.fill[3] != 0 || mb2.fill[4] != 0 || mb2.fill[5] != 0)
        {
            ++fail_count;
            std::cout << "FAILED fpos state()\n";
        }
        // state set
        fpos<klib::mbstate_t> fp3 {1};
        klib::mbstate_t mb3;
        mb3.fill[0] = 1; mb3.fill[1] = 2; mb3.fill[2] = 3; mb3.fill[3] = 4;
        mb3.fill[4] = 5; mb3.fill[5] = 6;
        fp3.state(mb3);
        klib::mbstate_t mb4 = fp3.state();
        if (mb4.fill[0] != 1 || mb4.fill[1] != 2 || mb4.fill[2] != 3 ||
            mb4.fill[3] != 4 || mb4.fill[4] != 5 || mb4.fill[5] != 6)
        {
            ++fail_count;
            std::cout << "FAILED fpos state(mbstate_t)\n";
        }
        // Get position
        fpos<klib::mbstate_t> fp4 {26};
        if (static_cast<streamoff>(fp4) != 26)
        {
            ++fail_count;
            std::cout << "FAILED fpos streamoff conversion positive\n";
        }
        fpos<klib::mbstate_t> fp5 {-26};
        if (static_cast<streamoff>(fp5) != -26)
        {
            ++fail_count;
            std::cout << "FAILED fpos streamoff conversion negative\n";
        }
        fpos<klib::mbstate_t> fp6 {0};
        if (static_cast<streamoff>(fp6) != 0)
        {
            ++fail_count;
            std::cout << "FAILED fpos streamoff conversion zero\n";
        }
        // Equality
        fpos<klib::mbstate_t> fp7 {26};
        fpos<klib::mbstate_t> fp8 {26};
        if (!(fp7 == fp8))
        {
            ++fail_count;
            std::cout << "FAILED fpos == equal\n";
        }
        fpos<klib::mbstate_t> fp9 {12};
        if (fp7 == fp9)
        {
            ++fail_count;
            std::cout << "FAILED fpos == pos not equal\n";
        }
        fpos<klib::mbstate_t> fp10 {26};
        klib::mbstate_t mb5;
        mb5.fill[0] = 1;
        fp10.state(mb5);
        if (!(fp7 == fp10))
        {
            ++fail_count;
            std::cout << "FAILED fpos == state not equal\n";
        }
        if (fp9 == fp10)
        {
            ++fail_count;
            std::cout << "FAILED fpos == pos and state not equal\n";
        }
        // Non-Equality
        if (fp7 != fp8)
        {
            ++fail_count;
            std::cout << "FAILED fpos != equal\n";
        }
        if (!(fp7 != fp9))
        {
            ++fail_count;
            std::cout << "FAILED fpos != pos not equal\n";
        }
        if (fp7 != fp10)
        {
            ++fail_count;
            std::cout << "FAILED fpos != state not equal\n";
        }
        if (!(fp9 != fp10))
        {
            ++fail_count;
            std::cout << "FAILED fpos != pos and state not equal\n";
        }
        // increase
        fpos<klib::mbstate_t> fp11 {26};
        fp11 += static_cast<streamoff>(0);
        if (static_cast<streamoff>(fp11) != 26)
        {
            ++fail_count;
            std::cout << "FAILED fpos += zero\n";
        }
        fp11 += static_cast<streamoff>(12);
        if (static_cast<streamoff>(fp11) != 38)
        {
            ++fail_count;
            std::cout << "FAILED fpos += positive\n";
        }
        fp11 += static_cast<streamoff>(-40);
        if (static_cast<streamoff>(fp11) != -2)
        {
            ++fail_count;
            std::cout << "FAILED fpos += negative\n";
        }
        // decrease
        fpos<klib::mbstate_t> fp12 {26};
        fp12 -= static_cast<streamoff>(0);
        if (static_cast<streamoff>(fp12) != 26)
        {
            ++fail_count;
            std::cout << "FAILED fpos -= zero\n";
        }
        fp12 -= static_cast<streamoff>(28);
        if (static_cast<streamoff>(fp12) != -2)
        {
            ++fail_count;
            std::cout << "FAILED fpos -= positive\n";
        }
        fp12 -= static_cast<streamoff>(-10);
        if (static_cast<streamoff>(fp12) != 8)
        {
            ++fail_count;
            std::cout << "FAILED fpos -= negative\n";
        }
        // Addition
        fpos<klib::mbstate_t> fp13 {26};
        fpos<klib::mbstate_t> fp14 {0};
        fp14 = fp13 + static_cast<streamoff>(0);
        if (static_cast<streamoff>(fp14) != 26)
        {
            ++fail_count;
            std::cout << "FAILED fpos + zero\n";
        }
        fp14 = static_cast<streamoff>(0) + fp13;
        if (static_cast<streamoff>(fp14) != 26)
        {
            ++fail_count;
            std::cout << "FAILED zero + fpos\n";
        }
        fp14 = fp13 + static_cast<streamoff>(12);
        if (static_cast<streamoff>(fp14) != 38)
        {
            ++fail_count;
            std::cout << "FAILED fpos + positive\n";
        }
        fp14 = static_cast<streamoff>(12) + fp13;
        if (static_cast<streamoff>(fp14) != 38)
        {
            ++fail_count;
            std::cout << "FAILED positive + fpos\n";
        }
        fp14 = fp13 + static_cast<streamoff>(-28);
        if (static_cast<streamoff>(fp14) != -2)
        {
            ++fail_count;
            std::cout << "FAILED fpos + negative\n";
        }
        fp14 = static_cast<streamoff>(-28) + fp13;
        if (static_cast<streamoff>(fp14) != -2)
        {
            ++fail_count;
            std::cout << "FAILED zero + negative\n";
        }
        // Subtraction
        fpos<klib::mbstate_t> fp15 {26};
        fpos<klib::mbstate_t> fp16 {0};
        fp16 = fp15 - static_cast<streamoff>(0);
        if (static_cast<streamoff>(fp16) != 26)
        {
            ++fail_count;
            std::cout << "FAILED fpos - zero\n";
        }
        fp16 = static_cast<streamoff>(0) - fp16;
        if (static_cast<streamoff>(fp16) != 26)
        {
            ++fail_count;
            std::cout << "FAILED zero - fpos\n";
        }
        fp16 = fp15 - static_cast<streamoff>(28);
        if (static_cast<streamoff>(fp16) != -2)
        {
            ++fail_count;
            std::cout << "FAILED fpos - positive\n";
        }
        fp16 = static_cast<streamoff>(28) - fp15;
        if (static_cast<streamoff>(fp16) != -2)
        {
            ++fail_count;
            std::cout << "FAILED positive - fpos\n";
        }
        fp16 = fp15 - static_cast<streamoff>(-12);
        if (static_cast<streamoff>(fp16) != 38)
        {
            ++fail_count;
            std::cout << "FAILED fpos - negative\n";
        }
        fp16 = static_cast<streamoff>(-12) - fp15;
        if (static_cast<streamoff>(fp16) != 38)
        {
            ++fail_count;
            std::cout << "FAILED zero - negative\n";
        }
        // Difference
        fpos<klib::mbstate_t> fp17 {26};
        fpos<klib::mbstate_t> fp18 {26};
        fpos<klib::mbstate_t> fp19 {12};
        if (fp17 - fp18 != 0)
        {
            ++fail_count;
            std::cout << "FAILED fpos - fpos zero\n";
        }
        if (fp17 - fp19 != 14)
        {
            ++fail_count;
            std::cout << "FAILED fpos - fpos positive\n";
        }
        if (fp19 - fp17 != -14)
        {
            ++fail_count;
            std::cout << "FAILED fpos - fpos negative\n";
        }
    }
    // streampos
    {
        // constructor
        streampos strp1 {1};
        klib::mbstate_t mb1 = strp1.state();
        if (static_cast<streamoff>(strp1) != 1 || mb1.fill[0] != 0 ||
            mb1.fill[1] != 0 || mb1.fill[2] != 0 || mb1.fill[3] != 0 ||
            mb1.fill[4] != 0 || mb1.fill[5] != 0)
        {
            ++fail_count;
            std::cout << "FAILED streampos constructor\n";
        }
        // state get
        streampos strp2 {1};
        klib::mbstate_t mb2 = strp2.state();
        if (mb2.fill[0] != 0 || mb2.fill[1] != 0 || mb2.fill[2] != 0 ||
            mb2.fill[3] != 0 || mb2.fill[4] != 0 || mb2.fill[5] != 0)
        {
            ++fail_count;
            std::cout << "FAILED streampos state()\n";
        }
        // state set
        streampos strp3 {1};
        klib::mbstate_t mb3;
        mb3.fill[0] = 1; mb3.fill[1] = 2; mb3.fill[2] = 3; mb3.fill[3] = 4;
        mb3.fill[4] = 5; mb3.fill[5] = 6;
        strp3.state(mb3);
        klib::mbstate_t mb4 = strp3.state();
        if (mb4.fill[0] != 1 || mb4.fill[1] != 2 || mb4.fill[2] != 3 ||
            mb4.fill[3] != 4 || mb4.fill[4] != 5 || mb4.fill[5] != 6)
        {
            ++fail_count;
            std::cout << "FAILED streampos state(mbstate_t)\n";
        }
        // Get position
        streampos strp4 {26};
        if (static_cast<streamoff>(strp4) != 26)
        {
            ++fail_count;
            std::cout << "FAILED streampos streamoff conversion positive\n";
        }
        streampos strp5 {-26};
        if (static_cast<streamoff>(strp5) != -26)
        {
            ++fail_count;
            std::cout << "FAILED streampos streamoff conversion negative\n";
        }
        streampos strp6 {0};
        if (static_cast<streamoff>(strp6) != 0)
        {
            ++fail_count;
            std::cout << "FAILED streampos streamoff conversion zero\n";
        }
        // Equality
        streampos strp7 {26};
        streampos strp8 {26};
        if (!(strp7 == strp8))
        {
            ++fail_count;
            std::cout << "FAILED streampos == equal\n";
        }
        streampos strp9 {12};
        if (strp7 == strp9)
        {
            ++fail_count;
            std::cout << "FAILED streampos == pos not equal\n";
        }
        streampos strp10 {26};
        klib::mbstate_t mb5;
        mb5.fill[0] = 1;
        strp10.state(mb5);
        if (!(strp7 == strp10))
        {
            ++fail_count;
            std::cout << "FAILED streampos == state not equal\n";
        }
        if (strp9 == strp10)
        {
            ++fail_count;
            std::cout << "FAILED streampos == pos and state not equal\n";
        }
        // Non-Equality
        if (strp7 != strp8)
        {
            ++fail_count;
            std::cout << "FAILED streampos != equal\n";
        }
        if (!(strp7 != strp9))
        {
            ++fail_count;
            std::cout << "FAILED streampos != pos not equal\n";
        }
        if (strp7 != strp10)
        {
            ++fail_count;
            std::cout << "FAILED streampos != state not equal\n";
        }
        if (!(strp9 != strp10))
        {
            ++fail_count;
            std::cout << "FAILED streampos != pos and state not equal\n";
        }
        // increase
        streampos strp11 {26};
        strp11 += static_cast<streamoff>(0);
        if (static_cast<streamoff>(strp11) != 26)
        {
            ++fail_count;
            std::cout << "FAILED streampos += zero\n";
        }
        strp11 += static_cast<streamoff>(12);
        if (static_cast<streamoff>(strp11) != 38)
        {
            ++fail_count;
            std::cout << "FAILED streampos += positive\n";
        }
        strp11 += static_cast<streamoff>(-40);
        if (static_cast<streamoff>(strp11) != -2)
        {
            ++fail_count;
            std::cout << "FAILED streampos += negative\n";
        }
        // decrease
        streampos strp12 {26};
        strp12 -= static_cast<streamoff>(0);
        if (static_cast<streamoff>(strp12) != 26)
        {
            ++fail_count;
            std::cout << "FAILED streampos -= zero\n";
        }
        strp12 -= static_cast<streamoff>(28);
        if (static_cast<streamoff>(strp12) != -2)
        {
            ++fail_count;
            std::cout << "FAILED streampos -= positive\n";
        }
        strp12 -= static_cast<streamoff>(-10);
        if (static_cast<streamoff>(strp12) != 8)
        {
            ++fail_count;
            std::cout << "FAILED streampos -= negative\n";
        }
        // Addition
        streampos strp13 {26};
        streampos strp14 {0};
        strp14 = strp13 + static_cast<streamoff>(0);
        if (static_cast<streamoff>(strp14) != 26)
        {
            ++fail_count;
            std::cout << "FAILED streampos + zero\n";
        }
        strp14 = static_cast<streamoff>(0) + strp13;
        if (static_cast<streamoff>(strp14) != 26)
        {
            ++fail_count;
            std::cout << "FAILED zero + strpos\n";
        }
        strp14 = strp13 + static_cast<streamoff>(12);
        if (static_cast<streamoff>(strp14) != 38)
        {
            ++fail_count;
            std::cout << "FAILED streampos + positive\n";
        }
        strp14 = static_cast<streamoff>(12) + strp13;
        if (static_cast<streamoff>(strp14) != 38)
        {
            ++fail_count;
            std::cout << "FAILED positive + streampos\n";
        }
        strp14 = strp13 + static_cast<streamoff>(-28);
        if (static_cast<streamoff>(strp14) != -2)
        {
            ++fail_count;
            std::cout << "FAILED streampos + negative\n";
        }
        strp14 = static_cast<streamoff>(-28) + strp13;
        if (static_cast<streamoff>(strp14) != -2)
        {
            ++fail_count;
            std::cout << "FAILED zero + negative\n";
        }
        // Subtraction
        streampos strp15 {26};
        streampos strp16 {0};
        strp16 = strp15 - static_cast<streamoff>(0);
        if (static_cast<streamoff>(strp16) != 26)
        {
            ++fail_count;
            std::cout << "FAILED streampos - zero\n";
        }
        strp16 = static_cast<streamoff>(0) - strp16;
        if (static_cast<streamoff>(strp16) != 26)
        {
            ++fail_count;
            std::cout << "FAILED zero - streampos\n";
        }
        strp16 = strp15 - static_cast<streamoff>(28);
        if (static_cast<streamoff>(strp16) != -2)
        {
            ++fail_count;
            std::cout << "FAILED streampos - positive\n";
        }
        strp16 = static_cast<streamoff>(28) - strp15;
        if (static_cast<streamoff>(strp16) != -2)
        {
            ++fail_count;
            std::cout << "FAILED positive - streampos\n";
        }
        strp16 = strp15 - static_cast<streamoff>(-12);
        if (static_cast<streamoff>(strp16) != 38)
        {
            ++fail_count;
            std::cout << "FAILED streampos - negative\n";
        }
        strp16 = static_cast<streamoff>(-12) - strp15;
        if (static_cast<streamoff>(strp16) != 38)
        {
            ++fail_count;
            std::cout << "FAILED zero - negative\n";
        }
        // Difference
        streampos strp17 {26};
        streampos strp18 {26};
        streampos strp19 {12};
        if (strp17 - strp18 != 0)
        {
            ++fail_count;
            std::cout << "FAILED streampos - streampos zero\n";
        }
        if (strp17 - strp19 != 14)
        {
            ++fail_count;
            std::cout << "FAILED streampos - streampos positive\n";
        }
        if (strp19 - strp17 != -14)
        {
            ++fail_count;
            std::cout << "FAILED streampos - streampos negative\n";
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests failed\n";
}
