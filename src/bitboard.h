#ifndef BITBOARD_H
#define BITBOARD_H

#include <array>

#include "types.h"

constexpr std::string_view COUNT_BITS_METHOD = HasPopCnt ? "using builtin count bits (__builtin_popcountll)"
                                                    : "using custom count bits (Brian Kernighan's Algorithm)";

constexpr uint8_t popcnt(Bitboard b){
    if (HasPopCnt){
        return __builtin_popcountll(b);
    }
    // Brian Kernighan's Algorithm
    b = b - ((b >> 1) & 0x5555555555555555ULL);        // add pairs of bits
    b = (b & 0x3333333333333333ULL) + ((b >> 2) & 0x3333333333333333ULL);  // quads
    b = (b + (b >> 4)) & 0x0F0F0F0F0F0F0F0FULL;        // groups of 8
    return ((b * 0x0101010101010101ULL) >> 56);  // horizontal sum of bytes
    
}

constexpr std::string_view GET_LSB_METHOD = HasCtz ? "using builtin get least significant bit (__builtin_ctzll)"
                                              : "using custom get least significant bit";

constexpr Square get_LSB(Bitboard b){
    if (HasCtz){
        return Square(__builtin_ctzll(b));
    }else{
        if (b){
            return Square(popcnt((b & -b)-1));
        }else{
            return SQ_NONE;
        }
    }
}

constexpr Bitboard FileABB = 0x0101010101010101ULL;
constexpr Bitboard FileBBB = FileABB << 1;
constexpr Bitboard FileCBB = FileABB << 2;
constexpr Bitboard FileDBB = FileABB << 3;
constexpr Bitboard FileEBB = FileABB << 4;
constexpr Bitboard FileFBB = FileABB << 5;
constexpr Bitboard FileGBB = FileABB << 6;
constexpr Bitboard FileHBB = FileABB << 7;

constexpr Bitboard Rank1BB = 0xFF;
constexpr Bitboard Rank2BB = Rank1BB << (8 * 1);
constexpr Bitboard Rank3BB = Rank1BB << (8 * 2);
constexpr Bitboard Rank4BB = Rank1BB << (8 * 3);
constexpr Bitboard Rank5BB = Rank1BB << (8 * 4);
constexpr Bitboard Rank6BB = Rank1BB << (8 * 5);
constexpr Bitboard Rank7BB = Rank1BB << (8 * 6);
constexpr Bitboard Rank8BB = Rank1BB << (8 * 7);

// Moves a bitboard one or two steps as specified by the direction D
template<Direction D>
constexpr Bitboard shift(Bitboard b) {
    return D == NORTH         ? b << 8
         : D == SOUTH         ? b >> 8
         : D == NORTH + NORTH ? b << 16
         : D == SOUTH + SOUTH ? b >> 16
         : D == EAST          ? (b & ~FileHBB) << 1
         : D == WEST          ? (b & ~FileABB) >> 1
         : D == NORTH_EAST    ? (b & ~FileHBB) << 9
         : D == NORTH_WEST    ? (b & ~FileABB) << 7
         : D == SOUTH_EAST    ? (b & ~FileHBB) >> 7
         : D == SOUTH_WEST    ? (b & ~FileABB) >> 9
                              : 0;
}

constexpr Bitboard square_bb(Square s) {
    return (1ULL << s);
}

inline Bitboard  operator&(Bitboard b, Square s) { return b & square_bb(s); }
inline Bitboard  operator|(Bitboard b, Square s) { return b | square_bb(s); }
inline Bitboard  operator^(Bitboard b, Square s) { return b ^ square_bb(s); }
inline Bitboard& operator|=(Bitboard& b, Square s) { return b |= square_bb(s); }
inline Bitboard& operator^=(Bitboard& b, Square s) { return b ^= square_bb(s); }

inline Bitboard operator&(Square s, Bitboard b) { return b & s; }
inline Bitboard operator|(Square s, Bitboard b) { return b | s; }
inline Bitboard operator^(Square s, Bitboard b) { return b ^ s; }

inline Bitboard operator|(Square s1, Square s2) { return square_bb(s1) | s2; }


constexpr Bitboard rank_bb(Rank r) { return Rank1BB << (8 * r); }

constexpr Bitboard rank_bb(Square s) { return rank_bb(rank_of(s)); }

constexpr Bitboard file_bb(File f) { return FileABB << f; }

constexpr Bitboard file_bb(Square s) { return file_bb(file_of(s)); }

inline void set_bit(Bitboard& b, const Square s){
    b |= s;
}

inline Bitboard get_bit(const Bitboard b, const Square s){
    return b & s;
}

inline void pop_bit(Bitboard& b, const Square s){
    b &= ~square_bb(s);
}

inline void pop_LSB(Bitboard& b){
    b &= b - 1;
}

void print_bitboard(Bitboard bitboard, bool Use_UTF8);

struct Board {
    //std::array<uint64_t, MAX_MOVES> repetition_table = {0};
    // piece bitboards
    std::array<Bitboard, 12> bitboards = {0};
    // occupancy bitboards
    std::array<Bitboard, 3> occupancies = {0};
    // side to move
    Color side = WHITE;
    // enpassant square
    Square enpassant = SQ_NONE;
    // castling rights
    uint8_t castle = 0;
    // "almost" unique position identifier (Hash key / Position key)
    uint64_t hash_key = 0;
};

void print_board(Board &board, bool Use_UTF8);

#endif