//
//  killer.h
//  Spartan
//
//
//
//

#ifndef killer_h
#define killer_h

#include "move.h"

#define KILLER_LIST_SIZE 3

void add_killer(Move_t killer, int depth);
void clear_killers();
Move_t *get_killers(int *n_killers, int depth);


#endif /* killer_h */
