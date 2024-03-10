#include <iostream>

#include "bitboard.h"

void print_bitboard(U64 bitboard){
    std::cout << "\n";

    // Top border
    if (USE_UTF8) {
        std::cout << "   ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    } else {
        std::cout << "   +---+---+---+---+---+---+---+---+\n";
    }

    for (uint8_t rank = 0; rank < 8; rank++) {
        // Row number
        if (USE_UTF8) {
            std::cout << " " << 8 - rank << " │";
        } else {
            std::cout << " " << 8 - rank << " |";
        }

        for (uint8_t file = 0; file < 8; file++) {
            int square = rank * 8 + file;
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
