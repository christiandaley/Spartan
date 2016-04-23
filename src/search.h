//
//  search.h
//  Spartan
//
//
//
//

#ifndef search_h
#define search_h

#define SEARCH_MODE_INFINITE 0
#define SEARCH_MODE_TIMED 1
#define SEARCH_MODE_NODES 2
#define SEARCH_MODE_DEPTH 3
#define SEARCH_MODE_NONE 4

#include <sys/time.h>
#include "movegen.h"

#define UPDATE_SPEED 1000
#define ASPIRATION_WINDOW 25
#define NULL_MOVE_REDUC 3
#define LMR_THRESHOLD 2
#define D_PRUNE_MARGINE 200
#define PROB_CUT_MARGINE 150
#define PROB_CUT_REDUC 3
#define HISTORY_THRESHOLD 81
#define HISTORY_REDUC 2
static const int Fultility_Margins[5] = {0, 300, 500, 700, 900};
static const int Razoring_Margins[5] = {0, 0, 100, 200, 300};


typedef struct {
    int nMoves;
    Move_t moves[MAX_DEPTH];
} Line_t;

typedef struct {
    int stop;
    int search_mode;
    int search_was_stopped;
    int depth, seldepth, score;
    unsigned int wtime, btime, winc, binc;
    unsigned int max_depth, movetime;
    uint64_t max_nodes;
    struct timeval start_time;
    double elapsed_time;
    
    uint64_t nodes;
    Line_t PV;
    Move_t current_move, best_move;
    int current_move_number;
    int currently_printing;
    
} Search_Signals_t;

typedef struct {
    int null_move_allowed;
    int isPv;
    int see_value;
    int stack_depth;
} Search_Stack_t;

extern Search_Signals_t SIGNALS;

void start_search(int mode, int wtime, int btime, int winc, int binc, int depth, int movetime, uint64_t nodes);
void set_think_time();
void *time_search(void *args);
void print_root_search_results();
void stop_search();
void *root_search(void *args);
int regular_search(int depth, int alpha, int beta, Line_t *pv, Search_Stack_t *SS);
int quiescence_search(int depth, int alpha, int beta, Search_Stack_t *SS);

#endif /* search_h */
