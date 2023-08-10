#ifndef MOVES_H
#define MOVES_H

#include <cstring>
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
typedef struct{
    int moves[256];
    int count;
}moves;

static inline void add_move(moves *move_list, int move){
    move_list->moves[move_list->count] = move;
    move_list->count++;
}

#define two_squares_available(s1, s2) (!(get_bit(occupancies[both], s1))) && (!(get_bit(occupancies[both], s2)))
#define three_squares_available(s1, s2, s3) two_squares_available(s1,s2) && (!(get_bit(occupancies[both], s3)))



#define copy_board()                                                    \
    U64 bitboards_copy[12], occupancies_copy[3];                        \
    int side_copy, enpassant_copy, castle_copy;                         \
    std::memcpy(bitboards_copy, bitboards, 96);                         \
    std::memcpy(occupancies_copy, occupancies, 24);                     \
    side_copy = side, enpassant_copy = enpassant, castle_copy = castle; 

// hola
#define take_back()                                                     \
    std::memcpy(bitboards, bitboards_copy, 96);                         \
    std::memcpy(occupancies, occupancies_copy, 24);                     \
    side = side_copy, enpassant = enpassant_copy, castle = castle_copy; 


inline void generate_moves(moves *move_list){
    move_list->count = 0;
    int source_square, target_square;
    U64 bitboard, attacks;

    for (int piece = P; piece <= k; piece++){
        bitboard = bitboards[piece];

        if (side^1){ // white
            if(piece == P){
                while(bitboard){
                    source_square = get_LSB(bitboard);
                    target_square = source_square - 8;

                    if ((target_square >= a8) && !get_bit(occupancies[both], target_square)){
                        // pawn promotion
                        if ((source_square >= a7) && (source_square <= h7)){
                            add_move(move_list, encode_move(source_square, target_square, piece, Q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, R, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, B, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, N, 0, 0, 0, 0));
                        }else{
                            // one square ahead pawn move
                            add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                            // two sqaures move
                            if (((source_square >= a2) && (source_square <= h2)) && (!get_bit(occupancies[both], target_square - 8))){
                                add_move(move_list, encode_move(source_square, target_square-8, piece, 0, 0, 1, 0, 0));
                            }
                        }
                    }
                    attacks = pawn_attacks[white][source_square] & occupancies[black];
                    while (attacks){
                        target_square = get_LSB(attacks);
                        // pawn promotion capture
                        if (source_square >= a7 && source_square <= h7){
                            add_move(move_list, encode_move(source_square, target_square, piece, Q, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, R, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, B, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, N, 1, 0, 0, 0));
                        }else{
                            // pawn capture
                            add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                        }
                        pop_LSB(attacks);
                    }
                    if (enpassant != no_sq){
                        attacks = pawn_attacks[white][source_square] & (1ULL << enpassant);
                        if (attacks){
                            target_square = get_LSB(attacks);
                            add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 1, 0));
                        }
                    }
                    pop_LSB(bitboard);
                }
            }
            else if (piece == K){
                // castling
                if (castle & wk){
                    if (two_squares_available(f1,g1)){
                        if (!is_square_attacked(e1, black) && !is_square_attacked(f1, black)){
                            add_move(move_list, encode_move(e1, g1, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
                if (castle & wq){
                    if (three_squares_available(b1,c1,d1)){
                        if (!is_square_attacked(e1, black) && !is_square_attacked(d1, black)){
                            add_move(move_list, encode_move(e1, c1, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
            }
        }else{ // black
            if(piece == p){
                while(bitboard){
                    source_square = get_LSB(bitboard);
                    target_square = source_square + 8;

                    if ((target_square <= h1) && !get_bit(occupancies[both], target_square)){
                        // pawn promotion
                        if ((source_square >= a2) && (source_square <= h2)){
                            add_move(move_list, encode_move(source_square, target_square, piece, q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, r, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, b, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, n, 0, 0, 0, 0));
                        }else{
                            // one square ahead pawn move 
                            add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                            // two sqaures move
                            if (((source_square >= a7) && (source_square <= h7)) && (!get_bit(occupancies[both], target_square + 8))){
                                add_move(move_list, encode_move(source_square, target_square+8, piece, 0, 0, 1, 0, 0));
                            }
                        }
                    }
                    attacks = pawn_attacks[black][source_square] & occupancies[white];
                    while (attacks){
                        target_square = get_LSB(attacks);
                        // pawn promotion capture
                        if ((source_square >= a2) && (source_square <= h2)){
                            add_move(move_list, encode_move(source_square, target_square, piece, q, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, r, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, b, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, n, 1, 0, 0, 0));
                        }else{
                            // pawn capture
                            add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                        }

                        pop_LSB(attacks);
                    }
                    if (enpassant != no_sq){
                        attacks = pawn_attacks[black][source_square] & (1ULL << enpassant);
                        if (attacks){
                            target_square = get_LSB(attacks);
                            add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 1, 0));
                            
                        }
                    }
                    pop_LSB(bitboard);
                }
            }
            else if (piece == k){
                // castling
                if (castle & bk){
                    if (two_squares_available(f8,g8)){
                        if (!is_square_attacked(e8, white) && !is_square_attacked(f8, white)){
                            add_move(move_list, encode_move(e8, g8, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
                if (castle & bq){
                    if (three_squares_available(b8,c8,d8)){
                        if (!is_square_attacked(e8, white) && !is_square_attacked(d8, white)){
                            add_move(move_list, encode_move(e8, c8, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
            }
        }
        // knights
        if ((side^1) ? piece == N : piece == n){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = knight_attacks[source_square] & (~occupancies[side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(occupancies[side^1], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                    // capture move
                    }else{
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                    }
                    pop_LSB(attacks);
                }
                pop_LSB(bitboard);
            }
        }
        // bishops
        if ((side^1) ? piece == B : piece == b){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = get_bishop_attacks(source_square, occupancies[both]) & (~occupancies[side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(occupancies[side^1], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                    // capture move
                    }else{
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                    }
                    pop_LSB(attacks);
                }
                pop_LSB(bitboard);
            }
        }
        // rooks
        if ((side^1) ? piece == R : piece == r){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = get_rook_attacks(source_square, occupancies[both]) & (~occupancies[side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(occupancies[side^1], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                    // capture move
                    }else{
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                    }
                    pop_LSB(attacks);
                }
                pop_LSB(bitboard);
            }
        }
        // queens
        if ((side^1) ? piece == Q : piece == q){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = get_queen_attacks(source_square, occupancies[both]) & (~occupancies[side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(occupancies[side^1], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                    // capture move
                    }else{
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                    }
                    pop_LSB(attacks);
                }
                pop_LSB(bitboard);
            }
        }
        // king
        if ((side^1) ? piece == K : piece == k){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = king_attacks[source_square] & (~occupancies[side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(occupancies[side^1], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                    // capture move
                    }else{
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                    }
                    pop_LSB(attacks);
                }
                pop_LSB(bitboard);
            }
        }
    }
}

inline int make_move(int move, int move_flag){
    // quiet moves
    if (move_flag == all_moves){
        copy_board();

        int source_square = get_move_source(move);
        int target_square = get_move_target(move);
        int piece = get_move_piece(move);
        int promoted = get_move_promoted(move);
        int capture = get_move_capture(move);
        int doublepp = get_move_double(move);
        int enpass = get_move_enpassant(move);
        int castling = get_move_castling(move);

        // move piece
        pop_bit(bitboards[piece], source_square);
        set_bit(bitboards[piece], target_square);
        // occupancies
        pop_bit(occupancies[side], source_square);
        set_bit(occupancies[side], target_square);

        // remove enpassant move
        enpassant = no_sq;

        // capture move
        if (capture){
            int start_piece, end_piece;
            if (side^1){
                start_piece = p;
                end_piece = k;
            }else{
                start_piece = P;
                end_piece = K;
            }
            for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++){
                // op_bit(bitboards[bb_piece], target_square); // we just eliminate everything, faster? no
                if (get_bit(bitboards[bb_piece], target_square)){
                    pop_bit(bitboards[bb_piece], target_square);
                    break;
                }
            }
            // occupancies
            pop_bit(occupancies[side^1], target_square);
        }
        // promotions
        if (promoted){
            pop_bit(bitboards[piece], target_square);
            set_bit(bitboards[promoted], target_square);
        }

        else if (enpass){
            if (side^1){
                pop_bit(bitboards[p], target_square + 8);
                pop_bit(occupancies[side^1], target_square + 8);
            }else{
                pop_bit(bitboards[P], target_square - 8);
                pop_bit(occupancies[side^1], target_square - 8);
            }

        }

        else if (doublepp){
            enpassant = (side^1) ? source_square - 8 : source_square + 8;
        }

        else if (castling){
            switch (target_square){
            case g1:
                pop_bit(bitboards[R], h1);
                set_bit(bitboards[R], f1);
                pop_bit(occupancies[side], h1);
                set_bit(occupancies[side], f1);
                break;
            case c1:
                pop_bit(bitboards[R], a1);
                set_bit(bitboards[R], d1);
                pop_bit(occupancies[side], a1);
                set_bit(occupancies[side], d1);
                break;
            case g8:
                pop_bit(bitboards[r], h8);
                set_bit(bitboards[r], f8);
                pop_bit(occupancies[side], h8);
                set_bit(occupancies[side], f8);
                break;
            case c8:
                pop_bit(bitboards[r], a8);
                set_bit(bitboards[r], d8);
                pop_bit(occupancies[side], a8);
                set_bit(occupancies[side], d8);
                break;
            }
        }
        // update castling rights
        castle &= castling_rights[source_square];
        castle &= castling_rights[target_square];
        // update both occupancies
        occupancies[both] = occupancies[white] | occupancies[black];
        // change side
        side ^= 1;
        // ilegal move (king in check after move)
        if (is_square_attacked((side^1) ? get_LSB(bitboards[k]) : get_LSB(bitboards[K]), side)){
            take_back();
            return 0; // ilegal move
        }
        return 1;
    }
    // capture moves
    else{
        if (get_move_capture(move)){ // capture
            make_move(move, all_moves);
        }else{ // not a capture
            return 0;
        }
    }
    return 0;
}

#endif