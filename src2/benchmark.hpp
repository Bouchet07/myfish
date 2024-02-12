#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "types.hpp"
#include "moves.hpp"
#include "io.hpp"
#include "misc.hpp"


#include <iostream>
#include <iomanip> // For std::fixed and std::setprecision



inline U64 perft_driver(Board &board, int depth){
    if (depth == 0) return 1;
    else{
        U64 nodes = 0ULL;
        moves move_list;
        generate_moves(board, move_list);

        for (int move_count = 0; move_count < move_list.count; move_count++){
            Board copy_board = board;
            if(!make_move(board, move_list.moves[move_count], all_moves)) continue;
            nodes += perft_driver(board, depth-1);
            board = copy_board;
        }
        return nodes;
    }
}

void perft_test(Board &board, int depth){
    std::cout << "\n   Performance test\n\n";

    U64 nodes = 0;
    
    moves move_list;
    generate_moves(board, move_list);

    int start = get_time_ms();
    
    for (int move_count = 0; move_count < move_list.count; move_count++){
        Board copy_board = board;
        if(!make_move(board, move_list.moves[move_count], all_moves)) continue;
        U64 child_nodes = perft_driver(board, depth-1);
        nodes += child_nodes;

        board = copy_board;
        std::cout << "   move: " << square_to_coordinates[get_move_source(move_list.moves[move_count])]
                  << square_to_coordinates[get_move_target(move_list.moves[move_count])]
                  << (get_move_promoted(move_list.moves[move_count]) ? 
                      promoted_pieces[get_move_promoted(move_list.moves[move_count])] : ' ')
                  << "  nodes: " << child_nodes << "\n";
    }
    int duration = get_time_ms() - start;
    std::cout << "\n  Depth: " << depth;
    std::cout << "\n  Nodes: " << nodes;
    std::cout << "\n   Time: " << duration << "ms";
    double result = static_cast<double>(nodes) / static_cast<double>(duration);
    std::cout << "\nNodes/s: " << std::fixed <<std::setprecision(1) << result << " k/s";
}

#endif