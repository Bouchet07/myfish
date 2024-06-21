#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h"

//#define set_bit(b, i) ((b) |= (1ULL << (i)))
//#define get_bit(b, i) ((b) & (1ULL << (i))) 
//#define pop_bit(b, i) ((b) &= ~(1ULL << (i))) 
//#define pop_LSB(b) ((b) &= (b) - 1)

constexpr void set_bit(U64 b, const Square s){
    b |= (1ULL << s);
}

constexpr U64 get_bit(const U64 b, const Square s){
    return b & (1ULL << s);
}

constexpr void pop_bit(U64 b, const Square s){
    b &= ~(1ULL << s);
}

constexpr void pop_LSB(U64 b){
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
    constexpr int custom_count_bits(U64 b){
        b = b - ((b >> 1) & 0x5555555555555555ULL);        // add pairs of bits
        b = (b & 0x3333333333333333ULL) + ((b >> 2) & 0x3333333333333333ULL);  // quads
        b = (b + (b >> 4)) & 0x0F0F0F0F0F0F0F0FULL;        // groups of 8
        return ((b * 0x0101010101010101ULL) >> 56);  // horizontal sum of bytes
    }
    #define count_bits(b) custom_count_bits(b)
    inline std::string COUNT_BITS_METHOD = "using custom count bits (Brian Kernighan's Algorithm)";
#endif

#ifdef HAS_BUILTIN_CTZLL
    #define get_LSB(b) __builtin_ctzll(b)
    inline std::string GET_LSB_METHOD = "using builtin get least significant bit (__builtin_ctzll)";
#else
    constexpr int custom_get_LSB(U64 b){
        if (b){
            return count_bits((b & -b)-1);
        }else{
            return 64;
        }
    }
    #define get_LSB(b) custom_get_LSB(b)
    inline std::string GET_LSB_METHOD = "using custom get least significant bit";
#endif

constexpr U64 west(const U64 b)       { return (b & NOT_FILE_A) >> 1; }
constexpr U64 east(const U64 b)       { return (b & NOT_FILE_H) << 1; }
constexpr U64 north(const U64 b)      { return b >> 8; }
constexpr U64 south(const U64 b)      { return b << 8; }
constexpr U64 north_west(const U64 b) { return (b & NOT_FILE_A) >> 9; }
constexpr U64 north_east(const U64 b) { return (b & NOT_FILE_H) >> 7; }
constexpr U64 south_west(const U64 b) { return (b & NOT_FILE_A) << 7; }
constexpr U64 south_east(const U64 b) { return (b & NOT_FILE_H) << 9; }

void print_bitboard(U64 bitboard);

#endif