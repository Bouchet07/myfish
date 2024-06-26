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

enum castling : uint8_t{
    WK=1, WQ=2, BK=4, BQ=8
};

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

#endif // MOVES_H