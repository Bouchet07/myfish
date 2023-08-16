#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "types.hpp"
#include "moves.hpp"
#include "io.hpp"


#include <chrono>
#include <iostream>

int get_time_ms() {
    using namespace std::chrono;

    auto now = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(now.time_since_epoch());

    return static_cast<int>(duration.count());
}

inline void perft_driver(Board &board, Tree &tree, int depth){
    if (depth == 0){
        tree.nodes++;
        return;
    }else{
        moves move_list;
        generate_moves(board, move_list);

        for (int move_count = 0; move_count < move_list.count; move_count++){
            Board copy_board = board;
            if(!make_move(board, move_list.moves[move_count], all_moves)) continue;
            perft_driver(board, tree, depth-1);
            board = copy_board;
        }
    }
}

using namespace std;
void perft_test(Board &board, int depth){
    cout << "\n   Performance test\n\n";

    // New tree
    Tree tree;
    
    moves move_list;
    generate_moves(board, move_list);

    int start = get_time_ms();
    
    for (int move_count = 0; move_count < move_list.count; move_count++){
        Board copy_board = board;
        if(!make_move(board, move_list.moves[move_count], all_moves)) continue;
        long cummulative_nodes = tree.nodes;
        perft_driver(board, tree, depth-1);
        long old_nodes = tree.nodes - cummulative_nodes;
        board = copy_board;
        cout << "   move: " << square_to_coordinates[get_move_source(move_list.moves[move_count])]
             << square_to_coordinates[get_move_target(move_list.moves[move_count])]
             << (get_move_promoted(move_list.moves[move_count]) ? 
                promoted_pieces[get_move_promoted(move_list.moves[move_count])] : ' ')
             << "  nodes: " << old_nodes << "\n";


    }
    cout << "\n  Depth: " << depth;
    cout << "\n  nodes: " << tree.nodes;
    cout << "\n   Time: " << get_time_ms()-start << "ms";
}

#endif