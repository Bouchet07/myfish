#include <iostream>

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
    std::cout << COUNT_BITS_METHOD << "\n";
    std::cout << GET_LSB_METHOD << "\n";

    Board board;
    parse_fen(board, start_position);
    print_board(board);
    perft_test(board, 5);
    //search_position(8);
    //uci_loop();
}