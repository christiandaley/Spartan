//
//  sort.h
//  Spartan
//
//
//
//

#ifndef sort_h
#define sort_h

#include "move.h"
#include "movegen.h"
#include "board.h"

#define NO_ORDER_FLAG 0
#define HISTORY_MOVE 1
#define KILLER_MOVE 2
#define TT_MOVE 3

typedef struct {
    int scores[MAX_DEPTH][MAX_NUM_MOVES];
    int flags[MAX_DEPTH][MAX_NUM_MOVES];
} Sort_Data_t;

extern Sort_Data_t sort_data;

int see(Move_t move, Board_t *board);
int mvvlva(Move_t move, Board_t *board);

void sort_qsearch_moves(int *scores);
void sort_global_moves(Move_t tt_move, int tt_score, int depth);
void sort_root_moves(int *scores);

#endif /* sort_h */
