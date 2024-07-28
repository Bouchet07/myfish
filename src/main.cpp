#include <iostream>
#include <chrono>

#include "types.h"
#include "bitboard.h"
#include "eval.h"
#include "uci.h"
#include "attacks.h"
#include "moves.h"
#include "search.h"
#include "tt.h"

// Global variables
TT tt(16);
BS::thread_pool pool(1);
std::mutex mtx;

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
    parse_position(board, "position kiwipete");
    //parse_position(board, "position repetition");
    parse_go(board, time, "go depth 8");
    parse_go(board, time, "go depth 8"); */
    //std::cout << get_game_phase_score(board);
    //parse_position(board, "position fen 3R4/8/4k3/8/P7/6K1/6PP/7r b - - 3 34");
    //make_move(board, parse_move(board, "a2a3"), ALL_MOVES);
    //search_position(board, time, 5);
    //parse_go(board, time, "go movetime 3000");
    /* Bitboard b = passed_pawn_mask(BLACK, SQ_C4);
    print_bitboard(b, true); */
    /* TT tt(16);
    std::cout << tt.table.capacity()*sizeof(TT_entry)/1024/1024 << '\n'; */
    /* Board board;
    TimeControl time;
    parse_fen(board, "3Qr1k1/4P1n1/8/8/3P4/1K6/PP6/8 w - - 1 42");
    print_board(board, true);
    //evaluate(board);
    parse_go(board, time, "go movetime 5200"); */
    /* Board board;
    parse_fen(board, start_position);
    TT_entry tt_entry = {board.hash_key,9,8,TT_flag(1),false};
    tt.store(tt_entry);
    TT_entry tt_entry2 = tt[board.hash_key];
    std::cout << (tt_entry == tt_entry2);

    parse_fen(board, Kiwipete);
    tt_entry = {board.hash_key,9,8,TT_flag(0),false};
    tt.store(tt_entry);
    tt_entry2 = tt[board.hash_key];
    std::cout << (tt_entry == tt_entry2); */

    return 0;
}