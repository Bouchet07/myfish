#include "search.h"


int negamax(Board &board, int alpha, int beta, int depth){
    if(depth == 0){
        return evaluate(board);
    }

    /* MoveList moves;
    generate_moves(board, moves);
    for(uint8_t i = 0; i < moves.count; i++){
        board.makeMove(moves[i]);
        int score = -negamax(board, -beta, -alpha, depth - 1);
        board.undoMove();

        if(score >= beta){
            return beta;
        }

        if(score > alpha){
            alpha = score;
        }
    } */

    return alpha;
}