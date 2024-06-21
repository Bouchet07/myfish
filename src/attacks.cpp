#include "attacks.h"

#include <cstring>
#include <iostream>

Bitboard pawn_attacks[2][64]; // Initialize the pawn attack table
Bitboard mask_pawn_attacks(Color color, Square square) {
    Bitboard attacks = 0ULL;
    Bitboard bitboard = 0ULL;
    set_bit(bitboard, square);

    if (color == WHITE) {
        attacks = shift<NORTH_EAST>(bitboard) | shift<NORTH_WEST>(bitboard);
    } else {
        attacks = shift<SOUTH_EAST>(bitboard)  | shift<SOUTH_WEST>(bitboard);
    }
    return attacks;
}
Bitboard knight_attacks[64]; // Initialize the knight attack table
Bitboard mask_knight_attacks(Square square){
    Bitboard attacks = 0ULL;
    Bitboard bitboard = 0ULL;

    set_bit(bitboard, square);
    attacks = shift<NORTH_WEST>(shift<NORTH>(bitboard)) | shift<NORTH_EAST>(shift<NORTH>(bitboard))
            | shift<NORTH_WEST>(shift<WEST>(bitboard))  | shift<NORTH_EAST>(shift<EAST>(bitboard))
            | shift<SOUTH_WEST>(shift<WEST>(bitboard))  | shift<SOUTH_EAST>(shift<EAST>(bitboard)) 
            | shift<SOUTH_WEST>(shift<SOUTH>(bitboard)) | shift<SOUTH_EAST>(shift<SOUTH>(bitboard));
    
    return attacks;
}

Bitboard king_attacks[64]; // Initialize the king attack table
Bitboard mask_king_attacks(Square square){
    Bitboard attacks = 0ULL;
    Bitboard bitboard = 0ULL;

    set_bit(bitboard, square);

    attacks = shift<NORTH_WEST>(bitboard) | shift<NORTH>(bitboard) | shift<NORTH_EAST>(bitboard)
            | shift<WEST>(bitboard)                   |              shift<EAST>(bitboard)
            | shift<SOUTH_WEST>(bitboard) | shift<SOUTH>(bitboard) | shift<SOUTH_EAST>(bitboard);
    
    return attacks;
}

void init_leapers_attacks() {
    for (Square square = Square::SQ_A1; square < Square::SQUARE_NB; ++square) {
        pawn_attacks[WHITE][square] = mask_pawn_attacks(WHITE, square);
        pawn_attacks[BLACK][square] = mask_pawn_attacks(BLACK, square);
        knight_attacks[square]      = mask_knight_attacks(square);
        king_attacks[square]        = mask_king_attacks(square);
    }
}

static Bitboard bishop_masks[64];
static Bitboard rook_masks[64];
static Bitboard bishop_attacks[64][512];
static Bitboard rook_attacks[64][4096];

Bitboard mask_bishop_attacks(Square square){
    Bitboard attacks = 0ULL;
    Rank r; File f;
    Rank tr = rank_of(square); // target rank
    File tf = file_of(square); // target file

    for (r = Rank(tr + 1), f = File(tf + 1); r < RANK_8 && f < FILE_H; ++r, ++f) set_bit(attacks, make_square(f, r));
    for (r = Rank(tr - 1), f = File(tf + 1); r > RANK_1 && f < FILE_H; --r, ++f) set_bit(attacks, make_square(f, r));
    for (r = Rank(tr + 1), f = File(tf - 1); r < RANK_8 && f > FILE_A; ++r, --f) set_bit(attacks, make_square(f, r));
    for (r = Rank(tr - 1), f = File(tf - 1); r > RANK_1 && f > FILE_A; --r, --f) set_bit(attacks, make_square(f, r));
    
    return attacks;
}

Bitboard mask_rook_attacks(Square square){
    Bitboard attacks = 0ULL;
    Rank r; File f;
    Rank tr = rank_of(square); // target rank
    File tf = file_of(square); // target file

    for (r = Rank(tr + 1); r < RANK_8; ++r) set_bit(attacks, make_square(tf, r));
    for (r = Rank(tr - 1); r > RANK_1; --r) set_bit(attacks, make_square(tf, r));
    for (f = File(tf + 1); f < FILE_H; ++f) set_bit(attacks, make_square(f, tr));
    for (f = File(tf - 1); f > FILE_A; --f) set_bit(attacks, make_square(f, tr));
    
    return attacks;
}

Bitboard bishop_attacks_on_the_fly(Square square, Bitboard block){
    Bitboard attacks = 0ULL;
    Rank r; File f;
    Rank tr = rank_of(square); // target rank
    File tf = file_of(square); // target file
    
    for (r = Rank(tr + 1), f = File(tf + 1); r < RANK_NB && f < FILE_NB; ++r, ++f) {
        set_bit(attacks, make_square(f, r));
        if (make_square(f, r) & block) break;
    } 
    for (r = Rank(tr - 1), f = File(tf + 1); r >= RANK_1 && f < FILE_NB; --r, ++f){
        set_bit(attacks, make_square(f, r));
        if (make_square(f, r) & block) break;
    }
    for (r = Rank(tr + 1), f = File(tf - 1); r < RANK_NB && f >= FILE_A; ++r, --f){
        set_bit(attacks, make_square(f, r));
        if (make_square(f, r) & block) break;
    }
    for (r = Rank(tr - 1), f = File(tf - 1); r >= RANK_1 && f >= FILE_A; --r, --f){
        set_bit(attacks, make_square(f, r));
        if (make_square(f, r) & block) break;
    }
    return attacks;
}


Bitboard rook_attacks_on_the_fly(Square square, Bitboard block){
    Bitboard attacks = 0ULL;
    Rank r; File f;
    Rank tr = rank_of(square); // target rank
    File tf = file_of(square); // target file

    for (r = Rank(tr + 1); r < RANK_NB; ++r){
        set_bit(attacks, make_square(tf, r));
        if (make_square(tf, r) & block) break;
    } 
    for (r = Rank(tr - 1); r >= RANK_1; --r){
        set_bit(attacks, make_square(tf, r));
        if (make_square(tf, r) & block) break;
    } 
    for (f = File(tf + 1); f < FILE_NB; ++f){
        set_bit(attacks, make_square(f, tr));
        if (make_square(tf, r) & block) break;
    } 
    for (f = File(tf - 1); f >= FILE_A; --f){
        set_bit(attacks, make_square(f, tr));
        if (make_square(tf, r) & block) break;
    }
    return attacks;
}

Bitboard set_occupancy(uint16_t index, uint8_t bits_in_mask, Bitboard attack_mask){
    Bitboard occupancy = 0ULL;
    Square square;

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
Bitboard get_random_Bitboard_number(){
    Bitboard n1, n2, n3, n4;

    // Slicing 16 bits from MSB side (fisrt conversion)
    n1 = (Bitboard) get_random_U32_number() & 0xFFFF;
    n2 = (Bitboard) get_random_U32_number() & 0xFFFF;
    n3 = (Bitboard) get_random_U32_number() & 0xFFFF;
    n4 = (Bitboard) get_random_U32_number() & 0xFFFF;

    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

Bitboard generate_magic_number(){
    return get_random_Bitboard_number() & get_random_Bitboard_number() & get_random_Bitboard_number();
}

Bitboard find_magic_number(Square square, uint8_t relevant_bits){ // 4096 neccesary for bishops?
    
    bool bishop = relevant_bits < 10;
    Bitboard occupacies[4096];

    Bitboard attacks[4096];
    Bitboard used_attacks[4096];
    Bitboard attack_mask = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

    uint16_t occupancy_indicies = 1 << relevant_bits; // max relevant bit is 12, possible scenarious
    for (uint16_t index = 0; index < occupancy_indicies; ++index){
        occupacies[index] = set_occupancy(index, relevant_bits, attack_mask);

        attacks[index] = bishop ? bishop_attacks_on_the_fly(square, occupacies[index]) : 
                                    rook_attacks_on_the_fly(square, occupacies[index]) ;
    }

    for (uint32_t random_count = 0; random_count < 100000000; ++random_count){
        // candidate for magic number
        Bitboard magic_number = generate_magic_number();

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
    for (Square square = SQUARE_ZERO; square < SQUARE_NB; ++square){
        Bitboard attack_mask;
        if (piece_type == PieceType::BISHOP){
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
            Bitboard occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
            
            if (piece_type == PieceType::BISHOP){
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - relevant_bits_count);
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            } else{
                int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - relevant_bits_count);
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
            }
        }
    }
}

Bitboard get_bishop_attacks(Square square, Bitboard occupancy){
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bishop_relevant_bits[square]; // magic number

    return bishop_attacks[square][occupancy];
}

Bitboard get_rook_attacks(Square square, Bitboard occupancy){
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rook_relevant_bits[square]; // magic number

    return rook_attacks[square][occupancy];
}

Bitboard get_queen_attacks(Square square, Bitboard occupancy){
    Bitboard bishop_occupancy = occupancy, rook_occupancy = occupancy;
    
    bishop_occupancy &= bishop_masks[square];
    bishop_occupancy *= bishop_magic_numbers[square];
    bishop_occupancy >>= 64 - bishop_relevant_bits[square]; // magic number

    rook_occupancy &= rook_masks[square];
    rook_occupancy *= rook_magic_numbers[square];
    rook_occupancy >>= 64 - rook_relevant_bits[square]; // magic number

    return bishop_attacks[square][bishop_occupancy] | rook_attacks[square][rook_occupancy];
}
