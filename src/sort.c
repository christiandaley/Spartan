//
//  sort.c
//  Spartan
//
//
//
//

#include <string.h>
#include <assert.h>
#include "sort.h"
#include "movegen.h"
#include "bitmasks.h"
#include "bitscan.h"
#include "eval.h"
#include "magicmoves.h"
#include "magicnumbers.h"
#include "debug.h"
#include "killer.h"
#include "history.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

const int Piece_Values[7] = {0, 100, 500, 325, 325, 1000, 9001};
Sort_Data_t sort_data;

int see(Move_t move, Board_t *board) {
    Board_t temp_board = *global_board();
    Board_t *ptr = &temp_board;
    
    int attacking_piece = MOVE_PIECE(move);
    int captured_piece;
    int gain[32];
    int depth = 0;
    int from = MOVE_FROM(move);
    int to = MOVE_TO(move);
    
    if (ENEMY_PAWNS(ptr) & SQ_MASK(to))
        captured_piece = PAWN;
    else if (ENEMY_KNIGHTS(ptr) & SQ_MASK(to))
        captured_piece = KNIGHT;
    else if (ENEMY_BISHOPS(ptr) & SQ_MASK(to))
        captured_piece = BISHOP;
    else if (ENEMY_ROOKS(ptr) & SQ_MASK(to))
        captured_piece = ROOK;
    else if (ENEMY_QUEENS(ptr) & SQ_MASK(to))
        captured_piece = QUEEN;
    else
        captured_piece = 0;
        
    gain[0] = Piece_Values[captured_piece];
    
    if (MOVE_IS_EN_PASSANT(move))
        gain[0] = Piece_Values[PAWN];
    
    do {
        make_quick_move_on_board(ptr, from, to, captured_piece, attacking_piece);
        
        captured_piece = attacking_piece;
        depth++;
        gain[depth] = Piece_Values[captured_piece] - gain[depth-1];
        
        if (MAX(-gain[depth-1], gain[depth]) < 0)
            break;
        
        attacking_piece = get_least_valuable_attacker(ptr, to, &from);
        
    } while (attacking_piece);
    
    while (--depth)
        gain[depth-1] = -MAX(-gain[depth-1], gain[depth]);
    
    return gain[0];
}

void sort_qsearch_moves(int *scores) {
    Move_t *moves = global_move_list();
    Move_t temp;
    int nMoves = global_move_count();
    int i, fail;
    
    for (i = 0; i < nMoves; i++)
        scores[i] = see(moves[i], global_board());
    
    do {
        fail = 0;
        for (i = 0; i < nMoves - 1; i++) {
            if (scores[i] < scores[i+1]) {
                temp = moves[i];
                moves[i] = moves[i+1];
                moves[i+1] = temp;
                temp = scores[i];
                scores[i] = scores[i+1];
                scores[i+1] = temp;
                fail = 1;
            }
        }
        
        
    } while (fail);
}

void sort_global_moves(Move_t tt_move, int tt_score, int depth) {
    
    Board_t *board = global_board();
    Move_t *moves = global_move_list();
    Move_t temp;
    int nMoves = global_move_count();
    int n_killers;
    
    Move_t *killers = get_killers(&n_killers, depth);
    
    if (nMoves <= 1)
        return;
    
    int *scores = sort_data.scores[depth];
    int *flags = sort_data.flags[depth];
    
    int i, j, fail;
    for (i = 0; i < nMoves; i++) {
        scores[i] = see(moves[i], board);
        flags[i] = NO_ORDER_FLAG;
        
        if (moves[i] == tt_move) {
            scores[i] = tt_score;
            flags[i] = TT_MOVE;
        } else {
            for (j = 0; j < n_killers; j++) {
                if (moves[i] == killers[j])
                    flags[i] = KILLER_MOVE;
            }
            
        }
        
        if (flags[i] == NO_ORDER_FLAG &&
            (temp = get_history_score(moves[i], TURN(board))) != -VALUE_INFINITE) {
            flags[i] = HISTORY_MOVE;
            scores[i] = temp;
        }
    }
    
    do {
        fail = 0;
        for (i = 0; i < nMoves - 1; i++) {
            if ((flags[i] < flags[i+1]) || (flags[i] == flags[i+1] && scores[i] < scores[i+1])) {
                temp = moves[i];
                moves[i] = moves[i+1];
                moves[i+1] = temp;
                temp = scores[i];
                scores[i] = scores[i+1];
                scores[i+1] = temp;
                temp = flags[i];
                flags[i] = flags[i+1];
                flags[i+1] = temp;
                fail = 1;
            }
        }
        
        
    } while (fail);
    
}

void sort_root_moves(int *scores) {
    Move_t *moves = global_move_list();
    Move_t temp;
    
    int nMoves = global_move_count();
    
    if (nMoves <= 1)
        return;
    
    int i, fail;
    
    do {
        fail = 0;
        for (i = 0; i < nMoves - 1; i++) {
            if (scores[i] < scores[i+1]) {
                temp = moves[i];
                moves[i] = moves[i+1];
                moves[i+1] = temp;
                temp = scores[i];
                scores[i] = scores[i+1];
                scores[i+1] = temp;
                fail = 1;
            }
        }
        
        
    } while (fail);
    
}