//
//  board.h
//  Spartan
//
//
//
//

#ifndef board_h
#define board_h

typedef unsigned long long BB;
typedef unsigned long long Hash_t;
typedef unsigned int Move_t;

#define WHITE 0
#define BLACK 1

#define EMPTY 0
#define PAWN 1
#define ROOK 2
#define KNIGHT 3
#define BISHOP 4
#define QUEEN 5
#define KING 6

#define RANK(sq) (((sq) >> 3))
#define FILE(sq) (((sq) & 0x7))
#define SQ_MASK(sq) ((BB)1 << (sq))

#define WHITE_PAWNS(b)   (b->BitBoards[0])
#define WHITE_ROOKS(b)   (b->BitBoards[1])
#define WHITE_KNIGHTS(b) (b->BitBoards[2])
#define WHITE_BISHOPS(b) (b->BitBoards[3])
#define WHITE_QUEENS(b)  (b->BitBoards[4])
#define WHITE_KING(b)    (b->BitBoards[5])

#define BLACK_PAWNS(b)   (b->BitBoards[6])
#define BLACK_ROOKS(b)   (b->BitBoards[7])
#define BLACK_KNIGHTS(b) (b->BitBoards[8])
#define BLACK_BISHOPS(b) (b->BitBoards[9])
#define BLACK_QUEENS(b)  (b->BitBoards[10])
#define BLACK_KING(b)    (b->BitBoards[11])

#define WHITE_PIECES(b)   (b->BitBoards[12])
#define BLACK_PIECES(b)   (b->BitBoards[13])
#define ALL_PIECES(b)     (b->BitBoards[14])

#define PIECES_OF_TYPE(b, t, c) (b->BitBoards[(t) - 1 + ((c) ? 6 : 0)])

// returns bitboards that depend on the turn
#define FRIENDLY_PAWNS(b)   (b->BitBoards[TURN(b) ? 6 : 0])
#define FRIENDLY_ROOKS(b)   (b->BitBoards[TURN(b) ? 7 : 1])
#define FRIENDLY_KNIGHTS(b) (b->BitBoards[TURN(b) ? 8 : 2])
#define FRIENDLY_BISHOPS(b) (b->BitBoards[TURN(b) ? 9 : 3])
#define FRIENDLY_QUEENS(b)  (b->BitBoards[TURN(b) ? 10 : 4])
#define FRIENDLY_KING(b)    (b->BitBoards[TURN(b) ? 11 : 5])
#define FRIENDLY_PIECES(b)  (b->BitBoards[TURN(b) ? 13 : 12])

#define ENEMY_PAWNS(b)   (b->BitBoards[TURN(b) ? 0 : 6])
#define ENEMY_ROOKS(b)   (b->BitBoards[TURN(b) ? 1 : 7])
#define ENEMY_KNIGHTS(b) (b->BitBoards[TURN(b) ? 2 : 8])
#define ENEMY_BISHOPS(b) (b->BitBoards[TURN(b) ? 3 : 9])
#define ENEMY_QUEENS(b)  (b->BitBoards[TURN(b) ? 4 : 10])
#define ENEMY_KING(b)    (b->BitBoards[TURN(b) ? 5 : 11])
#define ENEMY_PIECES(b)  (b->BitBoards[TURN(b) ? 12 : 13])

#define WHITE_PAWN_ON_SQUARE(sq, b)      ((WHITE_PAWNS(b) >> (sq)) & 1)
#define BLACK_PAWN_ON_SQUARE(sq, b)      ((BLACK_PAWNS(b) >> (sq)) & 1)
#define WHITE_ROOK_ON_SQUARE(sq, b)      ((WHITE_ROOKS(b) >> (sq)) & 1)
#define BLACK_ROOK_ON_SQUARE(sq, b)      ((BLACK_ROOKS(b) >> (sq)) & 1)
#define WHITE_KNIGHT_ON_SQUARE(sq, b)    ((WHITE_KNIGHTS(b) >> (sq)) & 1)
#define BLACK_KNIGHT_ON_SQUARE(sq, b)    ((BLACK_KNIGHTS(b) >> (sq)) & 1)
#define WHITE_BISHOP_ON_SQUARE(sq, b)    ((WHITE_BISHOPS(b) >> (sq)) & 1)
#define BLACK_BISHOP_ON_SQUARE(sq, b)    ((BLACK_BISHOPS(b) >> (sq)) & 1)
#define WHITE_QUEEN_ON_SQUARE(sq, b)     ((WHITE_QUEENS(b) >> (sq)) & 1)
#define BLACK_QUEEN_ON_SQUARE(sq, b)     ((BLACK_QUEENS(b) >> (sq)) & 1)
#define WHITE_KING_ON_SQUARE(sq, b)      ((WHITE_KING(b) >> (sq)) & 1)
#define BLACK_KING_ON_SQUARE(sq, b)      ((BLACK_KING(b) >> (sq)) & 1)
#define WHITE_PIECE_ON_SQUARE(sq, b)     ((WHITE_PIECES(b) >> (sq)) & 1)
#define BLACK_PIECE_ON_SQUARE(sq, b)     ((BLACK_PIECES(b) >> (sq)) & 1)
#define PIECE_ON_SQUARE(sq, b)           ((ALL_PIECES(b) >> (sq)) & 1)
#define FRIENDLY_PIECE_ON_SQUARE(sq, b)  (TURN(b) == WHITE ? WHITE_PIECE_ON_SQUARE(sq, b) : BLACK_PIECE_ON_SQUARE(sq, b))
#define ENEMY_PIECE_ON_SQUARE(sq, b)     (TURN(b) == WHITE ? BLACK_PIECE_ON_SQUARE(sq, b) : WHITE_PIECE_ON_SQUARE(sq, b))

#define PAWN_ON_SQUARE(sq, b)   (WHITE_PAWN_ON_SQUARE(sq, b) || BLACK_PAWN_ON_SQUARE(sq, b))
#define ROOK_ON_SQUARE(sq, b)   (WHITE_ROOK_ON_SQUARE(sq, b) || BLACK_ROOK_ON_SQUARE(sq, b))
#define KNIGHT_ON_SQUARE(sq, b) (WHITE_KNIGHT_ON_SQUARE(sq, b) || BLACK_KNIGHT_ON_SQUARE(sq, b))
#define BISHOP_ON_SQUARE(sq, b) (WHITE_BISHOP_ON_SQUARE(sq, b) || BLACK_BISHOP_ON_SQUARE(sq, b))
#define QUEEN_ON_SQUARE(sq, b)  (WHITE_QUEEN_ON_SQUARE(sq, b) || BLACK_QUEEN_ON_SQUARE(sq, b))
#define KING_ON_SQUARE(sq, b)   (WHITE_KING_ON_SQUARE(sq, b) || BLACK_KING_ON_SQUARE(sq, b))

#define WHITE_MAJOR_AND_MINOR_PIECES(b) (WHITE_KNIGHTS(b) | WHITE_BISHOPS(b) | WHITE_ROOKS(b) | WHITE_QUEENS(b))
#define BLACK_MAJOR_AND_MINOR_PIECES(b) (BLACK_KNIGHTS(b) | BLACK_BISHOPS(b) | BLACK_ROOKS(b) | BLACK_QUEENS(b))
#define FRIENDLY_MAJOR_AND_MINOR_PIECES(b) (TURN(b) == WHITE ? WHITE_MAJOR_AND_MINOR_PIECES(b) : BLACK_MAJOR_AND_MINOR_PIECES(b))
#define ENEMY_MAJOR_AND_MINOR_PIECES(b) (TURN(b) == WHITE ? BLACK_MAJOR_AND_MINOR_PIECES(b) : WHITE_MAJOR_AND_MINOR_PIECES(b))

#define REMOVE_WHITE_CASTLE_KINGSIDE(b) (b->flags &= 0x5ff)
#define REMOVE_WHITE_CASTLE_QUEENSIDE(b) (b->flags &= 0x6ff)
#define REMOVE_BLACK_CASTLE_KINGSIDE(b) (b->flags &= 0x77f)
#define REMOVE_BLACK_CASTLE_QUEENSIDE(b) (b->flags &= 0x7bf)

#define WHITE_CAN_CASTLE_KINGSIDE(b)   (b->flags & 0x200)
#define WHITE_CAN_CASTLE_QUEENSIDE(b)  (b->flags & 0x100)
#define BLACK_CAN_CASTLE_KINGSIDE(b)   (b->flags & 0x080)
#define BLACK_CAN_CASTLE_QUEENSIDE(b)  (b->flags & 0x040)

#define CASTLING_FLAGS(b) ((b->flags >> 6) & 0xf)

#define FRIENDLY_CAN_CASTLE_KINGSIDE(b)  (TURN(b) == WHITE ? WHITE_CAN_CASTLE_KINGSIDE(b) : BLACK_CAN_CASTLE_KINGSIDE(b))
#define FRIENDLY_CAN_CASTLE_QUEENSIDE(b) (TURN(b) == WHITE ? WHITE_CAN_CASTLE_QUEENSIDE(b) : BLACK_CAN_CASTLE_QUEENSIDE(b))

#define EP_SQUARE(b) (b->flags & 0x3f)
#define TURN(b) ((b->flags >> 10) & 1)

#define SET_TURN(b, t) (b->flags = (b->flags & 0x3ff) ^ ((t) << 10))
#define CHANGE_TURN(b) ((b->flags) ^= 0x400)
#define SET_EP_SQUARE(b, sq) (b->flags = (b->flags & 0x7c0) ^ (sq))
#define SET_CASTLING_RIGHTS(b, KQkq) (b->flags = (b->flags & 0x43f) ^ (((KQkq) & 0xf) << 6))

// parses a string that represents a square into a number. a1 becomes 0, h8 is 63, etc.
#define STRING_TO_SQUARE(c) ((c)[0] - 'a' + ((c)[1] - '1') * 8)

#define IS_LEGAL_SQUARE(rank, file) (((rank) >= 0) && ((rank) <= 7) && ((file) >= 0) && ((file) <= 7))

extern const int A1, A2, A3, A4, A5, A6, A7, A8;
extern const int B1, B2, B3, B4, B5, B6, B7, B8;
extern const int C1, C2, C3, C4, C5, C6, C7, C8;
extern const int D1, D2, D3, D4, D5, D6, D7, D8;
extern const int E1, E2, E3, E4, E5, E6, E7, E8;
extern const int F1, F2, F3, F4, F5, F6, F7, F8;
extern const int G1, G2, G3, G4, G5, G6, G7, G8;
extern const int H1, H2, H3, H4, H5, H6, H7, H8;

extern const char *Start_Fen;

typedef struct {
    BB BitBoards[15];
    int movesSinceCaptureORPawn; // # moves since a capture or pawn push
    
    // bits 0-5 are the en passant square.
    // bits 6-9 are the castling rights
    // bit 10 is the turn, where 0 = white and 1 = black
    unsigned int flags;
    int fullmoves, is_check;

    Hash_t hash; // the hash of the board state
    Move_t last_move_made; // the last move that was made
} Board_t;

typedef struct {
    Board_t stack[400];
    int ply;
} BoardStack_t;

void set_square_value(Board_t *board, int sq, int piece, int color);
void set_global_square_value(int sq, int piece, int color);

void update_bitboards(Board_t *board);
void update_global_bitboards();

// returns the board representing the current state of the game
Board_t *global_board();

// returns the set of all boards that are used to quickly make/unmake moves
BoardStack_t *global_board_stack();

void reset_global_ply();
void copy_and_inc_stack();

#endif /* board_h */
