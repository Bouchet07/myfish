#include <iostream>
#include <chrono>

#include "types.hpp"
#include "bitboard.hpp"
#include "attacks.hpp"
#include "moves.hpp"
#include "io.hpp"
#include "benchmark.hpp"
#include "search.hpp"
#include "uci.hpp"



int main(){
    init_all();
    #ifdef DEBUG 
    std::cout << COUNT_BITS_METHOD << "\n";
    std::cout << GET_LSB_METHOD << "\n";
    Board board;
    parse_fen(board, "r3k2r/pPppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ");
    
    print_board(board);

    perft_test(board, 5);
    //search_position(board, 7);
    //init_random_keys();

    #else
    uci_loop();
    #endif
}