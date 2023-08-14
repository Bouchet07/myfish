#ifndef TYPES_H
#define TYPES_H

#include <unordered_map>

using U64 = unsigned long long;

// board squares
enum {
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
enum{
    P, N, B, R, Q, K, p, n, b, r, q, k
};

// colors
enum{
    white, black, both
};

// pieces (rook and bishop)
enum{
    rook, bishop
};

// castling
enum{
    wk=1, wq=2, bk=4, bq=8
};

enum{
    all_moves, only_captures
};

// globals

// pieces bitboards
static U64 bitboards[12];
// occupancies bitboards
static U64 occupancies[3];
// side to move
static int side;
// enpassant square
static int enpassant = no_sq;
// castling rights
static int castle;
// leaf nodes (number of positions reached during the test of the move generator at a given depth)
static U64 nodes;

//extern const int castling_rights[64];

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
constexpr int castling_rights[64] = {
     7, 15, 15, 15,  3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};

constexpr const char* square_to_coordinates[64] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", 
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", 
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};

// ASCII pieces
constexpr char ascii_pieces[13] = "PNBRQKpnbrqk"; // 13 including null terminator \0

// unicode pieces
constexpr const char* unicode_pieces[12] = {
    "♙", "♘", "♗", "♖", "♕", "♔",
    "♟", "♞", "♝", "♜", "♛", "♚"
};

// convert ASCII char pieces to encoded constants

static std::unordered_map<char, int> char_pieces = {
        {'P', P}, {'N', N}, {'B', B}, {'R', R}, {'Q', Q}, {'K', K},
        {'p', p}, {'n', n}, {'b', b}, {'r', r}, {'q', q}, {'k', k}
};

static std::unordered_map<int, char> promoted_pieces = {
        {Q, 'q'}, {R, 'r'}, {B, 'b'}, {N, 'n'},
        {q, 'q'}, {r, 'r'}, {b, 'b'}, {n, 'n'}
};

#endif