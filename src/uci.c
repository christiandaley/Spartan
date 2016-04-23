//
//  uci.c
//  Spartan
//
//
//
//

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include "board.h"
#include "bitmasks.h"
#include "debug.h"
#include "eval.h"
#include "log.h"
#include "magicmoves.h"
#include "magicnumbersgen.h"
#include "move.h"
#include "movegen.h"
#include "search.h"
#include "uci.h"
#include "ttable.h"
#include "sort.h"
#include "test.h"

#define MAX_BUFFER_SIZE 2048
#define MAX_TOKEN_SIZE 256
#define ENGINE_VERSION 1.0

const static char *CommandQuit = "quit";
const static char *CommandUCI = "uci";
const static char *CommandPrint = "print";
const static char *CommandPosition = "position";
const static char *UCINewGame = "ucinewgame";
const static char *CommandUndo = "undo";
const static char *PositionStartpos = "startpos";
const static char *PositionFEN = "fen";
const static char *PositionMoves = "moves";
const static char *IsReady = "isready";
const static char *CommandList = "list";
const static char *CommandPerft = "perft";
const static char *CommandGo = "go";
const static char *CommandStop = "stop";
const static char *Eval = "eval";

const static char *GoMovetime = "movetime";
const static char *GoDepth = "depth";
const static char *GoInfinite = "infinite";
const static char *GoNodes = "nodes";
const static char *GoPonder = "ponder";
const static char *GoWTime = "wtime";
const static char *GoBTime = "btime";
const static char *GoWInc = "winc";
const static char *GoBInc = "binc";

const char *SetOption = "setoption";
const char *OptionHash = "hash";

const static char *ReadyOk = "readyok\n";
const static char *UCIOK = "uciok\n";
const static char *NameInfo = "id name Spartan %.1f\n";
const static char *AuthorInfo = "id author Christian Daley\n";

const static char *BKTest = "bktest";
const static char *LKTest = "lktest";


// reads the first token in the input line and then advances the input pointer to the next token
void get_token(char *token, char **input) {
    while (**input != ' ' && **input != '\n' && **input != '\0') {
        *token = **input;
        token++;
        (*input)++;
    }
    
    *token = '\0';
    
    while (**input == ' ' || **input == '\n')
        (*input)++;
    
    
}

int uciloop() {
    engine_output("Spartan %0.1f by Christian Daley\n", ENGINE_VERSION);
    init();
    char read_buffer[MAX_BUFFER_SIZE] = {};
    char token[MAX_TOKEN_SIZE] = {};
    char *input; // pointer to the current part of the input being read
    
    
    while (strcmp(token, CommandQuit) != 0) {
        // read the ui commands and store it in a temporary buffer
        fgets(read_buffer, MAX_BUFFER_SIZE, stdin);
        fflush(stdin);
        input = read_buffer;
        write_to_log("GUI", input);
        get_token(token, &input);
        
        if (strcmp(token, CommandUCI) == 0) {
            print_engine_info();
            print_engine_options();
            engine_output(UCIOK);
        } else if (strcmp(token, IsReady) == 0) {
            engine_output(ReadyOk);
        } else if (strcmp(token, CommandPrint) == 0) {
            print_global_board();
        } else if (strcmp(token, CommandUndo) == 0) {
            if (global_board_stack()->ply > 0)
                UNDO_MOVE_ON_GLOBAL_BOARD
        } else if (strcmp(token, CommandPosition) == 0) {
            position(input);
        } else if (strcmp(token, CommandList) == 0) {
            int i;
            generate_global_legal_moves();
            engine_output("%d legal moves\n", global_move_count());
            for (i = 0; i < global_move_count(); i++) {
                print_move(global_move_list()[i]);
                engine_output("\n");
            }
        } else if (strcmp(token, CommandPerft) == 0) {
            get_token(token, &input);
            int depth = atoi(token);
            double time;
            uint64_t nodes;
            struct timeval start, end;
            if (depth <= 0) {
                engine_output("Invalid Perft Depth\n");
                continue;
            }
            
            clear_global_tt();
            engine_output("\nPerft depth %d calculating...\n", depth);
            
            gettimeofday(&start, NULL);
            nodes = perft(depth);
            gettimeofday(&end, NULL);
            
            engine_output("Result: %llu nodes\n", nodes);
            
            time = end.tv_sec - start.tv_sec + (double)(end.tv_usec - start.tv_usec) / (1000000.0);
            
            engine_output("completed in %.2lf ms\n", time * 1000);
            engine_output("%d nodes/second\n", (int)(nodes / time));
            
        } else if (strcmp(token, CommandGo) == 0) {
            go(input);
        } else if (strcmp(token, CommandStop) == 0) {
            stop_search();
            usleep(100000);
        } else if (strcmp(token, Eval) == 0) {
            int eval = eval_global_board();
            engine_output("Static evaluation of board: %d\n", TURN(global_board()) == WHITE ? eval : -eval);
        } else if (strcmp(token, UCINewGame) == 0) {
            reset_global_ply();
            reset_global_move_stack_depth();
            clear_global_tt();
            fen((char *)Start_Fen);
        } else if (strcmp(token, BKTest) == 0) {
            int time;
            get_token(token, &input);
            if (*token == '\0')
                time = DEFAULT_BK_TEST_TIME;
            else
                time = atoi(token);
            
            bratko_kopec_test(time);
                
        } else if (strcmp(token, LKTest) == 0) {
            larry_kaufman_1_hr_test();
        } else if (strcmp(token, SetOption) == 0) {
            set_option(input);
        } else if (strcmp(token, CommandQuit) != 0) {
            engine_output("Unrecognized command %s\n", token);
        }
        
    }
    
    free_global_tt();
    
    return 0;
}


void print_engine_info() {
    engine_output(NameInfo, ENGINE_VERSION);
    engine_output(AuthorInfo);
}

void print_engine_options() {
    engine_output("option name hash type spin default %d min 1 max 1024\n", DEFAULT_TT_MB);
}

void init() {
    reset_global_ply();
    init_magic_moves();
    init_global_tt(DEFAULT_TT_MB);
    init_pawn_table();
    fen((char *)Start_Fen);
}

void position(char *input) {
    char token[MAX_TOKEN_SIZE];
    while (strlen(input) != 0) {
        
        get_token(token, &input);
        
        if (strcmp(token, PositionStartpos) == 0) {
            fen((char *)Start_Fen);
        } else if (strcmp(token, PositionFEN) == 0) {
            fen(input);
        } else if (strcmp(token, PositionMoves) == 0) {
            moves(input);
        }
    }
}

// parses a series of moves and executes them on the current board
void moves(char *input) {
    char token[MAX_TOKEN_SIZE];
    int from, to, piece_type, flags, capture;
    Move_t move;
    Board_t *board;
    
    while (strlen(input) != 0) {
        board = global_board();
        get_token(token, &input);
        from = STRING_TO_SQUARE(token);
        to = STRING_TO_SQUARE(token+2);
        
        
        // determine the type of piece that is moving
        if (PAWN_ON_SQUARE(from, board))
            piece_type = PAWN;
        else if (ROOK_ON_SQUARE(from, board))
            piece_type = ROOK;
        else if (KNIGHT_ON_SQUARE(from, board))
            piece_type = KNIGHT;
        else if (BISHOP_ON_SQUARE(from, board))
            piece_type = BISHOP;
        else if (QUEEN_ON_SQUARE(from, board))
            piece_type = QUEEN;
        else if (KING_ON_SQUARE(from, board))
            piece_type = KING;
        else {
            engine_output("Error: Illegal Move Recieved\n");
            return;
        }
        
        flags = FLAG_NONE;
        // is the move castling?
        if (piece_type == KING && from - to == 2)
            flags = FLAG_CASTLE_QUEENSIDE;
        else if (piece_type == KING && to - from == 2)
            flags = FLAG_CASTLE_KINGSIDE;
        else if (piece_type == PAWN) {
            // is it a pawn move?
            if (RANK(to) == 7 || RANK(to) == 0) { // if so, is it a promotion?
                if (token[4] == 'n') // what kind?
                    flags = FLAG_PROMOTE_KNIGHT;
                else if (token[4] == 'b')
                    flags = FLAG_PROMOTE_BISHOP;
                else if (token[4] == 'r')
                    flags = FLAG_PROMOTE_ROOK;
                else if (token[4] == 'q')
                    flags = FLAG_PROMOTE_QUEEN;
                
            } else if ((FILE(from) != FILE(to)) && PIECE_ON_SQUARE(to, board) == 0) {
                // is it en passant?
                flags = FLAG_EP;
            }
        }
        
        capture = PIECE_ON_SQUARE(to, board) ? CAPTURE : NON_CAPTURE;
        
        move = CREATE_MOVE(capture, flags, piece_type, to, from);
        make_move_on_global_board(move);
    }
}

// parses a fen string
void fen(char *input) {
    char token[MAX_TOKEN_SIZE];
    int loc = 0;
    int castling = 0;
    int sq = 56;
    // first is the list of piece locations
    get_token(token, &input);
    reset_global_ply();
    reset_global_move_stack_depth();
    memset(global_board(), 0, sizeof(Board_t));
    
    while (token[loc] != '\0') {
        if (token[loc] == '/') {
            loc++;
            continue;
        }
        
        // determine which piece, if any, goes on the curent square
        switch (token[loc]) {
            case 'p':
                set_global_square_value(sq, PAWN, BLACK);
                break;
                
            case 'P':
                set_global_square_value(sq, PAWN, WHITE);
                break;
                
            case 'r':
                set_global_square_value(sq, ROOK, BLACK);
                break;
                
            case 'R':
                set_global_square_value(sq, ROOK, WHITE);
                break;
                
            case 'n':
                set_global_square_value(sq, KNIGHT, BLACK);
                break;
                
            case 'N':
                set_global_square_value(sq, KNIGHT, WHITE);
                break;
                
            case 'b':
                set_global_square_value(sq, BISHOP, BLACK);
                break;
                
            case 'B':
                set_global_square_value(sq, BISHOP, WHITE);
                break;
                
            case 'q':
                set_global_square_value(sq, QUEEN, BLACK);
                break;
                
            case 'Q':
                set_global_square_value(sq, QUEEN, WHITE);
                break;
                
            case 'k':
                set_global_square_value(sq, KING, BLACK);
                break;
                
            case 'K':
                set_global_square_value(sq, KING, WHITE);
                break;
                
            default:
                sq += token[loc] - '1';
                break;
        }
        
        loc++;
        sq++;
        if (FILE(sq) == 0)
            sq -= 16;
    }
    
    // determine the turn
    get_token(token, &input);
    if (*token == 'b' || *token == 'B')
        SET_TURN(global_board(), BLACK);
    else
        SET_TURN(global_board(), WHITE);
    
    // parse castling rights
    get_token(token, &input);
    loc = 0;
    while (token[loc] != '\0') {
        switch (token[loc]) {
            case 'K':
                castling |= 0x8;
                break;
                
            case 'Q':
                castling |= 0x4;
                break;
                
            case 'k':
                castling |= 0x2;
                break;
                
            case 'q':
                castling |= 0x1;
                break;
                
                
            default:
                break;
        }
        
        loc++;
    }
    
    SET_CASTLING_RIGHTS(global_board(), castling);
    
    // get the en passant square, if there is one
    get_token(token, &input);
    if (*token != '-') {
        SET_EP_SQUARE(global_board(), STRING_TO_SQUARE(token));
    }
    
    global_board()->hash = hash_global_board();
    
    // non-complete fen string
    if (strlen(input) == 0)
        return;
    
    // get the # of moves since pawn or capture
    get_token(token, &input);
    global_board()->movesSinceCaptureORPawn = atoi(token);
    
    if (strlen(input) == 0)
        return;
    // get the fullmove number
    get_token(token, &input);
    global_board()->fullmoves = atoi(token);
    
    global_board()->last_move_made = NULL_MOVE;
    
    
}

void go(char *input) {
    char token[MAX_TOKEN_SIZE];
    int movetime = 0, wtime = 0, btime = 0, winc = 0, binc = 0, depth = 9999;
    int mode = SEARCH_MODE_NONE;
    uint64_t nodes = 0xffffffffffffffff;
    movetime = 86400000; // 1 day
    stop_search();
    
    do {
        get_token(token, &input);
        if (strcmp(token, GoInfinite) == 0) {
            mode = SEARCH_MODE_INFINITE;
        } else if (strcmp(token, GoMovetime) == 0) {
            get_token(token, &input);
            movetime = atoi(token);
            mode = SEARCH_MODE_TIMED;
        } else if (strcmp(token, GoDepth) == 0) {
            get_token(token, &input);
            depth = atoi(token);
            mode = SEARCH_MODE_DEPTH;
        } else if (strcmp(token, GoNodes) == 0) {
            get_token(token, &input);
            nodes = atoll(token);
            mode = SEARCH_MODE_NODES;
        } else if (strcmp(token, GoPonder) == 0) {
            
        } else if (strcmp(token, GoWTime) == 0) {
            get_token(token, &input);
            wtime = atoi(token);
        } else if (strcmp(token, GoBTime) == 0) {
            get_token(token, &input);
            btime = atoi(token);
        } else if (strcmp(token, GoWInc) == 0) {
            get_token(token, &input);
            winc = atoi(token);
        } else if (strcmp(token, GoBInc) == 0) {
            get_token(token, &input);
            binc = atoi(token);
        }
    } while (strlen(input) != 0);
    
    start_search(mode, wtime, btime, winc, binc, depth, movetime, nodes);
}

void set_option(char *input) {
    char token[MAX_TOKEN_SIZE];
    get_token(token, &input);
    assert(strcmp(token, "name") == 0);
    get_token(token, &input);
    if (strcmp(token, OptionHash) == 0) {
        get_token(token, &input);
        assert(strcmp(token, "value") == 0);
        get_token(token, &input);
        free_global_tt();
        init_global_tt(atoi(token));
        engine_output("info string hash table size set to %d MB\n",atoi(token));
    }
}