#include <iostream>

#include "types.h"
#include "bitboard.h"
#include "uci.h"
#include "attacks.h"



int main() {
    #ifdef DEBUG
    //init_leapers_attacks();
    //print_bitboard(rook_attacks_on_the_fly(e5, FILE_C | RANK_4));
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
    print_bitboard(get_rook_attacks(e5,FILE_C|RANK_4));
    #else
    UCI uci;
    uci.init();
    uci.loop();
    return 0;
    #endif
}