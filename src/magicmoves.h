//
//  magicmoves.h
//  Spartan
//
//
//
//

#ifndef magicmoves_h
#define magicmoves_h

#include "board.h"

#define MAGIC_INDEX(t, sq, occ) ((t) == BISHOP ? \
((int)(((occ) * magic_numbers_bishop[sq]) >> magic_bitshifts_bishop[sq])) : \
((int)(((occ) * magic_numbers_rook[sq]) >> magic_bitshifts_rook[sq])))

extern BB magic_moves_rook[64][4096];
extern BB magic_moves_bishop[64][512];
extern BB bishop_masks[64];
extern BB bishop_occupancy_variations[64][512];
extern BB bishop_attacks[64][512];
extern BB rook_masks[64];
extern BB rook_occupancy_variations[64][4096];
extern BB rook_attacks[64][4096];

void init_magic_moves();
void init_bishop_masks();
void init_rook_masks();


#endif /* magicmoves_h */
