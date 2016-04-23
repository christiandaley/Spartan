//
//  history.c
//  Spartan
//
//
//
//

#include "history.h"
#include "eval.h"

static int History_Table[2][64][64];

void clear_history_table() {
    int i, j;
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            History_Table[0][i][j] = -VALUE_INFINITE;
            History_Table[1][i][j] = -VALUE_INFINITE;
        }
    }
}
void adjust_history_table(Move_t move, int depth, int color) {
    if (History_Table[color][MOVE_FROM(move)][MOVE_TO(move)] == -VALUE_INFINITE)
        History_Table[color][MOVE_FROM(move)][MOVE_TO(move)] = depth * depth;
    else
        History_Table[color][MOVE_FROM(move)][MOVE_TO(move)] += depth * depth;
}

int get_history_score(Move_t move, int color) {
    return History_Table[color][MOVE_FROM(move)][MOVE_TO(move)];
}