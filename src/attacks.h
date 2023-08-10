#ifndef ATTACKS_H
#define ATTACKS_H

#include "types.h"

// Pawn attack table
extern U64 pawn_attacks[2][64];
U64 mask_pawn_attacks(int color, int square);

// knight attack table
extern U64 knight_attacks[64];
U64 mask_knight_attacks(int square);

// King attack table
extern U64 king_attacks[64];
U64 mask_king_attacks(int square);

void init_leapers_attacks();

U64 mask_bishop_attacks(int square);
U64 mask_rook_attacks(int square);

U64 bishop_attacks_on_the_fly(int square, U64 block);
U64 rook_attacks_on_the_fly(int square, U64 block);

U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask);

extern U64 bishop_masks[64];
extern U64 rook_masks[64];
extern U64 bishop_attacks[64][512];
extern U64 rook_attacks[64][4096];

// Relevant occupancy bit count
constexpr int bishop_relevant_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

// Relevant occupancy bit count
constexpr int rook_relevant_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

void init_sliders_attacks(int bishop);

U64 get_bishop_attacks(int square, U64 occupancy);
U64 get_rook_attacks(int square, U64 occupancy);
U64 get_queen_attacks(int square, U64 occupancy);

inline int is_square_attacked(int square, int side){
    int offset = 6*side;

    if ((pawn_attacks[side^1][square] & bitboards[P+offset])!=0) return 1; // P+offset = side pawn
    if ((knight_attacks[square] & bitboards[N+offset])!=0) return 1;
    if ((king_attacks[square] & bitboards[K+offset])!=0) return 1;

    if ((get_bishop_attacks(square, occupancies[both]) & bitboards[B+offset])!=0) return 1;
    if ((get_rook_attacks(square, occupancies[both]) & bitboards[R+offset])!=0) return 1;
    if ((get_queen_attacks(square, occupancies[both]) & bitboards[Q+offset])!=0) return 1;
    
    return 0;
}


void init_all();

#endif