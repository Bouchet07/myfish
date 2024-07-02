#ifndef BITBOARD_H
#define BITBOARD_H

#ifdef __GNUC__ // Compiler is GCC or compatible
    #ifndef FALLBACK
        #define HAS_BUILTIN_POPCOUNTLL
        #define HAS_BUILTIN_CTZLL
    #endif
#endif

#include "types.hpp"

constexpr U64 FILE_A = 0x0101010101010101ULL;   constexpr U64 NOT_FILE_A = ~FILE_A;
constexpr U64 FILE_B = 0x0202020202020202ULL;   constexpr U64 NOT_FILE_B = ~FILE_B;
constexpr U64 FILE_C = 0x0404040404040404ULL;   constexpr U64 NOT_FILE_C = ~FILE_C;
constexpr U64 FILE_D = 0x0808080808080808ULL;   constexpr U64 NOT_FILE_D = ~FILE_D;
constexpr U64 FILE_E = 0x1010101010101010ULL;   constexpr U64 NOT_FILE_E = ~FILE_E;
constexpr U64 FILE_F = 0x2020202020202020ULL;   constexpr U64 NOT_FILE_F = ~FILE_F;
constexpr U64 FILE_G = 0x4040404040404040ULL;   constexpr U64 NOT_FILE_G = ~FILE_G;
constexpr U64 FILE_H = 0x8080808080808080ULL;   constexpr U64 NOT_FILE_H = ~FILE_H;

constexpr U64 FILE_GH = FILE_G | FILE_H;        constexpr U64 NOT_FILE_GH = ~FILE_GH;
constexpr U64 FILE_AB = FILE_A | FILE_B;        constexpr U64 NOT_FILE_AB = ~FILE_AB;

constexpr U64 RANK_1 = 0xff00000000000000ULL;   constexpr U64 NOT_RANK_1 = ~RANK_1;
constexpr U64 RANK_2 = 0x00ff000000000000ULL;   constexpr U64 NOT_RANK_2 = ~RANK_2;
constexpr U64 RANK_3 = 0x0000ff0000000000ULL;   constexpr U64 NOT_RANK_3 = ~RANK_3;
constexpr U64 RANK_4 = 0x000000ff00000000ULL;   constexpr U64 NOT_RANK_4 = ~RANK_4;
constexpr U64 RANK_5 = 0x00000000ff000000ULL;   constexpr U64 NOT_RANK_5 = ~RANK_5;
constexpr U64 RANK_6 = 0x0000000000ff0000ULL;   constexpr U64 NOT_RANK_6 = ~RANK_6;
constexpr U64 RANK_7 = 0x000000000000ff00ULL;   constexpr U64 NOT_RANK_7 = ~RANK_7;
constexpr U64 RANK_8 = 0x00000000000000ffULL;   constexpr U64 NOT_RANK_8 = ~RANK_8;

/* constexpr void set_bit(U64 &b, const int i){b |= (1ULL << i);}
constexpr U64  get_bit(const U64 &b, const int i){return b & (1ULL << i);}
constexpr void pop_bit(U64 &b, const int i){b &= ~(1ULL << i);}
constexpr void pop_LSB(U64 &b){b &= b - 1;} */

#define set_bit(b, i) ((b) |= (1ULL << (i))) // a8, b8 ... h1
#define get_bit(b, i) ((b) & (1ULL << (i)))  // a8, b8 ... h1
#define pop_bit(b, i) ((b) &= ~(1ULL << (i))) 
#define pop_LSB(b) ((b) &= (b) - 1)

#ifdef HAS_BUILTIN_POPCOUNTLL
    #define count_bits(b) __builtin_popcountll(b)
    #define COUNT_BITS_METHOD "using builtin count bits (__builtin_popcountll)"
#else
    // Brian Kernighan's Algorithm
    constexpr int custom_count_bits(U64 b){
        b = b - ((b >> 1) & 0x5555555555555555ULL);        // add pairs of bits
        b = (b & 0x3333333333333333ULL) + ((b >> 2) & 0x3333333333333333ULL);  // quads
        b = (b + (b >> 4)) & 0x0F0F0F0F0F0F0F0FULL;        // groups of 8
        return ((b * 0x0101010101010101ULL) >> 56);  // horizontal sum of bytes
    }
    #define count_bits(b) custom_count_bits(b)
    #define COUNT_BITS_METHOD "using custom count bits (Brian Kernighan's Algorithm)"
#endif

#ifdef HAS_BUILTIN_CTZLL
    #define get_LSB(b) __builtin_ctzll(b)
    #define GET_LSB_METHOD "using builtin get least significant bit (__builtin_ctzll)"
#else
    constexpr int custom_get_LSB(U64 b){
        if (b){
            return count_bits((b & -b)-1);
        }else{
            return 64;
        }
    }
    #define get_LSB(b) custom_get_LSB(b)
    #define GET_LSB_METHOD "using custom get least significant bit"
#endif

constexpr U64 west(const U64 b) { return (b & NOT_FILE_A) >> 1; }
constexpr U64 east(const U64 b) { return (b & NOT_FILE_H) << 1; }
constexpr U64 north(const U64 b) { return b >> 8; }
constexpr U64 south(const U64 b) { return b << 8; }
constexpr U64 north_west(const U64 b) { return (b & NOT_FILE_A) >> 9; }
constexpr U64 north_east(const U64 b) { return (b & NOT_FILE_H) >> 7; }
constexpr U64 south_west(const U64 b) { return (b & NOT_FILE_A) << 7; }
constexpr U64 south_east(const U64 b) { return (b & NOT_FILE_H) << 9; }

#endif