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
        if (make_square(f, tr) & block) break;
    } 
    for (f = File(tf - 1); f >= FILE_A; --f){
        set_bit(attacks, make_square(f, tr));
        if (make_square(f, tr) & block) break;
    }
    return attacks;
}

Bitboard set_occupancy(uint16_t index, Bitboard attack_mask){
    Bitboard occupancy = 0ULL;
    Square square;

    /* for (uint8_t count = 0; count < bits_in_mask; ++count){
        square = get_LSB(attack_mask);
        pop_LSB(attack_mask);
        if (index & (1 << count)) set_bit(occupancy, square);
    } */
    uint8_t count = 0;
    while (attack_mask){
        square = get_LSB(attack_mask);
        pop_LSB(attack_mask);
        if (index & (1 << count)) set_bit(occupancy, square);
        count++;
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
uint64_t get_random_Bitboard_number(){
    uint64_t n1, n2, n3, n4;

    // Slicing 16 bits from MSB side (fisrt conversion)
    n1 = (uint64_t) get_random_U32_number() & 0xFFFF;
    n2 = (uint64_t) get_random_U32_number() & 0xFFFF;
    n3 = (uint64_t) get_random_U32_number() & 0xFFFF;
    n4 = (uint64_t) get_random_U32_number() & 0xFFFF;

    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

uint64_t generate_magic_number(){
    return get_random_Bitboard_number() & get_random_Bitboard_number() & get_random_Bitboard_number();
}

uint64_t find_magic_number(Square square, uint8_t relevant_bits){ // 4096 neccesary for bishops?
    
    bool bishop = relevant_bits < 10;
    uint16_t occupancy_indicies = 1 << relevant_bits; // max relevant bit is 12, possible scenarious
    
    Bitboard occupacies[occupancy_indicies];
    Bitboard attacks[occupancy_indicies];
    Bitboard used_attacks[occupancy_indicies];

    Bitboard attack_mask = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

    
    for (uint16_t index = 0; index < occupancy_indicies; ++index){
        occupacies[index] = set_occupancy(index, attack_mask);

        attacks[index] = bishop ? bishop_attacks_on_the_fly(square, occupacies[index]) : 
                                    rook_attacks_on_the_fly(square, occupacies[index]) ;
    }

    for (uint32_t random_count = 0; random_count < 100000000; ++random_count){
        // candidate for magic number
        uint64_t magic_number = generate_magic_number();

        // skip inappropiaet magic numbers
        if (popcnt((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;

        std::memset(used_attacks, 0ULL, sizeof(used_attacks));

        int index, fail;
        for (index = 0, fail = 0; !fail && index < occupancy_indicies; ++index){
            int magic_index = (int)((occupacies[index] * magic_number) >> (64 - relevant_bits));

            if (used_attacks[magic_index] == 0) used_attacks[magic_index] = attacks[index];
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

        int relevant_bits_count = popcnt(attack_mask);
        //int relevant_bits_count = bishop ? bishop_relevant_bits[square] : rook_relevant_bits[square];
        int occupancy_indicies = 1 << relevant_bits_count;

        for (int index = 0; index < occupancy_indicies; index++){
            Bitboard occupancy = set_occupancy(index, attack_mask);
            
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

bool is_square_attacked(Board &board, Square square, Color side){

    if ((pawn_attacks[~side][square] & board.bitboards[make_index_piece(side,PAWN)])!=0) return true; 
    if ((knight_attacks[square] & board.bitboards[make_index_piece(side,KNIGHT)])!=0) return true;
    if ((king_attacks[square] & board.bitboards[make_index_piece(side,KING)])!=0) return true;

    if ((get_bishop_attacks(square, board.occupancies[BOTH]) & board.bitboards[make_index_piece(side,BISHOP)])!=0) return true;
    if ((get_rook_attacks(square, board.occupancies[BOTH]) & board.bitboards[make_index_piece(side,ROOK)])!=0) return true;
    if ((get_queen_attacks(square, board.occupancies[BOTH]) & board.bitboards[make_index_piece(side,QUEEN)])!=0) return true;

    return false;
}