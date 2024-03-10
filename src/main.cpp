#include <iostream>

#include "types.h"
#include "bitboard.h"
#include "uci.h"
#include "attacks.h"



int main() {
    #ifdef DEBUG
    //init_leapers_attacks();
    //print_bitboard(rook_attacks_on_the_fly(e5, FILE_C | RANK_4));
    //std::cout << set_occupancy()
    auto magic = find_magic_number(a8, bishop_relevant_bits[a8]);
    std::cout << "0x" << std::hex << magic << "ULL";
    #else
    UCI uci;
    uci.init();
    uci.loop();
    return 0;
    #endif
}