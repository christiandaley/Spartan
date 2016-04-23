//
//  eval.c
//  Spartan
//
//
//
//

#include <math.h>
#include "eval.h"
#include "bitscan.h"
#include "bitmasks.h"
#include "board.h"
#include "magicmoves.h"
#include "magicnumbers.h"
#include "debug.h"
#include "sort.h"

//change the values for knights and bishops endgame (maybe??)

const int Piece_Weights[7] = {0, 1, 5, 3, 3, 9, 9001};

static const int Piece_Values[7][2] = {
    { 0,   0   }, // empty
    { 95,  115 }, // pawns
    { 500, 520 }, // rooks
    { 300, 315 }, // knights
    { 315, 325 }, // bishops
    { 910, 955 }, // queens
    { 0,   0   }, // kings
};

static const int Square_Values[7][2][64] = {
    { // empty buffer
        {   0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0
        },
        {   0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0
        }
    },
    
    
    { // Pawns
        {   0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  5,  5,  5,  5,  0,  0,
            0,  5,  15, 20, 20, 15, 5,  0,
            10, 10, 20, 25, 25, 20, 10, 10,
            20, 20, 30, 35, 35, 30, 20, 20,
            40, 50, 50, 50, 50, 50, 50, 40,
            0,  0,  0,  0,  0,  0,  0,  0
        },
        {   0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            10, 10, 10, 10, 10, 10, 10, 10,
            15, 15, 15, 15, 15, 15, 15, 15,
            25, 25, 25, 25, 25, 25, 25, 25,
            40, 40, 40, 40, 40, 40, 40, 40,
            65, 65, 65, 65, 65, 65, 65, 65,
            0,  0,  0,  0,  0,  0,  0,  0
        }
    },
    
    { // rooks
        {   0,  0,  5, 10, 10,  5,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            5,  5,  5,  5,  5,  5,  5,  5,
            10, 10, 10, 10, 10, 10, 10, 10,
            15, 15, 15, 15, 15, 15, 15, 15,
            20, 20, 20, 20, 20, 20, 20, 20,
            25, 25, 25, 25, 25, 25, 25, 25,
            15, 15, 15, 15, 15, 15, 15, 15
        },
        {   0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  5,  5,  5,  5,  5,  5,  0,
            5,  10, 10, 10, 10, 10, 10, 5,
            5,  10, 10, 10, 10, 10, 10, 5,
            5,  15, 15, 15, 15, 15, 15, 5,
            5,  10, 10, 10, 10, 10, 10, 10
        }
    },
    
    
    { // Knights
        {   0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  5,  10, 10, 10, 10, 5,  0,
            0,  10, 15, 15, 15, 15, 10, 0,
            0,  15, 20, 20, 20, 20, 15, 0,
            0,  20, 30, 30, 30, 30, 30, 0,
            0,  15, 25, 25, 25, 25, 15, 0,
            0,  0,  0,  0,  0,  0,  0,  0
        },
        {   0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            5,  10, 15, 15, 15, 10, 10, 5,
            5,  15, 20, 20, 20, 20, 15, 5,
            5,  20, 25, 25, 25, 25, 20, 5,
            5,  15, 20, 20, 20, 20, 15, 5,
            5,  10, 15, 15, 15, 15, 10, 5,
        }
    },
    
    { // bishops
        {   0,  0,  0,  0,  0,  0,  0,  0,
            0,  15, 0,  0,  0,  0,  15, 0,
            0,  10, 15, 10, 10, 15, 10, 0,
            5,  10, 20, 30, 30, 20, 10, 5,
            5,  15, 25, 30, 30, 25, 15, 5,
            5,  15, 30, 30, 30, 30, 15, 5,
            0,  15, 20, 20, 20, 20, 15, 0,
            0,  0,  0,  0,  0,  0,  0,  0
        },
        {   5,  0,  0,  0,  0,  0,  0,  5,
            0,  15, 0,  0,  0,  0,  15, 0,
            0,  10, 20, 15, 15, 20, 10, 0,
            0,  10, 15, 20, 20, 15, 10, 0,
            0,  10, 15, 20, 20, 15, 10, 0,
            0,  10, 20, 15, 15, 20, 10, 0,
            0,  15, 0,  0,  0,  0,  15, 0,
            5,  0,  0,  0,  0,  0,  0,  5
        }
    },
    
    { // queens
        {   0,   0,  0,  0,  0,  0,  0,  0,
            5,   5,  5,  5,  5,  5,  5,  5,
            5,  10, 10, 10, 10, 10, 10,  5,
            10, 15, 15, 15, 15, 15, 15, 10,
            10, 15, 15, 15, 15, 15, 15, 10,
            15, 20, 20, 20, 20, 20, 20, 15,
            20, 25, 25, 25, 25, 25, 25, 20,
            20, 20, 20, 20, 20, 20, 20, 20
        },
        {   0,   0,  0,  0,  0,  0,  0,  0,
            5,   5,  5,  5,  5,  5,  5,  5,
            5,  10, 10, 10, 10, 10, 10,  5,
            10, 15, 15, 15, 15, 15, 15, 10,
            10, 15, 15, 15, 15, 15, 15, 10,
            15, 20, 20, 20, 20, 20, 20, 15,
            20, 25, 25, 25, 25, 25, 25, 20,
            20, 20, 20, 20, 20, 20, 20, 20
        }
    },
    
    { // kings
        {   70, 70, 70, 70, 70, 70, 70, 70,
            60, 60, 60, 60, 60, 60, 60, 60,
            50, 50, 50, 50, 50, 50, 50, 50,
            40, 40, 40, 40, 40, 40, 40, 40,
            30, 30, 30, 30, 30, 30, 30, 30,
            20, 20, 20, 20, 20, 20, 20, 20,
            10, 10, 10, 10, 10, 10, 10, 10,
            0,  0,  0,  0,  0,  0,  0,  0
        },
        {   0,  0,  0,  0,  0,  0,  0,  0,
            5,  10, 10, 10, 10, 10, 10, 5,
            10, 15, 15, 15, 15, 15, 15, 10,
            15, 20, 20, 20, 20, 20, 20, 15,
            20, 25, 25, 25, 25, 25, 25, 20,
            25, 30, 30, 30, 30, 30, 30, 25,
            30, 35, 35, 35, 35, 35, 35, 30,
            35, 40, 40, 40, 40, 40, 40, 35,
        }
    }
};

static const int Doubled_Pawns[2] = {-25, -35};
static const int Isolated_Pawn[2] = {-15, -20};
static const int Backwards_Pawn[2] = {-25, -5};
static const int Passed_Pawn[8][2] = {
    {0,  0},
    {5,  5},
    {5,  10},
    {10, 20},
    {15, 30},
    {20, 45},
    {30, 60},
    {0,  0}
};

static const int Mobility_Weights[7][2] = {
    {0, 0}, // empty
    {2, 2}, // rooks
    {2, 2}, // knights
    {2, 3}, // bishops
    {2, 4}, // queens
    {0, 0}, // kings
};

static const int King_Attacked_Values[2][32] = {
    {0, -1, -3, -5, -9, -13, -17, -22, -27, -33, -39, -46, -53, -60, -68, -76,
        -84, -93, -101, -111, -120, -130, -140, -150, -161, -172, -183, -195, -206, -218, -230, -243},
    
    {0, -1, -3, -5, -9, -13, -17, -22, -27, -33, -39, -46, -53, -60, -68, -76,
        -84, -93, -101, -111, -120, -130, -140, -150, -161, -172, -183, -195, -206, -218, -230, -243},
};

static const int Pawn_Supporting_Minor_Piece[2] = {25, 5};
static const int Pawn_Shelter_Value[7] = {0, 30, 20, 5, 0, 0, 0};
static const int Pawn_Storm_Value[7] = {0, -40, -30, -15, 0, 0, 0};
static const int Unstoppable_Passer_Value = 200;
static const int Free_Passer_Value = 125;
static const int King_Dist_Passed_Pawn_Value[10] = {40, 80, 70, 60, 50, 40, 30, 20, 10, 0};

static const int Double_Bishops_Value[2] = {15, 25};
static const int Rook_On_Open_File_Value[2] = {25, 10};
static const int Rook_On_Semi_Open_File_Value[2] = {15, 5};
static const int Doubled_Rooks_Value[2] = {25, 25};
static const int Connected_Rooks_Value[2] = {25, 15};
static const int Blocked_Bishop_Value[2] = {-30, 0};
static const int Blocked_Rook_Value[2] = {-30, 0};

int mg_val;
int eg_val;

Pawn_Table_t pawn_table;

#define P_VALUE(p, c, sq, mg) (c == WHITE ? Square_Values[p][mg][sq] : Square_Values[p][mg][FILE(sq) + ((7 - RANK(sq)) << 3)])

int eval_board(Board_t *board) {
    int val = 0;
    int mg_weight;
    BB attacks[2][7];
    BB pieces[2][7];
    int i, total_material;
    for (i = PAWN; i <= KING; i++) {
        pieces[WHITE][i] = PIECES_OF_TYPE(board, i, WHITE);
        pieces[BLACK][i] = PIECES_OF_TYPE(board, i, BLACK);
    }
    
    mg_val = 0;
    eg_val = 0;
    
    total_material = eval_pieces(board, pieces);
    eval_mobility(board, pieces, attacks);
    eval_pawns(board);
    eval_kings(board, attacks);
    
    mg_weight = ((total_material - MATERIAL_OFFSET) * 256) / (MAX_MATERIAL - MATERIAL_OFFSET);
    
    if (mg_weight < 0)
        mg_weight = 0;
    else if (mg_weight > 256)
        mg_weight = 256;
    
    val = ((mg_val * mg_weight) + (eg_val * (256 - mg_weight))) / 256;
    
    return TURN(board) == WHITE ? val : -val;
}

int eval_global_board() {
    return eval_board(global_board());
}

int eval_pieces(Board_t *board, BB pieces[2][7]) {
    BB bb;
    int material = 0;
    int sq, ptype, i;
    // does not evaluate kings
    for (ptype = PAWN; ptype < KING; ptype++) {
        // white pieces
        for (bb = pieces[WHITE][ptype]; bb; bb &= (bb - 1)) {
            sq = LS1B(bb);
            mg_val += Piece_Values[ptype][MID_GAME] + P_VALUE(ptype, WHITE, sq, MID_GAME);
            eg_val += Piece_Values[ptype][END_GAME] + P_VALUE(ptype, WHITE, sq, END_GAME);
            material += Piece_Weights[ptype];
        }
        
        // black pieces
        for (bb = pieces[BLACK][ptype]; bb; bb &= (bb - 1)) {
            sq = LS1B(bb);
            mg_val -= Piece_Values[ptype][MID_GAME] + P_VALUE(ptype, BLACK, sq, MID_GAME);
            eg_val -= Piece_Values[ptype][END_GAME] + P_VALUE(ptype, BLACK, sq, END_GAME);
            material += Piece_Weights[ptype];
        }
    }
    
    // Double bishops bonus
    
    if (WHITE_BISHOPS(board) & (WHITE_BISHOPS(board) - 1)) {
        mg_val += Double_Bishops_Value[MID_GAME];
        eg_val += Double_Bishops_Value[END_GAME];
    }
    
    if (BLACK_BISHOPS(board) & (BLACK_BISHOPS(board) - 1)) {
        mg_val -= Double_Bishops_Value[MID_GAME];
        eg_val -= Double_Bishops_Value[END_GAME];
    }
    
    // Doubled rooks bonus
    for (i = 0; i < 8; i++) {
        bb = WHITE_ROOKS(board) & File_Masks[i];
        if (bb & (bb - 1)) {
            mg_val += Doubled_Rooks_Value[MID_GAME];
            eg_val += Doubled_Rooks_Value[END_GAME];
        }
        
        bb = WHITE_ROOKS(board) & Rank_Masks[i];
        if (bb & (bb-1)) {
            mg_val += Connected_Rooks_Value[MID_GAME];
            eg_val += Connected_Rooks_Value[END_GAME];
        }
        
        bb = BLACK_ROOKS(board) & File_Masks[i];
        if (bb & (bb - 1)) {
            mg_val -= Doubled_Rooks_Value[MID_GAME];
            eg_val -= Doubled_Rooks_Value[END_GAME];
        }
        
        bb = BLACK_ROOKS(board) & Rank_Masks[i];
        if (bb & (bb-1)) {
            mg_val -= Connected_Rooks_Value[MID_GAME];
            eg_val -= Connected_Rooks_Value[END_GAME];
        }
    }
    
    // Here we check to see if our bishops and rooks are being blocked from developing by our own pieces
    // This is exactly how Fruit checks for blocked bishops/rooks. Credit to Fruit.
    
    // Bishop blocked by its own pawns
    
    if (WHITE_BISHOP_ON_SQUARE(C1, board) && WHITE_PAWN_ON_SQUARE(D2, board) && PIECE_ON_SQUARE(D3, board)) {
        mg_val += Blocked_Bishop_Value[MID_GAME];
        eg_val += Blocked_Bishop_Value[END_GAME];
    }
    
    if (WHITE_BISHOP_ON_SQUARE(F1, board) && WHITE_PAWN_ON_SQUARE(E2, board) && PIECE_ON_SQUARE(E3, board)) {
        mg_val += Blocked_Bishop_Value[MID_GAME];
        eg_val += Blocked_Bishop_Value[END_GAME];
    }
    
    if (BLACK_BISHOP_ON_SQUARE(C8, board) && BLACK_PAWN_ON_SQUARE(D7, board) && PIECE_ON_SQUARE(D6, board)) {
        mg_val -= Blocked_Bishop_Value[MID_GAME];
        eg_val -= Blocked_Bishop_Value[END_GAME];
    }
    
    if (BLACK_BISHOP_ON_SQUARE(F8, board) && BLACK_PAWN_ON_SQUARE(E7, board) && PIECE_ON_SQUARE(E6, board)) {
        mg_val -= Blocked_Bishop_Value[MID_GAME];
        eg_val -= Blocked_Bishop_Value[END_GAME];
    }
    
    // Rook blocked by its own king
    
    if ((WHITE_KING_ON_SQUARE(B1, board) || WHITE_KING_ON_SQUARE(C1, board)) &&
        (WHITE_ROOK_ON_SQUARE(A1, board) || WHITE_ROOK_ON_SQUARE(A2, board) || WHITE_ROOK_ON_SQUARE(B1, board))) {
        mg_val += Blocked_Rook_Value[MID_GAME];
        eg_val += Blocked_Rook_Value[END_GAME];
        
    } else if ((WHITE_KING_ON_SQUARE(F1, board) || WHITE_KING_ON_SQUARE(G1, board)) &&
               (WHITE_ROOK_ON_SQUARE(G1, board) || WHITE_ROOK_ON_SQUARE(H1, board) || WHITE_ROOK_ON_SQUARE(H2, board))) {
        mg_val += Blocked_Rook_Value[MID_GAME];
        eg_val += Blocked_Rook_Value[END_GAME];
    }
    
    if ((BLACK_KING_ON_SQUARE(B8, board) || BLACK_KING_ON_SQUARE(C8, board)) &&
        (BLACK_ROOK_ON_SQUARE(A8, board) || BLACK_ROOK_ON_SQUARE(A7, board) || BLACK_ROOK_ON_SQUARE(B8, board))) {
        mg_val -= Blocked_Rook_Value[MID_GAME];
        eg_val -= Blocked_Rook_Value[END_GAME];
        
    } else if ((BLACK_KING_ON_SQUARE(F8, board) || BLACK_KING_ON_SQUARE(G8, board)) &&
               (BLACK_ROOK_ON_SQUARE(G8, board) || BLACK_ROOK_ON_SQUARE(H8, board) || BLACK_ROOK_ON_SQUARE(H8, board))) {
        mg_val -= Blocked_Rook_Value[MID_GAME];
        eg_val -= Blocked_Rook_Value[END_GAME];
    }
    
    
    return material;
}

void eval_mobility(Board_t *board, BB pieces[2][7], BB attacks[2][7]) {
    // white mobility
    int sq, color, ptype, index;
    BB bb, moves;
    
    for (color = WHITE; color <= BLACK; color++) {
        for (ptype = PAWN; ptype < KING; ptype++) {
            attacks[color][ptype] = 0;
            for (bb = pieces[color][ptype]; bb; bb &= (bb - 1)) {
                sq = LS1B(bb);
                switch (ptype) {
                    case PAWN:
                        attacks[color][PAWN] |= color == WHITE ? White_Pawn_Attacks_From[sq] : Black_Pawn_Attacks_From[sq];
                        break;
                    case KNIGHT:
                        attacks[color][KNIGHT] |= Knight_Attacks[sq];
                        break;
                    case BISHOP:
                        index = MAGIC_INDEX(BISHOP, sq, ALL_PIECES(board) & bishop_masks[sq]);
                        moves = magic_moves_bishop[sq][index];
                        attacks[color][BISHOP] |= moves;
                        for (; moves; moves &= (moves - 1)) {
                            mg_val += color == WHITE ? Mobility_Weights[BISHOP][MID_GAME] : -Mobility_Weights[BISHOP][MID_GAME];
                            eg_val += color == WHITE ? Mobility_Weights[BISHOP][END_GAME] : -Mobility_Weights[BISHOP][END_GAME];
                        }
                        break;
                    case ROOK:
                        index = MAGIC_INDEX(ROOK, sq, ALL_PIECES(board) & rook_masks[sq]);
                        moves = magic_moves_rook[sq][index];
                        attacks[color][ROOK] |= moves;
                        for (; moves; moves &= (moves - 1)) {
                            mg_val += color == WHITE ? Mobility_Weights[ROOK][MID_GAME] : -Mobility_Weights[ROOK][MID_GAME];
                            eg_val += color == WHITE ? Mobility_Weights[ROOK][END_GAME] : -Mobility_Weights[ROOK][END_GAME];
                        }
                        
                        if (((WHITE_PAWNS(board) | BLACK_PAWNS(board)) & File_Masks[FILE(sq)]) == 0) {
                            mg_val += color == WHITE ? Rook_On_Open_File_Value[MID_GAME] : -Rook_On_Open_File_Value[MID_GAME];
                            eg_val += color == WHITE ? Rook_On_Open_File_Value[MID_GAME] : -Rook_On_Open_File_Value[END_GAME];
                        } else {
                            if (color == WHITE && ((WHITE_PAWNS(board) & File_Masks[FILE(sq)]) == 0)) {
                                mg_val += Rook_On_Semi_Open_File_Value[MID_GAME];
                                eg_val += Rook_On_Semi_Open_File_Value[END_GAME];
                            } else if (color == BLACK && ((BLACK_PAWNS(board) & File_Masks[FILE(sq)]) == 0)) {
                                mg_val += Rook_On_Semi_Open_File_Value[MID_GAME];
                                eg_val += Rook_On_Semi_Open_File_Value[END_GAME];
                            }
                        }
                        
                        break;
                    case QUEEN:
                        index = MAGIC_INDEX(BISHOP, sq, ALL_PIECES(board) & bishop_masks[sq]);
                        moves = magic_moves_bishop[sq][index];
                        
                        index = MAGIC_INDEX(ROOK, sq, ALL_PIECES(board) & rook_masks[sq]);
                        moves |= magic_moves_rook[sq][index];
                        attacks[color][QUEEN] |= moves;
                        
                        for (; moves; moves &= (moves - 1)) {
                            mg_val += color == WHITE ? Mobility_Weights[QUEEN][MID_GAME] : -Mobility_Weights[QUEEN][MID_GAME];
                            eg_val += color == WHITE ? Mobility_Weights[QUEEN][END_GAME] : -Mobility_Weights[QUEEN][END_GAME];
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
}

void eval_pawns(Board_t *board) {
    BB white_pawns = WHITE_PAWNS(board);
    BB black_pawns = BLACK_PAWNS(board);
    BB bb;
    
    int white_mg, black_mg;
    int white_eg, black_eg;
    int i, sq;
    int turn = TURN(board);
    
    // white pawn evaluation
    if (probe_pawn_table(white_pawns, WHITE, &white_mg, &white_eg) == PAWN_TABLE_MISS) {
        white_mg = 0;
        white_eg = 0;
        // check for doubled, isolated, backward, and passed pawns
        // also award bonuses for "outposts" and potential passed pawns
        for (i = 0; i < 8; i++) {
            if ((white_pawns & File_Masks[i]) & ((white_pawns & File_Masks[i]) - 1)) {
                white_mg += Doubled_Pawns[MID_GAME];
                white_eg += Doubled_Pawns[END_GAME];
            }
            
            if ((white_pawns & File_Masks[i]) && (white_pawns & Pawn_Isolated[i]) == 0) {
                white_mg += Isolated_Pawn[MID_GAME];
                white_eg += Isolated_Pawn[END_GAME];
            }
        }
        
        for (bb = white_pawns; bb; bb &= (bb - 1)) {
            sq = LS1B(bb);
            // passed pawns
            if ((black_pawns & White_Pawn_Passed[sq]) == 0)
                eval_passed_pawn(board, sq, WHITE);
            
            // outposts
            if (White_Pawn_Attacks_From[sq] & (WHITE_KNIGHTS(board) | WHITE_BISHOPS(board))) {
                white_mg += Pawn_Supporting_Minor_Piece[MID_GAME];
                white_eg += Pawn_Supporting_Minor_Piece[END_GAME];
            }
            
            // checking for backwards pawns
            
            if ((white_pawns & White_Pawn_Backwards[sq]) == 0) {
                if (PIECE_ON_SQUARE(sq+8, board)) {
                    white_mg += Backwards_Pawn[MID_GAME];
                    white_eg += Backwards_Pawn[END_GAME];
                } else {
                    SET_TURN(board, WHITE);
                    // if we cant (safely) advance the pawn to be next to friendly pawns, it is backwards
                    if ((white_pawns & White_Pawn_Attacks_From[sq]) == 0 ||
                        see(CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, sq+8, sq), board) < 0) {
                        // pawns on the second rank can potentially move two spaces
                        if (RANK(sq) == 1) {
                            if((white_pawns & White_Pawn_Attacks_From[sq+8]) == 0 ||
                             see(CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, sq+16, sq), board) < 0) {
                                white_mg += Backwards_Pawn[MID_GAME];
                                white_eg += Backwards_Pawn[END_GAME];
                            }
                        } else {
                            white_mg += Backwards_Pawn[MID_GAME];
                            white_eg += Backwards_Pawn[END_GAME];
                        }
                    }
                    
                    SET_TURN(board, turn);
                }
            }
        }
        
        pawn_table_store_entry(white_pawns, WHITE, white_mg, white_eg);
    }
    
    // black pawn evaluation
    if (probe_pawn_table(black_pawns, BLACK, &black_mg, &black_eg) == PAWN_TABLE_MISS) {
        black_mg = 0;
        black_eg = 0;
        // check for doubled, isolated, and passed pawns
        for (i = 0; i < 8; i++) {
            if ((black_pawns & File_Masks[i]) & ((black_pawns & File_Masks[i]) - 1)) {
                black_mg += Doubled_Pawns[MID_GAME];
                black_eg += Doubled_Pawns[END_GAME];
            }
            
            if ((black_pawns & File_Masks[i]) && (black_pawns & Pawn_Isolated[i]) == 0) {
                black_mg += Isolated_Pawn[MID_GAME];
                black_eg += Isolated_Pawn[END_GAME];
            }
            
        }
        
        for (bb = black_pawns; bb; bb &= (bb - 1)) {
            sq = LS1B(bb);
            // passed pawns
            if ((white_pawns & Black_Pawn_Passed[sq]) == 0)
                eval_passed_pawn(board, sq, BLACK);
            
            // outposts
            if (Black_Pawn_Attacks_From[sq] & (BLACK_KNIGHTS(board) | BLACK_BISHOPS(board))) {
                black_mg += Pawn_Supporting_Minor_Piece[MID_GAME];
                black_eg += Pawn_Supporting_Minor_Piece[END_GAME];
            }
            
            if ((black_pawns & Black_Pawn_Backwards[sq]) == 0) {
                if (PIECE_ON_SQUARE(sq-8, board)) {
                    black_mg += Backwards_Pawn[MID_GAME];
                    black_eg += Backwards_Pawn[END_GAME];
                } else {
                    SET_TURN(board, BLACK);
                    // if we cant (safely) advance the pawn to be next to friendly pawns, it is backwards
                    if ((black_pawns & Black_Pawn_Attacks_From[sq]) == 0 ||
                        see(CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, sq-8, sq), board) < 0) {
                        // pawns on the seventh rank can potentially move two spaces
                        if (RANK(sq) == 6) {
                            if((black_pawns & Black_Pawn_Attacks_From[sq-8]) == 0 ||
                               see(CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, sq-16, sq), board) < 0) {
                                black_mg += Backwards_Pawn[MID_GAME];
                                black_eg += Backwards_Pawn[END_GAME];
                            }
                        } else {
                            black_mg += Backwards_Pawn[MID_GAME];
                            black_eg += Backwards_Pawn[END_GAME];
                        }
                    }
                    
                    SET_TURN(board, turn);
                }
            }

        }
        
        pawn_table_store_entry(black_pawns, BLACK, black_mg, black_eg);
    }
    
    mg_val += white_mg - black_mg;
    eg_val += white_eg - black_eg;
}

void eval_passed_pawn(Board_t *board, int sq, int color) {
    int turn = TURN(board);
    int wk = LS1B(WHITE_KING(board));
    int bk = LS1B(BLACK_KING(board));
    
    if (color == WHITE) {
        mg_val += Passed_Pawn[RANK(sq)][MID_GAME];
        eg_val += Passed_Pawn[RANK(sq)][END_GAME];
        
        if ((RANK(sq) == 5 || RANK(sq) == 6) && !BLACK_MAJOR_AND_MINOR_PIECES(board) &&
            (King_Attacks[FILE(sq) + 56] & WHITE_KING(board)))
                eg_val += Unstoppable_Passer_Value;
        else if (!PIECE_ON_SQUARE(sq + 8, board)) {
            SET_TURN(board, WHITE);
            if (see(CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, sq+8, sq), board) >= 0)
                eg_val += Free_Passer_Value;
            
            SET_TURN(board, turn);
        }
        
        eg_val += King_Dist_Passed_Pawn_Value[DIST(sq+8, wk)];
        eg_val -= King_Dist_Passed_Pawn_Value[DIST(sq+8, bk)];
        
        
    } else {
        mg_val -= Passed_Pawn[7 - RANK(sq)][MID_GAME];
        eg_val -= Passed_Pawn[7 - RANK(sq)][END_GAME];
        
        if ((RANK(sq) == 1 || RANK(sq) == 2) && !WHITE_MAJOR_AND_MINOR_PIECES(board) &&
            (King_Attacks[FILE(sq)] & BLACK_KING(board)))
            eg_val -= Unstoppable_Passer_Value;
        else if (!PIECE_ON_SQUARE(sq - 8, board)) {
            SET_TURN(board, BLACK);
            if (see(CREATE_MOVE(NON_CAPTURE, FLAG_NONE, PAWN, sq-8, sq), board) >= 0)
                eg_val -= Free_Passer_Value;
            
            SET_TURN(board, turn);
        }
        
        eg_val -= King_Dist_Passed_Pawn_Value[DIST(sq-8, bk)];
        eg_val += King_Dist_Passed_Pawn_Value[DIST(sq-8, wk)];
    }
}

void eval_kings(Board_t *board, BB attacks[2][7]) {
    int ptype;
    int wk = LS1B(WHITE_KING(board));
    int bk = LS1B(BLACK_KING(board));
    int attack_weight_white = 0;
    int attack_weight_black = 0;
    int q_shelter, k_shelter, curr_shelter;
    
    
    BB bb;
    
    // king location values
    mg_val += P_VALUE(KING, WHITE, wk, MID_GAME) - P_VALUE(KING, BLACK, bk, MID_GAME);
    eg_val += P_VALUE(KING, WHITE, wk, END_GAME) - P_VALUE(KING, BLACK, bk, END_GAME);
    
    
    // attacks to squares next to the king
    for (ptype = PAWN; ptype < KING; ptype++) {
        bb = attacks[BLACK][ptype] & King_Attacks[wk];
        for (; bb ; bb &= (bb-1))
            attack_weight_white += Piece_Weights[ptype];
        
        bb = attacks[WHITE][ptype] & King_Attacks[bk];
        for (; bb ; bb &= (bb-1))
            attack_weight_black += Piece_Weights[ptype];
    }
    
    if (attack_weight_white > 31)
        attack_weight_white = 31;
    if (attack_weight_black > 31)
        attack_weight_black = 31;
    
    mg_val += King_Attacked_Values[MID_GAME][attack_weight_white] - King_Attacked_Values[MID_GAME][attack_weight_black];
    eg_val += King_Attacked_Values[END_GAME][attack_weight_white] - King_Attacked_Values[END_GAME][attack_weight_black];
    
    // pawn shelters for the white king
    curr_shelter = eval_pawn_shelter(board, wk, WHITE);
    q_shelter = WHITE_CAN_CASTLE_QUEENSIDE(board) ? eval_pawn_shelter(board, C1, WHITE) : -VALUE_INFINITE;
    k_shelter = WHITE_CAN_CASTLE_KINGSIDE(board) ? eval_pawn_shelter(board, G1, WHITE) : -VALUE_INFINITE;
    
    if ((k_shelter >= q_shelter) && (k_shelter >= curr_shelter))
        curr_shelter += k_shelter;
    else if ((q_shelter >= k_shelter) && (q_shelter >= curr_shelter))
        curr_shelter += q_shelter;
    else
        curr_shelter += curr_shelter;
    
    mg_val += curr_shelter / 2;
    
    // pawn shelters for the black king
    curr_shelter = eval_pawn_shelter(board, bk, BLACK);
    q_shelter = BLACK_CAN_CASTLE_QUEENSIDE(board) ? eval_pawn_shelter(board, C8, BLACK) : -VALUE_INFINITE;
    k_shelter = BLACK_CAN_CASTLE_KINGSIDE(board) ? eval_pawn_shelter(board, G8, BLACK) : -VALUE_INFINITE;
    
    if ((k_shelter >= q_shelter) && (k_shelter >= curr_shelter))
        curr_shelter += k_shelter;
    else if ((q_shelter >= k_shelter) && (q_shelter >= curr_shelter))
        curr_shelter += q_shelter;
    else
        curr_shelter += curr_shelter;
    
    mg_val -= curr_shelter / 2;
    
}

int eval_pawn_shelter(Board_t *board, int sq, int color) {
    int val = 0;
    int file = FILE(sq);
    int i;
    BB bb;
    
    if (color == WHITE) {
        if (file != 0) {
            for (i = sq + 7; i < 56; i += 8) {
                bb = SQ_MASK(i);
                if (bb & WHITE_PAWNS(board))
                    val += Pawn_Shelter_Value[RANK(i) - RANK(sq)];
                else if (bb & BLACK_PAWNS(board))
                    val += Pawn_Storm_Value[RANK(i) - RANK(sq)];
            }
        }
        
        if (file != 7) {
            for (i = sq + 9; i < 56; i += 8) {
                bb = SQ_MASK(i);
                if (bb & WHITE_PAWNS(board))
                    val += Pawn_Shelter_Value[RANK(i) - RANK(sq)];
                else if (bb & BLACK_PAWNS(board))
                    val += Pawn_Storm_Value[RANK(i) - RANK(sq)];
            }
        }
        
        for (i = sq + 8; i < 56; i += 8) {
            bb = SQ_MASK(i);
            if (bb & WHITE_PAWNS(board))
                val += 2 * Pawn_Shelter_Value[RANK(i) - RANK(sq)];
            else if (bb & BLACK_PAWNS(board))
                val += Pawn_Storm_Value[RANK(i) - RANK(sq)];
        }
        
    } else {
        if (file != 0) {
            for (i = sq - 9; i >= 8; i -= 8) {
                bb = SQ_MASK(i);
                if (bb & BLACK_PAWNS(board))
                    val += Pawn_Shelter_Value[RANK(sq) - RANK(i)];
                else if (bb & WHITE_PAWNS(board))
                    val += Pawn_Storm_Value[RANK(sq) - RANK(i)];
            }
        }
        
        if (file != 7) {
            for (i = sq - 7; i >= 8; i -= 8) {
                bb = SQ_MASK(i);
                if (bb & BLACK_PAWNS(board))
                    val += Pawn_Shelter_Value[RANK(sq) - RANK(i)];
                else if (bb & WHITE_PAWNS(board))
                    val += Pawn_Storm_Value[RANK(sq) - RANK(i)];
            }
        }
        
        for (i = sq - 8; i >= 8; i -= 8) {
            bb = SQ_MASK(i);
            if (bb & BLACK_PAWNS(board))
                val += 2 * Pawn_Shelter_Value[RANK(sq) - RANK(i)];
            else if (bb & WHITE_PAWNS(board))
                val += Pawn_Storm_Value[RANK(sq) - RANK(i)];
        }

    }
    
    return val;
}

int count_material(Board_t *board) {
    int material = 0;
    int ptype, color;
    for (color = WHITE; color <= BLACK; color++) {
        for (ptype = PAWN; ptype < KING; ptype++) {
            material += Piece_Weights[ptype] * count_bits(PIECES_OF_TYPE(board, ptype, color));
        }
    }
    
    return material;
}

int is_draw(Board_t *board) {
    if (board->movesSinceCaptureORPawn >= 100)
        return 1;
    
    if (global_board_stack()->ply > 5) {
        int ply = global_board_stack()->ply;
        Board_t *stack = global_board_stack()->stack;
        Hash_t curr_hash = board->hash;
        if (stack[ply-1].hash == curr_hash || stack[ply-2].hash == curr_hash || stack[ply-3].hash == curr_hash || stack[ply-4].hash == curr_hash || stack[ply-5].hash == curr_hash)
            return 1;
    }
    
    return 0;
}

int global_board_is_draw() {
    return is_draw(global_board());
}

int probe_pawn_table(BB pawns, int color, int *mg_score, int *eg_score) {
    int index = pawns % PAWN_TABLE_SIZE;
    if (pawn_table.positions[color][index] != pawns)
        return PAWN_TABLE_MISS;
    
    *mg_score = pawn_table.mg_scores[color][index];
    *eg_score = pawn_table.eg_scores[color][index];
    
    return PAWN_TABLE_HIT;
}

void pawn_table_store_entry(BB pawns, int color, int mg_score, int eg_score) {
    
    int index = pawns % PAWN_TABLE_SIZE;
    
    pawn_table.positions[color][index] = pawns;
    pawn_table.mg_scores[color][index] = mg_score;
    pawn_table.eg_scores[color][index] = eg_score;
}

void init_pawn_table() {
    int i;
    for (i = 0; i < PAWN_TABLE_SIZE; i++) {
        pawn_table.positions[WHITE][i] = 0;
        pawn_table.positions[BLACK][i] = 0;
    }
}