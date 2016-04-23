//
//  search.c
//  Spartan
//
//
//
//

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "search.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "eval.h"
#include "log.h"
#include "debug.h"
#include "sort.h"
#include "ttable.h"
#include "killer.h"
#include "history.h"

Search_Signals_t SIGNALS;
pthread_t search_thread;
pthread_t timer_thread;

void start_search(int mode, int wtime, int btime, int winc, int binc, int depth, int movetime, uint64_t nodes) {
    SIGNALS.search_mode = mode;
    SIGNALS.wtime = wtime;
    SIGNALS.btime = btime;
    SIGNALS.winc = winc;
    SIGNALS.binc = binc;
    SIGNALS.max_depth = depth;
    SIGNALS.movetime = movetime;
    SIGNALS.max_nodes = nodes;
    SIGNALS.stop = 0;
    SIGNALS.search_was_stopped = 0;
    gettimeofday(&SIGNALS.start_time, NULL);
    
    if (SIGNALS.search_mode == SEARCH_MODE_NONE)
        set_think_time();
    
    int fail = pthread_create(&timer_thread, NULL, &time_search, NULL);
    if (fail) {
        engine_output("Error creating thread for timing the search\n");
        exit(1);
    }
    
    fail = pthread_create(&search_thread, NULL, &root_search, NULL);
    if (fail) {
        engine_output("Error creating thread for search algorithm\n");
        exit(1);
    }
    
    
}

void set_think_time() {
    int material = count_material(global_board());
    
    int mg = (material - MATERIAL_OFFSET) * 256 / (MAX_MATERIAL - MATERIAL_OFFSET);
    double x = (double)mg / 256.0;
    
    double y = (-0.26 * x * x) + (0.23 * x) + 0.05;
    
    SIGNALS.movetime = (int)(TURN(global_board()) == WHITE ? 0.5 * SIGNALS.winc + y * SIGNALS.wtime :
                                                0.5 * SIGNALS.binc + y * SIGNALS.btime);
}

void *time_search(void *args) {
    
    usleep(SIGNALS.movetime * 990);
    SIGNALS.stop = 1;
    
    return NULL;
}

void print_root_search_results() {
    struct timeval curr_time;
    int i;
    gettimeofday(&curr_time, NULL);
    double elapsed_time = (curr_time.tv_sec - SIGNALS.start_time.tv_sec) * 1000.0 + (curr_time.tv_usec - SIGNALS.start_time.tv_usec) / 1000.0;
    
    engine_output("info depth %d seldepth %d score ", SIGNALS.depth, SIGNALS.seldepth, (int)elapsed_time, SIGNALS.score);
    if (IS_WINNING_CHECKMATE(SIGNALS.score))
        engine_output("mate %d", 1 + (VALUE_INFINITE - SIGNALS.score) / 2);
    else if (IS_LOSING_CHECKMATE(SIGNALS.score))
        engine_output("mate -%d", (VALUE_INFINITE + SIGNALS.score) / 2);
    else
        engine_output("cp %d", SIGNALS.score);
    
    engine_output(" nodes %llu nps %d time %d", SIGNALS.nodes, (int)(SIGNALS.nodes * 1000 / elapsed_time), (int)elapsed_time);
    
    engine_output(" pv ");
    
    
    for (i = 0; i < SIGNALS.PV.nMoves; i++)
        print_move(SIGNALS.PV.moves[i]);
    
    engine_output("\n");
    SIGNALS.currently_printing = 0;
}

void stop_search() {
    SIGNALS.stop = 1;
    SIGNALS.search_was_stopped = 1;
}

void *root_search(void *args) {
    
    int depth, i, move_count, alpha, beta, value, score, R;
    int old_alpha;
    int scores[MAX_NUM_MOVES] = {};
    int aspiration_fail = 0;
    Move_t best_move, curr_move, *moves;
    Line_t local_pv;
    Search_Stack_t SS[MAX_DEPTH];
    SS->null_move_allowed = FRIENDLY_MAJOR_AND_MINOR_PIECES(global_board()) ? 1 : 0;
    generate_global_legal_moves();
    moves = global_move_list();
    move_count = global_move_count();
    tt_inc_entry_ages();
    clear_killers();
    clear_history_table();
    
    alpha = -MATE_IN(1);
    beta = MATE_IN(1);
    
    for (i = 0; i < MAX_DEPTH; i++)
        (SS+i)->stack_depth = i+1;
    
    for (depth = 1; SIGNALS.stop == 0 && depth <= SIGNALS.max_depth; depth++) {
        SIGNALS.depth = depth;
        value = -MATE_IN(1);
        sort_root_moves(scores);
        best_move = moves[0];
        SIGNALS.PV.moves[0] = best_move;
        SIGNALS.seldepth = 0;
        old_alpha = alpha;
        
        for (i = 0; i < move_count && !SIGNALS.stop; i++) {
            curr_move = moves[i];
            SIGNALS.current_move = curr_move;
            SIGNALS.current_move_number = i;
            //engine_output("info depth %d seldepth %d currmove ", SIGNALS.depth, SIGNALS.seldepth);
            //print_move(curr_move);
            //engine_output("currmove number %d\n", i+1);
            
            SS->isPv = i == 0;
            SS->see_value = see(curr_move, global_board());
            
            make_move_on_global_board(curr_move);
            
            
            R = (!global_board()->is_check && i >= LMR_THRESHOLD && !MOVE_IS_CAPTURE(curr_move)) ? 1 : 0;
            
            if (SS->isPv)
                score = -regular_search(depth - 1, -beta, -alpha, &local_pv, SS);
            else {
                score = -regular_search(depth - 1 - R, -alpha-1, -alpha, &local_pv, SS);
                if (score > alpha && score < beta) {
                    score = -regular_search(depth - 1, -beta, -alpha, &local_pv, SS);
                }
            }
            
            UNDO_MOVE_ON_GLOBAL_BOARD
            
            scores[i] = score;
            
            if (score > value && !SIGNALS.stop) {
                value = score;
                best_move = curr_move;
                
                if (value > alpha) {
                    alpha = value;
                    SIGNALS.PV.moves[0] = best_move;
                    memcpy(SIGNALS.PV.moves + 1, local_pv.moves, local_pv.nMoves * sizeof(Move_t));
                    SIGNALS.PV.nMoves = local_pv.nMoves + 1;
                    if (alpha >= beta)
                        break;
                }
            }
            
        }
        
        if (!SIGNALS.stop) {
            if (value > old_alpha && value < beta) {
                SIGNALS.best_move = best_move;
                SIGNALS.score = value;
                alpha = value - ASPIRATION_WINDOW;
                beta = value + ASPIRATION_WINDOW;
                aspiration_fail = 0;
            } else if (IS_WINNING_CHECKMATE(value)) {
                beta = MATE_IN(1);
                SIGNALS.score = value;
                clear_global_tt();
            } else if (IS_LOSING_CHECKMATE(value)) {
                alpha = -MATE_IN(1);
                beta = -MATE_IN(MAX_DEPTH);
                SIGNALS.score = value;
                clear_global_tt();
            } else {
                depth--;
                aspiration_fail++;
                if (value >= beta)
                    beta += (1 << aspiration_fail) * ASPIRATION_WINDOW;
                else
                    alpha -= (1 << aspiration_fail) * ASPIRATION_WINDOW;
            }
        } else {
            // search was killed before we finished evaluating at this depth
            SIGNALS.depth--;
        }
        
        print_root_search_results();
        
    }
    
    engine_output("bestmove ");
    print_move(SIGNALS.PV.moves[0]);
    //engine_output(" ponder ");
    //print_move(SIGNALS.PV.moves[1]);
    engine_output("\n");
    
    if (SIGNALS.search_was_stopped)
        pthread_cancel(timer_thread);
    
    return NULL;
}

int regular_search(int depth, int alpha, int beta, Line_t *pv, Search_Stack_t *SS) {
    
    int mCount, i, score, value, tt_score, tt_flags, R;
    int old_alpha = alpha;
    int old_beta = beta;
    int static_eval;
    value = -MATE_IN(1);
    Move_t curr_move, best_move = NULL_MOVE, tt_move;
    Move_t *moves;
    Line_t local_pv;
    generate_global_legal_moves();
    moves = global_move_list();
    mCount = global_move_count();
    Board_t *board = global_board();
    
    local_pv.nMoves = 0;
    
    if (SIGNALS.stop)
        return 0;
    
    if (mCount == 0) {
        SIGNALS.nodes++;
        pv->nMoves = 0;
        return board->is_check ? -MATE_IN(SS->stack_depth) : 0;
    } else if (global_board_is_draw()) {
        pv->nMoves = 0;
        return CONTEMPT_FACTOR;
    }
    
    if (depth <= 0) {
        pv->nMoves = 0;
        return quiescence_search(-1, alpha, beta, SS+1);
        //return eval_global_board();
    }
    
    (SS+1)->null_move_allowed = 1;
    
    // Transposiiton table lookup
    
    tt_score = (int)probe_tt(board->hash, &tt_flags, &tt_move);
    if (TT_FLAG_TYPE(tt_flags) != NO_ENTRY && TT_FLAG_DEPTH(tt_flags) >= depth && !SS->isPv) {
        if (TT_FLAG_TYPE(tt_flags) == EXACT_SCORE) {
            return tt_score;
        } else if (TT_FLAG_TYPE(tt_flags) == LOWER_BOUND) {
            alpha = tt_score;
            if (tt_score >= beta)
                return tt_score;
        } else {
            beta = tt_score;
            if (tt_score <= alpha)
                return tt_score;
        }
    }
    
    static_eval = eval_global_board();
    SIGNALS.nodes++;
    
    // razoring
    if (!SS->isPv && !board->is_check && depth <= 4 &&
        static_eval + Razoring_Margins[depth] <= alpha && tt_move == NULL_MOVE) {
        if (depth <= 2)
            return quiescence_search(-1, alpha, beta, SS+1);
        
        int adjusted_alpha = alpha - Razoring_Margins[depth];
        score = regular_search(depth - 2, adjusted_alpha, adjusted_alpha + 1, &local_pv, SS+1);
        if (score <= adjusted_alpha)
            return alpha;
    }
    
    // futility pruning
    if (!SS->isPv && SS->null_move_allowed && !board->is_check
        && depth <= 4 && !IS_LOSING_CHECKMATE(alpha) && !IS_WINNING_CHECKMATE(beta)
        && (static_eval - Fultility_Margins[depth] >= beta)) {
        return static_eval;
    }
    
    // null move pruning
    
    if (!SS->isPv && SS->null_move_allowed && !board->is_check
        && FRIENDLY_MAJOR_AND_MINOR_PIECES(board) && depth >= 2) {
        
        make_null_move_on_global_board();
        (SS+1)->null_move_allowed = 0;
        if (depth - 1 - NULL_MOVE_REDUC < 0)
            score = -quiescence_search(-1, -beta, -beta+1, SS+1);
        else
            score = -regular_search(depth - 1 - NULL_MOVE_REDUC, -beta, -beta+1, &local_pv, SS+1);
        
        (SS+1)->null_move_allowed = 1;
        UNDO_MOVE_ON_GLOBAL_BOARD;
        
        if (IS_WINNING_CHECKMATE(score) && score >= beta)
            score = beta;
        
        if (score >= beta) {
            return score;
        }
    }
    
    sort_global_moves(tt_move, tt_score, depth);
    
    // ProbCut
    
    if (!board->is_check && !SS->isPv && SS->null_move_allowed
        && !IS_LOSING_CHECKMATE(alpha) && !IS_WINNING_CHECKMATE(beta)
        && depth >= 5) {
        (SS+1)->null_move_allowed = 0;
        if (static_eval + SS->see_value >= beta + PROB_CUT_MARGINE) {
            int adjusted_beta = beta + PROB_CUT_MARGINE;
            score = regular_search(depth - 1 - PROB_CUT_REDUC, adjusted_beta-1, adjusted_beta, &local_pv, SS+1);
            if (score >= adjusted_beta)
                return score;
        }
        
        (SS+1)->null_move_allowed = 1;
    }
    
    for (i = 0; i < mCount; i++) {
        curr_move = moves[i];
        make_move_on_global_board(curr_move);
        
        // History pruning
        
        if (!board->is_check && !SS->isPv && !MOVE_IS_CAPTURE(curr_move) &&
            sort_data.flags[depth][i] == HISTORY_MOVE && depth <= 6
            && sort_data.scores[depth][i] <= HISTORY_THRESHOLD) {
            
            if (depth - 1 - HISTORY_REDUC <= 0)
                score = -quiescence_search(-1, -alpha-1, -alpha, SS+1);
            else
                score = -regular_search(depth - 1 - HISTORY_REDUC, -alpha-1, -alpha, &local_pv, SS+1);
            
            if (score <= alpha) {
                UNDO_MOVE_ON_GLOBAL_BOARD
                continue;
            }
            
        }
        
        (SS+1)->isPv = ((i == 0) && SS->isPv);
        (SS+1)->see_value = see(curr_move, board);
        
        R = (!SS->isPv && !board->is_check && depth >= 3 && i >= LMR_THRESHOLD && !MOVE_IS_CAPTURE(curr_move)) ? 1 : 0;
        
        if (i == 0)
            score = -regular_search(depth - 1, -beta, -alpha, &local_pv, SS+1);
        else {
            score = -regular_search(depth - 1 - R, -alpha-1, -alpha, &local_pv, SS+1);
            if (score > alpha && score < beta)
                score = -regular_search(depth - 1, -beta, -alpha, &local_pv, SS+1);
        }
        
        UNDO_MOVE_ON_GLOBAL_BOARD
        
        if (score > value) {
            value = score;
            best_move = curr_move;
            if (value > alpha) {
                pv->moves[0] = best_move;
                memcpy(pv->moves+1, local_pv.moves, local_pv.nMoves * sizeof(Move_t));
                pv->nMoves = local_pv.nMoves + 1;
                alpha = value;
                if (alpha >= beta) {
                    if (!MOVE_IS_CAPTURE(best_move)) {
                        add_killer(best_move, depth);
                        if (depth > 4)
                            adjust_history_table(best_move, depth, TURN(board));
                    }
                    
                    break;
                }
            }
        }
        
    }
    
    if (value <= old_alpha)
        tt_flags = MAKE_TT_FLAGS(UPPER_BOUND, SS->isPv, depth);
    else if (value >= old_beta)
        tt_flags = MAKE_TT_FLAGS(LOWER_BOUND, SS->isPv, depth);
    else
        tt_flags = MAKE_TT_FLAGS(EXACT_SCORE, SS->isPv, depth);
    
    tt_store_entry(board->hash, tt_flags, value, best_move);
    
    return value;
    
}

int quiescence_search(int depth, int alpha, int beta, Search_Stack_t *SS) {
    int mCount, i, temp, static_eval, tt_flags, tt_score;
    int old_alpha = alpha;
    int scores[MAX_NUM_MOVES];
    Move_t curr_move, tt_move = NULL_MOVE;
    Move_t best_move = NULL_MOVE;
    Move_t *moves;
    moves = global_move_list();
    mCount = global_move_count();
    Board_t *board = global_board();
    
    if (SIGNALS.stop)
        return 0;
    
    // if the move count is zero it may be because the moves have not been generated yet
    if (mCount == 0) {
        generate_global_legal_moves();
        mCount = global_move_count();
    }
    
    if (mCount == 0) {
        SIGNALS.nodes++;
        return board->is_check ? -MATE_IN(SS->stack_depth) : 0;
    } else if (global_board_is_draw())
        return CONTEMPT_FACTOR;
    
    tt_score = (int)probe_tt(board->hash, &tt_flags, &tt_move);
    
    
    if (TT_FLAG_TYPE(tt_flags) != NO_ENTRY) {
        if (TT_FLAG_TYPE(tt_flags) == EXACT_SCORE) {
            return tt_score;
        } else if (TT_FLAG_TYPE(tt_flags) == LOWER_BOUND) {
            alpha = tt_score;
            if (tt_score >= beta)
                return tt_score;
        } else {
            beta = tt_score;
            if (tt_score <= alpha)
                return tt_score;
        }
    }
    
    static_eval = eval_global_board();
    SIGNALS.nodes++;
    if (SIGNALS.nodes >= SIGNALS.max_nodes) {
        stop_search();
        return 0;
    }
    
    if (!board->is_check) {
        if (static_eval >= beta) {
            return static_eval;
        } else if (static_eval >= alpha)
            alpha = static_eval;
    }
    
    sort_qsearch_moves(scores);
    
    if (-depth + SIGNALS.depth > SIGNALS.seldepth)
        SIGNALS.seldepth = -depth + SIGNALS.depth;
    
    for (i = 0; i < mCount; i++) {
        curr_move = moves[i];
        if (!board->is_check && !MOVE_IS_CAPTURE(curr_move) && !MOVE_IS_EN_PASSANT(curr_move))
            continue;
        
        // prune all losing captures
        if (!board->is_check && scores[i] < 0)
            break;
        
        // delta pruning
        if (!board->is_check && FRIENDLY_MAJOR_AND_MINOR_PIECES(board) && static_eval + see(curr_move, board) + D_PRUNE_MARGINE <= alpha)
            continue;
        
        make_move_on_global_board(curr_move);
        temp = -quiescence_search(depth - 1, -beta, -alpha, SS+1);
        UNDO_MOVE_ON_GLOBAL_BOARD
        
        if (temp > alpha) {
            alpha = temp;
            best_move = curr_move;
            
            if (alpha >= beta)
                break;
        }
        
    }
    
    if (alpha == old_alpha)
        tt_flags = MAKE_TT_FLAGS(UPPER_BOUND, 0, 0);
    else if (alpha >= beta)
        tt_flags = MAKE_TT_FLAGS(LOWER_BOUND, 0, 0);
    else
        tt_flags = MAKE_TT_FLAGS(EXACT_SCORE, 0, 0);
    
    tt_store_entry(board->hash, tt_flags, alpha, best_move);
    
    return alpha;
    
}