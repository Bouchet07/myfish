#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"
#include "bitboard.h"
#include "moves.h"
#include "eval.h"

int64_t get_time_ms();

Value negamax(Board &board, Tree &tree, TimeControl &time, Value alpha, Value beta, int depth);

void search_position(Board &board, TimeControl &time, int depth);

uint64_t perft(Board &board, int depth);

void perft_test(Board &board, int depth);

void bench_perft(Board &board, int depth);

#endif // SEARCH_H