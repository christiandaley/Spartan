//
//  history.h
//  Spartan
//
//
//
//

#ifndef history_h
#define history_h

#include "move.h"

void clear_history_table();
void adjust_history_table(Move_t move, int depth, int color);
int get_history_score(Move_t move, int color);

#endif /* history_h */
