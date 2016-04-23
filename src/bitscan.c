//
//  bitscan.c
//  Spartan
//
//
//
//

#include "bitscan.h"

const int ls1b_64_table[64] =
{
    63, 30,  3, 32, 59, 14, 11, 33,
    60, 24, 50,  9, 55, 19, 21, 34,
    61, 29,  2, 53, 51, 23, 41, 18,
    56, 28,  1, 43, 46, 27,  0, 35,
    62, 31, 58,  4,  5, 49, 54,  6,
    15, 52, 12, 40,  7, 42, 45, 16,
    25, 57, 48, 13, 10, 39,  8, 44,
    20, 47, 38, 22, 17, 37, 36, 26
};

int count_bits(BB bits) {
    int n = 0;
    
    while (bits) {
        bits &= bits - 1;
        n++;
    }
    
    return n;
}

void get_set_bits(BB num, int *bits) {
    int n = 0;
    while (num) {
        bits[n++] = LS1B(num);
        num &= num - 1;
    }
    
    bits[n] = -1;
}