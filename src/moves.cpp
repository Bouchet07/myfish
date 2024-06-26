#include <iostream>

#include "moves.h"
#include <cstring>

void generate_moves(Board &board, MoveList &move_list){
    move_list.count = 0;
    Square source_square, target_square;
    Bitboard attacks, bitboard;
    for (PieceType piece = PAWN; piece < PIECE_TYPE_NB; ++piece){
        bitboard = board.bitboards[make_index_piece(board.side, piece)];
        if(board.side == WHITE){
            if(piece == PAWN){
                while(bitboard){
                    source_square = get_LSB(bitboard);
                    target_square = source_square + NORTH;

                    if (is_square_available(board, target_square)){
                        // pawn promotion
                        if (is_rank<RANK_7>(source_square)){
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), QUEEN, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), ROOK, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), BISHOP, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), KNIGHT, 0, 0, 0, 0));
                        }else{
                            // one square ahead pawn move
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 0));
                            // two sqaures move
                            if ((is_rank<RANK_2>(source_square)) && is_square_available(board, target_square + NORTH)){
                                add_move(move_list, encode_move(source_square, target_square + NORTH, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 1, 0, 0));
                            }
                        }
                    }
                    attacks = pawn_attacks[WHITE][source_square] & board.occupancies[BLACK];
                    while (attacks){
                        target_square = get_LSB(attacks);
                        // pawn promotion capture
                        if (is_rank<RANK_7>(source_square)){
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), QUEEN, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), ROOK, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), BISHOP, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), KNIGHT, 1, 0, 0, 0));
                        }else{
                            // pawn capture
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 1, 0, 0, 0));
                        }
                        pop_LSB(attacks);
                    }
                    if (is_ok(board.enpassant)){
                        attacks = pawn_attacks[WHITE][source_square] & board.enpassant;
                        if (attacks){
                            target_square = get_LSB(attacks);
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 1, 0, 1, 0));
                        }
                    }
                    pop_LSB(bitboard);
                }
            }
            else if (piece == KING){
                // castling
                if (board.castle & WK){
                    if (two_squares_available(board,SQ_F1,SQ_G1)){
                        if (!is_square_attacked(board, SQ_E1, BLACK) && !is_square_attacked(board, SQ_F1, BLACK)){
                            add_move(move_list, encode_move(SQ_E1, SQ_G1, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 1));
                        }   
                    }
                }
                if (board.castle & WQ){
                    if (three_squares_available(board,SQ_B1,SQ_C1,SQ_D1)){
                        if (!is_square_attacked(board, SQ_E1, BLACK) && !is_square_attacked(board, SQ_D1, BLACK)){
                            add_move(move_list, encode_move(SQ_E1, SQ_D1, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 1));
                        }
                    }
                }
            }
        }else{ // black
            if(piece == PAWN){
                while(bitboard){
                    source_square = get_LSB(bitboard);
                    target_square = source_square + SOUTH;

                    if (is_square_available(board, target_square)){
                        // pawn promotion
                        if (is_rank<RANK_2>(source_square)){
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), QUEEN, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), ROOK, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), BISHOP, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), KNIGHT, 0, 0, 0, 0));
                        }else{
                            // one square ahead pawn move
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 0));
                            // two sqaures move
                            if ((is_rank<RANK_7>(source_square)) && is_square_available(board, target_square + SOUTH)){
                                add_move(move_list, encode_move(source_square, target_square + SOUTH, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 1, 0, 0));
                            }
                        }
                    }
                    attacks = pawn_attacks[BLACK][source_square] & board.occupancies[WHITE];
                    while (attacks){
                        target_square = get_LSB(attacks);
                        // pawn promotion capture
                        if (is_rank<RANK_2>(source_square)){
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), QUEEN, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), ROOK, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), BISHOP, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), KNIGHT, 1, 0, 0, 0));
                        }else{
                            // pawn capture
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 1, 0, 0, 0));
                        }
                        pop_LSB(attacks);
                    }
                    if (is_ok(board.enpassant)){
                        attacks = pawn_attacks[BLACK][source_square] & board.enpassant;
                        if (attacks){
                            target_square = get_LSB(attacks);
                            add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 1, 0, 1, 0));
                        }
                    }
                    pop_LSB(bitboard);
                }
            }
            else if (piece == KING){
                // castling
                if (board.castle & BK){
                    if (two_squares_available(board,SQ_F8,SQ_G8)){
                        if (!is_square_attacked(board, SQ_E8, WHITE) && !is_square_attacked(board, SQ_F1, WHITE)){
                            add_move(move_list, encode_move(SQ_E8, SQ_G8, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 1));
                        }   
                    }
                }
                if (board.castle & BQ){
                    if (three_squares_available(board,SQ_B8,SQ_C8,SQ_D8)){
                        if (!is_square_attacked(board, SQ_E8, WHITE) && !is_square_attacked(board, SQ_D8, WHITE)){
                            add_move(move_list, encode_move(SQ_E8, SQ_D8, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 1));
                        }
                    }
                }
            }
        }
        // knights
        if (piece==KNIGHT){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = knight_attacks[source_square] & (~board.occupancies[board.side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(board.occupancies[~board.side], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 0));
                    // capture move
                    }else{
                        add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 1, 0, 0, 0));
                    }
                    pop_LSB(attacks);
                }
                pop_LSB(bitboard);
            }
        }
        // bishops
        else if (piece==BISHOP){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = get_bishop_attacks(source_square, board.occupancies[BOTH]) & (~board.occupancies[board.side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(board.occupancies[~board.side], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 0));
                    // capture move
                    }else{
                        add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 1, 0, 0, 0));
                    }
                    pop_LSB(attacks);
                }
                pop_LSB(bitboard);
            }
        }
        // rooks
        else if (piece==ROOK){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = get_rook_attacks(source_square, board.occupancies[BOTH]) & (~board.occupancies[board.side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(board.occupancies[~board.side], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 0));
                    // capture move
                    }else{
                        add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 1, 0, 0, 0));
                    }
                    pop_LSB(attacks);
                }
                pop_LSB(bitboard);
            }
        }
        // queens
        else if (piece==QUEEN){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = get_queen_attacks(source_square, board.occupancies[BOTH]) & (~board.occupancies[board.side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(board.occupancies[~board.side], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 0));
                    // capture move
                    }else{
                        add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 1, 0, 0, 0));
                    }
                    pop_LSB(attacks);
                }
                pop_LSB(bitboard);
            }
        }
        // king
        else if (piece==KING){
            while (bitboard){
                source_square = get_LSB(bitboard);
                attacks = king_attacks[source_square] & (~board.occupancies[board.side]);
                while (attacks){
                    target_square = get_LSB(attacks);
                    // quiet move
                    if (!get_bit(board.occupancies[~board.side], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 0));
                    // capture move
                    }else{
                        add_move(move_list, encode_move(source_square, target_square, make_piece(board.side, piece), NO_PIECE_TYPE, 1, 0, 0, 0));
                    }
                    pop_LSB(attacks);
                }
                pop_LSB(bitboard);
            }
        }
    }
}

void print_move_list(MoveList &move_list){
    std::cout << "\n     move    piece     capture   double    enpass    castling\n\n";
    for (uint8_t move_count = 0; move_count < move_list.count; move_count++){
        int move = move_list.moves[move_count];
        if (!Use_UTF8)
            // print move
            std::cout << "     " << square_to_coordinates[decode_move_source(move)] << square_to_coordinates[decode_move_target(move)]
                      << (decode_move_promoted(move) ? promoted_pieces[decode_move_promoted(move)] : ' ')
                      << "   " << ascii_pieces[make_index_piece(decode_move_piece(move))]
                      << "         " << (decode_move_capture(move) ? 1 : 0)
                      << "         " << (decode_move_double_push(move) ? 1 : 0)
                      << "         " << (decode_move_enpassant(move) ? 1 : 0)
                      << "         " << (decode_move_castling(move) ? 1 : 0) << '\n';

        else
            // print move
            std::cout << "     " << square_to_coordinates[decode_move_source(move)] << square_to_coordinates[decode_move_target(move)]
                      << (decode_move_promoted(move) ? promoted_pieces[decode_move_promoted(move)] : ' ')
                      << "   " << unicode_pieces[make_index_piece(decode_move_piece(move))]
                      << "         " << (decode_move_capture(move) ? 1 : 0)
                      << "         " << (decode_move_double_push(move) ? 1 : 0)
                      << "         " << (decode_move_enpassant(move) ? 1 : 0)
                      << "         " << (decode_move_castling(move) ? 1 : 0) << '\n';
    }
    std::cout << "\n\n     Total number of moves: " << static_cast<int>(move_list.count) << "\n\n";
}


void parse_fen(Board &board, const char *fen){
    std::memset(&board, 0, sizeof(board));
    board.enpassant = SQ_NONE;

    // board
    for (Rank rank = RANK_8; rank >= RANK_1; --rank){
        for (File file = FILE_A; file < FILE_NB; ++file){
            bool piece_down = 0;
            Square square = make_square(file, rank);

            if (_is_letter(*fen)){
                Piece piece = char_pieces[*fen];
                set_bit(board.bitboards[make_index_piece(piece)], square);
                fen++;
                piece_down = 1;
            }
            if (_is_number(*fen)){
                int offset = *fen - '0';
                file += (!file && !piece_down) ? offset - 1: offset;
                //file += (piece_down && !file) ? offset - 1 : offset;
                fen++;
            }
            if (*fen == '/') fen++;

        }
    }
    // side to move
    fen++; // skip initial space
    board.side = (*fen == 'w') ? WHITE : BLACK;

    // castling rights
    fen += 2; // skip 2 spaces
    while (*fen != ' '){
        switch (*fen){
            case 'K' : board.castle |= WK; break;
            case 'Q' : board.castle |= WQ; break;
            case 'k' : board.castle |= BK; break;
            case 'q' : board.castle |= BQ; break;
            case '-' : break;
        }
        fen++;
    }
    // enpassant square
    fen++; //skip initial space
    if (*fen != '-'){
        File file = File(fen[0] - 'a'); // fen[0] = *fen
        Rank rank = Rank(fen[1] - '0');
        board.enpassant = make_square(file, rank);
    }else{
        board.enpassant = SQ_NONE;
    }

    // occupancies
    for (PieceType piece = PAWN; piece < PIECE_TYPE_NB; ++piece){
        board.occupancies[WHITE] |= board.bitboards[make_index_piece(WHITE, piece)];
    }
    for (PieceType piece = PAWN; piece < PIECE_TYPE_NB; ++piece){
        board.occupancies[BLACK] |= board.bitboards[make_index_piece(BLACK, piece)];
    }
    board.occupancies[BOTH] = board.occupancies[WHITE] | board.occupancies[BLACK];

    //generate_hash_key(board);

    //printf("fen: %s\n", fen);
}