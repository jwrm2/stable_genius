#include <iostream>

#include "../include/cctype"

using namespace klib;

int main()
{
    size_t fail_count = 0;

    // Tests for each ASCII character
    int c1;
    c1 = 0; // NUL
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 1; // SOH
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 2; // STX
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 3; // ETX
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 4; // EOT
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 5; // ENQ
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 6; // ACK
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 7; // BEL
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 8; // BS
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 9; // HT
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        !klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 10; // LF
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        !klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 11; // VT
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        !klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 12; // FF
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        !klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 13; // CR
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        !klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 14; // SO
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 15; // SI
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 16; // DLE
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 17; // DC1
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 18; // DC2
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 19; // DC3
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 20; // DC4
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 21; // NAK
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 22; // SYN
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 23; // ETB
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 24; // CAN
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 25; // EM
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 26; // SUB
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 27; // ESC
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 28; // FS
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 29; // GS
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 30; // RS
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 31; // US
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 32; // space
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        !klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 33; // !
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 34; // "
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 35; // #
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 36; // $
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 37; // %
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 38; // &
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 39; // '
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 40; // (
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 41; // )
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 42; // *
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 43; // +
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 44; // ,
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 45; // -
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 46; // .
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 47; // /
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 48; // 0
    if (!klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || !klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 49; // 1
    if (!klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || !klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 50; // 2
    if (!klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || !klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 51; // 3
    if (!klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || !klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 52; // 4
    if (!klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || !klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 53; // 5
    if (!klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || !klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 54; // 6
    if (!klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || !klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 55; // 7
    if (!klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || !klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 56; // 8
    if (!klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || !klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 57; // 9
    if (!klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || !klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 58; // :
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 59; // ;
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 60; // <
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 61; // =
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 62; // >
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 63; // ?
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 64; // @
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 65; // A
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 66; // B
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 67; // C
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 68; // D
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 69; // E
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 70; // F
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 71; // G
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 72; // H
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 73; // I
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 74; // J
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 75; // K
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 76; // L
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 77; // M
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 78; // N
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 79; // O
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 80; // P
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 81; // Q
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 82; // R
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 83; // S
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 84; // T
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 85; // U
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 86; // V
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 87; // W
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 88; // X
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 89; // Y
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 90; // Z
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || !klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 91; // [
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 92; // backslash
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 93; // ]
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 94; // ^
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 95; // _
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 96; // `
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 97; // a
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 98; // b
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 99; // c
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 100; // d
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 101; // e
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 102; // f
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || !klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 103; // g
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 104; // h
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 105; // i
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 106; // j
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 107; // k
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 108; // l
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 109; // m
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 110; // n
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 111; // o
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 112; // p
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 113; // q
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 114; // r
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 115; // s
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 116; // t
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 117; // u
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 118; // v
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 119; // w
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 120; // x
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 121; // y
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 122; // z
    if (!klib::isalnum(c1) || !klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || !klib::islower(c1) || !klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 123; // {
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 124; // |
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 125; // }
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 126; // ~
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        !klib::isgraph(c1) || klib::islower(c1) || !klib::isprint(c1) || !klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 127; // DEL
    if (klib::isalnum(c1) || klib::isalpha(c1) || !klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    // Out of range
    c1 = EOF;
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 128;
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    c1 = 4096;
    if (klib::isalnum(c1) || klib::isalpha(c1) || klib::iscntrl(c1) || klib::isdigit(c1) ||
        klib::isgraph(c1) || klib::islower(c1) || klib::isprint(c1) || klib::ispunct(c1) ||
        klib::isspace(c1) || klib::isupper(c1) || klib::isxdigit(c1))
    {
        ++fail_count;
        std::cout << "FAILED ctype for " << c1 << '\n';
    }
    // toupper
    char uppercase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int c2 = -1; c2 < 'a'; ++c2)
    {
        if (klib::toupper(c2) != c2)
        {
            ++fail_count;
            std::cout << "FAILED toupper for " << c2 << '\n';
        }
    }
    for (int c2 = 'z' + 1; c2 < 130; ++c2)
    {
        if (klib::toupper(c2) != c2)
        {
            ++fail_count;
            std::cout << "FAILED toupper for " << c2 << '\n';
        }
    }
    for (int c2 = 'a'; c2 <= 'z'; ++c2)
    {
        if (klib::toupper(c2) != uppercase[c2 - 'a'])
        {
            ++fail_count;
            std::cout << "FAILED toupper for " << c2 << '\n';
        }
    }
    // tolower
    char lowercase[] = "abcdefghijklmnopqrstuvwxyz";
    for (int c2 = -1; c2 < 'A'; ++c2)
    {
        if (klib::tolower(c2) != c2)
        {
            ++fail_count;
            std::cout << "FAILED tolower for " << c2 << '\n';
        }
    }
    for (int c2 = 'Z' + 1; c2 < 130; ++c2)
    {
        if (klib::tolower(c2) != c2)
        {
            ++fail_count;
            std::cout << "FAILED tolower for " << c2 << '\n';
        }
    }
    for (int c2 = 'A'; c2 <= 'Z'; ++c2)
    {
        if (klib::tolower(c2) != lowercase[c2 - 'A'])
        {
            ++fail_count;
            std::cout << "FAILED tolower for " << c2 << '\n';
        }
    }

    // End
    if (fail_count == 0)
        std::cout << "All tests passed\n";
    else
        std::cout << fail_count << " tests FAILED\n";
}
