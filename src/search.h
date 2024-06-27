#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"
#include "bitboard.h"
#include "moves.h"
#include "eval.h"

int negamax(Board &board, int alpha, int beta, int depth);

void search_position(Board &board, int depth);

#endif // SEARCH_H