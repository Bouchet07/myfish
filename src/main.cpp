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
#include "tt.hpp"

inline void init_all(){
    init_leapers_attacks();
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
    // init_magic_numbers();
    init_random_keys();
}

int main(){
    init_all();
    #ifdef DEBUG 
    std::cout << COUNT_BITS_METHOD << "\n";
    std::cout << GET_LSB_METHOD << "\n";
    Board board;
    parse_fen(board, "4k3/Q7/8/4K3/8/8/8/8 w - - 1 0 ");
    //parse_fen(board, tricky_position);
    print_board(board);


    //perft_test(board, 5);
    Time time;
    search_position(board, time, 20);
    //init_random_keys();

    #else
    uci_loop();
    #endif
}