#ifndef MOVES_H
#define MOVES_H

#include <array>

#include "types.h"
#include "bitboard.h"
#include "attacks.h"

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

struct MoveList {
    std::array<Move, 256> moves = {};
    uint8_t count = 0;
};

void print_move_list(MoveList &move_list);


constexpr void add_move(MoveList &move_list, Move move){
    move_list.moves[move_list.count] = move;
    move_list.count++;
}
inline bool is_square_available(Board &board, Square s){
    return !(board.occupancies[BOTH] & s);
}

inline bool two_squares_available(Board &board, Square s1, Square s2){
    return !((board.occupancies[BOTH] & s1) && (board.occupancies[BOTH] & s2));
}
inline bool three_squares_available(Board &board, Square s1, Square s2, Square s3){
    return !((board.occupancies[BOTH] & s1) && (board.occupancies[BOTH] & s2) && (board.occupancies[BOTH] & s3));
}

void generate_moves(Board &board, MoveList &move_list);


constexpr bool _is_letter(const char fen){
    return (fen >= 'a' && fen <= 'z') || (fen >= 'A' && fen <= 'Z');
}

constexpr bool _is_number(const char fen){
    return (fen >= '0' && fen <= '8');
}

// FEN dedug positions
static const char* const empty_board = "8/8/8/8/8/8/8/8 w - - ";
static const char* const start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
static const char* const tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
static const char* const killer_position = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
static const char* const cmk_position = "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 ";

void parse_fen(Board &board, const char *fen);

enum MoveFlag : bool{
    ALL_MOVES = 0,
    CAPTURE_MOVES = 1,
};

int make_move(Board &board, Move move, MoveFlag move_flag);

#endif // MOVES_H