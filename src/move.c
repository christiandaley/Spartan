//
//  move.c
//  Spartan
//
//
//
//
#include <assert.h>
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "debug.h"
#include "bitscan.h"
#include "log.h"
#include "ttable.h"

void make_move_on_global_board(Move_t move) {
    
    Board_t *board;
    int from, to, ep_sq;
    int captured_piece_type = 0;
    int castling = CASTLING_FLAGS(global_board());
    int turn = TURN(global_board());
    ep_sq = EP_SQUARE(global_board());
    from = MOVE_FROM(move);
    to = MOVE_TO(move);
    
    copy_and_inc_stack();
    inc_move_stack_depth();
    board = global_board();
    
    board->movesSinceCaptureORPawn++;
    board->last_move_made = move;
    board->is_check = 0;
    SET_EP_SQUARE(board, 0);
    
    if (MOVE_IS_CAPTURE(move)) {
        board->movesSinceCaptureORPawn = 0;
        if (PAWN_ON_SQUARE(to, board)) {
            ENEMY_PAWNS(board) ^= SQ_MASK(to);
            captured_piece_type = PAWN;
        } else if (KNIGHT_ON_SQUARE(to, board)) {
            ENEMY_KNIGHTS(board) ^= SQ_MASK(to);
            captured_piece_type = KNIGHT;
        } else if (BISHOP_ON_SQUARE(to, board)) {
            ENEMY_BISHOPS(board) ^= SQ_MASK(to);
            captured_piece_type = BISHOP;
        } else if (ROOK_ON_SQUARE(to, board)) {
            ENEMY_ROOKS(board) ^= SQ_MASK(to);
            captured_piece_type = ROOK;
            switch(to) {
                case 0: // A1
                    REMOVE_WHITE_CASTLE_QUEENSIDE(board);
                    break;
                case 7: // H1
                    REMOVE_WHITE_CASTLE_KINGSIDE(board);
                    break;
                case 56: // A8
                    REMOVE_BLACK_CASTLE_QUEENSIDE(board);
                    break;
                case 63: // H8
                    REMOVE_BLACK_CASTLE_KINGSIDE(board);
                    break;
                    
                default:
                    break;
            }
            
        } else if (QUEEN_ON_SQUARE(to, board)) {
            ENEMY_QUEENS(board) ^= SQ_MASK(to);
            captured_piece_type = QUEEN;
        }
        
        ENEMY_PIECES(board) ^= SQ_MASK(to);
    }
    
    FRIENDLY_PIECES(board) ^= SQ_MASK(to) ^ SQ_MASK(from);
    
    switch (MOVE_PIECE(move)) {
        case PAWN:
            board->movesSinceCaptureORPawn = 0;
            FRIENDLY_PAWNS(board) ^= SQ_MASK(from);
            
            switch (MOVE_FLAGS(move)) {
                    
                case FLAG_PROMOTE_KNIGHT:
                    FRIENDLY_KNIGHTS(board) ^= SQ_MASK(to);
                    break;
                    
                case FLAG_PROMOTE_BISHOP:
                    FRIENDLY_BISHOPS(board) ^= SQ_MASK(to);
                    break;
                    
                case FLAG_PROMOTE_ROOK:
                    FRIENDLY_ROOKS(board) ^= SQ_MASK(to);
                    break;
                    
                case FLAG_PROMOTE_QUEEN:
                    FRIENDLY_QUEENS(board) ^= SQ_MASK(to);
                    break;
                    
                default:
                    FRIENDLY_PAWNS(board) ^= SQ_MASK(to);
                    break;
            }
            
            
            if (to - from == 16)
                SET_EP_SQUARE(board, from + 8);
            else if (from - to == 16)
                SET_EP_SQUARE(board, to + 8);
            
            break;
        case ROOK:
            FRIENDLY_ROOKS(board) ^= SQ_MASK(to) ^ SQ_MASK(from);
            switch (from) {
                case 0: // A1
                    REMOVE_WHITE_CASTLE_QUEENSIDE(board);
                    break;
                case 7: // H1
                    REMOVE_WHITE_CASTLE_KINGSIDE(board);
                    break;
                case 56: // A8
                    REMOVE_BLACK_CASTLE_QUEENSIDE(board);
                    break;
                case 63: // H8
                    REMOVE_BLACK_CASTLE_KINGSIDE(board);
                    break;
                default:
                    break;
            }
            
            
            break;
            
        case KNIGHT:
            FRIENDLY_KNIGHTS(board) ^= SQ_MASK(to) ^ SQ_MASK(from);
            
            break;
            
        case BISHOP:
            FRIENDLY_BISHOPS(board) ^= SQ_MASK(to) ^ SQ_MASK(from);
            
            break;
            
        case QUEEN:
            FRIENDLY_QUEENS(board) ^= SQ_MASK(to) ^ SQ_MASK(from);
            
            break;
            
        case KING:
            FRIENDLY_KING(board) ^= SQ_MASK(to) ^ SQ_MASK(from);
            
            if (TURN(board) == WHITE) {
                REMOVE_WHITE_CASTLE_KINGSIDE(board);
                REMOVE_WHITE_CASTLE_QUEENSIDE(board);
            } else {
                REMOVE_BLACK_CASTLE_KINGSIDE(board);
                REMOVE_BLACK_CASTLE_QUEENSIDE(board);
            }
            
            break;
            
    }
    
    
    switch (MOVE_FLAGS(move)) {
        case FLAG_EP:
            ENEMY_PAWNS(board) ^= SQ_MASK(ep_sq + (TURN(board) == WHITE ? -8 : 8));
            ENEMY_PIECES(board) ^= SQ_MASK(ep_sq + (TURN(board) == WHITE ? -8 : 8));
            break;
        case FLAG_CASTLE_KINGSIDE:
            FRIENDLY_PIECES(board) ^= SQ_MASK(from + 3) ^ SQ_MASK(from + 1);
            FRIENDLY_ROOKS(board) ^= SQ_MASK(from + 3) ^ SQ_MASK(from + 1);
            break;
        case FLAG_CASTLE_QUEENSIDE:
            FRIENDLY_PIECES(board) ^= SQ_MASK(from - 4) ^ SQ_MASK(from - 1);
            FRIENDLY_ROOKS(board) ^= SQ_MASK(from - 4) ^ SQ_MASK(from - 1);
            break;
        default:
            break;
    }
    
    ALL_PIECES(board) = WHITE_PIECES(board) | BLACK_PIECES(board);
    
    if (TURN(board) == BLACK)
        board->fullmoves++;
    
    CHANGE_TURN(board);
    board->hash = hash_move_on_board(board, move, captured_piece_type, castling, ep_sq, turn);
    
#if DO_ASSERTIONS
    assert(bitboards_are_ok(global_board()));
    assert(global_board()->hash == hash_global_board());
#endif
    
    
}

void make_null_move_on_global_board() {
    int ep_sq = EP_SQUARE(global_board());
    copy_and_inc_stack();
    inc_move_stack_depth();
    
    CHANGE_TURN(global_board());
    SET_EP_SQUARE(global_board(), 0);
    
    global_board()->hash = hash_null_move(global_board(), ep_sq);
}

void make_quick_move_on_board(Board_t *board, int from, int to, int captured_piece, int moveing_piece) {
    int turn = TURN(board);
    PIECES_OF_TYPE(board, moveing_piece, turn) ^= SQ_MASK(from) ^ SQ_MASK(to);
    
    if (captured_piece) {
        PIECES_OF_TYPE(board, captured_piece, turn == WHITE ? BLACK : WHITE) ^= SQ_MASK(to);
        ALL_PIECES(board) ^= SQ_MASK(from);
    } else
        ALL_PIECES(board) ^= SQ_MASK(from) ^ SQ_MASK(to);
    
    CHANGE_TURN(board);    
}