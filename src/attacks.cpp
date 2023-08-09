#include "attacks.h"
#include "bitboard.h"
#include "magic.h"

U64 pawn_attacks[2][64];

U64 mask_pawn_attacks(int color, int square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);

    if (!color) attacks = north_east(bitboard) | north_west(bitboard); // white
    else        attacks = south_east(bitboard) | south_west(bitboard); // black

    return attacks;
}

U64 knight_attacks[64];
// could change north2_west...
U64 mask_knight_attacks(int square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);
    attacks = north_west(north(bitboard)) | north_east(north(bitboard))
            | north_west(west(bitboard))  | north_east(east(bitboard))
            | south_west(west(bitboard))  | south_east(east(bitboard))
            | south_west(south(bitboard)) | south_east(south(bitboard));
    
    return attacks;
}

U64 king_attacks[64];

U64 mask_king_attacks(int square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);
    attacks = north_west(bitboard) | north(bitboard) | north_east(bitboard)
            | west(bitboard)                |                east(bitboard)
            | south_west(bitboard) | south(bitboard) | south_east(bitboard);
    
    return attacks;
}

void init_leapers_attacks(){
    // loop over all 64 squares, white and black
    for (int square = 0; square < 64; ++square){
        pawn_attacks[white][square] = mask_pawn_attacks(white, square);
        pawn_attacks[black][square] = mask_pawn_attacks(black, square);
        knight_attacks[square]      = mask_knight_attacks(square);
        king_attacks[square]        = mask_king_attacks(square);

    }
    
}

// Relevant
U64 mask_bishop_attacks(int square){
    U64 attacks = 0ULL;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r < 7 && f < 7; ++r, ++f) set_bit(attacks, 8*r + f);
    for (r = tr - 1, f = tf + 1; r > 0 && f < 7; --r, ++f) set_bit(attacks, 8*r + f);
    for (r = tr + 1, f = tf - 1; r < 7 && f > 0; ++r, --f) set_bit(attacks, 8*r + f);
    for (r = tr - 1, f = tf - 1; r > 0 && f > 0; --r, --f) set_bit(attacks, 8*r + f);
    
    return attacks;
}

// Relevant
U64 mask_rook_attacks(int square){
    U64 attacks = 0ULL;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1; r < 7; ++r) set_bit(attacks, 8*r + tf);
    for (r = tr - 1; r > 0; --r) set_bit(attacks, 8*r + tf);
    for (f = tf + 1; f < 7; ++f) set_bit(attacks, 8*tr + f);
    for (f = tf - 1; f > 0; --f) set_bit(attacks, 8*tr + f);
    
    return attacks;
}

U64 bishop_attacks_on_the_fly(int square, U64 block){
    U64 attacks = 0ULL;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r < 8 && f < 8; ++r, ++f){
        set_bit(attacks, 8*r + f);
        if ((1ULL << (8*r + f)) & block) break;
    } 
    for (r = tr - 1, f = tf + 1; r >=0 && f < 8; --r, ++f){
        set_bit(attacks, 8*r + f);
        if ((1ULL << (8*r + f)) & block) break;
    }
    for (r = tr + 1, f = tf - 1; r < 8 && f >=0; ++r, --f){
        set_bit(attacks, 8*r + f);
        if ((1ULL << (8*r + f)) & block) break;
    }
    for (r = tr - 1, f = tf - 1; r >=0 && f >=0; --r, --f){
        set_bit(attacks, 8*r + f);
        if ((1ULL << (8*r + f)) & block) break;
    }
    return attacks;
}

U64 rook_attacks_on_the_fly(int square, U64 block){
    U64 attacks = 0ULL;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1; r < 8; ++r){
        set_bit(attacks, 8*r + tf);
        if ((1ULL << (8*r + tf)) & block) break;
    } 
    for (r = tr - 1; r >= 0; --r){
        set_bit(attacks, 8*r + tf);
        if ((1ULL << (8*r + tf)) & block) break;
    } 
    for (f = tf + 1; f < 8; ++f){
        set_bit(attacks, 8*tr + f);
        if ((1ULL << (8*tr + f)) & block) break;
    } 
    for (f = tf - 1; f >= 0; --f){
        set_bit(attacks, 8*tr + f);
        if ((1ULL << (8*tr + f)) & block) break;
    }
    return attacks;
}

//?
U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask){
    U64 occupancy = 0ULL;
    int square;

    for (int count = 0; count < bits_in_mask; ++count){
        square = get_LSB(attack_mask);
        pop_LSB(attack_mask);
        if (index & (1 << count)) set_bit(occupancy, square);
    }
    return occupancy;
}

U64 bishop_masks[64];
U64 rook_masks[64];

// [square][occupancies]
U64 bishop_attacks[64][512];

// [square][occupancies]
U64 rook_attacks[64][4096];

void init_sliders_attacks(int bishop){
    for (int square = 0; square < 64; square++){
        U64 attack_mask;
        if (bishop){
            bishop_masks[square] = mask_bishop_attacks(square);
            attack_mask = bishop_masks[square];
        }else{
            rook_masks[square] = mask_rook_attacks(square);
            attack_mask = rook_masks[square];
        }

        int relevant_bits_count = count_bits(attack_mask);
        //int relevant_bits_count = bishop ? bishop_relevant_bits[square] : rook_relevant_bits[square];
        int occupancy_indicies = 1 << relevant_bits_count;

        for (int index = 0; index < occupancy_indicies; index++){
            U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
            
            if (bishop){
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - relevant_bits_count);
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            } else{
                int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - relevant_bits_count);
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
            }
        }
    }
}

U64 get_bishop_attacks(int square, U64 occupancy){
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bishop_relevant_bits[square]; // magic number

    return bishop_attacks[square][occupancy];
}

U64 get_rook_attacks(int square, U64 occupancy){
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rook_relevant_bits[square]; // magic number

    return rook_attacks[square][occupancy];
}

U64 get_queen_attacks(int square, U64 occupancy){
    U64 bishop_occupancy = occupancy, rook_occupancy = occupancy;
    
    bishop_occupancy &= bishop_masks[square];
    bishop_occupancy *= bishop_magic_numbers[square];
    bishop_occupancy >>= 64 - bishop_relevant_bits[square]; // magic number

    rook_occupancy &= rook_masks[square];
    rook_occupancy *= rook_magic_numbers[square];
    rook_occupancy >>= 64 - rook_relevant_bits[square]; // magic number

    return bishop_attacks[square][bishop_occupancy] | rook_attacks[square][rook_occupancy];
}

void init_all(){
    init_leapers_attacks();
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
}