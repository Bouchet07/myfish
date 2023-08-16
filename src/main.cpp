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
    parse_position(board, "position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    /* print_bitboard(pawn_attacks[board.side^1][f1] & board.bitboards[P]);
    std::cout << is_square_attacked(board, f1, black); */
    //print_board(board);
    perft_test(board, 5);
    //search_position(8);
    //uci_loop();
}