#include <iostream>
#include <chrono>

#include "types.h"
#include "bitboard.h"
#include "uci.h"
#include "attacks.h"

void init_all() {
    init_leapers_attacks();
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
}

int main() {
    #ifdef DEBUG
    init_all();
    
    #elif defined(BENCHMARK)
    init_all();
    auto start = std::chrono::high_resolution_clock::now();
    U64 test = 123456789;
    for (uint64_t i = 0; i < 50000000; i++) {
        for (uint8_t square = 0; square < 64; square++) {
            test ^= get_bishop_attacks(square, FILE_C|RANK_3);
            test |= get_rook_attacks(square, FILE_C|RANK_3);
            test ^= get_queen_attacks(square, FILE_C|RANK_3);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Time taken: " << elapsed.count() << " milliseconds\n";
    std::cout << test;
    #else
    UCI uci;
    uci.init();
    uci.loop();
    return 0;
    #endif
}