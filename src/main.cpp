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

    
    //parse_fen("rnbqkbnr/ppppPppp/8/4p3/8/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1 ");
    print_board();
    //perft_test(6);
    uci_loop();
}