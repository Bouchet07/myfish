#ifndef MOVES_H
#define MOVES_H

#include <cstring>

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

// encode moves

#define encode_move(source, target, piece, promoted, capture, doublepp, enpassants, castling) \
                   (source) | (target << 6) | (piece << 12) | (promoted << 16) | (capture << 20) | \
                   (doublepp << 21) | (enpassants << 22) | (castling << 23) \

#define get_move_source(move)    ((move) & 0x00003F)
#define get_move_target(move)    ((move) & 0x000FC0) >> 6
#define get_move_piece(move)     ((move) & 0x00F000) >> 12
#define get_move_promoted(move)  ((move) & 0x0F0000) >> 16
#define get_move_capture(move)   ((move) & 0x100000) //? 1 : 0 // ?? performance vs >> (tested and equal)
#define get_move_double(move)    ((move) & 0x200000) //? 1 : 0
#define get_move_enpassant(move) ((move) & 0x400000) //? 1 : 0
#define get_move_castling(move)  ((move) & 0x800000) //? 1 : 0

// move list structure
typedef struct
{
    int moves[256];

    int count;
}moves;

static inline void add_move(moves *move_list, int move){
    move_list->moves[move_list->count] = move;
    move_list->count++;
}

#define two_squares_available(s1, s2) (!(get_bit(occupancies[both], s1))) && (!(get_bit(occupancies[both], s2)))
#define three_squares_available(s1, s2, s3) two_squares_available(s1,s2) && (!(get_bit(occupancies[both], s3)))

/*static inline*/ void generate_moves(moves *move_list);

#define copy_board()                                                    \
    U64 bitboards_copy[12], occupancies_copy[3];                        \
    int side_copy, enpassant_copy, castle_copy;                         \
    std::memcpy(bitboards_copy, bitboards, 96);                              \
    std::memcpy(occupancies_copy, occupancies, 24);                          \
    side_copy = side, enpassant_copy = enpassant, castle_copy = castle; \

// hola
#define take_back()                                                     \
    std::memcpy(bitboards, bitboards_copy, 96);                              \
    std::memcpy(occupancies, occupancies_copy, 24);                          \
    side = side_copy, enpassant = enpassant_copy, castle = castle_copy; \


int make_move(int move, int move_flag);


#endif