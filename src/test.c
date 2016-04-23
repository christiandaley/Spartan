//
//  test.c
//  Spartan
//
//
//
//

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "test.h"
#include "debug.h"
#include "search.h"
#include "uci.h"
#include "log.h"
#include "ttable.h"

const char *bk_positions[24] = {
    "1k1r4/pp1b1R2/3q2pp/4p3/2B5/4Q3/PPP2B2/2K5 b - -",
    "3r1k2/4npp1/1ppr3p/p6P/P2PPPP1/1NR5/5K2/2R5 w - -",
    "2q1rr1k/3bbnnp/p2p1pp1/2pPp3/PpP1P1P1/1P2BNNP/2BQ1PRK/7R b - -",
    "rnbqkb1r/p3pppp/1p6/2ppP3/3N4/2P5/PPP1QPPP/R1B1KB1R w KQkq -",
    "r1b2rk1/2q1b1pp/p2ppn2/1p6/3QP3/1BN1B3/PPP3PP/R4RK1 w - -",
    "2r3k1/pppR1pp1/4p3/4P1P1/5P2/1P4K1/P1P5/8 w - -",
    "1nk1r1r1/pp2n1pp/4p3/q2pPp1N/b1pP1P2/B1P2R2/2P1B1PP/R2Q2K1 w - -",
    "4b3/p3kp2/6p1/3pP2p/2pP1P2/4K1P1/P3N2P/8 w - -",
    "2kr1bnr/pbpq4/2n1pp2/3p3p/3P1P1B/2N2N1Q/PPP3PP/2KR1B1R w - -",
    "3rr1k1/pp3pp1/1qn2np1/8/3p4/PP1R1P2/2P1NQPP/R1B3K1 b - -",
    "2r1nrk1/p2q1ppp/bp1p4/n1pPp3/P1P1P3/2PBB1N1/4QPPP/R4RK1 w - -",
    "r3r1k1/ppqb1ppp/8/4p1NQ/8/2P5/PP3PPP/R3R1K1 b - -",
    "r2q1rk1/4bppp/p2p4/2pP4/3pP3/3Q4/PP1B1PPP/R3R1K1 w - -",
    "rnb2r1k/pp2p2p/2pp2p1/q2P1p2/8/1Pb2NP1/PB2PPBP/R2Q1RK1 w - -",
    "2r3k1/1p2q1pp/2b1pr2/p1pp4/6Q1/1P1PP1R1/P1PN2PP/5RK1 w - -",
    "r1bqkb1r/4npp1/p1p4p/1p1pP1B1/8/1B6/PPPN1PPP/R2Q1RK1 w kq -",
    "r2q1rk1/1ppnbppp/p2p1nb1/3Pp3/2P1P1P1/2N2N1P/PPB1QP2/R1B2RK1 b - - ",
    "r1bq1rk1/pp2ppbp/2np2p1/2n5/P3PP2/N1P2N2/1PB3PP/R1B1QRK1 b - -",
    "3rr3/2pq2pk/p2p1pnp/8/2QBPP2/1P6/P5PP/4RRK1 b - -",
    "r4k2/pb2bp1r/1p1qp2p/3pNp2/3P1P2/2N3P1/PPP1Q2P/2KRR3 w - -",
    "3rn2k/ppb2rpp/2ppqp2/5N2/2P1P3/1P5Q/PB3PPP/3RR1K1 w - -",
    "2r2rk1/1bqnbpp1/1p1ppn1p/pP6/N1P1P3/P2B1N1P/1B2QPP1/R2R2K1 b - -",
    "r1bqk2r/pp2bppp/2p5/3pP3/P2Q1P2/2N1B3/1PP3PP/R4RK1 b kq -",
    "r2qnrnk/p2b2b1/1p1p2pp/2pPpp2/1PP1P3/PRNBB3/3QNPPP/5RK1 w - -"
};

const char *bk_moves[24] = {
    "d6d1",
    "d4d5",
    "f6f5",
    "e5e6",
    "c3d5",
    "g5g6",
    "h5f6",
    "f4f5",
    "f4f5",
    "c6e5",
    "f2f4",
    "e7f5",
    "b2b4",
    "d1d2",
    "g4g7",
    "d2e4",
    "h7h5",
    "c5b3",
    "e8e4",
    "g3g4",
    "f5h6",
    "b7e4",
    "f7f6",
    "f2f4",
};

const char *lk_positions[25] = {
    "rn1qkb1r/pp2pppp/5n2/3p1b2/3P4/2N1P3/PP3PPP/R1BQKBNR w KQkq - 0 1",
    "rn1qkb1r/pp2pppp/5n2/3p1b2/3P4/1QN1P3/PP3PPP/R1B1KBNR b KQkq - 1 1",
    "r1bqk2r/ppp2ppp/2n5/4P3/2Bp2n1/5N1P/PP1N1PP1/R2Q1RK1 b kq - 1 10",
    "r1bqrnk1/pp2bp1p/2p2np1/3p2B1/3P4/2NBPN2/PPQ2PPP/1R3RK1 w - - 1 12",
    "rnbqkb1r/ppp1pppp/5n2/8/3PP3/2N5/PP3PPP/R1BQKBNR b KQkq - 3 5",
    "rnbq1rk1/pppp1ppp/4pn2/8/1bPP4/P1N5/1PQ1PPPP/R1B1KBNR b KQ - 1 5",
    "r4rk1/3nppbp/bq1p1np1/2pP4/8/2N2NPP/PP2PPB1/R1BQR1K1 b - - 1 12",
    "rn1qkb1r/pb1p1ppp/1p2pn2/2p5/2PP4/5NP1/PP2PPBP/RNBQK2R w KQkq c6",
    "r1bq1rk1/1pp2pbp/p1np1np1/3Pp3/2P1P3/2N1BP2/PP4PP/R1NQKB1R b KQ - 1 9",
    "rnbqr1k1/1p3pbp/p2p1np1/2pP4/4P3/2N5/PP1NBPPP/R1BQ1RK1 w - - 1 11",
    "rnbqkb1r/pppp1ppp/5n2/4p3/4PP2/2N5/PPPP2PP/R1BQKBNR b KQkq f3 1 3",
    "r1bqk1nr/pppnbppp/3p4/8/2BNP3/8/PPP2PPP/RNBQK2R w KQkq - 2 6",
    "rnbq1b1r/ppp2kpp/3p1n2/8/3PP3/8/PPP2PPP/RNBQKB1R b KQ d3 1 5",
    "rnbqkb1r/pppp1ppp/3n4/8/2BQ4/5N2/PPP2PPP/RNB2RK1 b kq - 1 6",
    "r2q1rk1/2p1bppp/p2p1n2/1p2P3/4P1b1/1nP1BN2/PP3PPP/RN1QR1K1 w - - 1 12",
    "r1bqkb1r/2pp1ppp/p1n5/1p2p3/3Pn3/1B3N2/PPP2PPP/RNBQ1RK1 b kq - 2 7",
    "r2qkbnr/2p2pp1/p1pp4/4p2p/4P1b1/5N1P/PPPP1PP1/RNBQ1RK1 w kq - 1 8",
    "r1bqkb1r/pp3ppp/2np1n2/4p1B1/3NP3/2N5/PPP2PPP/R2QKB1R w KQkq e6 1 7",
    "rn1qk2r/1b2bppp/p2ppn2/1p6/3NP3/1BN5/PPP2PPP/R1BQR1K1 w kq - 5 10",
    "r1b1kb1r/1pqpnppp/p1n1p3/8/3NP3/2N1B3/PPP1BPPP/R2QK2R w KQkq - 3 8",
    "r1bqnr2/pp1ppkbp/4N1p1/n3P3/8/2N1B3/PPP2PPP/R2QK2R b KQ - 2 11",
    "r3kb1r/pp1n1ppp/1q2p3/n2p4/3P1Bb1/2PB1N2/PPQ2PPP/RN2K2R w KQkq - 3 11",
    "r1bq1rk1/pppnnppp/4p3/3pP3/1b1P4/2NB3N/PPP2PPP/R1BQK2R w KQ - 3 7",
    "r2qkbnr/ppp1pp1p/3p2p1/3Pn3/4P1b1/2N2N2/PPP2PPP/R1BQKB1R w KQkq - 2 6",
    "rn2kb1r/pp2pppp/1qP2n2/8/6b1/1Q6/PP1PPPBP/RNB1K1NR b KQkq - 1 6",
};

const char *lk_moves[25][2] = {
    {"d1b3", ""    },
    {"f5c8", ""    },
    {"g4h6", "g4e5"},
    {"b2b4", ""    },
    {"e7e5", ""    },
    {"b4c3", ""    },
    {"f8b8", ""    },
    {"d4d5", ""    },
    {"c6d4", ""    },
    {"a2a4", ""    },
    {"d7d5", ""    },
    {"c4f7", ""    },
    {""    , "f6e4"},
    {""    , "d6c4"},
    {"e5f6", ""    },
    {"d7d5", ""    },
    {""    , "h3g4"},
    {"g5f6", ""    },
    {""    , "b3e6"},
    {""    , "d4b5"},
    {""    , "f7e6"},
    {"a2a4", ""    },
    {"d3h7", ""    },
    {"f3e5", ""    },
    {""    , "b6b3"}
};


void bratko_kopec_test(int think_time) {
    int i, j;
    double score = 0.0;
    double temp_score;
    char buffer[32];
    char string[6];
    Move_t *moves = global_move_list();
    
    engine_output("----------------\n");
    engine_output("Running the Bratko-Kopec test suit with a search time of %0.1f seconds per position\n", (float)think_time / 1000.0f);
    engine_output("----------------\n");
    for (i = 0; i < 24; i++) {
        temp_score = 0.0;
        engine_output("Test position %d/%d: %s\n", i+1, 24, bk_positions[i]);
        output_off();
        clear_global_tt();
        fen((char *)bk_positions[i]);
        sprintf(buffer, "movetime %d\n", think_time);
        go(buffer);
        usleep((think_time * 1000) + 100000);
        
        for (j = 0; j < 4 && j < global_move_count(); j++) {
            move_to_string(moves[j], string);
            if (strcmp(string, bk_moves[i]) == 0) {
                temp_score = 1.0 / (double)(j + 1);
                break;
            }
        }
        
        score += temp_score;
        output_on();
        engine_output("Score: %.2lf\n", temp_score);
    }
    
    engine_output("----------------\n");
    engine_output("Bratko-Kopec test results:\n");
    engine_output("%.2lf points out of 24 (%.1lf%%)\n", score, score * 100.0 / 24.0);
}

void larry_kaufman_1_hr_test() {
    int i;
    int score = 0;
    int temp_score;
    char string[6];
    
    engine_output("----------------\n");
    engine_output("Running the 1 Hour CCR test by Larry Kaufman\n");
    engine_output("----------------\n");
    
    for (i = 0; i < 25; i++) {
        temp_score = 0;
        engine_output("Test position %d/%d: %s\n", i+1, 25, lk_positions[i]);
        output_off();
        clear_global_tt();
        fen((char *)lk_positions[i]);
        go("movetime 120000\n");
        usleep(15 * 1000000);
        
        move_to_string(SIGNALS.best_move, string);
        if (strcmp(string, lk_moves[i][0]) == 0 ||
            (strcmp(string, lk_moves[i][1]) != 0 && strlen(lk_moves[i][1]) != 0))
            temp_score++;
        
        usleep(15 * 1000000);
        move_to_string(SIGNALS.best_move, string);
        if (strcmp(string, lk_moves[i][0]) == 0 ||
            (strcmp(string, lk_moves[i][1]) != 0 && strlen(lk_moves[i][1]) != 0))
            temp_score++;
        
        usleep(30 * 1000000);
        move_to_string(SIGNALS.best_move, string);
        if (strcmp(string, lk_moves[i][0]) == 0 ||
            (strcmp(string, lk_moves[i][1]) != 0 && strlen(lk_moves[i][1]) != 0))
            temp_score++;
        
        usleep(60 * 1000000);
        move_to_string(SIGNALS.best_move, string);
        if (strcmp(string, lk_moves[i][0]) == 0 ||
            (strcmp(string, lk_moves[i][1]) != 0 && strlen(lk_moves[i][1]) != 0))
            temp_score++;
        
        usleep(1 * 1000000);
        
        output_on();
        engine_output("Score: %d\n", temp_score);
        
        score += temp_score;
    }
    
    engine_output("1 Hour CCR Test results:\n");
    engine_output("%d points out of 100 (%0.1lf%%)\n", score, (double)score * 100.0 / 100.0);
}