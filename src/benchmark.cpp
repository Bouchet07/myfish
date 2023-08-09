#include "benchmark.h"

#include <iostream>

int get_time_ms(){
    #ifdef WIN64
        return GetTickCount();
    #else
        struct timeval time_value;
        gettimeofday(&time_value, NULL);
        return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
    #endif
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