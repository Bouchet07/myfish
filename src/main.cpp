#include <iostream>
#include <chrono>

#include "types.h"
#include "bitboard.h"
#include "eval.h"
#include "uci.h"
#include "attacks.h"
#include "moves.h"
#include "search.h"

void init_all() {
    init_leapers_attacks();
    init_sliders_attacks(PieceType::BISHOP);
    init_sliders_attacks(PieceType::ROOK);
}

int main() {
    init_all();
    UCI uci;
    uci.init();
    #ifdef DEBUG
    
    #elif defined(BENCHMARK)
    init_all();
    auto start = std::chrono::high_resolution_clock::now();
    U64 test = 123456789;
    for (uint64_t i = 0; i < 50000000; i++) {
        for (Square s = 0; s < 64; s++) {
            test ^= get_bishop_attacks(square, FILE_C|RANK_3);
            test |= get_rook_attacks(square, FILE_C|RANK_3);
            test ^= get_queen_attacks(square, FILE_C|RANK_3);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Time taken for attacks: " << elapsed.count() << " milliseconds\n";
    std::cout << test;
    #else
    //uci.loop();
    /* Board board;
    parse_fen(board, start_position);
    make_move(board, parse_move(board, "b1a3"), ALL_MOVES);
    make_move(board, parse_move(board, "a7a6"), ALL_MOVES);
    perft_test(board, 1); */
    Bitboard b = get_rook_attacks(SQ_A1, FILE_B);
    print_bitboard(b);
    return 0;
    #endif
}