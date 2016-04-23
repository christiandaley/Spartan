//
//  eval.h
//  Spartan
//
//
//
//

#ifndef eval_h
#define eval_h

#include "board.h"

#define VALUE_INFINITE 1000000000
#define VALUE_DRAW 0
#define CONTEMPT_FACTOR -50
#define MATE_IN(n) (VALUE_INFINITE - (n))
#define IS_WINNING_CHECKMATE(v) (abs((v) - VALUE_INFINITE) <= MAX_DEPTH)
#define IS_LOSING_CHECKMATE(v) (abs((v) + VALUE_INFINITE) <= MAX_DEPTH)

#define MID_GAME 0
#define END_GAME 1

#define MAX_MATERIAL 78
#define MATERIAL_OFFSET 12 // when the total material drops to 12 points or lower, it is a full endgame

#define PAWN_TABLE_MB 16
#define PAWN_TABLE_SIZE (((PAWN_TABLE_MB * 1000000) / 16 / 2) + 1)
#define PAWN_TABLE_MISS 0
#define PAWN_TABLE_HIT 1

#define DIST(sq1, sq2) ((int)sqrt(((FILE(sq1) - FILE(sq2)) * (FILE(sq1) - FILE(sq2))) + ((RANK(sq1) - RANK(sq2)) * (RANK(sq1) - RANK(sq2)))))

extern const int Piece_Weights[7];

typedef struct  {
    BB positions[2][PAWN_TABLE_SIZE];
    int mg_scores[2][PAWN_TABLE_SIZE];
    int eg_scores[2][PAWN_TABLE_SIZE];
} Pawn_Table_t;

int eval_board(Board_t *board);
int eval_global_board();

int eval_pieces(Board_t *board, BB pieces[2][7]);
void eval_mobility(Board_t *board, BB pieces[2][7], BB attacks[2][7]);
void eval_pawns(Board_t *board);
void eval_passed_pawn(Board_t *board, int sq, int color);
void eval_kings(Board_t *board, BB attacks[2][7]);
int eval_pawn_shelter(Board_t *board, int sq, int color);

int count_material(Board_t *board);

int is_draw(Board_t *board);
int global_board_is_draw();

int probe_pawn_table(BB pawns, int color, int *mg_score, int *eg_score);
void pawn_table_store_entry(BB pawns, int color, int mg_score, int eg_score);
void init_pawn_table();

#endif /* eval_h */

