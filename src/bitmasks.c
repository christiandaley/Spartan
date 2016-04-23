//
//  bitmasks.c
//  Spartan
//
//
//
//

#include "bitmasks.h"

const BB Rank_Masks[8] = {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8};
const BB File_Masks[8] = {A_FILE, B_FILE, C_FILE, D_FILE, E_FILE, F_FILE, G_FILE, H_FILE};

const BB White_Pawn_Attacks_To[64] = {
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000200ULL, 0x0000000000000500ULL, 0x0000000000000a00ULL, 0x0000000000001400ULL,
    0x0000000000002800ULL, 0x0000000000005000ULL, 0x000000000000a000ULL, 0x0000000000004000ULL,
    0x0000000000020000ULL, 0x0000000000050000ULL, 0x00000000000a0000ULL, 0x0000000000140000ULL,
    0x0000000000280000ULL, 0x0000000000500000ULL, 0x0000000000a00000ULL, 0x0000000000400000ULL,
    0x0000000002000000ULL, 0x0000000005000000ULL, 0x000000000a000000ULL, 0x0000000014000000ULL,
    0x0000000028000000ULL, 0x0000000050000000ULL, 0x00000000a0000000ULL, 0x0000000040000000ULL,
    0x0000000200000000ULL, 0x0000000500000000ULL, 0x0000000a00000000ULL, 0x0000001400000000ULL,
    0x0000002800000000ULL, 0x0000005000000000ULL, 0x000000a000000000ULL, 0x0000004000000000ULL,
    0x0000020000000000ULL, 0x0000050000000000ULL, 0x00000a0000000000ULL, 0x0000140000000000ULL,
    0x0000280000000000ULL, 0x0000500000000000ULL, 0x0000a00000000000ULL, 0x0000400000000000ULL,
    0x0002000000000000ULL, 0x0005000000000000ULL, 0x000a000000000000ULL, 0x0014000000000000ULL,
    0x0028000000000000ULL, 0x0050000000000000ULL, 0x00a0000000000000ULL, 0x0040000000000000ULL
};

const BB White_Pawn_Attacks_From[64] = {
    0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,
    0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,
    0x0000000000020000,0x0000000000050000,0x00000000000a0000,0x0000000000140000,
    0x0000000000280000,0x0000000000500000,0x0000000000a00000,0x0000000000400000,
    0x0000000002000000,0x0000000005000000,0x000000000a000000,0x0000000014000000,
    0x0000000028000000,0x0000000050000000,0x00000000a0000000,0x0000000040000000,
    0x0000000200000000,0x0000000500000000,0x0000000a00000000,0x0000001400000000,
    0x0000002800000000,0x0000005000000000,0x000000a000000000,0x0000004000000000,
    0x0000020000000000,0x0000050000000000,0x00000a0000000000,0x0000140000000000,
    0x0000280000000000,0x0000500000000000,0x0000a00000000000,0x0000400000000000,
    0x0002000000000000,0x0005000000000000,0x000a000000000000,0x0014000000000000,
    0x0028000000000000,0x0050000000000000,0x00a0000000000000,0x0040000000000000,
    0x0200000000000000,0x0500000000000000,0x0a00000000000000,0x1400000000000000,
    0x2800000000000000,0x5000000000000000,0xa000000000000000,0x4000000000000000,
    0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,
    0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,
};

const BB Black_Pawn_Attacks_To[64] = {
    0x0000000000000200ULL, 0x0000000000000500ULL, 0x0000000000000a00ULL, 0x0000000000001400ULL,
    0x0000000000002800ULL, 0x0000000000005000ULL, 0x000000000000a000ULL, 0x0000000000004000ULL,
    0x0000000000020000ULL, 0x0000000000050000ULL, 0x00000000000a0000ULL, 0x0000000000140000ULL,
    0x0000000000280000ULL, 0x0000000000500000ULL, 0x0000000000a00000ULL, 0x0000000000400000ULL,
    0x0000000002000000ULL, 0x0000000005000000ULL, 0x000000000a000000ULL, 0x0000000014000000ULL,
    0x0000000028000000ULL, 0x0000000050000000ULL, 0x00000000a0000000ULL, 0x0000000040000000ULL,
    0x0000000200000000ULL, 0x0000000500000000ULL, 0x0000000a00000000ULL, 0x0000001400000000ULL,
    0x0000002800000000ULL, 0x0000005000000000ULL, 0x000000a000000000ULL, 0x0000004000000000ULL,
    0x0000020000000000ULL, 0x0000050000000000ULL, 0x00000a0000000000ULL, 0x0000140000000000ULL,
    0x0000280000000000ULL, 0x0000500000000000ULL, 0x0000a00000000000ULL, 0x0000400000000000ULL,
    0x0002000000000000ULL, 0x0005000000000000ULL, 0x000a000000000000ULL, 0x0014000000000000ULL,
    0x0028000000000000ULL, 0x0050000000000000ULL, 0x00a0000000000000ULL, 0x0040000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL
};

const BB Black_Pawn_Attacks_From[64] = {
    0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,
    0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,
    0x0000000000000002,0x0000000000000005,0x000000000000000a,0x0000000000000014,
    0x0000000000000028,0x0000000000000050,0x00000000000000a0,0x0000000000000040,
    0x0000000000000200,0x0000000000000500,0x0000000000000a00,0x0000000000001400,
    0x0000000000002800,0x0000000000005000,0x000000000000a000,0x0000000000004000,
    0x0000000000020000,0x0000000000050000,0x00000000000a0000,0x0000000000140000,
    0x0000000000280000,0x0000000000500000,0x0000000000a00000,0x0000000000400000,
    0x0000000002000000,0x0000000005000000,0x000000000a000000,0x0000000014000000,
    0x0000000028000000,0x0000000050000000,0x00000000a0000000,0x0000000040000000,
    0x0000000200000000,0x0000000500000000,0x0000000a00000000,0x0000001400000000,
    0x0000002800000000,0x0000005000000000,0x000000a000000000,0x0000004000000000,
    0x0000020000000000,0x0000050000000000,0x00000a0000000000,0x0000140000000000,
    0x0000280000000000,0x0000500000000000,0x0000a00000000000,0x0000400000000000,
    0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,
    0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000
};

const BB White_Pawn_Passed[64] = {
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0003030303030000ULL,0x0007070707070000ULL,0x000e0e0e0e0e0000ULL,0x001c1c1c1c1c0000ULL,
    0x0038383838380000ULL,0x0070707070700000ULL,0x00e0e0e0e0e00000ULL,0x00c0c0c0c0c00000ULL,
    0x0003030303000000ULL,0x0007070707000000ULL,0x000e0e0e0e000000ULL,0x001c1c1c1c000000ULL,
    0x0038383838000000ULL,0x0070707070000000ULL,0x00e0e0e0e0000000ULL,0x00c0c0c0c0000000ULL,
    0x0003030300000000ULL,0x0007070700000000ULL,0x000e0e0e00000000ULL,0x001c1c1c00000000ULL,
    0x0038383800000000ULL,0x0070707000000000ULL,0x00e0e0e000000000ULL,0x00c0c0c000000000ULL,
    0x0003030000000000ULL,0x0007070000000000ULL,0x000e0e0000000000ULL,0x001c1c0000000000ULL,
    0x0038380000000000ULL,0x0070700000000000ULL,0x00e0e00000000000ULL,0x00c0c00000000000ULL,
    0x0003000000000000ULL,0x0007000000000000ULL,0x000e000000000000ULL,0x001c000000000000ULL,
    0x0038000000000000ULL,0x0070000000000000ULL,0x00e0000000000000ULL,0x00c0000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL
};

const BB Black_Pawn_Passed[64] = {
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000300ULL,0x0000000000000700ULL,0x0000000000000e00ULL,0x0000000000001c00ULL,
    0x0000000000003800ULL,0x0000000000007000ULL,0x000000000000e000ULL,0x000000000000c000ULL,
    0x0000000000030300ULL,0x0000000000070700ULL,0x00000000000e0e00ULL,0x00000000001c1c00ULL,
    0x0000000000383800ULL,0x0000000000707000ULL,0x0000000000e0e000ULL,0x0000000000c0c000ULL,
    0x0000000003030300ULL,0x0000000007070700ULL,0x000000000e0e0e00ULL,0x000000001c1c1c00ULL,
    0x0000000038383800ULL,0x0000000070707000ULL,0x00000000e0e0e000ULL,0x00000000c0c0c000ULL,
    0x0000000303030300ULL,0x0000000707070700ULL,0x0000000e0e0e0e00ULL,0x0000001c1c1c1c00ULL,
    0x0000003838383800ULL,0x0000007070707000ULL,0x000000e0e0e0e000ULL,0x000000c0c0c0c000ULL,
    0x0000030303030300ULL,0x0000070707070700ULL,0x00000e0e0e0e0e00ULL,0x00001c1c1c1c1c00ULL,
    0x0000383838383800ULL,0x0000707070707000ULL,0x0000e0e0e0e0e000ULL,0x0000c0c0c0c0c000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL
};

const BB White_Pawn_Backwards[64] = {
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000200ULL,0x0000000000000500ULL,0x0000000000000a00ULL,0x0000000000001400ULL,
    0x0000000000002800ULL,0x0000000000005000ULL,0x000000000000a000ULL,0x0000000000004000ULL,
    0x0000000000020200ULL,0x0000000000050500ULL,0x00000000000a0a00ULL,0x0000000000141400ULL,
    0x0000000000282800ULL,0x0000000000505000ULL,0x0000000000a0a000ULL,0x0000000000404000ULL,
    0x0000000002020200ULL,0x0000000005050500ULL,0x000000000a0a0a00ULL,0x0000000014141400ULL,
    0x0000000028282800ULL,0x0000000050505000ULL,0x00000000a0a0a000ULL,0x0000000040404000ULL,
    0x0000000202020200ULL,0x0000000505050500ULL,0x0000000a0a0a0a00ULL,0x0000001414141400ULL,
    0x0000002828282800ULL,0x0000005050505000ULL,0x000000a0a0a0a000ULL,0x0000004040404000ULL,
    0x0000020202020200ULL,0x0000050505050500ULL,0x00000a0a0a0a0a00ULL,0x0000141414141400ULL,
    0x0000282828282800ULL,0x0000505050505000ULL,0x0000a0a0a0a0a000ULL,0x0000404040404000ULL,
    0x0002020202020200ULL,0x0005050505050500ULL,0x000a0a0a0a0a0a00ULL,0x0014141414141400ULL,
    0x0028282828282800ULL,0x0050505050505000ULL,0x00a0a0a0a0a0a000ULL,0x0040404040404000ULL,
    0x0202020202020200ULL,0x0505050505050500ULL,0x0a0a0a0a0a0a0a00ULL,0x1414141414141400ULL,
    0x2828282828282800ULL,0x5050505050505000ULL,0xa0a0a0a0a0a0a000ULL,0x4040404040404000ULL
};

const BB Black_Pawn_Backwards[64] = {
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0002020202020200ULL,0x0005050505050500ULL,0x000a0a0a0a0a0a00ULL,0x0014141414141400ULL,
    0x0028282828282800ULL,0x0050505050505000ULL,0x00a0a0a0a0a0a000ULL,0x0040404040404000ULL,
    0x0002020202020000ULL,0x0005050505050000ULL,0x000a0a0a0a0a0000ULL,0x0014141414140000ULL,
    0x0028282828280000ULL,0x0050505050500000ULL,0x00a0a0a0a0a00000ULL,0x0040404040400000ULL,
    0x0002020202000000ULL,0x0005050505000000ULL,0x000a0a0a0a000000ULL,0x0014141414000000ULL,
    0x0028282828000000ULL,0x0050505050000000ULL,0x00a0a0a0a0000000ULL,0x0040404040000000ULL,
    0x0002020200000000ULL,0x0005050500000000ULL,0x000a0a0a00000000ULL,0x0014141400000000ULL,
    0x0028282800000000ULL,0x0050505000000000ULL,0x00a0a0a000000000ULL,0x0040404000000000ULL,
    0x0002020000000000ULL,0x0005050000000000ULL,0x000a0a0000000000ULL,0x0014140000000000ULL,
    0x0028280000000000ULL,0x0050500000000000ULL,0x00a0a00000000000ULL,0x0040400000000000ULL,
    0x0002000000000000ULL,0x0005000000000000ULL,0x000a000000000000ULL,0x0014000000000000ULL,
    0x0028000000000000ULL,0x0050000000000000ULL,0x00a0000000000000ULL,0x0040000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,
    0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL,0x0000000000000000ULL
};

const BB Pawn_Isolated[8] = {
    0x0002020202020200ULL,0x0005050505050500ULL,0x000a0a0a0a0a0a00ULL,0x0014141414141400ULL,
    0x0028282828282800ULL,0x0050505050505000ULL,0x00a0a0a0a0a0a000ULL,0x0040404040404000ULL
};

const BB Knight_Attacks[64] = {
    0x0000000000020400ULL, 0x0000000000050800ULL, 0x00000000000a1100ULL, 0x0000000000142200ULL,
    0x0000000000284400ULL, 0x0000000000508800ULL, 0x0000000000a01000ULL, 0x0000000000402000ULL,
    0x0000000002040004ULL, 0x0000000005080008ULL, 0x000000000a110011ULL, 0x0000000014220022ULL,
    0x0000000028440044ULL, 0x0000000050880088ULL, 0x00000000a0100010ULL, 0x0000000040200020ULL,
    0x0000000204000402ULL, 0x0000000508000805ULL, 0x0000000a1100110aULL, 0x0000001422002214ULL,
    0x0000002844004428ULL, 0x0000005088008850ULL, 0x000000a0100010a0ULL, 0x0000004020002040ULL,
    0x0000020400040200ULL, 0x0000050800080500ULL, 0x00000a1100110a00ULL, 0x0000142200221400ULL,
    0x0000284400442800ULL, 0x0000508800885000ULL, 0x0000a0100010a000ULL, 0x0000402000204000ULL,
    0x0002040004020000ULL, 0x0005080008050000ULL, 0x000a1100110a0000ULL, 0x0014220022140000ULL,
    0x0028440044280000ULL, 0x0050880088500000ULL, 0x00a0100010a00000ULL, 0x0040200020400000ULL,
    0x0204000402000000ULL, 0x0508000805000000ULL, 0x0a1100110a000000ULL, 0x1422002214000000ULL,
    0x2844004428000000ULL, 0x5088008850000000ULL, 0xa0100010a0000000ULL, 0x4020002040000000ULL,
    0x0400040200000000ULL, 0x0800080500000000ULL, 0x1100110a00000000ULL, 0x2200221400000000ULL,
    0x4400442800000000ULL, 0x8800885000000000ULL, 0x100010a000000000ULL, 0x2000204000000000ULL,
    0x0004020000000000ULL, 0x0008050000000000ULL, 0x00110a0000000000ULL, 0x0022140000000000ULL,
    0x0044280000000000ULL, 0x0088500000000000ULL, 0x0010a00000000000ULL, 0x0020400000000000ULL
};

const BB King_Attacks[64] = {
    0x0000000000000302ULL, 0x0000000000000705ULL, 0x0000000000000e0aULL, 0x0000000000001c14ULL,
    0x0000000000003828ULL, 0x0000000000007050ULL, 0x000000000000e0a0ULL, 0x000000000000c040ULL,
    0x0000000000030203ULL, 0x0000000000070507ULL, 0x00000000000e0a0eULL, 0x00000000001c141cULL,
    0x0000000000382838ULL, 0x0000000000705070ULL, 0x0000000000e0a0e0ULL, 0x0000000000c040c0ULL,
    0x0000000003020300ULL, 0x0000000007050700ULL, 0x000000000e0a0e00ULL, 0x000000001c141c00ULL,
    0x0000000038283800ULL, 0x0000000070507000ULL, 0x00000000e0a0e000ULL, 0x00000000c040c000ULL,
    0x0000000302030000ULL, 0x0000000705070000ULL, 0x0000000e0a0e0000ULL, 0x0000001c141c0000ULL,
    0x0000003828380000ULL, 0x0000007050700000ULL, 0x000000e0a0e00000ULL, 0x000000c040c00000ULL,
    0x0000030203000000ULL, 0x0000070507000000ULL, 0x00000e0a0e000000ULL, 0x00001c141c000000ULL,
    0x0000382838000000ULL, 0x0000705070000000ULL, 0x0000e0a0e0000000ULL, 0x0000c040c0000000ULL,
    0x0003020300000000ULL, 0x0007050700000000ULL, 0x000e0a0e00000000ULL, 0x001c141c00000000ULL,
    0x0038283800000000ULL, 0x0070507000000000ULL, 0x00e0a0e000000000ULL, 0x00c040c000000000ULL,
    0x0302030000000000ULL, 0x0705070000000000ULL, 0x0e0a0e0000000000ULL, 0x1c141c0000000000ULL,
    0x3828380000000000ULL, 0x7050700000000000ULL, 0xe0a0e00000000000ULL, 0xc040c00000000000ULL,
    0x0203000000000000ULL, 0x0507000000000000ULL, 0x0a0e000000000000ULL, 0x141c000000000000ULL,
    0x2838000000000000ULL, 0x5070000000000000ULL, 0xa0e0000000000000ULL, 0x40c0000000000000ULL
};
