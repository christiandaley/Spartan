//
//  movegen.c
//  Spartan
//
//
//
//

#include <assert.h>
#include <string.h>
#include "movegen.h"
#include "bitscan.h"
#include "bitmasks.h"
#include "magicmoves.h"
#include "magicnumbers.h"
#include "debug.h"
#include "log.h"

Move_Stack_t global_move_stack;

Move_t *global_move_list() {
    return global_move_stack.stack[global_move_stack.depth];
}

int global_move_count() {
    return global_move_stack.counts[global_move_stack.depth];
}

void inc_move_stack_depth() {
    global_move_stack.depth++;
    global_move_stack.counts[global_move_stack.depth] = 0;
}

void dec_move_stack_depth() {
    global_move_stack.depth--;
}

void reset_global_move_stack_depth() {
    global_move_stack.depth = 0;
}

void generate_global_legal_moves() {
    Board_t *board = global_board();
    int i, lcount, pl_count, check_count, checker_loc, checker_type, check_pattern;
    Move_t move;
    lcount = 0;
    check_count = is_check(board, &checker_loc, &checker_type, &check_pattern);
    board->is_check = check_count;
    
    switch (check_count) {
        case DOUBLE_CHECK:
            global_move_stack.pl_count = 0;
            generate_king_moves(board);
            // in double check, we can only move our king
            pl_count = global_move_stack.pl_count;
            for (i = 0; i < pl_count; i++) {
                move = global_move_stack.pl_stack[i];
                if (is_legal(move, board))
                    global_move_list()[lcount++] = move;
            }
            
            break;
        case SINGLE_CHECK:
            generate_evasions(board, checker_loc, checker_type, check_pattern);
            pl_count = global_move_stack.pl_count;
            for (i = 0; i < pl_count; i++) {
                move = global_move_stack.pl_stack[i];
                if (MOVE_PIECE(move) == KING || MOVE_FLAGS(move) == FLAG_EP) {
                    // only king moves and en passant captures need to be tested for legality
                    // the blocks and regular captures are guaranteed to be legal
                    if (is_legal(move, board))
                        global_move_list()[lcount++] = move;
                } else
                    global_move_list()[lcount++] = move;
            }
            break;
            
        case NOT_CHECK:
            generate_pseudo_legal_moves(board);
            pl_count = global_move_stack.pl_count;
            for (i = 0; i < pl_count; i++) {
                move = global_move_stack.pl_stack[i];
                if (is_legal(move, board))
                    global_move_list()[lcount++] = move;
            }
            break;
        default:
            break;
    }
    
    global_move_stack.counts[global_move_stack.depth] = lcount;
    
}

void generate_pseudo_legal_moves(Board_t *board) {
    BB pawns, rooks, knights, bishops, queens;
    BB sliding_moves;
    Move_t *moves = global_move_stack.pl_stack;
    int sq, to, index;
    int n = 0;
    
    // pawn moves
    for (pawns = FRIENDLY_PAWNS(board); pawns; pawns &= (pawns - 1)) {
        sq = LS1B(pawns);
        if (TURN(board) == WHITE) {
            if (PIECE_ON_SQUARE(sq + 8, board) == 0) {
                if (RANK(sq) != 6) {
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, sq + 8, sq);
                } else {
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_QUEEN, PAWN, sq + 8, sq);
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_ROOK, PAWN, sq + 8, sq);
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_BISHOP, PAWN, sq + 8, sq);
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_KNIGHT, PAWN, sq + 8, sq);
                }
                
                if (RANK(sq) == 1 && !PIECE_ON_SQUARE(sq + 16, board))
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, sq + 16, sq);
            }
            
            if (FILE(sq) != 0 && BLACK_PIECE_ON_SQUARE(sq + 7, board)) {
                if (RANK(sq) != 6) {
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, PAWN, sq + 7, sq);
                } else {
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_QUEEN, PAWN, sq + 7, sq);
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_ROOK, PAWN, sq + 7, sq);
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_BISHOP, PAWN, sq + 7, sq);
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_KNIGHT, PAWN, sq + 7, sq);
                    
                }
                
            }
            
            if (FILE(sq) != 7 && BLACK_PIECE_ON_SQUARE(sq + 9, board)) {
                if (RANK(sq) != 6) {
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, PAWN, sq + 9, sq);
                } else {
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_QUEEN, PAWN, sq + 9, sq);
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_ROOK, PAWN, sq + 9, sq);
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_BISHOP, PAWN, sq + 9, sq);
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_KNIGHT, PAWN, sq + 9, sq);
                    
                }
                
            }
            
            if (EP_SQUARE(board) && (((EP_SQUARE(board) == sq + 7) && FILE(sq) != 0) ||
                                     ((EP_SQUARE(board) == sq + 9) && FILE(sq) != 7)))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_EP, PAWN, EP_SQUARE(board), sq);
            
        } else {
            if (PIECE_ON_SQUARE(sq - 8, board) == 0) {
                if (RANK(sq) != 1) {
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, sq - 8, sq);
                } else {
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_QUEEN, PAWN, sq - 8, sq);
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_ROOK, PAWN, sq - 8, sq);
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_BISHOP, PAWN, sq - 8, sq);
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_KNIGHT, PAWN, sq - 8, sq);
                    
                }
                
                if (RANK(sq) == 6 && PIECE_ON_SQUARE(sq - 16, board) == 0)
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, sq - 16, sq);
            }
            
            if (FILE(sq) != 7 && WHITE_PIECE_ON_SQUARE(sq - 7, board)) {
                if (RANK(sq) != 1) {
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, PAWN, sq - 7, sq);
                } else {
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_QUEEN, PAWN, sq - 7, sq);
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_ROOK, PAWN, sq - 7, sq);
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_BISHOP, PAWN, sq - 7, sq);
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_KNIGHT, PAWN, sq - 7, sq);
                    
                }
                
            }
            
            if (FILE(sq) != 0 && WHITE_PIECE_ON_SQUARE(sq - 9, board)) {
                if (RANK(sq) != 1) {
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, PAWN, sq - 9, sq);
                } else {
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_QUEEN, PAWN, sq - 9, sq);
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_ROOK, PAWN, sq - 9, sq);
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_BISHOP, PAWN, sq - 9, sq);
                    moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_KNIGHT, PAWN, sq - 9, sq);
                    
                }
                
            }
            
            if (EP_SQUARE(board) && (((EP_SQUARE(board) == sq - 7) && FILE(sq) != 7) ||
                                     ((EP_SQUARE(board) == sq - 9) && FILE(sq) != 0))) {
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_EP, PAWN, EP_SQUARE(board), sq);
            }
            
            
        }
        
        
    }
    
    
    // knight moves
    for (knights = FRIENDLY_KNIGHTS(board); knights; knights &= (knights - 1)) {
        sq = LS1B(knights);
        /*
         0  X  0  0  0
         0  0  0  0  0
         0  0  N  0  0
         0  0  0  0  0
         0  0  0  0  0
         */
        if (FILE(sq) > 0 && RANK(sq) < 6) {
            if (!PIECE_ON_SQUARE(sq + 15, board))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, KNIGHT, sq + 15, sq);
            
            else if (ENEMY_PIECE_ON_SQUARE(sq + 15, board))
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, KNIGHT, sq + 15, sq);
        }
        
        /*
         0  0  0  X  0
         0  0  0  0  0
         0  0  N  0  0
         0  0  0  0  0
         0  0  0  0  0
         */
        if (FILE(sq) < 7 && RANK(sq) < 6) {
            if (!PIECE_ON_SQUARE(sq + 17, board))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, KNIGHT, sq + 17, sq);
            
            else if (ENEMY_PIECE_ON_SQUARE(sq + 17, board))
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, KNIGHT, sq + 17, sq);
        }
        
        /*
         0  0  0  0  0
         X  0  0  0  0
         0  0  N  0  0
         0  0  0  0  0
         0  0  0  0  0
         */
        if (FILE(sq) > 1 && RANK(sq) < 7) {
            if (!PIECE_ON_SQUARE(sq + 6, board))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, KNIGHT, sq + 6, sq);
            
            else if (ENEMY_PIECE_ON_SQUARE(sq + 6, board))
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, KNIGHT, sq + 6, sq);
        }
        
        /*
         0  0  0  0  0
         0  0  0  0  X
         0  0  N  0  0
         0  0  0  0  0
         0  0  0  0  0
         */
        if (FILE(sq) < 6 && RANK(sq) < 7) {
            if (!PIECE_ON_SQUARE(sq + 10, board))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, KNIGHT, sq + 10, sq);
            
            else if (ENEMY_PIECE_ON_SQUARE(sq + 10, board))
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, KNIGHT, sq + 10, sq);
        }
        
        /*
         0  0  0  0  0
         0  0  0  0  0
         0  0  N  0  0
         X  0  0  0  0
         0  0  0  0  0
         */
        if (FILE(sq) > 1 && RANK(sq) > 0) {
            if (!PIECE_ON_SQUARE(sq - 10, board))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, KNIGHT, sq - 10, sq);
            
            else if (ENEMY_PIECE_ON_SQUARE(sq - 10, board))
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, KNIGHT, sq - 10, sq);
        }
        
        /*
         0  0  0  0  0
         0  0  0  0  0
         0  0  N  0  0
         0  0  0  0  X
         0  0  0  0  0
         */
        if (FILE(sq) < 6 && RANK(sq) > 0) {
            if (!PIECE_ON_SQUARE(sq - 6, board))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, KNIGHT, sq - 6, sq);
            
            else if (ENEMY_PIECE_ON_SQUARE(sq - 6, board))
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, KNIGHT, sq - 6, sq);
        }
        
        
        /*
         0  0  0  0  0
         0  0  0  0  0
         0  0  N  0  0
         0  0  0  0  0
         0  X  0  0  0
         */
        if (FILE(sq) > 0 && RANK(sq) > 1) {
            if (!PIECE_ON_SQUARE(sq - 17, board))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, KNIGHT, sq - 17, sq);
            
            else if (ENEMY_PIECE_ON_SQUARE(sq - 17, board))
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, KNIGHT, sq - 17, sq);
        }
        
        /*
         0  0  0  0  0
         0  0  0  0  0
         0  0  N  0  0
         0  0  0  0  0
         0  0  0  X  0
         */
        if (FILE(sq) < 7 && RANK(sq) > 1) {
            if (!PIECE_ON_SQUARE(sq - 15, board))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, KNIGHT, sq - 15, sq);
            
            else if (ENEMY_PIECE_ON_SQUARE(sq - 15, board))
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, KNIGHT, sq - 15, sq);
        }
        
    }
    
    // bishops
    for (bishops = FRIENDLY_BISHOPS(board); bishops; bishops &= (bishops - 1)) {
        sq = LS1B(bishops);
        index = MAGIC_INDEX(BISHOP, sq, ALL_PIECES(board) & bishop_masks[sq]);
        sliding_moves = magic_moves_bishop[sq][index] & ~FRIENDLY_PIECES(board);
        for (; sliding_moves; sliding_moves &= (sliding_moves - 1)) {
            to = LS1B(sliding_moves);
            moves[n++] = CREATE_MOVE(PIECE_ON_SQUARE(to, board) != 0, FLAG_NONE, BISHOP, to, sq);
            
        }
    }
    
    for (rooks = FRIENDLY_ROOKS(board); rooks; rooks &= (rooks - 1)) {
        sq = LS1B(rooks);
        index = MAGIC_INDEX(ROOK, sq, ALL_PIECES(board) & rook_masks[sq]);
        sliding_moves = magic_moves_rook[sq][index] & ~FRIENDLY_PIECES(board);
        for (; sliding_moves; sliding_moves &= (sliding_moves - 1)) {
            to = LS1B(sliding_moves);
            moves[n++] = CREATE_MOVE(PIECE_ON_SQUARE(to, board) != 0, FLAG_NONE, ROOK, to, sq);
            
        }
    }
    
    for (queens = FRIENDLY_QUEENS(board); queens; queens &= (queens - 1)) {
        sq = LS1B(queens);
        // bishop moves
        index = MAGIC_INDEX(BISHOP, sq, ALL_PIECES(board) & bishop_masks[sq]);
        sliding_moves = magic_moves_bishop[sq][index] & ~FRIENDLY_PIECES(board);
        
        // rook moves
        index = MAGIC_INDEX(ROOK, sq, ALL_PIECES(board) & rook_masks[sq]);
        sliding_moves |= magic_moves_rook[sq][index] & ~FRIENDLY_PIECES(board);
        
        for (; sliding_moves; sliding_moves &= (sliding_moves - 1)) {
            to = LS1B(sliding_moves);
            moves[n++] = CREATE_MOVE(PIECE_ON_SQUARE(to, board) != 0, FLAG_NONE, QUEEN, to, sq);
            
        }
    }
    
    if (FRIENDLY_CAN_CASTLE_KINGSIDE(board)) {
        int king = LS1B(FRIENDLY_KING(board));
        if (!PIECE_ON_SQUARE(king + 1, board) && !PIECE_ON_SQUARE(king + 2, board)) {
            Move_t intermediate_move = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, KING, king + 1, king);
            if (is_legal(intermediate_move, board))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_CASTLE_KINGSIDE, KING, king + 2, king);
        }
    }
    
    if (FRIENDLY_CAN_CASTLE_QUEENSIDE(board)) {
        int king = LS1B(FRIENDLY_KING(board));
        if (!PIECE_ON_SQUARE(king - 1, board) && !PIECE_ON_SQUARE(king - 2, board) && !PIECE_ON_SQUARE(king - 3, board)) {
            Move_t intermediate_move = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, KING, king - 1, king);
            if (is_legal(intermediate_move, board))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_CASTLE_QUEENSIDE, KING, king - 2, king);
        }
    }
    
    
    global_move_stack.pl_count = n;
    generate_king_moves(board);
}

void generate_king_moves(Board_t *board) {
    // generates all normal king moves (not castling)
    int sq = LS1B(FRIENDLY_KING(board));
    Move_t *moves = global_move_stack.pl_stack;
    int n = global_move_stack.pl_count;
    /*
     0  0  0  0  0
     0  X  0  0  0
     0  0  K  0  0
     0  0  0  0  0
     0  0  0  0  0
     */
    if (FILE(sq) != 0 && RANK(sq) != 7 && !FRIENDLY_PIECE_ON_SQUARE(sq + 7, board))
        moves[n++] = CREATE_MOVE(PIECE_ON_SQUARE(sq + 7, board) != 0, FLAG_NONE, KING, sq + 7, sq);
    
    /*
     0  0  0  0  0
     0  0  X  0  0
     0  0  K  0  0
     0  0  0  0  0
     0  0  0  0  0
     */
    if (RANK(sq) != 7 && !FRIENDLY_PIECE_ON_SQUARE(sq + 8, board))
        moves[n++] = CREATE_MOVE(PIECE_ON_SQUARE(sq + 8, board) != 0, FLAG_NONE, KING, sq + 8, sq);
    
    /*
     0  0  0  0  0
     0  0  0  X  0
     0  0  K  0  0
     0  0  0  0  0
     0  0  0  0  0
     */
    if (FILE(sq) != 7 && RANK(sq) != 7 && !FRIENDLY_PIECE_ON_SQUARE(sq + 9, board))
        moves[n++] = CREATE_MOVE(PIECE_ON_SQUARE(sq + 9, board) != 0, FLAG_NONE, KING, sq + 9, sq);
    
    /*
     0  0  0  0  0
     0  0  0  0  0
     0  X  K  0  0
     0  0  0  0  0
     0  0  0  0  0
     */
    if (FILE(sq) != 0 && !FRIENDLY_PIECE_ON_SQUARE(sq - 1, board))
        moves[n++] = CREATE_MOVE(PIECE_ON_SQUARE(sq - 1, board) != 0, FLAG_NONE, KING, sq - 1, sq);
    
    /*
     0  0  0  0  0
     0  0  0  0  0
     0  0  K  X  0
     0  0  0  0  0
     0  0  0  0  0
     */
    if (FILE(sq) != 7 && !FRIENDLY_PIECE_ON_SQUARE(sq + 1, board))
        moves[n++] = CREATE_MOVE(PIECE_ON_SQUARE(sq + 1, board) != 0, FLAG_NONE, KING, sq + 1, sq);
    
    /*
     0  0  0  0  0
     0  0  0  0  0
     0  0  K  0  0
     0  X  0  0  0
     0  0  0  0  0
     */
    if (FILE(sq) != 0 && RANK(sq) != 0 && !FRIENDLY_PIECE_ON_SQUARE(sq - 9, board))
        moves[n++] = CREATE_MOVE(PIECE_ON_SQUARE(sq - 9, board) != 0, FLAG_NONE, KING, sq - 9, sq);
    
    /*
     0  0  0  0  0
     0  0  0  0  0
     0  0  K  0  0
     0  0  X  0  0
     0  0  0  0  0
     */
    if (RANK(sq) != 0 && !FRIENDLY_PIECE_ON_SQUARE(sq - 8, board))
        moves[n++] = CREATE_MOVE(PIECE_ON_SQUARE(sq - 8, board) != 0, FLAG_NONE, KING, sq - 8, sq);
    
    /*
     0  0  0  0  0
     0  0  0  0  0
     0  0  K  0  0
     0  0  0  X  0
     0  0  0  0  0
     */
    if (FILE(sq) != 7 && RANK(sq) != 0 && !FRIENDLY_PIECE_ON_SQUARE(sq - 7, board))
        moves[n++] = CREATE_MOVE(PIECE_ON_SQUARE(sq - 7, board) != 0, FLAG_NONE, KING, sq - 7, sq);
    
    global_move_stack.pl_count = n;
}

/* generates all legal responses to a single check. These are:
 1. King moves
 2. Capturing the checker
 3. Blocks (in case of a bishop/rook/queen checker)
 */
void generate_evasions(Board_t *board, int checker_loc, int checker_type, int check_pattern) {
    global_move_stack.pl_count = 0;
    
    // first we generate all king moves
    generate_king_moves(board);
    int n = global_move_stack.pl_count;
    Move_t *moves = global_move_stack.pl_stack;
    
    // then we see if we can capture
    BB attackers, potential_blocks, blocking_squares, bishops, rooks, queens;
    int capturer_loc, index, from, to;
    int king_loc = LS1B(FRIENDLY_KING(board));
    
    // pawn captures first
    attackers = FRIENDLY_PAWN_ATTACKS_TO(checker_loc, board) & FRIENDLY_PAWNS(board);
    for (; attackers; attackers &= (attackers - 1)) {
        capturer_loc = LS1B(attackers);
        if (piece_can_move(board, capturer_loc, checker_loc)) {
            if ((TURN(board) == WHITE && RANK(checker_loc) == 7) || (TURN(board) == BLACK && RANK(checker_loc) == 0)) {
                // promotion captures
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_QUEEN, PAWN, checker_loc, capturer_loc);
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_ROOK, PAWN, checker_loc, capturer_loc);
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_KNIGHT, PAWN, checker_loc, capturer_loc);
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_PROMOTE_BISHOP, PAWN, checker_loc, capturer_loc);
            } else // regular capture
                moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, PAWN, checker_loc, capturer_loc);
        }
        
    }
    
    // knight captures
    attackers = Knight_Attacks[checker_loc] & FRIENDLY_KNIGHTS(board);
    for (; attackers; attackers &= (attackers - 1)) {
        capturer_loc = LS1B(attackers);
        if (piece_can_move(board, capturer_loc, checker_loc))
            moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, KNIGHT, checker_loc, capturer_loc);
        
    }
    
    // bishop/queen captures. We check to see if a bishop/queen on the square could capture one of our bishops/queens.
    // if it could, then that means that we have a bishop or queen that can capture whatever is on the square
    index = MAGIC_INDEX(BISHOP, checker_loc, ALL_PIECES(board) & bishop_masks[checker_loc]);
    attackers = magic_moves_bishop[checker_loc][index] & (FRIENDLY_BISHOPS(board) | FRIENDLY_QUEENS(board));
    
    for (;attackers; attackers &= (attackers - 1)) {
        capturer_loc = LS1B(attackers);
        if (piece_can_move(board, capturer_loc, checker_loc)) {
            moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, BISHOP_ON_SQUARE(capturer_loc, board) ? BISHOP : QUEEN, checker_loc, capturer_loc);
        }
    }
    
    // rook/queen captures. We check to see if a rook/queen on the square could capture one of our rooks/queens.
    // if it could, then that means that we have a bishop or queen that can capture whatever is on the square
    index = MAGIC_INDEX(ROOK, checker_loc, ALL_PIECES(board) & rook_masks[checker_loc]);
    attackers = magic_moves_rook[checker_loc][index] & (FRIENDLY_ROOKS(board) | FRIENDLY_QUEENS(board));
    
    for (;attackers; attackers &= (attackers - 1)) {
        capturer_loc = LS1B(attackers);
        if (piece_can_move(board, capturer_loc, checker_loc))
            moves[n++] = CREATE_MOVE(CAPTURE, FLAG_NONE, ROOK_ON_SQUARE(capturer_loc, board) ? ROOK : QUEEN, checker_loc, capturer_loc);
    }
    
    /* now we generate blocks and e.p captures. A knight or pawn that is giving check can not be blocked,
      but a pawn might be able to be captured en passant */
    if (checker_type == KNIGHT || (checker_type == PAWN && EP_SQUARE(board) == 0)) {
        global_move_stack.pl_count = n;
        return;
    }
    
    blocking_squares = 0;
    switch (checker_type) {
        case QUEEN:
            // bishop-type blocks
            if (check_pattern == BISHOP) {
                index = MAGIC_INDEX(BISHOP, checker_loc, ALL_PIECES(board) & bishop_masks[checker_loc]);
                blocking_squares = magic_moves_bishop[checker_loc][index];
                
                index = MAGIC_INDEX(BISHOP, king_loc, ALL_PIECES(board) & bishop_masks[king_loc]);
                blocking_squares &= magic_moves_bishop[king_loc][index];
            } else {
                
                // rook-type blocks
                index = MAGIC_INDEX(ROOK, checker_loc, ALL_PIECES(board) & rook_masks[checker_loc]);
                blocking_squares = magic_moves_rook[checker_loc][index];
                
                index = MAGIC_INDEX(ROOK, king_loc, ALL_PIECES(board) & rook_masks[king_loc]);
                blocking_squares &= magic_moves_rook[king_loc][index];
                
            }
            
            break;
            
        case BISHOP:
            // this determines all of the squares that could be used for blocking a bishop attack
            index = MAGIC_INDEX(BISHOP, checker_loc, ALL_PIECES(board) & bishop_masks[checker_loc]);
            blocking_squares = magic_moves_bishop[checker_loc][index];
            
            index = MAGIC_INDEX(BISHOP, king_loc, ALL_PIECES(board) & bishop_masks[king_loc]);
            blocking_squares &= magic_moves_bishop[king_loc][index];
            break;
            
        case ROOK:
            
            // this determines all of the squares that could be used for blocking a rook attack
            index = MAGIC_INDEX(ROOK, checker_loc, ALL_PIECES(board) & rook_masks[checker_loc]);
            blocking_squares = magic_moves_rook[checker_loc][index];
            
            index = MAGIC_INDEX(ROOK, king_loc, ALL_PIECES(board) & rook_masks[king_loc]);
            blocking_squares &= magic_moves_rook[king_loc][index];
            
            break;
            
        case PAWN:
            // check if we can e.p capture the checker
            if (EP_SQUARE(board) == 0) // no ep square
                break;
            
            /* if the piece giving check is a pawn, then that pawn must have just been moved 
              (because obviously a pawn cant give check via discovered attack)
             if there is also an en passant square, the pawn giving check is (potentialy) capturable by en passant*/
            
            if (TURN(board) == WHITE) {
                
                if (FILE(checker_loc) != 0 && WHITE_PAWN_ON_SQUARE(checker_loc - 1, board))
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_EP, PAWN, EP_SQUARE(board), checker_loc - 1);
                
                if (FILE(checker_loc) != 7 && WHITE_PAWN_ON_SQUARE(checker_loc + 1, board))
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_EP, PAWN, EP_SQUARE(board), checker_loc + 1);
                
            } else if (TURN(board) == BLACK) {
                
                if (FILE(checker_loc) != 0 && BLACK_PAWN_ON_SQUARE(checker_loc - 1, board))
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_EP, PAWN, EP_SQUARE(board), checker_loc - 1);
                
                if (FILE(checker_loc) != 7 && BLACK_PAWN_ON_SQUARE(checker_loc + 1, board))
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_EP, PAWN, EP_SQUARE(board), checker_loc + 1);
            }
            
            break;
            
        default:
            break;
    }
    
    // bishop blocks
    for (bishops = FRIENDLY_BISHOPS(board); bishops; bishops &= (bishops - 1)) {
        from = LS1B(bishops);
        index = MAGIC_INDEX(BISHOP, from, ALL_PIECES(board) & bishop_masks[from]);
        potential_blocks = magic_moves_bishop[from][index] & blocking_squares;
        for (; potential_blocks; potential_blocks &= (potential_blocks - 1))
            if (piece_can_move(board, from, LS1B(potential_blocks)))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, BISHOP, LS1B(potential_blocks), from);
    }
    
    // rook blocks
    for (rooks = FRIENDLY_ROOKS(board); rooks; rooks &= (rooks - 1)) {
        from = LS1B(rooks);
        index = MAGIC_INDEX(ROOK, from, ALL_PIECES(board) & rook_masks[from]);
        potential_blocks = magic_moves_rook[from][index] & blocking_squares;
        for (; potential_blocks; potential_blocks &= (potential_blocks - 1))
            if (piece_can_move(board, from, LS1B(potential_blocks)))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, ROOK, LS1B(potential_blocks), from);
    }
    
    // queen blocks
    for (queens = FRIENDLY_QUEENS(board); queens; queens &= (queens - 1)) {
        from = LS1B(queens);
        index = MAGIC_INDEX(BISHOP, from, ALL_PIECES(board) & bishop_masks[from]);
        potential_blocks = magic_moves_bishop[from][index];
        
        index = MAGIC_INDEX(ROOK, from, ALL_PIECES(board) & rook_masks[from]);
        potential_blocks |= magic_moves_rook[from][index];
        potential_blocks &= blocking_squares;
        
        for (; potential_blocks; potential_blocks &= (potential_blocks - 1))
            if (piece_can_move(board, from, LS1B(potential_blocks)))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, QUEEN, LS1B(potential_blocks), from);
        
    }
    
    for (;blocking_squares; blocking_squares &= (blocking_squares - 1)) {
        to = LS1B(blocking_squares);
        // pawn blocks
        if (TURN(board) == WHITE) {
            if (WHITE_PAWN_ON_SQUARE(to - 8, board) && piece_can_move(board, to - 8, to)) {
                if (RANK(to) == 7) {
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_QUEEN, PAWN, to, to - 8);
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_ROOK, PAWN, to, to - 8);
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_BISHOP, PAWN, to, to - 8);
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_KNIGHT, PAWN, to, to - 8);
                } else
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, to, to - 8);
                
            } else if (WHITE_PAWN_ON_SQUARE(to - 16, board) && !PIECE_ON_SQUARE(to - 8, board) && RANK(to) == 3 && piece_can_move(board, to - 16, to))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, to, to - 16);
            
        } else {
            if (BLACK_PAWN_ON_SQUARE(to + 8, board) && piece_can_move(board, to + 8, to)) {
                if (RANK(to) == 0) {
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_QUEEN, PAWN, to, to + 8);
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_ROOK, PAWN, to, to + 8);
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_BISHOP, PAWN, to, to + 8);
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_PROMOTE_KNIGHT, PAWN, to, to + 8);
                } else
                    moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, to, to + 8);
                
            } else if (BLACK_PAWN_ON_SQUARE(to + 16, board) && !PIECE_ON_SQUARE(to + 8, board) && RANK(to) == 4 && piece_can_move(board, to + 16, to))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, to, to + 16);
            
        }
        
        // knight blocks
        potential_blocks = Knight_Attacks[to] & FRIENDLY_KNIGHTS(board);
        
        for (; potential_blocks; potential_blocks &= (potential_blocks - 1)) {
            from = LS1B(potential_blocks);
            if (piece_can_move(board, from, to))
                moves[n++] = CREATE_MOVE(NON_CAPTURE, FLAG_NONE, KNIGHT, to, from);
        }
        
    }
    
    
    
    global_move_stack.pl_count = n;
}

// this function assumes that either player to move is NOT in check, or they are moving their king
int is_legal(Move_t move, Board_t *board) {
    BB sliding_moves;
    int index;
    int to = MOVE_TO(move);
    int from = MOVE_FROM(move);
    
    // handle king moves
    if (MOVE_PIECE(move) == KING) {
        //for a king move, just check to see if the square its moving to is attacked
        
        // check for pawn, knight, and king attacks on the destination
        if ((Knight_Attacks[to] & ENEMY_KNIGHTS(board)) || (ENEMY_PAWN_ATTACKS_TO(to, board) & ENEMY_PAWNS(board)) || (King_Attacks[to] & ENEMY_KING(board)))
            return ILLEGAL;
        
        
        // detect attacks by bishops/queens
        index = MAGIC_INDEX(BISHOP, to, (ALL_PIECES(board) ^ SQ_MASK(from)) & bishop_masks[to]);
        sliding_moves = magic_moves_bishop[to][index];
        
        if (sliding_moves & (ENEMY_BISHOPS(board) | ENEMY_QUEENS(board)))
            return ILLEGAL;
        
        
        // detect attacks by rooks/queens
        index = MAGIC_INDEX(ROOK, to, (ALL_PIECES(board) ^ SQ_MASK(from)) & rook_masks[to]);
        sliding_moves = magic_moves_rook[to][index];
        
        if (sliding_moves & (ENEMY_ROOKS(board) | ENEMY_QUEENS(board)))
            return ILLEGAL;
        
        
        return LEGAL;
        
    } else if (MOVE_FLAGS(move) == FLAG_EP) {
        // the move is made and we see if it results in check
        make_move_on_global_board(move);
        CHANGE_TURN(global_board());
        int t1, t2, t3; // dummy vars
        int check = is_check_full(global_board(), &t1, &t2, &t3);
        UNDO_MOVE_ON_GLOBAL_BOARD
        return check ? ILLEGAL : LEGAL;
        
    }
    
    // if it's not a king move or en passant, check to make sure the piece is not moving out of a pin
    return piece_can_move(board, from, to) ? LEGAL : ILLEGAL;
}

int is_check(Board_t *board, int *checker_loc, int *checker_type, int *check_pattern) {
    // if there's no information about the last move that was made, we have to do a "full" test for check
    if (board->last_move_made == NULL_MOVE)
        return is_check_full(board, checker_loc, checker_type, check_pattern);
    
    BB king = FRIENDLY_KING(board);
    BB sliding_moves;
    int index;
    int sq = LS1B(king);
    int is_check = 0;
    int last_moved_piece = MOVE_PIECE(board->last_move_made);
    
    // here is where we detect direct checks
    // discovered checks are detected later on
    switch (last_moved_piece) {
        case PAWN: // a pawn move that checks can never result in a double check.
            if (ENEMY_PAWN_ATTACKS_TO(sq, board) & ENEMY_PAWNS(board)) {
                *checker_loc = LS1B(ENEMY_PAWN_ATTACKS_TO(sq, board) & ENEMY_PAWNS(board));
                *checker_type = PAWN;
                return SINGLE_CHECK;
            }
            
            // check for knight promotion resulting in check
            if (MOVE_IS_PROMOTE_KNIGHT(board->last_move_made) && (Knight_Attacks[sq] & ENEMY_KNIGHTS(board))) {
                *checker_loc = LS1B(Knight_Attacks[sq] & ENEMY_KNIGHTS(board));
                *checker_type = KNIGHT;
                is_check = 1;
            }
            
            break;
            
        case KNIGHT:
            // a knight move could be a single or double check, so we can't return right away even if we find a check
            if (Knight_Attacks[sq] & ENEMY_KNIGHTS(board)) {
                *checker_loc = LS1B(ENEMY_KNIGHTS(board) & Knight_Attacks[sq]);
                *checker_type = KNIGHT;
                is_check = 1;
            }
            break;
            
        case BISHOP:
            // pretends that there is a bishop in place of the king and generates the moves for that bishop accordingly
            // if that "pretend" bishop can capture an enemy bishop, the king is actually in check
            index = MAGIC_INDEX(BISHOP, sq, ALL_PIECES(board) & bishop_masks[sq]);
            sliding_moves = magic_moves_bishop[sq][index];
            
            if (sliding_moves & ENEMY_BISHOPS(board)) {
                *checker_loc = LS1B(sliding_moves & ENEMY_BISHOPS(board));
                *checker_type = BISHOP;
                is_check = 1;
            }
            
            break;
            
        case ROOK:
            // same idea as the bishop
            index = MAGIC_INDEX(ROOK, sq, ALL_PIECES(board) & rook_masks[sq]);
            sliding_moves = magic_moves_rook[sq][index];
            if (sliding_moves & ENEMY_ROOKS(board)) {
                *checker_loc = LS1B(sliding_moves & ENEMY_ROOKS(board));
                *checker_type = ROOK;
                is_check = 1;
            }
            
            break;
            
        case QUEEN:
            // for obvious reasons, a queen move can never result in double check
            // so if a check is found we immediately return
            
            // get the bishop-type moves
            index = MAGIC_INDEX(BISHOP, sq, ALL_PIECES(board) & bishop_masks[sq]);
            sliding_moves = magic_moves_bishop[sq][index];
            
            if (sliding_moves & ENEMY_QUEENS(board)) {
                *checker_loc = LS1B(sliding_moves & ENEMY_QUEENS(board));
                *checker_type = QUEEN;
                *check_pattern = BISHOP;
                return SINGLE_CHECK;
            }
            
            // now the rook-type moves
            index = MAGIC_INDEX(ROOK, sq, ALL_PIECES(board) & rook_masks[sq]);
            sliding_moves = magic_moves_rook[sq][index];
            
            if (sliding_moves & ENEMY_QUEENS(board)) {
                *checker_loc = LS1B(sliding_moves & ENEMY_QUEENS(board));
                *checker_type = QUEEN;
                *check_pattern = ROOK;
                return SINGLE_CHECK;
            }
            
            
            break;
            
        default:
            break;
    }
    
    
    // here we detect discovered checks
    switch (last_moved_piece) {
        case PAWN:
        case KING:
        case KNIGHT:
        case ROOK:
            // here is where we determine if a king, pawn, knight, or rook move resulted in a "bishop-type" discovered check
            
            index = MAGIC_INDEX(BISHOP, sq, ALL_PIECES(board) & bishop_masks[sq]);
            sliding_moves = magic_moves_bishop[sq][index];
            
            if (sliding_moves & (ENEMY_BISHOPS(board) | ENEMY_QUEENS(board))) {
                *checker_loc = LS1B(sliding_moves & (ENEMY_BISHOPS(board) | ENEMY_QUEENS(board)));
                *checker_type = sliding_moves & ENEMY_BISHOPS(board) ? BISHOP : QUEEN;
                *check_pattern = BISHOP;
                // if a previous check has already been detected, it is now a double check.
                return is_check ? DOUBLE_CHECK : SINGLE_CHECK;
            }
            
            // a rook move can't result in a rook-type discovered attack, so we can break here
            if (last_moved_piece == ROOK)
                break;
            
        case BISHOP:
            // here is where we determine if a king, pawn, knight, or rook move resulted in a "rook-type" discovered check
            
            index = MAGIC_INDEX(ROOK, sq, ALL_PIECES(board) & rook_masks[sq]);
            sliding_moves = magic_moves_rook[sq][index];
            
            if (sliding_moves & (ENEMY_ROOKS(board) | ENEMY_QUEENS(board))) {
                *checker_loc = LS1B(sliding_moves & (ENEMY_ROOKS(board) | ENEMY_QUEENS(board)));
                *checker_type = sliding_moves & ENEMY_ROOKS(board) ? ROOK : QUEEN;
                *check_pattern = ROOK;
                // if a previous check has already been detected, it is now a double check.
                return is_check ? DOUBLE_CHECK : SINGLE_CHECK;
            }
            
            break;
            
        default:
            break;
            
    }
    
    
    return is_check ? SINGLE_CHECK : NOT_CHECK;
}


/* This function does the same thing as is_check(), but it calculates everything from scratch.
 This is (slightly) slower, but is needed in case there is not enough information about the last move that was made.
 For example, when Spartan is given an fen string to parse, or when a pawn has just been captured en passant */

int is_check_full(Board_t *board, int *checker_loc, int *checker_type, int *check_pattern) {
    int sq = LS1B(FRIENDLY_KING(board));
    int is_check = 0;
    int index;
    BB bishop_attacks, rook_attacks;
    
    index = MAGIC_INDEX(BISHOP, sq, ALL_PIECES(board) & bishop_masks[sq]);
    bishop_attacks = magic_moves_bishop[sq][index];
    
    index = MAGIC_INDEX(ROOK, sq, ALL_PIECES(board) & rook_masks[sq]);
    rook_attacks = magic_moves_rook[sq][index];
    
    
    if (ENEMY_PAWN_ATTACKS_TO(sq, board) & ENEMY_PAWNS(board)) {
        *checker_loc = LS1B(ENEMY_PAWN_ATTACKS_TO(sq, board) & ENEMY_PAWNS(board));
        *checker_type = PAWN;
        return SINGLE_CHECK;
    }
    
    if ((rook_attacks | bishop_attacks) & ENEMY_QUEENS(board)) {
        *checker_loc = LS1B((rook_attacks | bishop_attacks) & ENEMY_QUEENS(board));
        *checker_type = QUEEN;
        *check_pattern = rook_attacks & ENEMY_QUEENS(board) ? ROOK : BISHOP;
        is_check = 1;
    }
    
    if (Knight_Attacks[sq] & ENEMY_KNIGHTS(board)) {
        *checker_loc = LS1B(Knight_Attacks[sq] & ENEMY_KNIGHTS(board));
        *checker_type = KNIGHT;
        if (is_check)
            return DOUBLE_CHECK;
        else
            is_check = 1;
    }
    
    if (bishop_attacks & ENEMY_BISHOPS(board)) {
        *checker_loc = LS1B(bishop_attacks & ENEMY_BISHOPS(board));
        *checker_type = BISHOP;
        if (is_check)
            return DOUBLE_CHECK;
        else
            is_check = 1;
    }
    
    if (rook_attacks & ENEMY_ROOKS(board)) {
        *checker_loc = LS1B(rook_attacks & ENEMY_ROOKS(board));
        *checker_type = ROOK;
        return is_check ? DOUBLE_CHECK : SINGLE_CHECK;
    }
    
    return is_check ? SINGLE_CHECK : NOT_CHECK;
}

// checks to see if a piece on "from" can move to "to" without leaving its king in check.
// it may not be able to move if it is pinned
int piece_can_move(Board_t *board, int from, int to) {
    
    BB attackers;
    int index;
    int king_loc = LS1B(FRIENDLY_KING(board));
    
    
    index = MAGIC_INDEX(BISHOP, king_loc, (ALL_PIECES(board) ^ SQ_MASK(from) | SQ_MASK(to)) & bishop_masks[king_loc]);
    attackers = magic_moves_bishop[king_loc][index] & (ENEMY_BISHOPS(board) | ENEMY_QUEENS(board));
    
    if (attackers && (attackers ^ SQ_MASK(to)))
        return 0;
    
    index = MAGIC_INDEX(ROOK, king_loc, (ALL_PIECES(board) ^ SQ_MASK(from) | SQ_MASK(to)) & rook_masks[king_loc]);
    attackers = magic_moves_rook[king_loc][index] & (ENEMY_ROOKS(board) | ENEMY_QUEENS(board));
    
    if (attackers && (attackers ^ SQ_MASK(to)))
        return 0;
    
    return 1;
}

int get_least_valuable_attacker(Board_t *board, int attacked_sq, int *attacker_loc) {
    BB bb;
    int index, temp;
    int turn = TURN(board);
    
    // pawn attacks
    if (turn == WHITE) {
        bb = White_Pawn_Attacks_To[attacked_sq] & WHITE_PAWNS(board);
        if (bb) {
            *attacker_loc = LS1B(bb);
            return PAWN;
        }
    } else {
        bb = Black_Pawn_Attacks_To[attacked_sq] & BLACK_PAWNS(board);
        if (bb) {
            *attacker_loc = LS1B(bb);
            return PAWN;
        }
    }
    
    // knight attacks
    bb = Knight_Attacks[attacked_sq] & (turn == WHITE ? WHITE_KNIGHTS(board) : BLACK_KNIGHTS(board));
    if (bb) {
        *attacker_loc = LS1B(bb);
        return KNIGHT;
    }
    
    // bishop attacks
    temp = index = MAGIC_INDEX(BISHOP, attacked_sq, ALL_PIECES(board) & bishop_masks[attacked_sq]);
    bb = magic_moves_bishop[attacked_sq][index] & (turn == WHITE ? WHITE_BISHOPS(board) : BLACK_BISHOPS(board));
    if (bb) {
        *attacker_loc = LS1B(bb);
        return BISHOP;
    }
    
    // rook attacks
    index = MAGIC_INDEX(ROOK, attacked_sq, ALL_PIECES(board) & rook_masks[attacked_sq]);
    bb = magic_moves_rook[attacked_sq][index] & (turn == WHITE ? WHITE_ROOKS(board) : BLACK_ROOKS(board));
    if (bb) {
        *attacker_loc = LS1B(bb);
        return ROOK;
    }
    
    // queen attacks
    
    // rook-type queen moves. we can reuse the index calculated for the rook moves
    bb = magic_moves_rook[attacked_sq][index] & (turn == WHITE ? WHITE_QUEENS(board) : BLACK_QUEENS(board));
    
    if (bb) {
        *attacker_loc = LS1B(bb);
        return QUEEN;
    }
    
    // bishop-type queen moves
    index = temp; // reusing the index we calculated for the bishop moves
    bb = magic_moves_bishop[attacked_sq][index] & (turn == WHITE ? WHITE_QUEENS(board) : BLACK_QUEENS(board));
    
    if (bb) {
        *attacker_loc = LS1B(bb);
        return QUEEN;
    }
    
    bb = King_Attacks[attacked_sq] & (turn == WHITE ? WHITE_KING(board) : BLACK_KING(board));
    if (bb) {
        *attacker_loc = LS1B(bb);
        return KING;
    }
    
    return 0;
}