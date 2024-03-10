#ifndef TYPES_H
#define TYPES_H

#include <sstream>
#include <stdint.h>

constexpr const char* ENGINE_NAME = "MyFish";
constexpr const char* ENGINE_VERSION = "2.0";
constexpr const char* ENGINE_AUTHOR = "Diego Bouchet";

typedef uint64_t U64;

#ifdef __GNUC__ // Compiler is GCC or compatible
    #ifndef NO_POPCNT
        #define HAS_BUILTIN_POPCOUNTLL
    #endif
    #ifndef NO_CTZ
        #define HAS_BUILTIN_CTZLL
    #endif
#endif

#if defined(UTF8)
    inline constexpr bool USE_UTF8 = true;
#elif defined(NO_UTF8)
    inline constexpr bool USE_UTF8 = false;
#elif defined(_WIN32)
    inline constexpr bool USE_UTF8 = false;
#else
    inline constexpr bool USE_UTF8 = true;
#endif

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

// board squares
enum : uint16_t{
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4, 
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

// pieces
enum : uint8_t{
    P, N, B, R, Q, K, p, n, b, r, q, k
};

// colors
enum : uint8_t{
    white, black, both
};

#endif