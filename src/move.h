//
//  move.h
//  Spartan
//
//
//
//

#ifndef move_h
#define move_h

#include "board.h"

#define NON_CAPTURE 0
#define CAPTURE 1

#define ILLEGAL 0
#define LEGAL 1

#define NOT_CHECK 0
#define SINGLE_CHECK 1
#define DOUBLE_CHECK 2

#define NULL_MOVE 0

#define FLAG_NONE 0
#define FLAG_CASTLE_KINGSIDE 1
#define FLAG_CASTLE_QUEENSIDE 2
#define FLAG_PROMOTE_KNIGHT 3
#define FLAG_PROMOTE_BISHOP 4
#define FLAG_PROMOTE_ROOK 5
#define FLAG_PROMOTE_QUEEN 6
#define FLAG_EP 7

#define MOVE_FROM(m) (m & 0x3f)
#define MOVE_TO(m) ((m >> 6) & 0x3f)
#define MOVE_PIECE(m) ((m >> 12) & 0x7)
#define MOVE_FLAGS(m) ((m >> 15) & 0x7)
#define MOVE_IS_CASTLE_KINGSIDE(m) (MOVE_FLAGS(m) == FLAG_CASTLE_KINGSIDE)
#define MOVE_IS_CASTLE_QUEENSIDE(m) (MOVE_FLAGS(m) == FLAG_CASTLE_QUEENSIDE)
#define MOVE_IS_PROMOTE_KNIGHT(m) (MOVE_FLAGS(m) == FLAG_PROMOTE_KNIGHT)
#define MOVE_IS_PROMOTE_BISHOP(m) (MOVE_FLAGS(m) == FLAG_PROMOTE_BISHOP)
#define MOVE_IS_PROMOTE_ROOK(m) (MOVE_FLAGS(m) == FLAG_PROMOTE_ROOK)
#define MOVE_IS_PROMOTE_QUEEN(m) (MOVE_FLAGS(m) == FLAG_PROMOTE_QUEEN)
#define MOVE_IS_PROMOTION(m) (MOVE_IS_PROMOTE_KNIGHT(m) || MOVE_IS_PROMOTE_BISHOP(m) || MOVE_IS_PROMOTE_ROOK(m) || MOVE_IS_PROMOTE_QUEEN(m))
#define MOVE_IS_EN_PASSANT(m) (MOVE_FLAGS(m) == FLAG_EP)
#define MOVE_IS_CAPTURE(m) (m >> 18)

#define CREATE_MOVE(capture, flags, piece, to, from) (((capture) << 18) | ((flags) << 15) | ((piece) << 12) | ((to) << 6) | (from))
#define UNDO_MOVE_ON_GLOBAL_BOARD {global_board_stack()->ply--; dec_move_stack_depth(); }

// bits 0-5 are the the starting square
// bits 6-11 are the ending square
// bits 12-14 are the type of piece that is moving
// bits 15-17 are additional flags describing the type of move, such as promotion, castling, etc.
// bit 18 is whether the move is a (regular) capture or not. En passant does not count as a regular capture

typedef unsigned int Move_t;

// makes a move on the global board
void make_move_on_global_board(Move_t move);
void make_null_move_on_global_board();

/* This function makes a move directly on the board without copying to the stack. 
 It also does not update castling rights, the en passant square, white/black bitboards, or whether a king is in check or not.
 This function is much faster than make_move_on_global_board() and is only used for Static Exchange Evaluations
 Basically, just enough information is updated to we can perform a series of captures on the board
 */
void make_quick_move_on_board(Board_t *board, int from, int to, int captured_piece, int moveing_piece);

#endif /* move_h */
