#include <iostream>
#include <chrono>

#include "types.h"
#include "bitboard.h"
#include "uci.h"
#include "attacks.h"
#include "moves.h"

void init_all() {
    init_leapers_attacks();
    init_sliders_attacks(PieceType::BISHOP);
    init_sliders_attacks(PieceType::ROOK);
}

int main() {
    init_all();
    //UCI uci;
    //uci.init();
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
    //const char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const char *fen = start_position;
    Board board;
    parse_fen(board, fen);
    MoveList moves;
    generate_moves(board, moves);
    print_move_list(moves);


    return 0;
    #endif
}