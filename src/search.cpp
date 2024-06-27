#include "search.h"
#include <iostream>

int negamax(Board &board, int depth, Move &best_move, int ply){
    if(depth == 0){
        return evaluate(board);
    }

    MoveList moves;
    generate_moves(board, moves);
    Move best_move_local;
    int score=0, best_score=-1000;
    for(uint8_t i = 0; i < moves.count; i++){
        Board board_copy = board;
        if(make_move(board, moves.moves[i], ALL_MOVES)==false){
            continue;
        }
        score = -negamax(board, depth - 1, best_move, ply);
        if (score > best_score) best_move_local = moves.moves[i];
        board = board_copy;
    }
    if (ply == depth){
        best_move = best_move_local;
    }
    return score;
}

void search_position(Board &board, int depth){
    Move best_move;
    negamax(board,depth,best_move,depth);
    std::cout << "bestmove ";
    print_move(best_move);
    std::cout << '\n';
    //std::cout << " Score: " << best_score << '\n';
}