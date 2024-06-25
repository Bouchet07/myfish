#include <iostream>

#include "bitboard.h"

void print_bitboard(Bitboard bitboard){
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
