#include "search.h"
#include <iostream>

uint64_t perft(Board &board, int depth){
    if(depth == 0){
        return 1;
    }
    MoveList moves = generate_moves(board);
    uint64_t nodes = 0;
    Board board_copy;
    for(uint8_t i = 0; i < moves.count; i++){
        board_copy = board;
        if(make_move(board, moves.moves[i], ALL_MOVES)==false){
            continue;
        }
        nodes += perft(board, depth - 1);
        board = board_copy;
    }
    return nodes;
}

void perft_test(Board &board, int depth){
    print_board(board);
    std::cout << "\nStarting Perft Test to depth: " << depth << '\n';
    MoveList moves = generate_moves(board);
    uint64_t nodes = 0, moves_count = 0;
    Board board_copy;
    for(uint8_t i = 0; i < moves.count; i++){
        board_copy = board;
        if(make_move(board, moves.moves[i], ALL_MOVES)==false){
            continue;
        }
        moves_count++;
        uint64_t n = perft(board, depth - 1);
        nodes += n;
        std::cout << "Move: ";
        print_move(moves.moves[i]);
        std::cout << " Nodes: " << n << '\n';
        board = board_copy;
    }
    std::cout << "Total Nodes: " << nodes << '\n' << "Total Moves: " << moves_count << '\n';
}

int negamax(Board &board, int depth, Move &best_move, int ply){
    if(depth == 0){
        return evaluate(board);
    }

    MoveList moves = generate_moves(board);

    for(uint8_t i = 0; i < moves.count; i++){
        Board board_copy = board;
        if(make_move(board, moves.moves[i], ALL_MOVES)==false){
            continue;
        }
        best_move = moves.moves[i];
        board = board_copy;
    }
    return 0;
}

void search_position(Board &board, int depth){
    Move best_move;
    negamax(board,depth,best_move,depth);
    std::cout << "bestmove ";
    print_move(best_move);
    std::cout << '\n';
    //std::cout << " Score: " << best_score << '\n';
}