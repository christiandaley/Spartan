//
//  movegen.h
//  Spartan
//
//
//
//

#ifndef movegen_h
#define movegen_h

#include "move.h"
#include "board.h"

#define MAX_NUM_MOVES 200
#define MAX_DEPTH 200

typedef struct {
    int depth;
    int counts[MAX_DEPTH];
    Move_t stack[MAX_DEPTH][MAX_NUM_MOVES];
    Move_t pl_stack[MAX_NUM_MOVES]; // used to store pseudo legal moves before they've been tested for legality
    int pl_count; // pseudo legal move count
} Move_Stack_t;

Move_t *global_move_list();
int global_move_count();
void inc_move_stack_depth();
void dec_move_stack_depth();
void reset_global_move_stack_depth();

// called once when the program starts
void init_move_generation();

void generate_global_legal_moves();
void generate_pseudo_legal_moves(Board_t *board);
void generate_king_moves(Board_t *board);
void generate_evasions(Board_t *board, int checker_loc, int checker_type, int check_pattern);
int is_legal(Move_t move, Board_t *board);

// returns the number of enemy checkers, if any. In the case of a single checker, it also returns its location and type
int is_check(Board_t *board, int *checker_loc, int *checker_type, int *check_pattern);
int is_check_full(Board_t *board, int *checker_loc, int *checker_type, int *check_pattern);
int piece_can_move(Board_t *board, int from, int to);

int get_least_valuable_attacker(Board_t *board, int attacked_sq, int *attacker_loc);

#endif /* movegen_h */
