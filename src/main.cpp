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
    parse_position(board, "position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ");
    //make_move(board, parse_move(board, "a2a3"), ALL_MOVES);
    search_position(board, time, 7); */
    //parse_go(board, time, "go depth 4");
    /*for (Square s = SQ_A1; s <= SQ_H8; ++s) {
        num = find_magic_number(s, bishop_relevant_bits[s]);
        std::cout << num << "ULL,\n";
    } */
    /* Bitboard b = get_rook_attacks(SQ_B4, 1135261358510080);
    print_bitboard(b); */
    return 0;
}