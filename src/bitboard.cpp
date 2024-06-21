#include <iostream>

#include "bitboard.h"

void print_bitboard(Bitboard bitboard){
    std::cout << "\n";

    // Top border
    if (USE_UTF8) {
        std::cout << "   ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    } else {
        std::cout << "   +---+---+---+---+---+---+---+---+\n";
    }

    for (Rank rank = Rank::RANK_1; rank < Rank::RANK_NB; ++rank) {
        // Row number
        if (USE_UTF8) {
            std::cout << " " << 8 - rank << " │";
        } else {
            std::cout << " " << 8 - rank << " |";
        }

        for (File file = File::FILE_A; file < File::FILE_NB; ++file) {
            Square square = make_square(file, rank);
            if (USE_UTF8) {
                std::cout << " " << (get_bit(bitboard, square) ? "●" : " ") << " │"; // Use filled/empty circles
            } else {
                std::cout << " " << (get_bit(bitboard, square) ? "1" : " ") << " |";
            }
        }   

        std::cout << "\n";
        if (rank != 7) { 
            if (USE_UTF8) {
                std::cout << "   ├───┼───┼───┼───┼───┼───┼───┼───┤\n"; // Inner dividers
            } else {
                std::cout << "   +---+---+---+---+---+---+---+---+\n"; // Inner dividers
            }
        }
    }

    // Bottom border
    if (USE_UTF8) {
        std::cout << "   └───┴───┴───┴───┴───┴───┴───┴───┘\n";
    } else {
        std::cout << "   +---+---+---+---+---+---+---+---+\n";
    }
    std::cout << "     a   b   c   d   e   f   g   h\n\n";
    std::cout << "   Bitboard: " << bitboard << "d\n\n"; 
}
