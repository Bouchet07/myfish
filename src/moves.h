#ifndef MOVES_H
#define MOVES_H

#include <vector>
#include <iostream>

#include "types.h"
#include "bitboard.h"
#include "attacks.h"
#include "tt.h"

/**
 *        binary move bits                      description                         hexadecimal move bits
 * 
 * 0000 0000 0000 0000 0011 1111    source square (6 bits for 64 squares)           0x00003F
 * 0000 0000 0000 1111 1100 0000    target square (6 bits for 64 squares)           0x000FC0
 * 0000 0000 1111 0000 0000 0000    piece (4 bits for 12 pieces)                    0x00F000
 * 0000 1111 0000 0000 0000 0000    promoted piece (4 bits for 8 promoted pieces)   0x0F0000
 * 0001 0000 0000 0000 0000 0000    capture flag (1 bit)                            0x100000
 * 0010 0000 0000 0000 0000 0000    double push flag (1 bit)                        0x200000
 * 0100 0000 0000 0000 0000 0000    enpassant flag (1 bit)                          0x400000
 * 1000 0000 0000 0000 0000 0000    castling flag (1 bit)                           0x800000
*/
using Move = uint32_t;
struct MoveScore{
    Move move;
    int score = 0;
    MoveScore(Move move) : move(move) {}  // Constructor initializes score to 0
};



constexpr Move encode_move(Square source, Square target, Piece piece,
                           PieceType promoted, bool capture,
                           bool double_push, bool enpassant, bool castling) {
    
    return (static_cast<uint32_t>(source)) | 
           (static_cast<uint32_t>(target) << 6) | 
           (static_cast<uint32_t>(piece) << 12) | 
           (static_cast<uint32_t>(promoted) << 16) |
           (static_cast<uint32_t>(capture) << 20) | 
           (static_cast<uint32_t>(double_push) << 21) | 
           (static_cast<uint32_t>(enpassant) << 22) | 
           (static_cast<uint32_t>(castling) << 23);
}

constexpr Square decode_move_source(Move move) {
    return Square(move & 0x3F);
}
constexpr Square decode_move_target(Move move) {
    return Square((move >> 6) & 0x3F);
}
constexpr Piece decode_move_piece(Move move) {
    return Piece((move >> 12) & 0xF);
}
constexpr PieceType decode_move_promoted(Move move) {
    return PieceType((move >> 16) & 0xF);
}
constexpr bool decode_move_capture(Move move) {
    return (move >> 20) & 1;
}
constexpr bool decode_move_double_push(Move move) {
    return (move >> 21) & 1;
}
constexpr bool decode_move_enpassant(Move move) {
    return (move >> 22) & 1;
}
constexpr bool decode_move_castling(Move move) {
    return (move >> 23) & 1;
}


using MoveList = std::vector<MoveScore>;

void print_move_list(MoveList &move_list);


inline void add_move(MoveList &move_list, const Move move){
    /* move_list.moves[move_list.count] = move;
    move_list.count++; */
    move_list.push_back(move);
}
inline bool is_square_available(Board &board, Square s){
    return !(board.occupancies[BOTH] & s);
}

inline bool two_squares_available(Board &board, Square s1, Square s2){
    return is_square_available(board, s1) && is_square_available(board, s2);
}
inline bool three_squares_available(Board &board, Square s1, Square s2, Square s3){
    return is_square_available(board, s1) && is_square_available(board, s2) && is_square_available(board, s3);
}

MoveList generate_moves(Board &board);


constexpr bool _is_letter(const char fen){
    return (fen >= 'a' && fen <= 'z') || (fen >= 'A' && fen <= 'Z');
}

constexpr bool _is_number(const char fen){
    return (fen >= '0' && fen <= '8');
}

// FEN dedug positions
constexpr std::string_view empty_board = "8/8/8/8/8/8/8/8 w - - ";
constexpr std::string_view start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
constexpr std::string_view Kiwipete = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
constexpr std::string_view killer_position = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
constexpr std::string_view cmk_position = "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 ";
constexpr std::string_view endgame = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
constexpr std::string_view repetition = "2r3k1/R7/8/1R6/8/8/P4KPP/8 w - - 0 40";

void parse_fen(Board &board, std::string_view fen);

enum MoveFlag : bool{
    ALL_MOVES = 0,
    CAPTURE_MOVES = 1,
};

bool make_move(Board &board, Move move, MoveFlag move_flag);

void print_move(Move move);

Move parse_move(Board &board, std::string_view move_string);

void parse_position(Board &board, std::string_view command);

struct Tree
{
    int ply=0;
    uint32_t visited_nodes=0;
    Move killer_moves[2][MAX_PLY] = {0}; // [1st, 2nd][ply]
    Move history_moves[12][SQUARE_NB] = {0}; //  [piece][to_square]
    int pv_length[MAX_PLY] = {0};
    Move pv[MAX_PLY][MAX_PLY] = {0};
    bool follow_pv = false;
    bool score_pv = false;
    bool found_pv = false;
};

void sort_moves(MoveList &move_list, Tree &tree, const Board &board);

void print_moves_score(Board &board, Tree &tree, MoveList &move_list);

#endif // MOVES_H