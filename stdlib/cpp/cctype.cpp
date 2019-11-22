#include <stdint.h>

#include "../include/cctype"
#include "../include/cstdio"

namespace NMSP {

// Masks for different properties
const uint16_t alnum_mk = 0x0001;
const uint16_t alpha_mk = 0x0002;
const uint16_t cntrl_mk = 0x0004;
const uint16_t digit_mk = 0x0008;
const uint16_t graph_mk = 0x0010;
const uint16_t lower_mk = 0x0020;
const uint16_t print_mk = 0x0040;
const uint16_t punct_mk = 0x0080;
const uint16_t space_mk = 0x0100;
const uint16_t upper_mk = 0x0200;
const uint16_t xdigit_mk = 0x0400;

// ASCII table of properties
const uint16_t properties[128] = {
    cntrl_mk, // 0 NUL
    cntrl_mk, // 1 SOH
    cntrl_mk, // 2 STX
    cntrl_mk, // 3 ETX
    cntrl_mk, // 4 EOT
    cntrl_mk, // 5 ENQ
    cntrl_mk, // 6 ACK
    cntrl_mk, // 7 BEL
    cntrl_mk, // 8 BS
    cntrl_mk | space_mk, // 9 HT
    cntrl_mk | space_mk, // 10 LF
    cntrl_mk | space_mk, // 11 VT
    cntrl_mk | space_mk, // 12 FF
    cntrl_mk | space_mk, // 13 CR
    cntrl_mk, // 14 SO
    cntrl_mk, // 15 SI
    cntrl_mk, // 16 DLE
    cntrl_mk, // 17 DC1
    cntrl_mk, // 18 DC2
    cntrl_mk, // 19 DC3
    cntrl_mk, // 20 DC4
    cntrl_mk, // 21 NAK
    cntrl_mk, // 22 SYN
    cntrl_mk, // 23 ETB
    cntrl_mk, // 24 CAN
    cntrl_mk, // 25 EM
    cntrl_mk, // 26 SUB
    cntrl_mk, // 27 ESC
    cntrl_mk, // 28 FS
    cntrl_mk, // 29 GS
    cntrl_mk, // 30 RS
    cntrl_mk, // 31 US
    print_mk | space_mk, // 32 space
    graph_mk | print_mk | punct_mk, // 33 !
    graph_mk | print_mk | punct_mk, // 34 "
    graph_mk | print_mk | punct_mk, // 35 #
    graph_mk | print_mk | punct_mk, // 36 $
    graph_mk | print_mk | punct_mk, // 37 %
    graph_mk | print_mk | punct_mk, // 38 &
    graph_mk | print_mk | punct_mk, // 39 '
    graph_mk | print_mk | punct_mk, // 40 (
    graph_mk | print_mk | punct_mk, // 41 )
    graph_mk | print_mk | punct_mk, // 42 *
    graph_mk | print_mk | punct_mk, // 43 +
    graph_mk | print_mk | punct_mk, // 44 ,
    graph_mk | print_mk | punct_mk, // 45 -
    graph_mk | print_mk | punct_mk, // 46 .
    graph_mk | print_mk | punct_mk, // 47 /
    alnum_mk | digit_mk | graph_mk | print_mk | xdigit_mk, // 48 0
    alnum_mk | digit_mk | graph_mk | print_mk | xdigit_mk, // 49 1
    alnum_mk | digit_mk | graph_mk | print_mk | xdigit_mk, // 50 2
    alnum_mk | digit_mk | graph_mk | print_mk | xdigit_mk, // 51 3
    alnum_mk | digit_mk | graph_mk | print_mk | xdigit_mk, // 52 4
    alnum_mk | digit_mk | graph_mk | print_mk | xdigit_mk, // 53 5
    alnum_mk | digit_mk | graph_mk | print_mk | xdigit_mk, // 54 6
    alnum_mk | digit_mk | graph_mk | print_mk | xdigit_mk, // 55 7
    alnum_mk | digit_mk | graph_mk | print_mk | xdigit_mk, // 56 8
    alnum_mk | digit_mk | graph_mk | print_mk | xdigit_mk, // 57 9
    graph_mk | print_mk | punct_mk, // 58 :
    graph_mk | print_mk | punct_mk, // 59 ;
    graph_mk | print_mk | punct_mk, // 60 <
    graph_mk | print_mk | punct_mk, // 61 =
    graph_mk | print_mk | punct_mk, // 62 >
    graph_mk | print_mk | punct_mk, // 63 ?
    graph_mk | print_mk | punct_mk, // 64 @
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk | xdigit_mk, // 65 A
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk | xdigit_mk, // 66 B
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk | xdigit_mk, // 67 C
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk | xdigit_mk, // 68 D
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk | xdigit_mk, // 69 E
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk | xdigit_mk, // 70 F
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 71 G
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 72 H
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 73 I
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 74 J
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 75 K
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 76 L
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 77 M
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 78 N
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 79 O
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 80 P
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 81 Q
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 82 R
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 83 S
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 84 T
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 85 U
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 86 V
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 87 W
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 88 X
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 89 Y
    alnum_mk | alpha_mk | graph_mk | print_mk | upper_mk, // 90 Z
    graph_mk | print_mk | punct_mk, // 91 [
    graph_mk | print_mk | punct_mk, // 92 backslash
    graph_mk | print_mk | punct_mk, // 93 ]
    graph_mk | print_mk | punct_mk, // 94 ^
    graph_mk | print_mk | punct_mk, // 95 _
    graph_mk | print_mk | punct_mk, // 96 `
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk | xdigit_mk, // 97 a
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk | xdigit_mk, // 98 b
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk | xdigit_mk, // 99 c
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk | xdigit_mk, // 100 d
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk | xdigit_mk, // 101 e
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk | xdigit_mk, // 102 f
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 103 g
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 104 h
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 105 i
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 106 j
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 107 k
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 108 l
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 109 m
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 110 n
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 111 o
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 112 p
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 113 q
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 114 r
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 115 s
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 116 t
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 117 u
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 118 v
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 119 w
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 120 x
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 121 y
    alnum_mk | alpha_mk | graph_mk | lower_mk | print_mk, // 122 z
    graph_mk | print_mk | punct_mk, // 123 {
    graph_mk | print_mk | punct_mk, // 124 |
    graph_mk | print_mk | punct_mk, // 125 }
    graph_mk | print_mk | punct_mk, // 126 ~
    cntrl_mk}; // 127 DEL

/******************************************************************************
 ******************************************************************************/

int istype (int c, const uint16_t test)
{
    if (c == EOF)
        return 0;
    if ((c < 0 || c > 127))
        return 0;

    return static_cast<int>(properties[c] & test);
}

/******************************************************************************
 ******************************************************************************/

int isalnum(int i)
{
    return istype(i, alnum_mk);
}

/******************************************************************************/

int isalpha(int i)
{
    return istype(i, alpha_mk);
}

/******************************************************************************/

int iscntrl(int i)
{
    return istype(i, cntrl_mk);
}

/******************************************************************************/

int isdigit(int i)
{
    return istype(i, digit_mk);
}

/******************************************************************************/

int isgraph(int i)
{
    return istype(i, graph_mk);
}

/******************************************************************************/

int islower(int i)
{
    return istype(i, lower_mk);
}

/******************************************************************************/

int isprint(int i)
{
    return istype(i, print_mk);
}

/******************************************************************************/

int ispunct(int i)
{
    return istype(i, punct_mk);
}

/******************************************************************************/

int isspace(int i)
{
    return istype(i, space_mk);
}

/******************************************************************************/

int isupper(int i)
{
    return istype(i, upper_mk);
}

/******************************************************************************/

int isxdigit(int i)
{
    return istype(i, xdigit_mk);
}

/******************************************************************************
 ******************************************************************************/

int tolower(int ch)
{
    if ('A' <= ch && ch <= 'Z')
        return (ch - 'A' + 'a');
    else
        return ch;
}

/******************************************************************************/

int toupper(int ch)
{
    if ('a' <= ch && ch <= 'z')
        return (ch - 'a' + 'A');
    else
        return ch;
}

/******************************************************************************
 ******************************************************************************/

} // NMSP namespace
