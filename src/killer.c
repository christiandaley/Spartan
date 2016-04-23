//
//  killer.c
//  Spartan
//
//
//
//

#include "killer.h"
#include "movegen.h"

Move_t killers[MAX_DEPTH][KILLER_LIST_SIZE];

void add_killer(Move_t killer, int depth) {
    int index = 0;
    while (killers[depth][index] != NULL_MOVE && index < KILLER_LIST_SIZE) {
        index++;
        if (killers[depth][index] == killer)
            return;
    }
    
    if (index == KILLER_LIST_SIZE)
        index = 0;
    
    killers[depth][index] = killer;
}

void clear_killers() {
    int i, j;
    for (i = 0; i < MAX_DEPTH; i++) {
        for (j = 0; j < KILLER_LIST_SIZE; j++)
            killers[i][j] = NULL_MOVE;
    }
}

Move_t *get_killers(int *n_killers, int depth) {
    *n_killers = 0;
    while (killers[depth][*n_killers] != NULL_MOVE && (*n_killers) < KILLER_LIST_SIZE)
        (*n_killers)++;
    
    
    return killers[depth];
}