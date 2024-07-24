#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"
#include "bitboard.h"
#include "moves.h"
#include "eval.h"
#include "thread.h"

int64_t get_time_ms();

Value negamax(Board &board, Tree &tree, TimeControl &time, RT &rt, Value alpha, Value beta, uint8_t depth);

void search_position(Board &board, TimeControl &time, RT &rt, uint8_t depth);

uint64_t perft(Board &board, uint8_t depth);

void perft_test(Board &board, uint8_t depth, bool Use_UTF8 = false);

void bench_perft(Board &board, uint8_t depth);

void bench_go(Board &board, TimeControl &time, RT &rt, uint8_t depth);

#endif // SEARCH_H