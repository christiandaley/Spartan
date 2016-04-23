//
//  board.c
//  Spartan
//
//
//
//

#include <assert.h>
#include "board.h"
#include "bitmasks.h"
#include "movegen.h"

const int A1 = 0;
const int A2 = 8;
const int A3 = 16;
const int A4 = 24;
const int A5 = 32;
const int A6 = 40;
const int A7 = 48;
const int A8 = 56;

const int B1 = 1;
const int B2 = 9;
const int B3 = 17;
const int B4 = 25;
const int B5 = 33;
const int B6 = 41;
const int B7 = 49;
const int B8 = 57;

const int C1 = 2;
const int C2 = 10;
const int C3 = 18;
const int C4 = 26;
const int C5 = 34;
const int C6 = 42;
const int C7 = 50;
const int C8 = 58;

const int D1 = 3;
const int D2 = 11;
const int D3 = 19;
const int D4 = 27;
const int D5 = 35;
const int D6 = 43;
const int D7 = 51;
const int D8 = 59;

const int E1 = 4;
const int E2 = 12;
const int E3 = 20;
const int E4 = 28;
const int E5 = 36;
const int E6 = 44;
const int E7 = 52;
const int E8 = 60;

const int F1 = 5;
const int F2 = 13;
const int F3 = 21;
const int F4 = 29;
const int F5 = 37;
const int F6 = 45;
const int F7 = 53;
const int F8 = 61;

const int G1 = 6;
const int G2 = 14;
const int G3 = 22;
const int G4 = 30;
const int G5 = 38;
const int G6 = 46;
const int G7 = 54;
const int G8 = 62;

const int H1 = 7;
const int H2 = 15;
const int H3 = 23;
const int H4 = 31;
const int H5 = 39;
const int H6 = 47;
const int H7 = 55;
const int H8 = 63;

const char *Start_Fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

BoardStack_t global_stack;

void set_square_value(Board_t *board, int sq, int piece, int color) {
    
    // remove any piece on the current square
    WHITE_PAWNS(board) &= ~SQ_MASK(sq);
    BLACK_PAWNS(board) &= ~SQ_MASK(sq);
    
    WHITE_ROOKS(board) &= ~SQ_MASK(sq);
    BLACK_ROOKS(board) &= ~SQ_MASK(sq);
    
    WHITE_KNIGHTS(board) &= ~SQ_MASK(sq);
    BLACK_KNIGHTS(board) &= ~SQ_MASK(sq);
    
    WHITE_BISHOPS(board) &= ~SQ_MASK(sq);
    BLACK_BISHOPS(board) &= ~SQ_MASK(sq);
    
    WHITE_QUEENS(board) &= ~SQ_MASK(sq);
    BLACK_QUEENS(board) &= ~SQ_MASK(sq);
    
    WHITE_KING(board) &= ~SQ_MASK(sq);
    BLACK_KING(board) &= ~SQ_MASK(sq);
    
    if (piece != 0)
        PIECES_OF_TYPE(board, piece, color) ^= SQ_MASK(sq);
    
    update_bitboards(board);
}

void set_global_square_value(int sq, int piece, int color) {
    set_square_value(global_board(), sq, piece, color);
}

void update_bitboards(Board_t *board) {
    WHITE_PIECES(board) = WHITE_PAWNS(board) | WHITE_ROOKS(board) | WHITE_KNIGHTS(board) | WHITE_BISHOPS(board) | WHITE_QUEENS(board) | WHITE_KING(board);
    
    BLACK_PIECES(board) = BLACK_PAWNS(board) | BLACK_ROOKS(board) | BLACK_KNIGHTS(board) | BLACK_BISHOPS(board) | BLACK_QUEENS(board) | BLACK_KING(board);
    
    ALL_PIECES(board) = WHITE_PIECES(board) | BLACK_PIECES(board);
}

void update_global_bitboards() {
    update_bitboards(global_board());
}

BoardStack_t *global_board_stack() {
    return &global_stack;
}

Board_t *global_board() {
    return global_stack.stack + global_stack.ply;
}

void reset_global_ply() {
    global_stack.ply = 0;
}

void copy_and_inc_stack() {
    global_stack.stack[global_stack.ply+1] = global_stack.stack[global_stack.ply];
    global_stack.ply++;
}