#ifndef SEARCH_H
#define SEARCH_H

#include "moves.hpp"
#include "eval.hpp"

#include <iostream>

#include "io.hpp"

// half move counter
static int ply;

static int best_move;

inline int quiescence(int alpha, int beta){
    int evaluation = evaluate();
    // fail-hard beta cutoff
    if (evaluation >= beta){
        return beta; // node (move) fails high
    }
    // found better move
    if (evaluation > alpha){
        alpha = evaluation; // Principal Variation (PV) node (move)
    }
    
    moves move_list[1];
    generate_moves(move_list);

    for (int count = 0; count < move_list->count; count++){
        copy_board();
        ply++;

        // ilegal move
        if (make_move(move_list->moves[count], only_captures) == 0){
            ply--;
            continue;
        }

        int score = -quiescence(-beta, -alpha);
        
        take_back(); // ?
        ply--;
        
        // fail-hard beta cutoff
        if (score >= beta){
            return beta; // node (move) fails high
        }
        // found better move
        if (score > alpha){
            alpha = score; // Principal Variation (PV) node (move)
        }
    }
    // node (move) fails low
    return alpha;
}

/**
 * alpha -> maximazing player best socre
 * beta -> minimazing player best socre
*/
inline int negamax(int alpha, int beta, int depth){
    if (depth == 0) return quiescence(alpha, beta);

    nodes++;

    int in_check = is_square_attacked((side^1) ? get_LSB(bitboards[K]) : get_LSB(bitboards[k]), side^1);
    int legal_moves = 0;

    moves move_list[1];
    generate_moves(move_list);

    for (int count = 0; count < move_list->count; count++){
        copy_board();
        ply++;

        // ilegal move
        if (make_move(move_list->moves[count], all_moves) == 0){
            ply--;
            continue;
        }

        legal_moves++;
        int score = -negamax(-beta, -alpha, depth-1);
        
        take_back(); // ?
        ply--;
        
        // fail-hard beta cutoff
        if (score >= beta){
            return beta; // node (move) fails high
        }
        // found better move
        if (score > alpha){
            alpha = score; // Principal Variation (PV) node (move)
            if (ply == 0) best_move = move_list->moves[count];// root move
        }
    }
    if (legal_moves == 0){
        if (in_check) return -49000 + ply; // +ply is important to priorize shortest checkmate
        return 0; // stalemate
    }
    // node (move) fails low
    return alpha;
}

inline int rmove(){
    //srand(time(NULL));
    moves move_list[1];
    generate_moves(move_list);
    int random = std::rand() % move_list->count + 1;
    while (make_move(move_list->moves[random], all_moves) == 0){
        random = rand() % move_list->count + 1;
    }
    make_move(move_list->moves[random], all_moves);
    return move_list->moves[random];

}

inline void search_position(int depth){
    int score = negamax(-50000, 50000, depth);
    std::cout << "bestmove ";
    print_move(best_move);
    std::cout << "\n";
}


#endif