#include "attacks.h"

#include <cstring>
#include <iostream>

U64 pawn_attacks[2][64]; // Initialize the pawn attack table
U64 mask_pawn_attacks(Color color, Square square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    set_bit(bitboard, square);

    if (color == Color::white) {
        attacks = north_east(bitboard) | north_west(bitboard);
    } else {
        attacks = south_east(bitboard) | south_west(bitboard);
    }
    return attacks;
}
U64 knight_attacks[64]; // Initialize the knight attack table
U64 mask_knight_attacks(Square square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);
    attacks = north_west(north(bitboard)) | north_east(north(bitboard))
            | north_west(west(bitboard))  | north_east(east(bitboard))
            | south_west(west(bitboard))  | south_east(east(bitboard))
            | south_west(south(bitboard)) | south_east(south(bitboard));
    
    return attacks;
}

U64 king_attacks[64]; // Initialize the king attack table
U64 mask_king_attacks(Square square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);
    attacks = north_west(bitboard) | north(bitboard) | north_east(bitboard)
            | west(bitboard)                |                east(bitboard)
            | south_west(bitboard) | south(bitboard) | south_east(bitboard);
    
    return attacks;
}

void init_leapers_attacks() {
    for (Square square = Square::a8; square < Square::no_sq; square+=1) {
        pawn_attacks[Color::white][square] = mask_pawn_attacks(Color::white, square);
        pawn_attacks[Color::black][square] = mask_pawn_attacks(Color::black, square);
        knight_attacks[square]      = mask_knight_attacks(square);
        king_attacks[square]        = mask_king_attacks(square);
    }
}

static U64 bishop_masks[64];
static U64 rook_masks[64];
static U64 bishop_attacks[64][512];
static U64 rook_attacks[64][4096];

U64 mask_bishop_attacks(Square square){
    U64 attacks = 0ULL;
    uint8_t r, f;
    uint8_t tr = static_cast<uint16_t>(square) / 8; // target rank
    uint8_t tf = static_cast<uint16_t>(square) % 8; // target file

    for (r = tr + 1, f = tf + 1; r < 7 && f < 7; ++r, ++f) set_bit(attacks, Square(8*r + f));
    for (r = tr - 1, f = tf + 1; r > 0 && f < 7; --r, ++f) set_bit(attacks, Square(8*r + f));
    for (r = tr + 1, f = tf - 1; r < 7 && f > 0; ++r, --f) set_bit(attacks, Square(8*r + f));
    for (r = tr - 1, f = tf - 1; r > 0 && f > 0; --r, --f) set_bit(attacks, Square(8*r + f));
    
    return attacks;
}

U64 mask_rook_attacks(Square square){
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

U64 bishop_attacks_on_the_fly(Square square, U64 block){
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

U64 rook_attacks_on_the_fly(Square square, U64 block){
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

U64 set_occupancy(uint16_t index, uint8_t bits_in_mask, U64 attack_mask){
    U64 occupancy = 0ULL;
    int square;

    for (uint8_t count = 0; count < bits_in_mask; ++count){
        square = get_LSB(attack_mask);
        pop_LSB(attack_mask);
        if (index & (1 << count)) set_bit(occupancy, square);
    }
    return occupancy;
}

// pseudo random number state 
static uint32_t random_state = 1804289383;

// generate 32-bit pseudo legal numbers (XORSHIFT32)
uint32_t get_random_U32_number(){
    uint32_t number = random_state;
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;
    
    random_state = number;

    return number;
}

// generate 64-bit pseudo legal numbers
U64 get_random_U64_number(){
    U64 n1, n2, n3, n4;

    // Slicing 16 bits from MSB side (fisrt conversion)
    n1 = (U64) get_random_U32_number() & 0xFFFF;
    n2 = (U64) get_random_U32_number() & 0xFFFF;
    n3 = (U64) get_random_U32_number() & 0xFFFF;
    n4 = (U64) get_random_U32_number() & 0xFFFF;

    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

U64 generate_magic_number(){
    return get_random_U64_number() & get_random_U64_number() & get_random_U64_number();
}

U64 find_magic_number(Square square, uint8_t relevant_bits){ // 4096 neccesary for bishops?
    
    bool bishop = relevant_bits < 10;
    U64 occupacies[4096];

    U64 attacks[4096];
    U64 used_attacks[4096];
    U64 attack_mask = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

    uint16_t occupancy_indicies = 1 << relevant_bits; // max relevant bit is 12, possible scenarious
    for (uint16_t index = 0; index < occupancy_indicies; ++index){
        occupacies[index] = set_occupancy(index, relevant_bits, attack_mask);

        attacks[index] = bishop ? bishop_attacks_on_the_fly(square, occupacies[index]) : 
                                    rook_attacks_on_the_fly(square, occupacies[index]) ;
    }

    for (uint32_t random_count = 0; random_count < 100000000; ++random_count){
        // candidate for magic number
        U64 magic_number = generate_magic_number();

        // skip inappropiaet magic numbers
        if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;

        std::memset(used_attacks, 0ULL, sizeof(used_attacks));

        int index, fail;
        for (index = 0, fail = 0; !fail && index < occupancy_indicies; ++index){
            int magic_index = (int)((occupacies[index] * magic_number) >> (64 - relevant_bits));

            if (used_attacks[magic_index] == 0ULL) used_attacks[magic_index] = attacks[index];
            else if (used_attacks[magic_index] != attacks[index]) fail = 1;

        }
        if(!fail) return magic_number;

    }
    // magic number doesn't work
    std::cout << "magic number doesn't work\n";
    return 0ULL;
}

void init_sliders_attacks(PieceType piece_type){
    for (uint8_t square = 0; square < 64; square++){
        U64 attack_mask;
        if (piece_type == PieceType::bishop){
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
            
            if (piece_type == PieceType::bishop){
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - relevant_bits_count);
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            } else{
                int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - relevant_bits_count);
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
            }
        }
    }
}

U64 get_bishop_attacks(Square square, U64 occupancy){
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bishop_relevant_bits[square]; // magic number

    return bishop_attacks[square][occupancy];
}

U64 get_rook_attacks(Square square, U64 occupancy){
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rook_relevant_bits[square]; // magic number

    return rook_attacks[square][occupancy];
}

U64 get_queen_attacks(Square square, U64 occupancy){
    U64 bishop_occupancy = occupancy, rook_occupancy = occupancy;
    
    bishop_occupancy &= bishop_masks[square];
    bishop_occupancy *= bishop_magic_numbers[square];
    bishop_occupancy >>= 64 - bishop_relevant_bits[square]; // magic number

    rook_occupancy &= rook_masks[square];
    rook_occupancy *= rook_magic_numbers[square];
    rook_occupancy >>= 64 - rook_relevant_bits[square]; // magic number

    return bishop_attacks[square][bishop_occupancy] | rook_attacks[square][rook_occupancy];
}
