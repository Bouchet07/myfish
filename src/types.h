#ifndef TYPES_H
#define TYPES_H

#include <sstream>
#include <stdint.h>

//typedef uint64_t U64;

constexpr const char* ENGINE_NAME = "MyFish";
constexpr const char* ENGINE_VERSION = "2.0";
constexpr const char* ENGINE_AUTHOR = "Diego Bouchet";

#if defined(USE_PEXT)
        #include <immintrin.h>  // Header for _pext_u64() intrinsic
        #define pext(b, m) _pext_u64(b, m)
    #else
        #define pext(b, m) 0
#endif



/* constexpr U64 FILE_A = 0x0101010101010101ULL;   constexpr U64 NOT_FILE_A = ~FILE_A;
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
constexpr U64 RANK_8 = 0x00000000000000ffULL;   constexpr U64 NOT_RANK_8 = ~RANK_8; */

//namespace Myfish{

#ifdef __GNUC__ // Compiler is GCC or compatible
    #ifndef NO_POPCNT
        #define HAS_BUILTIN_POPCOUNTLL
    #endif
    #ifndef NO_CTZ
        #define HAS_BUILTIN_CTZLL
    #endif
#endif

    #ifdef USE_POPCNT
constexpr bool HasPopCnt = true;
    #else
constexpr bool HasPopCnt = false;
    #endif

    #ifdef USE_PEXT
constexpr bool HasPext = true;
    #else
constexpr bool HasPext = false;
    #endif

    #ifdef IS_64BIT
constexpr bool Is64Bit = true;
    #else
constexpr bool Is64Bit = false;
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

using Bitboard = uint64_t;

enum Color: uint8_t{
    WHITE,
    BLACK,
    COLOR_NB
};

enum PieceType: uint8_t{
    NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    ALL_PIECES = 0,
    PIECE_TYPE_NB = 8
};

enum Piece: uint8_t{
    NO_PIECE,
    W_PAWN = PAWN,     W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN = PAWN + 8, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
    PIECE_NB = 16
};

enum Square : int8_t {
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_NONE,

    SQUARE_ZERO = 0,
    SQUARE_NB   = 64
};

enum Direction : int8_t {
    NORTH = 8,
    EAST  = 1,
    SOUTH = -NORTH,
    WEST  = -EAST,

    NORTH_EAST = NORTH + EAST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    NORTH_WEST = NORTH + WEST
};

enum File : int8_t {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
    FILE_NB
};

enum Rank : int8_t {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    RANK_NB
};


    #define ENABLE_INCR_OPERATORS_ON(T) \
        inline T& operator++(T& d) { return d = T(static_cast<int8_t>(d) + 1); } \
        inline T& operator--(T& d) { return d = T(static_cast<int8_t>(d) - 1); }

ENABLE_INCR_OPERATORS_ON(PieceType)
ENABLE_INCR_OPERATORS_ON(Square)
ENABLE_INCR_OPERATORS_ON(File)
ENABLE_INCR_OPERATORS_ON(Rank)

    #undef ENABLE_INCR_OPERATORS_ON

constexpr Direction operator+(Direction d1, Direction d2) { return Direction(static_cast<int8_t>(d1) + static_cast<int8_t>(d2)); }
constexpr Direction operator*(int8_t i, Direction d) { return Direction(i * static_cast<int8_t>(d)); }

// Additional operators to add a Direction to a Square
constexpr Square operator+(Square s, Direction d) { return Square(static_cast<int8_t>(s) + static_cast<int8_t>(d)); }
constexpr Square operator-(Square s, Direction d) { return Square(static_cast<int8_t>(s) - static_cast<int8_t>(d)); }
inline Square&   operator+=(Square& s, Direction d) { return s = s + d; }
inline Square&   operator-=(Square& s, Direction d) { return s = s - d; }

// Toggle color
constexpr Color operator~(Color c) { return Color(c ^ BLACK); }

// Swap A1 <-> A8
constexpr Square flip_rank(Square s) { return Square(s ^ SQ_A8); }

// Swap A1 <-> H1
constexpr Square flip_file(Square s) { return Square(s ^ SQ_H1); }

// Swap color of piece B_KNIGHT <-> W_KNIGHT
constexpr Piece operator~(Piece pc) { return Piece(pc ^ 8); }

// castling rights
// mate_in, mated in

constexpr Square make_square(File f, Rank r) { return Square((r << 3) + f); }

constexpr Piece make_piece(Color c, PieceType pt) { return Piece((c << 3) + pt); }

constexpr PieceType type_of(Piece pc) { return PieceType(pc & 7); }

inline Color color_of(Piece pc) {
    //assert(pc != NO_PIECE);
    return Color(pc >> 3);
}

constexpr bool is_ok(Square s) { return s >= SQ_A1 && s <= SQ_H8; }

constexpr File file_of(Square s) { return File(s & 7); }

constexpr Rank rank_of(Square s) { return Rank(s >> 3); }
//} // Namespace Myfish

#endif