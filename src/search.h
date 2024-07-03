#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"
#include "bitboard.h"
#include "moves.h"
#include "eval.h"

struct Tree
{
    int ply=0;
    Move best_move=0;
    uint32_t visited_nodes=0;
};

int64_t get_time_ms();

Value negamax(Board &board, Tree &tree, TimeControl &time, Value alpha, Value beta, int depth);

void search_position(Board &board, TimeControl &tiem, int depth);

uint64_t perft(Board &board, int depth);

void perft_test(Board &board, int depth);

void bench_perft(Board &board, int depth);

#endif // SEARCH_H