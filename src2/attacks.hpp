#ifndef ATTACKS_H
#define ATTACKS_H

#include "types.hpp"
#include "bitboard.hpp"

#include <cstring>
#include <iostream>

// Pawn attack table
static U64 pawn_attacks[2][64];
inline U64 mask_pawn_attacks(int color, int square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);

    if (!color) attacks = north_east(bitboard) | north_west(bitboard); // white
    else        attacks = south_east(bitboard) | south_west(bitboard); // black

    return attacks;
}

// knight attack table
static U64 knight_attacks[64];
inline U64 mask_knight_attacks(int square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);
    attacks = north_west(north(bitboard)) | north_east(north(bitboard))
            | north_west(west(bitboard))  | north_east(east(bitboard))
            | south_west(west(bitboard))  | south_east(east(bitboard))
            | south_west(south(bitboard)) | south_east(south(bitboard));
    
    return attacks;
}

// King attack table
static U64 king_attacks[64];
inline U64 mask_king_attacks(int square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);
    attacks = north_west(bitboard) | north(bitboard) | north_east(bitboard)
            | west(bitboard)                |                east(bitboard)
            | south_west(bitboard) | south(bitboard) | south_east(bitboard);
    
    return attacks;
}

inline void init_leapers_attacks(){
    // loop over all 64 squares, white and black
    for (int square = 0; square < 64; ++square){
        pawn_attacks[white][square] = mask_pawn_attacks(white, square);
        pawn_attacks[black][square] = mask_pawn_attacks(black, square);
        knight_attacks[square]      = mask_knight_attacks(square);
        king_attacks[square]        = mask_king_attacks(square);

    }
    
}

// Relevant
inline U64 mask_bishop_attacks(int square){
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
inline U64 mask_rook_attacks(int square){
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

inline U64 bishop_attacks_on_the_fly(int square, U64 block){
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
inline U64 rook_attacks_on_the_fly(int square, U64 block){
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
inline U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask){
    U64 occupancy = 0ULL;
    int square;

    for (int count = 0; count < bits_in_mask; ++count){
        square = get_LSB(attack_mask);
        pop_LSB(attack_mask);
        if (index & (1 << count)) set_bit(occupancy, square);
    }
    return occupancy;
}

static U64 bishop_masks[64];
static U64 rook_masks[64];
static U64 bishop_attacks[64][512];
static U64 rook_attacks[64][4096];

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

// pseudo random number state 
static unsigned int random_state = 1804289383;
// generate 32-bit pseudo legal numbers (XORSHIFT32)
inline unsigned int get_random_U32_number(){
    unsigned int number = random_state;
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;
    
    random_state = number;

    return number;
}
// generate 64-bit pseudo legal numbers
inline U64 get_random_U64_number(){
    U64 n1, n2, n3, n4;

    // Slicing 16 bits from MSB side (fisrt conversion)
    n1 = (U64) get_random_U32_number() & 0xFFFF;
    n2 = (U64) get_random_U32_number() & 0xFFFF;
    n3 = (U64) get_random_U32_number() & 0xFFFF;
    n4 = (U64) get_random_U32_number() & 0xFFFF;

    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

inline U64 generate_magic_number(){
    return get_random_U64_number() & get_random_U64_number() & get_random_U64_number();
}
inline U64 find_magic_number(int square, int relevant_bits, int bishop){ // 4096 neccesary for bishops?
    
    U64 occupacies[4096];

    U64 attacks[4096];
    U64 used_attacks[4096];
    U64 attack_mask = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

    int occupancy_indicies = 1 << relevant_bits;
    for (int index = 0; index < occupancy_indicies; ++index){
        occupacies[index] = set_occupancy(index, relevant_bits, attack_mask);

        attacks[index] = bishop ? bishop_attacks_on_the_fly(square, occupacies[index]) : 
                                    rook_attacks_on_the_fly(square, occupacies[index]) ;
    }

    for (int random_count = 0; random_count < 100000000; ++random_count){
        // candidate for magic number
        U64 magic_number = generate_magic_number();

        // skip inappropiaet magic numbers
        if (popcnt((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;

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

constexpr U64 bishop_magic_numbers[64] = {
    0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL
};

constexpr U64 rook_magic_numbers[64] = {
    0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL
};

/* inline void init_magic_numbers(){
    for (int square = 0; square < 64; square++)
        // init rook magic numbers
        rook_magic_numbers[square] = find_magic_number(square, rook_relevant_bits[square], rook);

    for (int square = 0; square < 64; square++)
        // init bishop magic numbers
        bishop_magic_numbers[square] = find_magic_number(square, bishop_relevant_bits[square], bishop);
} */

inline void init_sliders_attacks(int bishop){
    for (int square = 0; square < 64; square++){
        U64 attack_mask;
        if (bishop){
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

inline U64 get_bishop_attacks(int square, U64 occupancy){
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bishop_relevant_bits[square]; // magic number

    return bishop_attacks[square][occupancy];
}

inline U64 get_rook_attacks(int square, U64 occupancy){
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rook_relevant_bits[square]; // magic number

    return rook_attacks[square][occupancy];
}

inline U64 get_queen_attacks(int square, U64 occupancy){
    U64 bishop_occupancy = occupancy, rook_occupancy = occupancy;
    
    bishop_occupancy &= bishop_masks[square];
    bishop_occupancy *= bishop_magic_numbers[square];
    bishop_occupancy >>= 64 - bishop_relevant_bits[square]; // magic number

    rook_occupancy &= rook_masks[square];
    rook_occupancy *= rook_magic_numbers[square];
    rook_occupancy >>= 64 - rook_relevant_bits[square]; // magic number

    return bishop_attacks[square][bishop_occupancy] | rook_attacks[square][rook_occupancy];
}
// is square attacked by side
inline bool is_square_attacked(Board &board, int square, int side){
    //int offset = 6*side; // P+offset = side pawn
    if (side^1){ // white
        if ((pawn_attacks[side^1][square] & board.bitboards[P])!=0) return true; 
        if ((knight_attacks[square] & board.bitboards[N])!=0) return true;
        if ((king_attacks[square] & board.bitboards[K])!=0) return true;

        if ((get_bishop_attacks(square, board.occupancies[both]) & board.bitboards[B])!=0) return true;
        if ((get_rook_attacks(square, board.occupancies[both]) & board.bitboards[R])!=0) return true;
        if ((get_queen_attacks(square, board.occupancies[both]) & board.bitboards[Q])!=0) return true;
    }
    else{ // black
        if ((pawn_attacks[side^1][square] & board.bitboards[p])!=0) return true; 
        if ((knight_attacks[square] & board.bitboards[n])!=0) return true;
        if ((king_attacks[square] & board.bitboards[k])!=0) return true;

        if ((get_bishop_attacks(square, board.occupancies[both]) & board.bitboards[b])!=0) return true;
        if ((get_rook_attacks(square, board.occupancies[both]) & board.bitboards[r])!=0) return true;
        if ((get_queen_attacks(square, board.occupancies[both]) & board.bitboards[q])!=0) return true;
    }
    return false;
}

#endif