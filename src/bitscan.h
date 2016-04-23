//
//  bitscan.h
//  Spartan
//
//
//
//

#ifndef bitscan_h
#define bitscan_h

#include "board.h"


extern const int ls1b_64_table[64];

#define FOLD(x) ((x) ^ ((x) - 1))
#define LS1B(bits) ls1b_64_table[(((unsigned int)(FOLD(bits) ^ (FOLD(bits) >> 32))) * 0x78291ACF) >> 26]

int count_bits(BB bits);
void get_set_bits(BB num, int *bits);

#endif /* bitscan_h */
