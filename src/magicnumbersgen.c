//
//  magicnumbersgen.c
//  Spartan
//
//
//
//

#include <stdlib.h>
#include <stdio.h>
#include "magicnumbersgen.h"
#include "bitmasks.h"
#include "bitscan.h"
#include "debug.h"
#include "log.h"
#include "magicnumbers.h"
#include "magicmoves.h"

uint64_t rand64() {
    
    uint64_t rand1, rand2;
    
    rand1 = (uint64_t)rand();
    rand2 = (uint64_t)rand();
    
    return rand1 | (rand2 << 32);
}

MagicNumber_t random_magic() {
    
    
    return rand64() & rand64() & rand64(); // sparse bitboards are more likely to be magic numbers
}


void generate_magic_numbers_bishop() {
    init_bishop_masks();
    int var_count, index, bit_count, fail, i, j;
    BB *used;
    
    MagicNumber_t magic = 0;
    
    BB occupancy, attacks, mask;
    
    engine_output("generating magic numbers for bishops moves...\n");
    
    for (i = 0; i < 64; i++) {
        mask = bishop_masks[i];
        bit_count = count_bits(mask);
        var_count = 1 << bit_count;
        
        do {
            magic = random_magic();
            fail = 0;
            used = calloc(var_count, sizeof(BB));
            for (j = 0; j < var_count; j++)
                used[j] = 0xffffffffffffffff;
            
            for (j = 0; j < var_count; j++) {
                occupancy = bishop_occupancy_variations[i][j];
                attacks = bishop_attacks[i][j];
                index = (int)((occupancy * magic) >> magic_bitshifts_bishop[i]);
                
                if (used[index] != 0xffffffffffffffff && used[index] != attacks) {
                    fail = 1;
                    break;
                }
                
                used[index] = attacks;
            }
            
            free(used);
        } while (fail);
        
        engine_output("0x%016llxULL, ", magic);
    }
    
}

void generate_magic_numbers_rook() {
    init_rook_masks();
    int var_count, index, bit_count, fail, i, j;
    BB *used;
    
    MagicNumber_t magic = 0;
    
    BB occupancy, attacks, mask;
    
    engine_output("generating magic numbers for rook moves...\n");
    
    for (i = 0; i < 64; i++) {
        mask = rook_masks[i];
        bit_count = count_bits(mask);
        var_count = 1 << bit_count;
        
        do {
            magic = random_magic();
            fail = 0;
            used = calloc(var_count, sizeof(BB));
            for (j = 0; j < var_count; j++)
                used[j] = 0xffffffffffffffff;
            
            for (j = 0; j < var_count; j++) {
                occupancy = rook_occupancy_variations[i][j];
                attacks = rook_attacks[i][j];
                index = (int)((occupancy * magic) >> magic_bitshifts_rook[i]);
                
                if (used[index] != 0xffffffffffffffff && used[index] != attacks) {
                    fail = 1;
                    break;
                }
                
                used[index] = attacks;
            }
            
            free(used);
        } while (fail);
        
        engine_output("0x%016llxULL, ", magic);
    }
    
}