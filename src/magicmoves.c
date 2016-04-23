//
//  magicmoves.c
//  Spartan
//
//
//
//

#include <stdio.h>
#include <stdlib.h>
#include "magicmoves.h"
#include "bitscan.h"
#include "bitmasks.h"
#include "magicnumbersgen.h"
#include "magicnumbers.h"
#include "debug.h"

BB magic_moves_rook[64][4096] = {};
BB magic_moves_bishop[64][512] = {};
BB bishop_masks[64] = {};
BB bishop_occupancy_variations[64][512] = {};
BB bishop_attacks[64][512] = {};
BB rook_masks[64] = {};
BB rook_occupancy_variations[64][4096] = {};
BB rook_attacks[64][4096] = {};

void init_bishop_masks() {
    int i, j, k, square, rank, file, x, y, var_count, bit_count;
    int mask_bits[16], index_bits[16];
    BB mask;
    for (i = 0; i < 64; i++) {
        bishop_masks[i] = 0;
        rank = RANK(i);
        file = FILE(i);
        for (x = file + 1, y = rank + 1; IS_LEGAL_SQUARE(x, y); x++,y++) {
            bishop_masks[i] ^= SQ_MASK(y * 8 + x);
        }
        
        for (x = file - 1, y = rank + 1; IS_LEGAL_SQUARE(x, y); x--,y++) {
            bishop_masks[i] ^= SQ_MASK(y * 8 + x);
        }
        
        for (x = file + 1, y = rank - 1; IS_LEGAL_SQUARE(x, y); x++,y--) {
            bishop_masks[i] ^= SQ_MASK(y * 8 + x);
        }
        
        for (x = file - 1, y = rank - 1; IS_LEGAL_SQUARE(x, y); x--,y--) {
            bishop_masks[i] ^= SQ_MASK(y * 8 + x);
        }
        
        bishop_masks[i] &= NO_EDGES;
    }
    
    for (i = 0; i < 64; i++) {
        mask = bishop_masks[i];
        get_set_bits(mask, mask_bits);
        bit_count = count_bits(mask);
        var_count = 1 << bit_count;
        for (j = 0; j < var_count; j++) {
            bishop_occupancy_variations[i][j] = 0;
            get_set_bits(j, index_bits);
            for (k = 0; index_bits[k] != -1; k++)
                bishop_occupancy_variations[i][j] ^= SQ_MASK(mask_bits[index_bits[k]]);
            
            for (square = i + 9; FILE(square) != 0 && FILE(square) != 7 && square < 56 && (bishop_occupancy_variations[i][j] & SQ_MASK(square)) == 0; square += 9)
                bishop_attacks[i][j] ^= SQ_MASK(square);
            
            for (square = i + 7; FILE(square) != 0 && FILE(square) != 7 && square < 56 && (bishop_occupancy_variations[i][j] & SQ_MASK(square)) == 0; square += 7)
                bishop_attacks[i][j] ^= SQ_MASK(square);
            
            for (square = i - 9; FILE(square) != 0 && FILE(square) != 7 && square > 7 && (bishop_occupancy_variations[i][j] & SQ_MASK(square)) == 0; square -= 9)
                bishop_attacks[i][j] ^= SQ_MASK(square);
            
            for (square = i - 7; FILE(square) != 0 && FILE(square) != 7 && square > 7 && (bishop_occupancy_variations[i][j] & SQ_MASK(square)) == 0; square -= 7)
                bishop_attacks[i][j] ^= SQ_MASK(square);
        }
        
    }
    
}

void init_rook_masks() {
    int i, j, k, square, rank, file, var_count, bit_count;
    int mask_bits[16], index_bits[16];
    BB mask;
    for (i = 0; i < 64; i++) {
        rank = RANK(i);
        file = FILE(i);
        rook_masks[i] = Rank_Masks[rank] ^ File_Masks[file];
        if (file != 0)
            rook_masks[i] &= ~A_FILE;
        if (file != 7)
            rook_masks[i] &= ~H_FILE;
        if (rank != 0)
            rook_masks[i] &= ~RANK_1;
        if (rank != 7)
            rook_masks[i] &= ~RANK_8;
    }
    
    for (i = 0; i < 64; i++) {
        mask = rook_masks[i];
        get_set_bits(mask, mask_bits);
        bit_count = count_bits(mask);
        var_count = 1 << bit_count;
        for (j = 0; j < var_count; j++) {
            rook_occupancy_variations[i][j] = 0;
            get_set_bits(j, index_bits);
            for (k = 0; index_bits[k] != -1; k++)
                rook_occupancy_variations[i][j] ^= SQ_MASK(mask_bits[index_bits[k]]);
            
            for (square = i + 8; square < 56 && (rook_occupancy_variations[i][j] & SQ_MASK(square)) == 0; square += 8)
                rook_attacks[i][j] ^= SQ_MASK(square);
            
            for (square = i - 8; square > 7 && (rook_occupancy_variations[i][j] & SQ_MASK(square)) == 0; square -= 8)
                rook_attacks[i][j] ^= SQ_MASK(square);
            
            for (square = i + 1; FILE(square) != 0 && FILE(square) != 7 && (rook_occupancy_variations[i][j] & SQ_MASK(square)) == 0;square++)
                rook_attacks[i][j] ^= SQ_MASK(square);
            
            for (square = i - 1; FILE(square) != 0 && FILE(square) != 7 && square > 0 && (rook_occupancy_variations[i][j] & SQ_MASK(square)) == 0;square--)
                rook_attacks[i][j] ^= SQ_MASK(square);
        }
    }
}

void init_bishop_moves() {
    init_bishop_masks();
    BB legal_moves;
    int vars, bit_count, index, square, i, j;
    
    for (i = 0; i < 64; i++) {
        bit_count = count_bits(bishop_masks[i]);
        vars = 1 << bit_count;
        
        for (j = 0; j < vars; j++) {
            legal_moves = 0;
            index = MAGIC_INDEX(BISHOP, i, bishop_occupancy_variations[i][j]);
            
            for (square = i + 9; FILE(square) != 0 && square < 64; square += 9) {
                legal_moves ^= SQ_MASK(square);
                if (bishop_occupancy_variations[i][j] & SQ_MASK(square))
                    break;
            }
            
            for (square = i + 7; FILE(square) != 7 && square < 64; square += 7) {
                legal_moves ^= SQ_MASK(square);
                if (bishop_occupancy_variations[i][j] & SQ_MASK(square))
                    break;
            }
            
            for (square = i - 9; FILE(square) != 7 && square >= 0; square -= 9) {
                legal_moves ^= SQ_MASK(square);
                if (bishop_occupancy_variations[i][j] & SQ_MASK(square))
                    break;
            }
            
            for (square = i - 7; FILE(square) != 0 && square >= 0; square -= 7) {
                legal_moves ^= SQ_MASK(square);
                if (bishop_occupancy_variations[i][j] & SQ_MASK(square))
                    break;
            }
            
            
            
            if (magic_moves_bishop[i][index] != 0 && magic_moves_bishop[i][index] != legal_moves) {
                printf("Error: magic moves collision for bishops at square %d\n", i);
                exit(1);
            }
            
            magic_moves_bishop[i][index] = legal_moves;
        }
    }
}

void init_rook_moves() {
    init_rook_masks();
    BB legal_moves;
    int vars, bit_count, index, square, i, j;
    
    for (i = 0; i < 64; i++) {
        bit_count = count_bits(rook_masks[i]);
        vars = 1 << bit_count;
        
        for (j = 0; j < vars; j++) {
            legal_moves = 0;
            index = MAGIC_INDEX(ROOK, i, rook_occupancy_variations[i][j]);
            
            for (square = i + 8; square < 64; square += 8) {
                legal_moves ^= SQ_MASK(square);
                if (rook_occupancy_variations[i][j] & SQ_MASK(square))
                    break;
            }
            
            for (square = i + 1; FILE(square) != 0; square++) {
                legal_moves ^= SQ_MASK(square);
                if (rook_occupancy_variations[i][j] & SQ_MASK(square))
                    break;
            }
            
            for (square = i - 1; FILE(square) != 7 && square >= 0; square--) {
                legal_moves ^= SQ_MASK(square);
                if (rook_occupancy_variations[i][j] & SQ_MASK(square))
                    break;
            }
            
            for (square = i - 8; square >= 0; square -= 8) {
                legal_moves ^= SQ_MASK(square);
                if (rook_occupancy_variations[i][j] & SQ_MASK(square))
                    break;
            }
            
            
            
            if (magic_moves_rook[i][index] != 0 && magic_moves_rook[i][index] != legal_moves) {
                printf("Error: magic moves collision for bishops at square %d\n", i);
                exit(1);
            }
            
            magic_moves_rook[i][index] = legal_moves;
        }
    }
}

void init_magic_moves() {
    init_bishop_moves();
    init_rook_moves();
}