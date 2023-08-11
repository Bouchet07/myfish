#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "moves.hpp"
#include "types.hpp"
#include "io.hpp"


#include <chrono>
#include <iostream>

int get_time_ms() {
    using namespace std::chrono;

    auto now = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(now.time_since_epoch());

    return static_cast<int>(duration.count());
}

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

using namespace std;
void perft_test(int depth){
    cout << "\n   Performance test\n\n";
    
    moves move_list[1];
    generate_moves(move_list);

    int start = get_time_ms();
    
    for (int move_count = 0; move_count < move_list->count; move_count++){
        copy_board();
        if(!make_move(move_list->moves[move_count], all_moves)) continue;
        long cummulative_nodes = nodes;
        perft_driver(depth-1);
        long old_nodes = nodes - cummulative_nodes;
        take_back();
        cout << "   move: " << square_to_coordinates[get_move_source(move_list->moves[move_count])]
             << square_to_coordinates[get_move_target(move_list->moves[move_count])]
             << (get_move_promoted(move_list->moves[move_count]) ? 
                promoted_pieces[get_move_promoted(move_list->moves[move_count])] : ' ')
             << "  nodes: " << old_nodes << "\n";


    }
    cout << "\n  Depth: " << depth;
    cout << "\n  Nodes: " << nodes;
    cout << "\n   Time: " << get_time_ms()-start << "ms";
}

#endif