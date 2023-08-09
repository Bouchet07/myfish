#include <iostream>

#include "types.h"
#include "bitboard.h"
#include "magic.h"
#include "attacks.h"
#include "moves.h"
#include "io.h"
#include "benchmark.h"

int main(){
    init_all();
    std::cout << COUNT_BITS_METHOD << "\n";
    std::cout << GET_LSB_METHOD << "\n";

    
    parse_fen("rnbqkbnr/ppppPppp/8/4p3/8/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1 ");
    print_board();
    //printf("negamax: %d\n", negamax(-5000, 5000, 1));
    perft_test(5);

}