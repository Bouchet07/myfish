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

int main(int argc, char* argv[]) {
    init_all();
    UCI uci;
    uci.init();
    uci.loop(argc, argv);
    /* Board board;
    TimeControl time;
    Tree tree;
    TT tt(16);
    //parse_position(board, "position startpos");
    parse_position(board, "position fen r2qkb1r/pp2nppp/3p4/2pNN1B1/2BnP3/3P4/PPP2PPP/R2bK2R w KQkq - 0 1 ");
    //parse_position(board, "position fen 3R4/8/4k3/8/P7/6K1/6PP/7r b - - 3 34");
    //make_move(board, parse_move(board, "a2a3"), ALL_MOVES);
    //search_position(board, time, 5);
    parse_go(board, time, tt, "go movetime 3000"); */
    /*for (Square s = SQ_A1; s <= SQ_H8; ++s) {
        num = find_magic_number(s, bishop_relevant_bits[s]);
        std::cout << num << "ULL,\n";
    } */
    /* Bitboard b = get_rook_attacks(SQ_B4, 1135261358510080);
    print_bitboard(b); */
    /* TT tt(16);
    std::cout << tt.table.capacity()*sizeof(TT_entry)/1024/1024 << '\n'; */
    return 0;
}