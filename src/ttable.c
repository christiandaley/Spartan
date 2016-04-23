//
//  ttable.c
//  Spartan
//
//
//
//

#include <stdlib.h>
#include <assert.h>
#include "ttable.h"
#include "move.h"

TTable_t global_ttable;

#define N_RANDS 793

Hash_t rands[N_RANDS];

#define RAND_FOR_PIECE_ON_SQUARE(p, c, sq) (rands[((c) * 6 + (p) - 1) + (12 * (sq))])
#define CASTLING_RAND(KQkq) rands[768 + (KQkq)]
#define TURN_RAND (rands[784])
#define EP_RAND(ep_sq) ((ep_sq) ? rands[785 + FILE(ep_sq)] : 0)

void init_global_tt(int mb) {
    global_ttable.size = MB_TO_TT_SIZE(mb);
    if ((global_ttable.size & 1) == 0)
        global_ttable.size++;
    
    global_ttable.entries = calloc(sizeof(TT_Entry_t), global_ttable.size);
    generate_tt_randoms();
}

void free_global_tt() {
    free(global_ttable.entries);
}

void clear_global_tt() {
    int i;
    for (i = 0; i < global_ttable.size; i++) {
        global_ttable.entries[i].flags = 0;
        global_ttable.entries[i].score = 0;
        global_ttable.entries[i].hash = 0;
        global_ttable.entries[i].best_move = NULL_MOVE;
    }
}

void generate_tt_randoms() {
    int i, j;
    for (i = 0; i < N_RANDS; i++)
        rands[i] = (Hash_t)rand() | ((Hash_t)rand() << 32);
    
    // hackish code to ensure there are no identical random numbers in the list.
    for (i = 0; i < N_RANDS; i++) {
        for (j = 0; j < N_RANDS; j++) {
            if (rands[i] == rands[j] && i != j)
                generate_tt_randoms();
        }
    }
    
}

void tt_store_entry(Hash_t hash, int flags, uint64_t score, Move_t tt_move) {
    int index = hash % global_ttable.size;
    int tries = 0;
    while (global_ttable.entries[index].hash && global_ttable.entries[index].hash != hash
           && TT_FLAG_AGE(global_ttable.entries[index].flags) <  REPLACEMENT_AGE && tries < BUCKET_SIZE) {
        index = (index + 1) % global_ttable.size;
        tries++;
    }
    
    if (tries == BUCKET_SIZE) {
        // there was no place to store the new entry
        index = hash % global_ttable.size;
        tries = 0;
        while (TT_FLAG_DEPTH(global_ttable.entries[index].flags) >= TT_FLAG_DEPTH(flags) && tries < BUCKET_SIZE) {
            // try to overwrite an entry with a lower depth than this one. Entries with a lower depth are less useful
            index = (index + 1) % global_ttable.size;
            tries++;
        }
        
        if (tries == BUCKET_SIZE) // if there are no entries with a lower depth, don't store the new entry
            return;
        
    }
    
    global_ttable.entries[index].hash = hash;
    global_ttable.entries[index].flags = flags;
    global_ttable.entries[index].score = score;
    global_ttable.entries[index].best_move = tt_move;
}

uint64_t probe_tt(Hash_t hash, int *flags, Move_t *tt_move) {
    int index = hash % global_ttable.size;
    int tries = 0;
    while (global_ttable.entries[index].hash != hash && tries < BUCKET_SIZE) {
        index = (index + 1) % global_ttable.size;
        tries++;
    }
    
    if (tries == BUCKET_SIZE) {
        *flags = 0;
        if (tt_move)
            *tt_move = NULL_MOVE;
        
        return 0;
    }
    
    *flags = global_ttable.entries[index].flags;
    if (tt_move)
        *tt_move = global_ttable.entries[index].best_move;
    
    return global_ttable.entries[index].score;
}

Hash_t hash_board(Board_t *board) {
    Hash_t hash = 0;
    
    int i, j;
    for (i = 0; i < 64; i++) { // dirty code, but at least it's short
        for (j = 0; j < 12; j++) {
            if (board->BitBoards[j] & SQ_MASK(i))
                hash ^= RAND_FOR_PIECE_ON_SQUARE((j % 6) + 1, j / 6, i);
        }
    }
    
    
    hash ^= CASTLING_RAND(CASTLING_FLAGS(board));
    
    hash ^= TURN(board) == WHITE ? 0 : TURN_RAND;
    hash ^= EP_RAND(EP_SQUARE(board));
    
    return hash;
}

void tt_inc_entry_ages() {
    int i;
    int flags;
    for (i = 0; i < global_ttable.size; i++) {
        if (global_ttable.entries[i].hash) {
            flags = global_ttable.entries[i].flags;
            global_ttable.entries[i].flags = TT_FLAG_INC_AGE(flags);
        }
    }
}

Hash_t hash_global_board() {
    return hash_board(global_board());
}

Hash_t hash_move_on_board(Board_t *board, Move_t move, int captured_piece_type, int old_castling, int old_ep_square, int old_turn) {
    Hash_t hash = board->hash;
    
    int from = MOVE_FROM(move);
    int to = MOVE_TO(move);
    
    hash ^= RAND_FOR_PIECE_ON_SQUARE(MOVE_PIECE(move), old_turn, from);
    hash ^= RAND_FOR_PIECE_ON_SQUARE(MOVE_PIECE(move), old_turn, to);
    
    switch (MOVE_FLAGS(move)) {
        case FLAG_CASTLE_KINGSIDE:
            hash ^= RAND_FOR_PIECE_ON_SQUARE(ROOK, old_turn, from + 3);
            hash ^= RAND_FOR_PIECE_ON_SQUARE(ROOK, old_turn, from + 1);
            break;
        case FLAG_CASTLE_QUEENSIDE:
            hash ^= RAND_FOR_PIECE_ON_SQUARE(ROOK, old_turn, from - 4);
            hash ^= RAND_FOR_PIECE_ON_SQUARE(ROOK, old_turn, from - 1);
            break;
        case FLAG_PROMOTE_QUEEN:
            hash ^= RAND_FOR_PIECE_ON_SQUARE(PAWN, old_turn, to);
            hash ^= RAND_FOR_PIECE_ON_SQUARE(QUEEN, old_turn, to);
            break;
        case FLAG_PROMOTE_ROOK:
            hash ^= RAND_FOR_PIECE_ON_SQUARE(PAWN, old_turn, to);
            hash ^= RAND_FOR_PIECE_ON_SQUARE(ROOK, old_turn, to);
            break;
        case FLAG_PROMOTE_BISHOP:
            hash ^= RAND_FOR_PIECE_ON_SQUARE(PAWN, old_turn, to);
            hash ^= RAND_FOR_PIECE_ON_SQUARE(BISHOP, old_turn, to);
            break;
        case FLAG_PROMOTE_KNIGHT:
            hash ^= RAND_FOR_PIECE_ON_SQUARE(PAWN, old_turn, to);
            hash ^= RAND_FOR_PIECE_ON_SQUARE(KNIGHT, old_turn, to);
            break;
        default:
            
            break;
    }
    
    if (MOVE_IS_CAPTURE(move))
        hash ^= RAND_FOR_PIECE_ON_SQUARE(captured_piece_type, old_turn == WHITE ? BLACK : WHITE, to);
    else if (MOVE_IS_EN_PASSANT(move)) {
        if (old_turn == WHITE)
            hash ^= RAND_FOR_PIECE_ON_SQUARE(PAWN, BLACK, to - 8);
        else
            hash ^= RAND_FOR_PIECE_ON_SQUARE(PAWN, WHITE, to + 8);
    }
    
    hash ^= TURN_RAND;
    hash ^= EP_RAND(old_ep_square);
    hash ^= EP_RAND(EP_SQUARE(board));
    hash ^= CASTLING_RAND(old_castling);
    hash ^= CASTLING_RAND(CASTLING_FLAGS(board));
    
    return hash;
}

Hash_t hash_null_move(Board_t *board, int ep_sq) {
    Hash_t hash = board->hash;
    return hash ^ TURN_RAND ^ (ep_sq ? EP_RAND(ep_sq) : 0);
}

