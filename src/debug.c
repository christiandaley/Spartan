//
//  debug.c
//  Spartan
//
//
//
//

#include "debug.h"
#include "board.h"
#include "log.h"
#include "move.h"
#include "movegen.h"
#include "ttable.h"

static const char *flag_names[8] = {"Normal", "Castle Kingside", "Castle Queenside", "Promote Knight", "Promote Bishop", "Promote Rook", "Promote Queen", "En Passant"};
static const char *piece_names[7] = {"null piece", "Pawn", "Rook", "Knight", "Bishop", "Queen", "King"};

static const char *square_names[64] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};

void print_board(Board_t *board) {
    int checker_count, checker_loc, checker_type, check_pattern;
    checker_count = is_check(board, &checker_loc, &checker_type, &check_pattern);
    
    engine_output("Castling Rights (KQkq): %d, %d, %d, %d\n", WHITE_CAN_CASTLE_KINGSIDE(board) != 0, WHITE_CAN_CASTLE_QUEENSIDE(board) != 0, BLACK_CAN_CASTLE_KINGSIDE(board) != 0, BLACK_CAN_CASTLE_QUEENSIDE(board) != 0);
    engine_output("E.P square: %s\n", EP_SQUARE(board) ? square_names[EP_SQUARE(board)] : "None");
    engine_output("Turn: %s\n", TURN(board) == WHITE ? "white" : "black");
    engine_output("Check: %s\n", checker_count ? "YES" : "NO");
    
    if (checker_count == 1)
        engine_output("Checking Piece: %s, square: %s\n", piece_names[checker_type], square_names[checker_loc]);
    else if (checker_count == 2)
        engine_output("Double check\n");
    
    engine_output("Moves since capture/pawn: %d\n", board->movesSinceCaptureORPawn);
    engine_output("Total moves: %d\n", board->fullmoves);
    engine_output("Last move made: ");
    print_move(board->last_move_made);
    engine_output("\nHash: %016llx\n", board->hash);
    engine_output("Stack index: %d\n", global_board_stack()->ply);
    
    char fen[200];
    char *square;
    int i, j, loc;
    int fen_loc = 0;
    int empty_count = 0;
    
    for (i = 7; i >= 0; i--) {
        for (j = 0; j < 8; j++) {
            square = "  ";
            loc = i * 8 + j;
            if (PIECE_ON_SQUARE(loc, board) && empty_count) {
                fen[fen_loc++] = empty_count + '0';
                empty_count = 0;
            }
            
            if (WHITE_PAWN_ON_SQUARE(loc, board)) {
                fen[fen_loc] = 'P';
                square = "WP";
            } else if (WHITE_ROOK_ON_SQUARE(loc, board)) {
                fen[fen_loc] = 'R';
                square = "WR";
            } else if (WHITE_KNIGHT_ON_SQUARE(loc, board)) {
                fen[fen_loc] = 'N';
                square = "WN";
            } else if (WHITE_BISHOP_ON_SQUARE(loc, board)) {
                fen[fen_loc] = 'B';
                square = "WB";
            } else if (WHITE_QUEEN_ON_SQUARE(loc, board)) {
                fen[fen_loc] = 'Q';
                square = "WQ";
            } else if (WHITE_KING_ON_SQUARE(loc, board)) {
                fen[fen_loc] = 'K';
                square = "WK";
            } else if (BLACK_PAWN_ON_SQUARE(loc, board)) {
                fen[fen_loc] = 'p';
                square = "BP";
            } else if (BLACK_ROOK_ON_SQUARE(loc, board)) {
                fen[fen_loc] = 'r';
                square = "BR";
            } else if (BLACK_KNIGHT_ON_SQUARE(loc, board)) {
                fen[fen_loc] = 'n';
                square = "BN";
            } else if (BLACK_BISHOP_ON_SQUARE(loc, board)) {
                fen[fen_loc] = 'b';
                square = "BB";
            } else if (BLACK_QUEEN_ON_SQUARE(loc, board)) {
                fen[fen_loc] = 'q';
                square = "BQ";
            } else if (BLACK_KING_ON_SQUARE(loc, board)) {
                fen[fen_loc] = 'k';
                square = "BK";
            } else {
                empty_count++;
                fen_loc--;
            }
            
            fen_loc++;
            engine_output("|%s", square);
        }
        
        if (empty_count) {
            fen[fen_loc++] = '0' + empty_count;
            empty_count = 0;
        }
        
        if (i)
            fen[fen_loc++] = '/';
        
        engine_output("|\n");
        engine_output("------------------------\n");
    }
    
    fen[fen_loc] = '\0';
    
    // determine the castling rights
    char castling[5] = {'-', '\0', '\0', '\0', '\0'};
    int temp = 0;
    if (WHITE_CAN_CASTLE_KINGSIDE(board)) {
        castling[temp] = 'K';
        temp++;
    }
    
    if (WHITE_CAN_CASTLE_QUEENSIDE(board)) {
        castling[temp] = 'Q';
        temp++;
    }
    
    if (BLACK_CAN_CASTLE_KINGSIDE(board)) {
        castling[temp] = 'k';
        temp++;
    }
    
    if (BLACK_CAN_CASTLE_QUEENSIDE(board)) {
        castling[temp] = 'q';
        temp++;
    }
    
    // get the en passant square
    char ep_sq[3] = {'-', '\0', '\0'};
    if (EP_SQUARE(board)) {
        ep_sq[0] = FILE(EP_SQUARE(board)) + 'a';
        ep_sq[1] = RANK(EP_SQUARE(board)) + '1';
    }
    
    // print the full fen string
    engine_output("%s %c %s %s %d %d\n", fen, TURN(board) == WHITE ? 'w' : 'b', castling, ep_sq, board->movesSinceCaptureORPawn, board->fullmoves);
}

void print_global_board() {
    print_board(global_board());
}

void print_move_with_detail(Move_t move) {
    engine_output("Move Info\n");
    engine_output("From: %s To: %s\n", square_names[MOVE_FROM(move)], square_names[MOVE_TO(move)]);
    engine_output("Piece: %s\n", piece_names[MOVE_PIECE(move)]);
    engine_output("Flags: %s\n", flag_names[MOVE_FLAGS(move)]);
    engine_output("Capture: %s\n\n", MOVE_IS_CAPTURE(move) ? "YES" : "NO");
}

void print_move(Move_t move) {
    if (move == 0) {
        engine_output("null move");
        return;
    }
    
    char string[6] = {};
    move_to_string(move, string);
    engine_output("%s ", string);
    
}

void move_to_string(Move_t move, char string[6]) {
    char promote = '\0';
    if (MOVE_IS_PROMOTE_KNIGHT(move))
        promote = 'n';
    else if (MOVE_IS_PROMOTE_BISHOP(move))
        promote = 'b';
    else if (MOVE_IS_PROMOTE_ROOK(move))
        promote = 'r';
    else if (MOVE_IS_PROMOTE_QUEEN(move))
        promote = 'q';
    
    string[0] = FILE(MOVE_FROM(move)) + 'a';
    string[1] = RANK(MOVE_FROM(move)) + '1';
    string[2] = FILE(MOVE_TO(move)) + 'a';
    string[3] = RANK(MOVE_TO(move)) + '1';
    string[4] = promote;
    string[5] = '\0';
}

int bitboards_are_ok(Board_t *board) {
    if (BLACK_PIECES(board) != (BLACK_PAWNS(board) | BLACK_ROOKS(board) | BLACK_KNIGHTS(board) | BLACK_BISHOPS(board) | BLACK_QUEENS(board) | BLACK_KING(board)))
        return 0;
    
    if (WHITE_PIECES(board) != (WHITE_PAWNS(board) | WHITE_ROOKS(board) | WHITE_KNIGHTS(board) | WHITE_BISHOPS(board) | WHITE_QUEENS(board) | WHITE_KING(board)))
        return 0;
    
    if (ALL_PIECES(board) != (BLACK_PIECES(board) | WHITE_PIECES(board)))
        return 0;
    
    int i;
    for (i = 0; i < 64; i++) {
        if (WHITE_PIECE_ON_SQUARE(i, board) && BLACK_PIECE_ON_SQUARE(i, board))
            return 0;
    }
    
    return 1;
}

uint64_t perft(int depth) {
    uint64_t n = 0;
    int flags;
    int mCount, i;
    Move_t *moves;
    generate_global_legal_moves();
    moves = global_move_list();
    mCount = global_move_count();
    
    if (depth == 1)
        return mCount;
    
    n = probe_tt(global_board()->hash, &flags, NULL);
    if (flags != 0 && TT_FLAG_DEPTH(flags) == depth)
        return n;
    
    n = 0;
    
    for (i = 0; i < mCount; i++) {
        make_move_on_global_board(moves[i]);
        n += perft(depth - 1);
        UNDO_MOVE_ON_GLOBAL_BOARD
    }
    
    tt_store_entry(global_board()->hash, MAKE_TT_FLAGS(EXACT_SCORE, 0, depth), n, NULL_MOVE);
    
    return n;
}

void print_bitboard(BB bitboard) {
    engine_output("\n");
    for (int b = 7; b >= 0;b--) {
        for (int a = 0;a < 8;a++) {
            engine_output("%d ", (bitboard & SQ_MASK(b * 8 + a)) != 0);
        }
        
        engine_output("\n");
    }
    
    engine_output("\n");
    
}