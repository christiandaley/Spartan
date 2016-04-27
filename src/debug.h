//
//  debug.h
//  Spartan
//
//
//
//

#ifndef debug_h
#define debug_h

#include <stdlib.h>
#include <stdint.h>
#include "board.h"
#include "move.h"

// turn this on for debugging to make sure the board is behaving properly. Slows down the program significantly.
#define DO_ASSERTIONS 0

void print_board(Board_t *board);
void print_global_board();
void print_move_with_detail(Move_t move);
void print_move(Move_t move);
void move_to_string(Move_t move, char string[6]);
int bitboards_are_ok(Board_t *board);
uint64_t perft(int depth);
void print_bitboard(const BB bitboard); // print the bits in the bitboard


#endif /* debug_h */
