#ifndef TYPES_H
#define TYPES_H

#include <sstream>
#include <stdint.h>

constexpr std::string_view ENGINE_NAME = "MyFish";
constexpr std::string_view ENGINE_VERSION = "2.0";
constexpr std::string_view ENGINE_AUTHOR = "Diego Bouchet";
constexpr std::string_view UCI_OPTIONS = "option name Hash type spin default 16 min 1 max 33554432\n";

#if defined(USE_PEXT)
        #include <immintrin.h>  // Header for _pext_u64() intrinsic
        #define pext(b, m) _pext_u64(b, m)
    #else
        #define pext(b, m) 0
#endif
#include <unordered_map>


/* #ifdef __GNUC__ // Compiler is GCC or compatible
    #ifndef NO_POPCNT
        #define HAS_BUILTIN_POPCOUNTLL
    #endif
    #ifndef NO_CTZ
        #define HAS_BUILTIN_CTZLL
    #endif
#endif */

    #ifdef USE_POPCNT
constexpr bool HasPopCnt = true;
    #else
constexpr bool HasPopCnt = false;
    #endif

    #ifdef USE_CTZ
constexpr bool HasCtz = true;
    #else
constexpr bool HasCtz = false;
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

    #ifdef BENCHMARK
constexpr bool Benchmark = true;
    #else
constexpr bool Benchmark = false;
    #endif

using Bitboard = uint64_t;

constexpr uint16_t MAX_MOVES = 256;
constexpr uint8_t MAX_PLY    = 246;
constexpr uint8_t MAX_DEPTH  = 64; // implement
constexpr uint8_t FULL_DEPTH_MOVES = 4;
constexpr uint8_t REDUCTION_LIMIT = 3;

enum Color: uint8_t{
    WHITE,
    BLACK,
    COLOR_NB,
    BOTH=COLOR_NB,
};

enum CastlingRights: uint8_t{
    NO_CASTLING,
    WHITE_OO,
    WHITE_OOO = WHITE_OO << 1,
    BLACK_OO  = WHITE_OO << 2,
    BLACK_OOO = WHITE_OO << 3,

    KING_SIDE      = WHITE_OO | BLACK_OO,
    QUEEN_SIDE     = WHITE_OOO | BLACK_OOO,
    WHITE_CASTLING = WHITE_OO | WHITE_OOO,
    BLACK_CASTLING = BLACK_OO | BLACK_OOO,
    ANY_CASTLING   = WHITE_CASTLING | BLACK_CASTLING,

    CASTLING_RIGHT_NB = 16
};

enum Bound {
    BOUND_NONE,
    BOUND_UPPER,
    BOUND_LOWER,
    BOUND_EXACT = BOUND_UPPER | BOUND_LOWER
};

// Value is used as an alias for int16_t, this is done to differentiate between
// a search value and any other integer value. The values used in search are always
// supposed to be in the range (-VALUE_NONE, VALUE_NONE] and should not exceed this range.
using Value = int16_t;

constexpr Value VALUE_ZERO     = 0;
constexpr Value VALUE_DRAW     = 0;
constexpr Value VALUE_NONE     = 32002;
constexpr Value VALUE_INFINITE = 32001;

constexpr Value VALUE_MATE             = 32000;
constexpr Value VALUE_MATE_IN_MAX_PLY  = VALUE_MATE - MAX_PLY;
constexpr Value VALUE_MATED_IN_MAX_PLY = -VALUE_MATE_IN_MAX_PLY;

constexpr Value VALUE_TB                 = VALUE_MATE_IN_MAX_PLY - 1;
constexpr Value VALUE_TB_WIN_IN_MAX_PLY  = VALUE_TB - MAX_PLY;
constexpr Value VALUE_TB_LOSS_IN_MAX_PLY = -VALUE_TB_WIN_IN_MAX_PLY;

// In the code, we make the assumption that these values
// are such that non_pawn_material() can be used to uniquely
// identify the material on the board.
constexpr Value PawnValue   = 208;
constexpr Value KnightValue = 781;
constexpr Value BishopValue = 825;
constexpr Value RookValue   = 1276;
constexpr Value QueenValue  = 2538;

enum PieceType: uint8_t{
    NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    ALL_PIECES = 0,
    PIECE_TYPE_NB = 7 // stockfish uses 8, mine breaks
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
ENABLE_INCR_OPERATORS_ON(Color)
ENABLE_INCR_OPERATORS_ON(Piece)
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

inline File&   operator+=(File& f, int8_t num) { return f = File(f + num); }

// Toggle color
constexpr Color operator~(Color c) { return Color(c ^ BLACK); }

// Swap A1 <-> A8
constexpr Square flip_rank(Square s) { return Square(s ^ SQ_A8); }

// Swap A1 <-> H1
constexpr Square flip_file(Square s) { return Square(s ^ SQ_H1); }

// Swap color of piece B_KNIGHT <-> W_KNIGHT
constexpr Piece operator~(Piece pc) { return Piece(pc ^ 8); }

// castling rights
constexpr Value mate_in(int ply) { return VALUE_MATE - ply; }

constexpr Value mated_in(int ply) { return -VALUE_MATE + ply; }

constexpr Square make_square(File f, Rank r) { return Square((r << 3) + f); }

constexpr Piece make_piece(Color c, const PieceType pt) { return Piece((c << 3) + pt); }

constexpr PieceType type_of(Piece pc) { return PieceType(pc & 7); }

constexpr Color color_of(Piece pc) {
    //assert(pc != NO_PIECE);
    return Color(pc >> 3);
}

constexpr uint8_t make_index_piece(Color c, PieceType pt) { return pt + (c ? 6 : 0) - 1; }

constexpr uint8_t make_index_piece(Piece p) {return make_index_piece(color_of(p), type_of(p));}

constexpr uint8_t make_index_piece_type(PieceType pt) { return pt - 1; }

constexpr bool is_ok(Square s) { return s >= SQ_A1 && s <= SQ_H8; }

constexpr File file_of(Square s) { return File(s & 7); }

constexpr Rank rank_of(Square s) { return Rank(s >> 3); }

constexpr Square relative_square(Color c, Square s) { return Square(s ^ (c * 56)); }

constexpr Rank relative_rank(Color c, Rank r) { return Rank(r ^ (c * 7)); }

constexpr Rank relative_rank(Color c, Square s) { return relative_rank(c, rank_of(s)); }

template <Rank R>
constexpr bool is_rank(Square s) { return rank_of(s) == R; }

template <File F>
constexpr bool is_file(Square s) { return file_of(s) == F; }

constexpr std::string_view square_to_coordinates[64] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};

// ASCII pieces
constexpr char ascii_pieces[13] = "PNBRQKpnbrqk"; // 13 including null terminator \0

// unicode pieces
constexpr std::string_view unicode_pieces[12] = {
    "♙", "♘", "♗", "♖", "♕", "♔",
    "♟", "♞", "♝", "♜", "♛", "♚"
};

// convert ASCII char pieces to encoded constants

static std::unordered_map<char, Piece> char_pieces = {
        {'P', W_PAWN}, {'N', W_KNIGHT}, {'B', W_BISHOP}, {'R', W_ROOK}, {'Q', W_QUEEN}, {'K', W_KING},
        {'p', B_PAWN}, {'n', B_KNIGHT}, {'b', B_BISHOP}, {'r', B_ROOK}, {'q', B_QUEEN}, {'k', B_KING}
};

static std::unordered_map<PieceType, char> promoted_pieces = {
        {QUEEN, 'q'}, {ROOK, 'r'}, {BISHOP, 'b'}, {KNIGHT, 'n'}
};

enum castling : uint8_t{
    WK=1, WQ=2, BK=4, BQ=8
};

/*
                           castling   move     in      in
                              right update     binary  decimal

 king & rooks didn't move:     1111 & 1111  =  1111    15

        white king  moved:     1111 & 1100  =  1100    12
  white king's rook moved:     1111 & 1110  =  1110    14
 white queen's rook moved:     1111 & 1101  =  1101    13
     
         black king moved:     1111 & 0011  =  1011    3
  black king's rook moved:     1111 & 1011  =  1011    11
 black queen's rook moved:     1111 & 0111  =  0111    7

*/
constexpr uint8_t castling_rights[64] = {
    13, 15, 15, 15, 12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
     7, 15, 15, 15,  3, 15, 15, 11
};

enum GamePhase : uint8_t{
    OPENING,
    ENDGAME,
    MIDDLEGAME,
    GAMEPHASE_NB
};
constexpr Value opening_phase_score = 6192;
constexpr Value endgame_phase_score = 518;

struct TimeControl {
    bool quit = false;          // quit the program
    bool stop = false;          // stop the search
    bool timeset = false;       // we are playing with time control
    int64_t moves_to_go = 30;   // moves to the next time control
    int64_t move_time = -1;     // time for the next move
    int64_t time = -1;          // time left for the player
    int64_t inc = 0;            // increment
    int64_t start_time = 0;     // start time
    int64_t stop_time = 0;      // stop time
    void reset() {
        quit = false;
        stop = false;
        timeset = false;
        moves_to_go = 30;
        move_time = -1;
        time = -1;
        inc = 0;
        start_time = 0;
        stop_time = 0;
    }
};

#endif