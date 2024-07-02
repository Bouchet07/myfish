#ifndef MOVES_H
#define MOVES_H

#include <cstring>
#include <iostream>

#include "types.hpp"
#include "bitboard.hpp"
#include "attacks.hpp"
#include "tt.hpp"

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

// decode moves

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

static inline void add_move(moves &move_list, int move){
    move_list.moves[move_list.count] = move;
    move_list.count++;
}

#define two_squares_available(board, s1, s2) (!(get_bit(board.occupancies[both], s1)) && !(get_bit(board.occupancies[both], s2)))
#define three_squares_available(board, s1, s2, s3) two_squares_available(board,s1,s2) && (!(get_bit(board.occupancies[both], s3)))


inline void generate_moves(Board &board, moves &move_list){
    move_list.count = 0;
    int source_square, target_square;
    U64 bitboard, attacks;

    for (int piece = P; piece <= k; piece++){
        bitboard = board.bitboards[piece];

        if (board.side^1){ // white
            if(piece == P){
                while(bitboard){
                    source_square = get_LSB(bitboard);
                    target_square = source_square - 8;

                    if ((target_square >= a8) && !get_bit(board.occupancies[both], target_square)){
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
                            if (((source_square >= a2) && (source_square <= h2)) && (!get_bit(board.occupancies[both], target_square - 8))){
                                add_move(move_list, encode_move(source_square, (target_square - 8), piece, 0, 0, 1, 0, 0));
                            }
                        }
                    }
                    attacks = pawn_attacks[white][source_square] & board.occupancies[black];
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
                    if (board.enpassant != no_sq){
                        attacks = pawn_attacks[white][source_square] & (1ULL << board.enpassant);
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
                if (board.castle & wk){
                    if (two_squares_available(board,f1,g1)){
                        if (!is_square_attacked(board, e1, black) && !is_square_attacked(board, f1, black)){
                            add_move(move_list, encode_move(e1, g1, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
                if (board.castle & wq){
                    if (three_squares_available(board,b1,c1,d1)){
                        if (!is_square_attacked(board, e1, black) && !is_square_attacked(board, d1, black)){
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

                    if ((target_square <= h1) && !get_bit(board.occupancies[both], target_square)){
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
                            if (((source_square >= a7) && (source_square <= h7)) && (!get_bit(board.occupancies[both], target_square + 8))){
                                add_move(move_list, encode_move(source_square, (target_square + 8), piece, 0, 0, 1, 0, 0));
                            }
                        }
                    }
                    attacks = pawn_attacks[black][source_square] & board.occupancies[white];
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
                    if (board.enpassant != no_sq){
                        attacks = pawn_attacks[black][source_square] & (1ULL << board.enpassant);
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
                if (board.castle & bk){
                    if (two_squares_available(board,f8,g8)){
                        if (!is_square_attacked(board, e8, white) && !is_square_attacked(board, f8, white)){
                            add_move(move_list, encode_move(e8, g8, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
                if (board.castle & bq){
                    if (three_squares_available(board,b8,c8,d8)){
                        if (!is_square_attacked(board, e8, white) && !is_square_attacked(board, d8, white)){
                            add_move(move_list, encode_move(e8, c8, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
            }
        }
        // knights
        if ((board.side^1) ? piece == N : piece == n){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = knight_attacks[source_square] & (~board.occupancies[board.side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(board.occupancies[board.side^1], target_square)){
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
        if ((board.side^1) ? piece == B : piece == b){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = get_bishop_attacks(source_square, board.occupancies[both]) & (~board.occupancies[board.side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(board.occupancies[board.side^1], target_square)){
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
        if ((board.side^1) ? piece == R : piece == r){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = get_rook_attacks(source_square, board.occupancies[both]) & (~board.occupancies[board.side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(board.occupancies[board.side^1], target_square)){
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
        if ((board.side^1) ? piece == Q : piece == q){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = get_queen_attacks(source_square, board.occupancies[both]) & (~board.occupancies[board.side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(board.occupancies[board.side^1], target_square)){
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
        if ((board.side^1) ? piece == K : piece == k){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = king_attacks[source_square] & (~board.occupancies[board.side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(board.occupancies[board.side^1], target_square)){
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

inline int make_move(Board &board, int move, int move_flag){
    // quiet moves
    if (move_flag == all_moves){
        Board copy_board = board;

        int source_square = get_move_source(move);
        int target_square = get_move_target(move);
        int piece = get_move_piece(move);
        int promoted = get_move_promoted(move);
        int capture = get_move_capture(move);
        int doublepp = get_move_double(move);
        int enpass = get_move_enpassant(move);
        int castling = get_move_castling(move);

        // move piece
        pop_bit(board.bitboards[piece], source_square);
        set_bit(board.bitboards[piece], target_square);

        // uptade hash key
        board.hash_key ^= piece_keys[piece][source_square]  // take back source square
                       ^  piece_keys[piece][target_square]; // put target square

        // occupancies
        pop_bit(board.occupancies[board.side], source_square);
        set_bit(board.occupancies[board.side], target_square);

        // remove enpassant move
        if (board.enpassant != no_sq) board.hash_key ^= enpassant_keys[board.enpassant];
        board.enpassant = no_sq;

        // capture move
        if (capture){
            int start_piece, end_piece;
            if (board.side^1){
                start_piece = p;
                end_piece = k;
            }else{
                start_piece = P;
                end_piece = K;
            }
            for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++){
                // op_bit(bitboards[bb_piece], target_square); // we just eliminate everything, faster? no
                if (get_bit(board.bitboards[bb_piece], target_square)){
                    pop_bit(board.bitboards[bb_piece], target_square);
                    board.hash_key ^= piece_keys[bb_piece][target_square]; // uptade capture hash
                    break;
                }
            }
            // occupancies
            pop_bit(board.occupancies[board.side^1], target_square);
        }
        // promotions
        if (promoted){
            pop_bit(board.bitboards[piece], target_square);
            set_bit(board.bitboards[promoted], target_square);
            board.hash_key ^= piece_keys[piece][target_square]
                           ^  piece_keys[promoted][target_square];
        }

        else if (enpass){
            if (board.side^1){ // white
                pop_bit(board.bitboards[p], target_square + 8);
                pop_bit(board.occupancies[black], target_square + 8);
                board.hash_key ^= piece_keys[p][target_square + 8];
            }else{ // black
                pop_bit(board.bitboards[P], target_square - 8);
                pop_bit(board.occupancies[white], target_square - 8);
                board.hash_key ^= piece_keys[P][target_square - 8];
            }

        }

        else if (doublepp){
            board.enpassant = (board.side^1) ? source_square - 8 : source_square + 8;
            board.hash_key ^= enpassant_keys[board.enpassant];
        }

        else if (castling){
            switch (target_square){
            case g1:
                pop_bit(board.bitboards[R], h1);
                set_bit(board.bitboards[R], f1);
                pop_bit(board.occupancies[board.side], h1);
                set_bit(board.occupancies[board.side], f1);
                board.hash_key ^= piece_keys[R][h1] ^ piece_keys[R][f1];
                break;
            case c1:
                pop_bit(board.bitboards[R], a1);
                set_bit(board.bitboards[R], d1);
                pop_bit(board.occupancies[board.side], a1);
                set_bit(board.occupancies[board.side], d1);
                board.hash_key ^= piece_keys[R][a1] ^ piece_keys[R][d1];
                break;
            case g8:
                pop_bit(board.bitboards[r], h8);
                set_bit(board.bitboards[r], f8);
                pop_bit(board.occupancies[board.side], h8);
                set_bit(board.occupancies[board.side], f8);
                board.hash_key ^= piece_keys[R][h8] ^ piece_keys[R][f8];
                break;
            case c8:
                pop_bit(board.bitboards[r], a8);
                set_bit(board.bitboards[r], d8);
                pop_bit(board.occupancies[board.side], a8);
                set_bit(board.occupancies[board.side], d8);
                board.hash_key ^= piece_keys[R][a8] ^ piece_keys[R][d8];
                break;
            }
        }
        // update both occupancies
        board.occupancies[both] = board.occupancies[white] | board.occupancies[black];
        // change side
        board.side ^= 1;
        board.hash_key ^= side_key;
        // ilegal move (king in check after move) (bitboards, occupancies, and side have to be updated for is_sq_at to work)
        if (is_square_attacked(board, (board.side^1) ? get_LSB(board.bitboards[k]) : get_LSB(board.bitboards[K]), board.side)){
            board = copy_board;
            return 0; // ilegal move
        }
        // update castling rights
        board.hash_key ^= castle_keys[board.castle]; // remove previous castling
        board.castle &= castling_rights[source_square]
                     &  castling_rights[target_square];
        board.hash_key ^= castle_keys[board.castle]; // set current one
        return 1;
    }
    // capture moves
    else{
        if (get_move_capture(move)){ // capture
            return make_move(board, move, all_moves); // this return is crazy
        }
        return 0;
    }
}

inline void print_move(int move){
    if (get_move_promoted(move)){
        std::cout << square_to_coordinates[get_move_source(move)]
             << square_to_coordinates[get_move_target(move)]
             << promoted_pieces[get_move_promoted(move)];

    }else{
        std::cout << square_to_coordinates[get_move_source(move)]
             << square_to_coordinates[get_move_target(move)];
    }
    
}

#endif