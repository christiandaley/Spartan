//
//  bitmasks.h
//  Spartan
//
//
//
//

#ifndef bitmasks_h
#define bitmasks_h

#include "board.h"

#define A_FILE 0x0101010101010101
#define B_FILE 0x0202020202020202
#define C_FILE 0x0404040404040404
#define D_FILE 0x0808080808080808
#define E_FILE 0x1010101010101010
#define F_FILE 0x2020202020202020
#define G_FILE 0x4040404040404040
#define H_FILE 0x8080808080808080

#define RANK_1 0x00000000000000ff
#define RANK_2 0x000000000000ff00
#define RANK_3 0x0000000000ff0000
#define RANK_4 0x00000000ff000000
#define RANK_5 0x000000ff00000000
#define RANK_6 0x0000ff0000000000
#define RANK_7 0x00ff000000000000
#define RANK_8 0xff00000000000000

#define EDGES (A_FILE | H_FILE | RANK_1 | RANK_8)
#define NO_EDGES (~EDGES)

extern const BB Rank_Masks[8];
extern const BB File_Masks[8];
extern const BB White_Pawn_Attacks_To[64];
extern const BB White_Pawn_Attacks_From[64];
extern const BB Black_Pawn_Attacks_To[64];
extern const BB Black_Pawn_Attacks_From[64];
extern const BB White_Pawn_Passed[64];
extern const BB Black_Pawn_Passed[64];
extern const BB White_Pawn_Backwards[64];
extern const BB Black_Pawn_Backwards[64];
extern const BB Pawn_Isolated[8];
extern const BB Knight_Attacks[64];
extern const BB King_Attacks[64];

#define ENEMY_PAWN_ATTACKS_TO(sq, b)    (TURN(b) == WHITE ? Black_Pawn_Attacks_To[sq] : White_Pawn_Attacks_To[sq])
#define FRIENDLY_PAWN_ATTACKS_TO(sq, b) (TURN(b) == WHITE ? White_Pawn_Attacks_To[sq] : Black_Pawn_Attacks_To[sq])


#endif /* bitmasks_h */
