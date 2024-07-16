#include <iostream>

#include "moves.h"
#include "tt.h"
#include <cstring>
#include <algorithm>

MoveList generate_moves(Board &board){
    MoveList move_list;
    move_list.reserve(256);
    //move_list.count = 0;
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
                if (board.castle & WHITE_OO){
                    if (two_squares_available(board,SQ_F1,SQ_G1)){
                        if (!is_square_attacked(board, SQ_E1, BLACK) && !is_square_attacked(board, SQ_F1, BLACK)){
                            add_move(move_list, encode_move(SQ_E1, SQ_G1, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 1));
                        }   
                    }
                }
                if (board.castle & WHITE_OOO){
                    if (three_squares_available(board,SQ_B1,SQ_C1,SQ_D1)){
                        if (!is_square_attacked(board, SQ_E1, BLACK) && !is_square_attacked(board, SQ_D1, BLACK)){
                            add_move(move_list, encode_move(SQ_E1, SQ_C1, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 1));
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
                if (board.castle & BLACK_OO){
                    if (two_squares_available(board,SQ_F8,SQ_G8)){
                        if (!is_square_attacked(board, SQ_E8, WHITE) && !is_square_attacked(board, SQ_F8, WHITE)){
                            add_move(move_list, encode_move(SQ_E8, SQ_G8, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 1));
                        }   
                    }
                }
                if (board.castle & BLACK_OOO){
                    if (three_squares_available(board,SQ_B8,SQ_C8,SQ_D8)){
                        if (!is_square_attacked(board, SQ_E8, WHITE) && !is_square_attacked(board, SQ_D8, WHITE)){
                            add_move(move_list, encode_move(SQ_E8, SQ_C8, make_piece(board.side, piece), NO_PIECE_TYPE, 0, 0, 0, 1));
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
    return move_list;
}

void print_move_list(MoveList &move_list, bool Use_UTF8){
    std::cout << "\n     move    piece     capture   double    enpass    castling\n\n";
    for (uint8_t move_count = 0; move_count < move_list.size(); move_count++){
        int move = move_list[move_count].move;
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
    std::cout << "\n\n     Total number of moves: " << static_cast<int>(move_list.size()) << "\n\n";
}

void parse_fen(Board &board, std::string_view fen){
    rt.clear();
    board = Board();
    board.enpassant = SQ_NONE;

    // board
    for (Rank rank = RANK_8; rank >= RANK_1; --rank){
        for (File file = FILE_A; file < FILE_NB; ++file){
            bool piece_down = 0;
            Square square = make_square(file, rank);

            if (_is_letter(fen.front())){
                Piece piece = char_pieces.at(fen.front());
                set_bit(board.bitboards[make_index_piece(piece)], square);
                fen.remove_prefix(1);
                piece_down = true;
            }
            if (_is_number(fen.front())){
                int offset = fen.front() - '0';
                file += (!file && !piece_down) ? offset - 1 : offset;
                fen.remove_prefix(1);
            }
            if (fen.front() == '/') fen.remove_prefix(1);

        }
    }
    // side to move
    fen.remove_prefix(1); // skip initial space
    board.side = (fen.front() == 'w') ? WHITE : BLACK;

    // castling rights
    fen.remove_prefix(2); // skip 2 spaces
    while (fen.front() != ' ') {
        switch (fen.front()) {
            case 'K' : board.castle |= WHITE_OO; break;
            case 'Q' : board.castle |= WHITE_OOO; break;
            case 'k' : board.castle |= BLACK_OO; break;
            case 'q' : board.castle |= BLACK_OOO; break;
            case '-' : break;
        }
        fen.remove_prefix(1);
    }
    // enpassant square
    fen.remove_prefix(1); // skip initial space
    if (fen.front() != '-') {
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

    generate_hash_key(board);
    rt[rt.index] = board.hash_key;

    //printf("fen: %s\n", fen);
}

// updates board if move is valid
bool make_move(Board &board, Move move, MoveFlag move_flag){
    // quiet moves
    if (move_flag == ALL_MOVES){
        Board copy_board = board;

        Square source_square = decode_move_source(move);
        Square target_square = decode_move_target(move);
        Piece piece = decode_move_piece(move);
        PieceType promoted = decode_move_promoted(move);
        bool capture = decode_move_capture(move);
        bool doublepp = decode_move_double_push(move);
        bool enpass = decode_move_enpassant(move);
        bool castling = decode_move_castling(move);

        // move piece
        pop_bit(board.bitboards[make_index_piece(piece)], source_square);
        set_bit(board.bitboards[make_index_piece(piece)], target_square);

        // uptade hash key
        board.hash_key ^= zobrist_keys[get_zobrist_key(piece, source_square)]  // take back source square
                       ^  zobrist_keys[get_zobrist_key(piece, target_square)]; // put target square

        // occupancies
        pop_bit(board.occupancies[board.side], source_square);
        set_bit(board.occupancies[board.side], target_square);

        // remove enpassant move
        if (board.enpassant != SQ_NONE) board.hash_key ^= zobrist_keys[ep_index];
        board.enpassant = SQ_NONE;

        // capture move
        if (capture){
            Piece start_piece, end_piece;
            if (board.side==WHITE){
                start_piece = B_PAWN;
                end_piece = B_KING;
            }else{
                start_piece = W_PAWN;
                end_piece = W_KING;
            }
            for (Piece bb_piece = start_piece; bb_piece <= end_piece; ++bb_piece){
                // op_bit(bitboards[bb_piece], target_square); // we just eliminate everything, faster? no
                if (get_bit(board.bitboards[make_index_piece(bb_piece)], target_square)){
                    pop_bit(board.bitboards[make_index_piece(bb_piece)], target_square);
                    board.hash_key ^= zobrist_keys[get_zobrist_key(bb_piece, target_square)]; // uptade capture hash
                    break;
                }
            }
            // occupancies
            pop_bit(board.occupancies[~board.side], target_square);
        }
        // promotions
        if (promoted){
            pop_bit(board.bitboards[make_index_piece(piece)], target_square);
            set_bit(board.bitboards[make_index_piece(board.side, promoted)], target_square);
            board.hash_key ^= zobrist_keys[get_zobrist_key(piece, target_square)]
                           ^  zobrist_keys[get_zobrist_key(promoted, board.side, target_square)];
        }

        else if (enpass){
            if (board.side==WHITE){ // white
                pop_bit(board.bitboards[make_index_piece(B_PAWN)], target_square + SOUTH);
                pop_bit(board.occupancies[BLACK], target_square + SOUTH);
                board.hash_key ^= zobrist_keys[get_zobrist_key(B_PAWN, target_square + SOUTH)];
            }else{ // black
                pop_bit(board.bitboards[make_index_piece(W_PAWN)], target_square + NORTH);
                pop_bit(board.occupancies[WHITE], target_square + NORTH);
                board.hash_key ^= zobrist_keys[get_zobrist_key(W_PAWN, target_square + NORTH)];
            }

        }

        else if (doublepp){
            board.enpassant = (board.side==WHITE) ? source_square + NORTH : source_square + SOUTH;
            board.hash_key ^= zobrist_keys[ep_index];
        }

        else if (castling){
            switch (target_square){
            case SQ_G1:
                pop_bit(board.bitboards[make_index_piece(W_ROOK)], SQ_H1);
                set_bit(board.bitboards[make_index_piece(W_ROOK)], SQ_F1);
                pop_bit(board.occupancies[board.side], SQ_H1);
                set_bit(board.occupancies[board.side], SQ_F1);
                board.hash_key ^= zobrist_keys[get_zobrist_key(W_ROOK, SQ_H1)] ^ zobrist_keys[get_zobrist_key(W_ROOK, SQ_F1)];
                break;
            case SQ_C1:
                pop_bit(board.bitboards[make_index_piece(W_ROOK)], SQ_A1);
                set_bit(board.bitboards[make_index_piece(W_ROOK)], SQ_D1);
                pop_bit(board.occupancies[board.side], SQ_A1);
                set_bit(board.occupancies[board.side], SQ_D1);
                board.hash_key ^= zobrist_keys[get_zobrist_key(W_ROOK, SQ_A1)] ^ zobrist_keys[get_zobrist_key(W_ROOK, SQ_D1)];
                break;
            case SQ_G8:
                pop_bit(board.bitboards[make_index_piece(B_ROOK)], SQ_H8);
                set_bit(board.bitboards[make_index_piece(B_ROOK)], SQ_F8);
                pop_bit(board.occupancies[board.side], SQ_H8);
                set_bit(board.occupancies[board.side], SQ_F8);
                board.hash_key ^= zobrist_keys[get_zobrist_key(B_ROOK, SQ_H8)] ^ zobrist_keys[get_zobrist_key(B_ROOK, SQ_F8)];
                break;
            case SQ_C8:
                pop_bit(board.bitboards[make_index_piece(B_ROOK)], SQ_A8);
                set_bit(board.bitboards[make_index_piece(B_ROOK)], SQ_D8);
                pop_bit(board.occupancies[board.side], SQ_A8);
                set_bit(board.occupancies[board.side], SQ_D8);
                board.hash_key ^= zobrist_keys[get_zobrist_key(B_ROOK, SQ_A8)] ^ zobrist_keys[get_zobrist_key(B_ROOK, SQ_D8)];
                break;
            default:
                break;
            }
        }
        // update both occupancies
        board.occupancies[BOTH] = board.occupancies[WHITE] | board.occupancies[BLACK];
        // change side
        board.side = ~board.side;
        board.hash_key ^= zobrist_keys[side_index];
        // ilegal move (king in check after move) (bitboards, occupancies, and side have to be updated for is_sq_at to work)
        if (is_square_attacked(board, (board.side==WHITE) ? get_LSB(board.bitboards[make_index_piece(B_KING)]) : get_LSB(board.bitboards[make_index_piece(W_KING)]), board.side)){
            board = copy_board; // Nothing happend here
            return 0; // ilegal move
        }
        // update castling rights
        board.hash_key ^= get_zobrist_castle(board.castle); // remove previous castling
        board.castle &= castling_rights(source_square)
                     &  castling_rights(target_square);
        board.hash_key ^= get_zobrist_castle(board.castle); // set current one

        if (capture || type_of(piece)==PAWN){
            board.fifty = 0;
        }else{
            board.fifty++;
        }

        return 1;
    }
    // capture moves
    else{
        if (decode_move_capture(move)){ // capture
            return make_move(board, move, ALL_MOVES); // this return is crazy
        }
        return 0;
    }
}

void print_move(Move move){
    if (decode_move_promoted(move)){
        std::cout << square_to_coordinates[decode_move_source(move)]
             << square_to_coordinates[decode_move_target(move)]
             << promoted_pieces[decode_move_promoted(move)];
    }else{
        std::cout << square_to_coordinates[decode_move_source(move)]
             << square_to_coordinates[decode_move_target(move)];
    }
    
}

Move parse_move(Board &board, std::string_view move_string){
    MoveList move_list = generate_moves(board);

    Square source_square = make_square(File(move_string[0] - 'a'), Rank(move_string[1] - '0'-1));
    Square target_square = make_square(File(move_string[2] - 'a'), Rank(move_string[3] - '0'-1));


    for (size_t move_count = 0; move_count < move_list.size(); move_count++){
        Move move = move_list[move_count].move;

        if (source_square == decode_move_source(move) && target_square == decode_move_target(move)){
            PieceType promoted_piece = decode_move_promoted(move);
            if (promoted_piece){
                if ((promoted_piece == QUEEN) && move_string[4] == 'q') return move;
                if ((promoted_piece == ROOK) && move_string[4] == 'r') return move;
                if ((promoted_piece == BISHOP) && move_string[4] == 'b') return move;
                if ((promoted_piece == KNIGHT) && move_string[4] == 'n') return move;
                continue;
            }
            return move;
        }
    }
    return 0;

}

void parse_position(Board &board, std::string_view command){
    command.remove_prefix(9); // Skip "position "
    std::string_view current_view = command;

    if (command.substr(0, 8) == "startpos") {
        parse_fen(board, start_position);
    } else if (command.substr(0, 8) == "kiwipete"){
        parse_fen(board, Kiwipete);
    } else if (command.substr(0, 6) == "killer"){
        parse_fen(board, killer_position);
    } else if (command.substr(0, 3) == "cmk"){
        parse_fen(board, cmk_position);
    } else if (command.substr(0, 7) == "endgame"){
        parse_fen(board, endgame);
    }else if(command.substr(0,10) == "repetition"){
        parse_fen(board, repetition);
    }
    else {
        auto fen_pos = command.find("fen");
        if (fen_pos == std::string_view::npos) {
            parse_fen(board, start_position);
        } else {
            fen_pos += 4; // Skip "fen "
            current_view = command.substr(fen_pos);
            parse_fen(board, current_view);
        }
    }
    auto moves_pos = command.find("moves");
    if (moves_pos != std::string_view::npos) {
        moves_pos += 6; // Skip "moves "
        current_view = command.substr(moves_pos);

        while (!current_view.empty()) {
            auto space_pos = current_view.find(' ');
            std::string_view move_str = space_pos == std::string_view::npos
                ? current_view
                : current_view.substr(0, space_pos);

            Move move = parse_move(board, move_str);
            if (move == 0) break;
            if (make_move(board, move, ALL_MOVES)==false){
                break;
            }
            rt[++rt.index] = board.hash_key;

            current_view.remove_prefix(move_str.size() + (space_pos == std::string_view::npos ? 0 : 1));
        }
    }
    //print_board(board);
}

// most valuable victim & less valuable attacker

/*
                          
    (Victims) Pawn Knight Bishop   Rook  Queen   King
  (Attackers)
        Pawn   105    205    305    405    505    605
      Knight   104    204    304    404    504    604
      Bishop   103    203    303    403    503    603
        Rook   102    202    302    402    502    602
       Queen   101    201    301    401    501    601
        King   100    200    300    400    500    600

*/

// MVV LVA [attacker][victim]
constexpr int mvv_lva[12][12] = {
 	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
};

int score_move(const Board &board, Tree &tree, const Move move){
    if (tree.score_pv){
        if (tree.pv[0][tree.ply] == move){
            tree.score_pv = 0;
            return 20000;
        }
    } 
    int score = 0;
    if (decode_move_capture(move)){ // capture
        for (PieceType piece = PAWN; piece < PIECE_TYPE_NB; ++piece){
            if (get_bit(board.bitboards[make_index_piece(~board.side, piece)], decode_move_target(move))){
                score = mvv_lva[make_index_piece(decode_move_piece(move))][make_index_piece(board.side,piece)]+10000;//+10000 to make sure captures go first
                break;
            }
        }
    }else{  // quiet move
        if (tree.killer_moves[0][tree.ply] == move){
            score = 9000;
        }else if(tree.killer_moves[1][tree.ply] == move){
            score = 8000;
        }else{
            score = tree.history_moves[make_index_piece(decode_move_piece(move))][decode_move_target(move)];
        }
    }
    return score;
}

void score_moves(MoveList &move_list, Tree &tree, const Board &board){
    for (uint8_t i = 0; i < move_list.size(); i++){
        move_list[i].score = score_move(board, tree, move_list[i].move);
    }
}

void sort_moves(MoveList &move_list, Tree &tree, const Board &board){
    score_moves(move_list, tree, board);
    std::sort(move_list.begin(), move_list.end(), [](const MoveScore &a, const MoveScore &b){
        return a.score > b.score;
    });
}

void print_moves_score(Board &board, Tree &tree, MoveList &move_list){
    std::cout << "\nmove \t\tscore\n";
    for (uint8_t i = 0; i < move_list.size(); i++){
        print_move(move_list[i].move);
        std::cout << "\t\t" << score_move(board, tree, move_list[i].move) << '\n';
    }
}