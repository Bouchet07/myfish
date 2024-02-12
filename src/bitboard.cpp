#include <iostream>

#include "bitboard.h"

void print_bitboard(U64 bitboard){
    std::cout << "\n";

    for (int rank = 0; rank < 8; rank++){
        for (int file = 0; file < 8; file++){

            int square = rank * 8 + file;
            if (!file) std::cout << "  " << 8 - rank << " ";
            
            std::cout << " " << (get_bit(bitboard, square) ? 1 : 0);
            
        }
        std::cout << "\n";
    }
    std::cout << "\n     a b c d e f g h\n\n";

    // print bitboard as unsigned decimal number
    std::cout << "     Bitboard: " << bitboard << "d\n\n";
    //printf("     Bitboard: %llud\n\n", bitboard);
}