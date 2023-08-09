#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "moves.h"
#include "types.h"
#include "io.h"

#ifdef WIN64
    #include<windows.h>
    #define PLATFORM "Windows"
#else
    #include<sys/time.h>
    #define PLATFORM "Linux"
#endif

int get_time_ms();

static inline void perft_driver(int depth){
    if (depth == 0){
        nodes++;
        return;
    }else{
        moves move_list[1];
        generate_moves(move_list);

        for (int move_count = 0; move_count < move_list->count; move_count++){
            copy_board();
            if(!make_move(move_list->moves[move_count], all_moves)) continue;
            perft_driver(depth-1);
            take_back();
        }
    }
}
void perft_test(int depth);

#endif