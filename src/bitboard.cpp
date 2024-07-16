#include <iostream>

#include "bitboard.h"

void print_bitboard(Bitboard bitboard, bool Use_UTF8){
    std::cout << "\n";
    
    // Top border
    if (Use_UTF8) {
        std::cout << "   ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    } else {
        std::cout << "   +---+---+---+---+---+---+---+---+\n";
    }

    for (Rank rank = Rank::RANK_8; rank >= Rank::RANK_1; --rank) {
        // Row number
        if (Use_UTF8) {
            std::cout << " " << rank+1 << " │";
        } else {
            std::cout << " " << rank+1 << " |";
        }

        for (File file = File::FILE_A; file < File::FILE_NB; ++file) {
            Square square = make_square(file, rank);
            if (Use_UTF8) {
                std::cout << " " << (get_bit(bitboard, square) ? "●" : " ") << " │"; // Use filled/empty circles
            } else {
                std::cout << " " << (get_bit(bitboard, square) ? "1" : " ") << " |";
            }
        }   

        std::cout << "\n";
        if (rank) { 
            if (Use_UTF8) {
                std::cout << "   ├───┼───┼───┼───┼───┼───┼───┼───┤\n"; // Inner dividers
            } else {
                std::cout << "   +---+---+---+---+---+---+---+---+\n"; // Inner dividers
            }
        }
    }

    // Bottom border
    if (Use_UTF8) {
        std::cout << "   └───┴───┴───┴───┴───┴───┴───┴───┘\n";
    } else {
        std::cout << "   +---+---+---+---+---+---+---+---+\n";
    }
    std::cout << "     a   b   c   d   e   f   g   h\n\n";
    std::cout << "   Bitboard: " << bitboard << "d\n\n"; 
}

void print_board(Board &board, bool Use_UTF8){
    std::cout << "\n";
    
    // Top border
    if (Use_UTF8) {
        std::cout << "   ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    } else {
        std::cout << "   +---+---+---+---+---+---+---+---+\n";
    }

    for (Rank rank = Rank::RANK_8; rank >= Rank::RANK_1; --rank) {
        // Row number
        if (Use_UTF8) {
            std::cout << " " << rank+1 << " │";
        } else {
            std::cout << " " << rank+1 << " |";
        }

        for (File file = File::FILE_A; file < File::FILE_NB; ++file) {
            Square square = make_square(file, rank);
            bool broken = false;
            if (Use_UTF8) {
                for (PieceType pt = PieceType::PAWN; pt <= PieceType::KING; ++pt) {
                    if (get_bit(board.bitboards[make_index_piece(WHITE, pt)], square)) {
                        std::cout << " " << unicode_pieces[make_index_piece(WHITE, pt)] << " │"; // Use filled/empty circles
                        broken = true;
                        break;
                    } else if (get_bit(board.bitboards[make_index_piece(BLACK, pt)], square)) {
                        std::cout << " " << unicode_pieces[make_index_piece(BLACK, pt)] << " │"; // Use filled/empty circles
                        broken = true;
                        break;
                    }
                }
                if (!broken) std::cout << "   │";
            } else {
                for (PieceType pt = PieceType::PAWN; pt <= PieceType::KING; ++pt) {
                    if (get_bit(board.bitboards[make_index_piece(WHITE, pt)], square)) {
                        std::cout << " " << ascii_pieces[make_index_piece(WHITE, pt)] << " |"; // Use filled/empty circles
                        broken = true;
                        break;
                    } else if (get_bit(board.bitboards[make_index_piece(BLACK, pt)], square)) {
                        std::cout << " " << ascii_pieces[make_index_piece(BLACK, pt)] << " |"; // Use filled/empty circles
                        broken = true;
                        break;
                    }
                }
                if (!broken) std::cout << "   │";
            }
        }   

        std::cout << "\n";
        if (rank) { 
            if (Use_UTF8) {
                std::cout << "   ├───┼───┼───┼───┼───┼───┼───┼───┤\n"; // Inner dividers
            } else {
                std::cout << "   +---+---+---+---+---+---+---+---+\n"; // Inner dividers
            }
        }
    }

    // Bottom border
    if (Use_UTF8) {
        std::cout << "   └───┴───┴───┴───┴───┴───┴───┴───┘\n";
    } else {
        std::cout << "   +---+---+---+---+---+---+---+---+\n";
    }

    std::cout << "     a   b   c   d   e   f   g   h\n\n";
    std::cout << "     Side:     " << (board.side^1 ? "white" : "black") << '\n';
    std::cout << "     Enpassant:   " << (board.enpassant != SQ_NONE ? square_to_coordinates[board.enpassant] : "no") << '\n';
    std::cout << "     Castling:  " << (board.castle & WHITE_OO ? 'K' : '-') <<
                                      (board.castle & WHITE_OOO ? 'Q' : '-') <<
                                      (board.castle & BLACK_OO ? 'k' : '-') <<
                                      (board.castle & BLACK_OOO ? 'q' : '-') << "\n\n";
    std::cout << "     Hash key: " << std::hex << board.hash_key << '\n' << std::dec;
}