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
    //parse_position(board, "position startpos moves b1c3 b8c6 d2d4 d7d5 e2e4 d5e4 d4d5 c6e5 d1d4 e5d7 d4e4 g8f6 e4a4 e7e6");
    //parse_position(board, "position fen 3R4/8/4k3/8/P7/6K1/6PP/7r b - - 3 34");
    //make_move(board, parse_move(board, "a2a3"), ALL_MOVES);
    //search_position(board, time, 5);
    parse_go(board, time, "go bench 3"); */
    /*for (Square s = SQ_A1; s <= SQ_H8; ++s) {
        num = find_magic_number(s, bishop_relevant_bits[s]);
        std::cout << num << "ULL,\n";
    } */
    /* Bitboard b = get_rook_attacks(SQ_B4, 1135261358510080);
    print_bitboard(b); */
    return 0;
}