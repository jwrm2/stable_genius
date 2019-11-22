#include "Idt.h"

#include "Gdt.h"
#include "interrupt.h"

/******************************************************************************
 ******************************************************************************/

IdtEntry::IdtEntry(void (*addr)(), GdtRegister cs, IdtType type, bool priv)
    : value{0}
{
    // See http://wiki.osdev.org/IDT for data layout.
    value |= static_cast<uint64_t>(reinterpret_cast<uint32_t>(addr) &
                                   0x0000FFFF);
    value |= static_cast<uint64_t>(cs.val()) << 16;
    value |= static_cast<uint64_t>(type) << 40;
    if(!priv)
        value |= 0x3ll << 45;
    value |= 0x1ll << 47; // Present
    value |= static_cast<uint64_t>(reinterpret_cast<uint32_t>(addr) &
                                   0xFFFF0000) << 32;
}

/******************************************************************************
 ******************************************************************************/

Idt::Idt()
{
    for (size_t i = 0; i < max_entries; ++i)
        entries[i] = 0;
}

/******************************************************************************/

void Idt::set(const IdtEntry& data, size_t n)
{
    if (n >= max_entries)
        return;

    entries[n] = data.val();
}

/******************************************************************************/

IdtEntry Idt::inspect(size_t n)
{
    if (n >= max_entries)
        return IdtEntry{};

    return IdtEntry{entries[n]};
}

/******************************************************************************/

void Idt::load() const
{
    load_idt(reinterpret_cast<uint32_t>(entries.data()),
             static_cast<uint16_t>(max_entries * sizeof(uint64_t)));
}

/******************************************************************************
 ******************************************************************************/

void fill_idt(Idt* idt)
{
    idt->set(IdtEntry{interrupt_handler_0,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            0);
    idt->set(IdtEntry{interrupt_handler_1,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            1);
    idt->set(IdtEntry{interrupt_handler_2,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            2);
    idt->set(IdtEntry{interrupt_handler_3,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            3);
    idt->set(IdtEntry{interrupt_handler_4,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            4);
    idt->set(IdtEntry{interrupt_handler_5,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            5);
    idt->set(IdtEntry{interrupt_handler_6,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            6);
    idt->set(IdtEntry{interrupt_handler_7,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            7);
    idt->set(IdtEntry{interrupt_handler_8,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            8);
    idt->set(IdtEntry{interrupt_handler_9,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            9);
    idt->set(IdtEntry{interrupt_handler_10,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            10);
    idt->set(IdtEntry{interrupt_handler_11,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            11);
    idt->set(IdtEntry{interrupt_handler_12,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            12);
    idt->set(IdtEntry{interrupt_handler_13,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            13);
    idt->set(IdtEntry{interrupt_handler_14,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            14);
    idt->set(IdtEntry{interrupt_handler_15,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            15);
    idt->set(IdtEntry{interrupt_handler_16,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            16);
    idt->set(IdtEntry{interrupt_handler_17,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            17);
    idt->set(IdtEntry{interrupt_handler_18,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            18);
    idt->set(IdtEntry{interrupt_handler_19,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            19);
    idt->set(IdtEntry{interrupt_handler_20,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            20);
    idt->set(IdtEntry{interrupt_handler_21,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            21);
    idt->set(IdtEntry{interrupt_handler_22,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            22);
    idt->set(IdtEntry{interrupt_handler_23,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            23);
    idt->set(IdtEntry{interrupt_handler_24,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            24);
    idt->set(IdtEntry{interrupt_handler_25,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            25);
    idt->set(IdtEntry{interrupt_handler_26,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            26);
    idt->set(IdtEntry{interrupt_handler_27,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            27);
    idt->set(IdtEntry{interrupt_handler_28,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            28);
    idt->set(IdtEntry{interrupt_handler_29,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            29);
    idt->set(IdtEntry{interrupt_handler_30,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            30);
    idt->set(IdtEntry{interrupt_handler_31,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            31);
    idt->set(IdtEntry{interrupt_handler_32,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            32);
    idt->set(IdtEntry{interrupt_handler_33,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            33);
    idt->set(IdtEntry{interrupt_handler_34,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            34);
    idt->set(IdtEntry{interrupt_handler_35,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            35);
    idt->set(IdtEntry{interrupt_handler_36,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            36);
    idt->set(IdtEntry{interrupt_handler_37,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            37);
    idt->set(IdtEntry{interrupt_handler_38,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            38);
    idt->set(IdtEntry{interrupt_handler_39,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            39);
    idt->set(IdtEntry{interrupt_handler_40,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            40);
    idt->set(IdtEntry{interrupt_handler_41,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            41);
    idt->set(IdtEntry{interrupt_handler_42,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            42);
    idt->set(IdtEntry{interrupt_handler_43,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            43);
    idt->set(IdtEntry{interrupt_handler_44,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            44);
    idt->set(IdtEntry{interrupt_handler_45,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            45);
    idt->set(IdtEntry{interrupt_handler_46,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            46);
    idt->set(IdtEntry{interrupt_handler_47,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            47);
    idt->set(IdtEntry{interrupt_handler_48,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            48);
    idt->set(IdtEntry{interrupt_handler_49,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            49);
    idt->set(IdtEntry{interrupt_handler_50,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            50);
    idt->set(IdtEntry{interrupt_handler_51,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            51);
    idt->set(IdtEntry{interrupt_handler_52,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            52);
    idt->set(IdtEntry{interrupt_handler_53,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            53);
    idt->set(IdtEntry{interrupt_handler_54,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            54);
    idt->set(IdtEntry{interrupt_handler_55,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            55);
    idt->set(IdtEntry{interrupt_handler_56,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            56);
    idt->set(IdtEntry{interrupt_handler_57,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            57);
    idt->set(IdtEntry{interrupt_handler_58,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            58);
    idt->set(IdtEntry{interrupt_handler_59,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            59);
    idt->set(IdtEntry{interrupt_handler_60,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            60);
    idt->set(IdtEntry{interrupt_handler_61,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            61);
    idt->set(IdtEntry{interrupt_handler_62,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            62);
    idt->set(IdtEntry{interrupt_handler_63,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            63);
    idt->set(IdtEntry{interrupt_handler_64,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            64);
    idt->set(IdtEntry{interrupt_handler_65,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            65);
    idt->set(IdtEntry{interrupt_handler_66,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            66);
    idt->set(IdtEntry{interrupt_handler_67,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            67);
    idt->set(IdtEntry{interrupt_handler_68,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            68);
    idt->set(IdtEntry{interrupt_handler_69,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            69);
    idt->set(IdtEntry{interrupt_handler_70,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            70);
    idt->set(IdtEntry{interrupt_handler_71,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            71);
    idt->set(IdtEntry{interrupt_handler_72,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            72);
    idt->set(IdtEntry{interrupt_handler_73,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            73);
    idt->set(IdtEntry{interrupt_handler_74,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            74);
    idt->set(IdtEntry{interrupt_handler_75,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            75);
    idt->set(IdtEntry{interrupt_handler_76,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            76);
    idt->set(IdtEntry{interrupt_handler_77,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            77);
    idt->set(IdtEntry{interrupt_handler_78,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            78);
    idt->set(IdtEntry{interrupt_handler_79,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            79);
    idt->set(IdtEntry{interrupt_handler_80,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            80);
    idt->set(IdtEntry{interrupt_handler_81,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            81);
    idt->set(IdtEntry{interrupt_handler_82,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            82);
    idt->set(IdtEntry{interrupt_handler_83,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            83);
    idt->set(IdtEntry{interrupt_handler_84,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            84);
    idt->set(IdtEntry{interrupt_handler_85,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            85);
    idt->set(IdtEntry{interrupt_handler_86,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            86);
    idt->set(IdtEntry{interrupt_handler_87,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            87);
    idt->set(IdtEntry{interrupt_handler_88,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            88);
    idt->set(IdtEntry{interrupt_handler_89,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            89);
    idt->set(IdtEntry{interrupt_handler_90,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            90);
    idt->set(IdtEntry{interrupt_handler_91,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            91);
    idt->set(IdtEntry{interrupt_handler_92,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            92);
    idt->set(IdtEntry{interrupt_handler_93,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            93);
    idt->set(IdtEntry{interrupt_handler_94,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            94);
    idt->set(IdtEntry{interrupt_handler_95,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            95);
    idt->set(IdtEntry{interrupt_handler_96,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            96);
    idt->set(IdtEntry{interrupt_handler_97,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            97);
    idt->set(IdtEntry{interrupt_handler_98,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            98);
    idt->set(IdtEntry{interrupt_handler_99,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            99);
    idt->set(IdtEntry{interrupt_handler_100,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            100);
    idt->set(IdtEntry{interrupt_handler_101,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            101);
    idt->set(IdtEntry{interrupt_handler_102,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            102);
    idt->set(IdtEntry{interrupt_handler_103,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            103);
    idt->set(IdtEntry{interrupt_handler_104,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            104);
    idt->set(IdtEntry{interrupt_handler_105,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            105);
    idt->set(IdtEntry{interrupt_handler_106,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            106);
    idt->set(IdtEntry{interrupt_handler_107,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            107);
    idt->set(IdtEntry{interrupt_handler_108,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            108);
    idt->set(IdtEntry{interrupt_handler_109,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            109);
    idt->set(IdtEntry{interrupt_handler_110,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            110);
    idt->set(IdtEntry{interrupt_handler_111,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            111);
    idt->set(IdtEntry{interrupt_handler_112,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            112);
    idt->set(IdtEntry{interrupt_handler_113,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            113);
    idt->set(IdtEntry{interrupt_handler_114,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            114);
    idt->set(IdtEntry{interrupt_handler_115,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            115);
    idt->set(IdtEntry{interrupt_handler_116,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            116);
    idt->set(IdtEntry{interrupt_handler_117,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            117);
    idt->set(IdtEntry{interrupt_handler_118,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            118);
    idt->set(IdtEntry{interrupt_handler_119,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            119);
    idt->set(IdtEntry{interrupt_handler_120,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            120);
    idt->set(IdtEntry{interrupt_handler_121,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            121);
    idt->set(IdtEntry{interrupt_handler_122,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            122);
    idt->set(IdtEntry{interrupt_handler_123,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            123);
    idt->set(IdtEntry{interrupt_handler_124,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            124);
    idt->set(IdtEntry{interrupt_handler_125,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            125);
    idt->set(IdtEntry{interrupt_handler_126,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            126);
    idt->set(IdtEntry{interrupt_handler_127,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            127);
    // System call interrupt. Must be accessible from user mode.
    idt->set(IdtEntry{interrupt_handler_128,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, false},
            128);
    idt->set(IdtEntry{interrupt_handler_129,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            129);
    idt->set(IdtEntry{interrupt_handler_130,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            130);
    idt->set(IdtEntry{interrupt_handler_131,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            131);
    idt->set(IdtEntry{interrupt_handler_132,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            132);
    idt->set(IdtEntry{interrupt_handler_133,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            133);
    idt->set(IdtEntry{interrupt_handler_134,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            134);
    idt->set(IdtEntry{interrupt_handler_135,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            135);
    idt->set(IdtEntry{interrupt_handler_136,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            136);
    idt->set(IdtEntry{interrupt_handler_137,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            137);
    idt->set(IdtEntry{interrupt_handler_138,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            138);
    idt->set(IdtEntry{interrupt_handler_139,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            139);
    idt->set(IdtEntry{interrupt_handler_140,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            140);
    idt->set(IdtEntry{interrupt_handler_141,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            141);
    idt->set(IdtEntry{interrupt_handler_142,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            142);
    idt->set(IdtEntry{interrupt_handler_143,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            143);
    idt->set(IdtEntry{interrupt_handler_144,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            144);
    idt->set(IdtEntry{interrupt_handler_145,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            145);
    idt->set(IdtEntry{interrupt_handler_146,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            146);
    idt->set(IdtEntry{interrupt_handler_147,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            147);
    idt->set(IdtEntry{interrupt_handler_148,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            148);
    idt->set(IdtEntry{interrupt_handler_149,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            149);
    idt->set(IdtEntry{interrupt_handler_150,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            150);
    idt->set(IdtEntry{interrupt_handler_151,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            151);
    idt->set(IdtEntry{interrupt_handler_152,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            152);
    idt->set(IdtEntry{interrupt_handler_153,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            153);
    idt->set(IdtEntry{interrupt_handler_154,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            154);
    idt->set(IdtEntry{interrupt_handler_155,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            155);
    idt->set(IdtEntry{interrupt_handler_156,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            156);
    idt->set(IdtEntry{interrupt_handler_157,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            157);
    idt->set(IdtEntry{interrupt_handler_158,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            158);
    idt->set(IdtEntry{interrupt_handler_159,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            159);
    idt->set(IdtEntry{interrupt_handler_160,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            160);
    idt->set(IdtEntry{interrupt_handler_161,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            161);
    idt->set(IdtEntry{interrupt_handler_162,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            162);
    idt->set(IdtEntry{interrupt_handler_163,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            163);
    idt->set(IdtEntry{interrupt_handler_164,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            164);
    idt->set(IdtEntry{interrupt_handler_165,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            165);
    idt->set(IdtEntry{interrupt_handler_166,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            166);
    idt->set(IdtEntry{interrupt_handler_167,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            167);
    idt->set(IdtEntry{interrupt_handler_168,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            168);
    idt->set(IdtEntry{interrupt_handler_169,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            169);
    idt->set(IdtEntry{interrupt_handler_170,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            170);
    idt->set(IdtEntry{interrupt_handler_171,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            171);
    idt->set(IdtEntry{interrupt_handler_172,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            172);
    idt->set(IdtEntry{interrupt_handler_173,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            173);
    idt->set(IdtEntry{interrupt_handler_174,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            174);
    idt->set(IdtEntry{interrupt_handler_175,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            175);
    idt->set(IdtEntry{interrupt_handler_176,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            176);
    idt->set(IdtEntry{interrupt_handler_177,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            177);
    idt->set(IdtEntry{interrupt_handler_178,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            178);
    idt->set(IdtEntry{interrupt_handler_179,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            179);
    idt->set(IdtEntry{interrupt_handler_180,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            180);
    idt->set(IdtEntry{interrupt_handler_181,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            181);
    idt->set(IdtEntry{interrupt_handler_182,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            182);
    idt->set(IdtEntry{interrupt_handler_183,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            183);
    idt->set(IdtEntry{interrupt_handler_184,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            184);
    idt->set(IdtEntry{interrupt_handler_185,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            185);
    idt->set(IdtEntry{interrupt_handler_186,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            186);
    idt->set(IdtEntry{interrupt_handler_187,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            187);
    idt->set(IdtEntry{interrupt_handler_188,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            188);
    idt->set(IdtEntry{interrupt_handler_189,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            189);
    idt->set(IdtEntry{interrupt_handler_190,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            190);
    idt->set(IdtEntry{interrupt_handler_191,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            191);
    idt->set(IdtEntry{interrupt_handler_192,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            192);
    idt->set(IdtEntry{interrupt_handler_193,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            193);
    idt->set(IdtEntry{interrupt_handler_194,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            194);
    idt->set(IdtEntry{interrupt_handler_195,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            195);
    idt->set(IdtEntry{interrupt_handler_196,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            196);
    idt->set(IdtEntry{interrupt_handler_197,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            197);
    idt->set(IdtEntry{interrupt_handler_198,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            198);
    idt->set(IdtEntry{interrupt_handler_199,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            199);
    idt->set(IdtEntry{interrupt_handler_200,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            200);
    idt->set(IdtEntry{interrupt_handler_201,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            201);
    idt->set(IdtEntry{interrupt_handler_202,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            202);
    idt->set(IdtEntry{interrupt_handler_203,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            203);
    idt->set(IdtEntry{interrupt_handler_204,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            204);
    idt->set(IdtEntry{interrupt_handler_205,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            205);
    idt->set(IdtEntry{interrupt_handler_206,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            206);
    idt->set(IdtEntry{interrupt_handler_207,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            207);
    idt->set(IdtEntry{interrupt_handler_208,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            208);
    idt->set(IdtEntry{interrupt_handler_209,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            209);
    idt->set(IdtEntry{interrupt_handler_210,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            210);
    idt->set(IdtEntry{interrupt_handler_211,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            211);
    idt->set(IdtEntry{interrupt_handler_212,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            212);
    idt->set(IdtEntry{interrupt_handler_213,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            213);
    idt->set(IdtEntry{interrupt_handler_214,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            214);
    idt->set(IdtEntry{interrupt_handler_215,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            215);
    idt->set(IdtEntry{interrupt_handler_216,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            216);
    idt->set(IdtEntry{interrupt_handler_217,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            217);
    idt->set(IdtEntry{interrupt_handler_218,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            218);
    idt->set(IdtEntry{interrupt_handler_219,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            219);
    idt->set(IdtEntry{interrupt_handler_220,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            220);
    idt->set(IdtEntry{interrupt_handler_221,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            221);
    idt->set(IdtEntry{interrupt_handler_222,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            222);
    idt->set(IdtEntry{interrupt_handler_223,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            223);
    idt->set(IdtEntry{interrupt_handler_224,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            224);
    idt->set(IdtEntry{interrupt_handler_225,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            225);
    idt->set(IdtEntry{interrupt_handler_226,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            226);
    idt->set(IdtEntry{interrupt_handler_227,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            227);
    idt->set(IdtEntry{interrupt_handler_228,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            228);
    idt->set(IdtEntry{interrupt_handler_229,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            229);
    idt->set(IdtEntry{interrupt_handler_230,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            230);
    idt->set(IdtEntry{interrupt_handler_231,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            231);
    idt->set(IdtEntry{interrupt_handler_232,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            232);
    idt->set(IdtEntry{interrupt_handler_233,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            233);
    idt->set(IdtEntry{interrupt_handler_234,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            234);
    idt->set(IdtEntry{interrupt_handler_235,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            235);
    idt->set(IdtEntry{interrupt_handler_236,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            236);
    idt->set(IdtEntry{interrupt_handler_237,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            237);
    idt->set(IdtEntry{interrupt_handler_238,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            238);
    idt->set(IdtEntry{interrupt_handler_239,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            239);
    idt->set(IdtEntry{interrupt_handler_240,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            240);
    idt->set(IdtEntry{interrupt_handler_241,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            241);
    idt->set(IdtEntry{interrupt_handler_242,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            242);
    idt->set(IdtEntry{interrupt_handler_243,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            243);
    idt->set(IdtEntry{interrupt_handler_244,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            244);
    idt->set(IdtEntry{interrupt_handler_245,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            245);
    idt->set(IdtEntry{interrupt_handler_246,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            246);
    idt->set(IdtEntry{interrupt_handler_247,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            247);
    idt->set(IdtEntry{interrupt_handler_248,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            248);
    idt->set(IdtEntry{interrupt_handler_249,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            249);
    idt->set(IdtEntry{interrupt_handler_250,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            250);
    idt->set(IdtEntry{interrupt_handler_251,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            251);
    idt->set(IdtEntry{interrupt_handler_252,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            252);
    idt->set(IdtEntry{interrupt_handler_253,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            253);
    idt->set(IdtEntry{interrupt_handler_254,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            254);
    idt->set(IdtEntry{interrupt_handler_255,
                     GdtRegister{1, true, false},
                     IdtType::trap_32, true},
            255);
}

/******************************************************************************
 ******************************************************************************/
