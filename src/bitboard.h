#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h"

//#define set_bit(b, i) ((b) |= (1ULL << (i)))
//#define get_bit(b, i) ((b) & (1ULL << (i))) 
//#define pop_bit(b, i) ((b) &= ~(1ULL << (i))) 
//#define pop_LSB(b) ((b) &= (b) - 1)

constexpr void set_bit(Bitboard b, const Square s){
    b |= (1ULL << s);
}

constexpr Bitboard get_bit(const Bitboard b, const Square s){
    return b & (1ULL << s);
}

constexpr void pop_bit(Bitboard b, const Square s){
    b &= ~(1ULL << s);
}

constexpr void pop_LSB(Bitboard b){
    b &= b - 1;
}

/* constexpr std::string COUNT_BITS_METHOD;   // defined in bitboard.h
constexpr std::string GET_LSB_METHOD;      // defined in bitboard.h */

#ifdef HAS_BUILTIN_POPCOUNTLL
    #define count_bits(b) __builtin_popcountll(b)
    inline std::string COUNT_BITS_METHOD = "using builtin count bits (__builtin_popcountll)";
    //#define COUNT_BITS_METHOD "using builtin count bits (__builtin_popcountll)"
#else
    // Brian Kernighan's Algorithm
    constexpr uint8_t custom_count_bits(Bitboard b){
        b = b - ((b >> 1) & 0x5555555555555555ULL);        // add pairs of bits
        b = (b & 0x3333333333333333ULL) + ((b >> 2) & 0x3333333333333333ULL);  // quads
        b = (b + (b >> 4)) & 0x0F0F0F0F0F0F0F0FULL;        // groups of 8
        return ((b * 0x0101010101010101ULL) >> 56);  // horizontal sum of bytes
    }
    #define count_bits(b) custom_count_bits(b)
    inline std::string COUNT_BITS_METHOD = "using custom count bits (Brian Kernighan's Algorithm)";
#endif



#ifdef HAS_BUILTIN_CTZLL
    #define get_LSB(b) Square(__builtin_ctzll(b))
    inline std::string GET_LSB_METHOD = "using builtin get least significant bit (__builtin_ctzll)";
#else
    constexpr Square custom_get_LSB(Bitboard b){
        if (b){
            return Square(count_bits((b & -b)-1));
        }else{
            return SQ_NONE;
        }
    }
    #define get_LSB(b) custom_get_LSB(b)
    inline std::string GET_LSB_METHOD = "using custom get least significant bit";
#endif

/* constexpr Bitboard west(const Bitboard b)       { return (b & NOT_FILE_A) >> 1; }
constexpr Bitboard east(const Bitboard b)       { return (b & NOT_FILE_H) << 1; }
constexpr Bitboard north(const Bitboard b)      { return b >> 8; }
constexpr Bitboard south(const Bitboard b)      { return b << 8; }
constexpr Bitboard north_west(const Bitboard b) { return (b & NOT_FILE_A) >> 9; }
constexpr Bitboard north_east(const Bitboard b) { return (b & NOT_FILE_H) >> 7; }
constexpr Bitboard south_west(const Bitboard b) { return (b & NOT_FILE_A) << 7; }
constexpr Bitboard south_east(const Bitboard b) { return (b & NOT_FILE_H) << 9; } */

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


void print_bitboard(Bitboard bitboard);

#endif