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
    uci.loop();
    /* Board board;
    parse_position(board, "position startpos moves e2e4 e7e5 g1f3 d8f6 c2c3 f6f4 d2d4 f4e4 f1e2 e5d4 f3d4 e8d8 e1g1 f8c5 c1e3 e4d5 b1d2 g8f6 d2f3 h8e8 f1e1 d7d6 b2b3 c8g4 c3c4 d5h5 h2h3 g4f3 e2f3 h5e5 f3b7 c5d4 e3d4 e5e1 d1e1 e8e1 a1e1 b8c6 d4f6 d8d7 b7a8 g7f6 a8c6 d7c6 e1e7 d6d5 b3b4 d5c4 g1f1 c4c3 f1e2 c3c2 e2d2 c6d6 e7f7 a7a5 b4a5 d6c5 d2c2 h7h5 f7f6 c5b5 a5a6 c7c5 a6a7 h5h4 a7a8q b5c4 a8a4 c4d5 f6f5 d5d6 a4a6 d6e7 a6f6");
    //make_move(board, parse_move(board, "a2a3"), ALL_MOVES);
    parse_go(board, "go depth 1"); */
    /* uint64_t num;
    for (Square s = SQ_A1; s <= SQ_H8; ++s) {
        num = find_magic_number(s, bishop_relevant_bits[s]);
        std::cout << num << "ULL,\n";
    } */
    /* Bitboard b = get_rook_attacks(SQ_B4, 1135261358510080);
    print_bitboard(b); */
    return 0;
    #endif
}