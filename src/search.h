#ifndef SEARCH_H
#define SEARCH_H

#include "moves.h"
#include "eval.h"

extern int ply;
extern int best_move;

/**
 * alpha -> maximazing player best socre
 * beta -> minimazing player best socre
*/
static inline int negamax(int alpha, int beta, int depth){
    if (depth == 0){
        return evaluate();
    }
    nodes++;

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
            
            if (ply == 0){ // root move
                best_move = move_list->moves[count];
            }
        }
    }
    // node (move) fails low
    return alpha;
}

int rmove();
void search_position(int depth);


#endif