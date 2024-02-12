#ifndef TT_H
#define TT_H

#include "types.hpp"
#include "bitboard.hpp"
#include "attacks.hpp"

// https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm

// random piece key [piece][square]
static U64 piece_keys[12][64];
// random enpassant square
static U64 enpassant_keys[64];
// random castling keys
static U64 castle_keys[16];
// random side key
static U64 side_key;

// init random hash keys
void init_random_keys(){
    random_state = 1804289383; // in case it's been modified
    
    for (int piece = P; piece <= k; piece++){
        for (int square = 0; square < 64; square++){
            piece_keys[piece][square] = get_random_U64_number();
        }
    }
    for (int square = 0; square < 64; square++){
            enpassant_keys[square] = get_random_U64_number();
    }
    for (int castle = 0; castle < 16; castle++){
            castle_keys[castle] = get_random_U64_number();
    }
    side_key = get_random_U64_number();
}

// generate hash key from scratch
void generate_hash_key(Board &board){
    U64 final_key = 0ULL;
    U64 bitboard;

    for (int piece = P; piece <= k; piece++){
        bitboard = board.bitboards[piece];
        while(bitboard){
            int square = get_LSB(bitboard);
            final_key ^= piece_keys[piece][square]; // hash the piece
            pop_LSB(bitboard);
        }
    }
    if (board.enpassant != no_sq) final_key ^= enpassant_keys[board.enpassant]; // hash enpassant
    final_key ^= castle_keys[board.castle]; // hash castling right
    if (board.side) final_key ^= side_key; // hash side only if black is to move

    board.hash_key = final_key;
}

constexpr int hash_size = 0x400000; // 4 Mb

constexpr int no_hash_entry = 100'000; // outside alpha-beta bounds

// TT flags
constexpr int hashfEXACT = 0; // hash flag exact
constexpr int hashfALPHA = 1; // hash flag alpha
constexpr int hashfBETA = 2;  // hash flag beta

// Transposition Table
struct TT{
    U64 key;    // hash key
    int depth;  // current search depth
    int flags;  // flag the type of node
    int score;  // score (alpha/beta/PV)
    //int best;   
};

static TT transposition_table[hash_size] = {}; // explicit initialization with 0s

constexpr void clear_tt(TT *table){std::memset(table, 0, hash_size * sizeof(TT));}
//#define clear_tt(table) std::memset(table, 0, hash_size * sizeof(TT))


inline int read_hash_entry(Board &board, const int alpha, const int beta, const int depth){
    // create a TT instance pointer to the hash entry containig if available, the scoring data
    TT *hash_entry = &transposition_table[board.hash_key % hash_size];

    if (hash_entry->key == board.hash_key){ // make sure it's the same position
        if (hash_entry->depth == depth){    // make sure it's the same depth
            if (hash_entry->flags == hashfEXACT) return hash_entry->score;

            if ((hash_entry->flags == hashfALPHA) &&
                (hash_entry->score <= alpha))    return alpha;

            if ((hash_entry->flags == hashfBETA) &&
                (hash_entry->score >= beta))     return beta;
        }
    }
    return no_hash_entry;
}

inline void write_hash_entry(Board &board, const int score, const int depth, const int hash_flag){
    // create a TT instance pointer to the hash entry containig if available, the scoring data
    TT *hash_entry = &transposition_table[board.hash_key % hash_size];

    // write hash entry data
    hash_entry->key = board.hash_key;
    hash_entry->depth = depth;
    hash_entry->flags = hash_flag;
    hash_entry->score = score;
}


#endif