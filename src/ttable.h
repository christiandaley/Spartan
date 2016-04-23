//
//  ttable.h
//  Spartan
//
//
//
//

#ifndef ttable_h
#define ttable_h

#include "board.h"

#define MB_TO_TT_SIZE(mb) ((mb * 1000000) / sizeof(TT_Entry_t))
#define DEFAULT_TT_MB 64
#define NO_ENTRY 0
#define EXACT_SCORE 1
#define LOWER_BOUND 2
#define UPPER_BOUND 3

#define BUCKET_SIZE 4
#define REPLACEMENT_AGE 3

#define MAKE_TT_FLAGS(type, PV, depth) ((type & 0x3) | ((PV & 1) << 2) | (depth << 3))
#define TT_FLAG_TYPE(f)       ((f) & 0x03)
#define TT_FLAG_IS_PV(f)      (((f) >> 2) & 1)
#define TT_FLAG_DEPTH(f)      (((f) >> 3) & 0xff)
#define TT_FLAG_AGE(f)        ((f) >> 11)
#define TT_FLAG_INC_AGE(f)    (((f) & 0x7ff) ^ ((TT_FLAG_AGE(f) + 1) << 11))

typedef unsigned long long Hash_t;

typedef struct  {
    Hash_t hash;
    // the flags give info about the type of entry
    // bits 0-1 are the type of score
    // bit 2 is whether the position is a PV or not
    // bits 3-10 are the depth
    // bits 11+ are the age of the entry
    uint64_t score;
    int flags;
    Move_t best_move;
} TT_Entry_t;

typedef struct  {
    int size;
    TT_Entry_t *entries;
} TTable_t;

void init_global_tt(int mb);
void free_global_tt();
void clear_global_tt();
void generate_tt_randoms();

void tt_store_entry(Hash_t hash, int flags, uint64_t score, Move_t tt_move);
uint64_t probe_tt(Hash_t hash, int *flags, Move_t *tt_move);
void tt_inc_entry_ages();

Hash_t hash_board(Board_t *board);
Hash_t hash_global_board();

// this function asumes the move in question has already been made
Hash_t hash_move_on_board(Board_t *board, Move_t move, int captured_piece_type, int old_castling, int old_ep_square, int old_turn);

Hash_t hash_null_move(Board_t *board, int ep_sq);

#endif /* ttable_h */
