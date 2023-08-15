#ifndef SEARCH_H
#define SEARCH_H

#include "moves.hpp"
#include "eval.hpp"

#include <iostream>
#include <algorithm>

// most valuable victim & less valuable attacker

/*
                          
    (Victims) Pawn Knight Bishop   Rook  Queen   King
  (Attackers)
        Pawn   105    205    305    405    505    605
      Knight   104    204    304    404    504    604
      Bishop   103    203    303    403    503    603
        Rook   102    202    302    402    502    602
       Queen   101    201    301    401    501    601
        King   100    200    300    400    500    600

*/

// MVV LVA [attacker][victim]
constexpr int mvv_lva[12][12] = {
 	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
};

// max ply reachable within a search
constexpr int max_ply = 64;

// killer moves [id][ply]
static int killer_moves[2][max_ply];

// history moves [piece][square]
static int history_moves[12][64];

/*
      ================================
            Triangular PV table
      --------------------------------
        PV line: e2e4 e7e5 g1f3 b8c6
      ================================

           0    1    2    3    4    5
      
      0    m1   m2   m3   m4   m5   m6
      
      1    0    m2   m3   m4   m5   m6 
      
      2    0    0    m3   m4   m5   m6
      
      3    0    0    0    m4   m5   m6
       
      4    0    0    0    0    m5   m6
      
      5    0    0    0    0    0    m6
*/
// This whole pv must be optimize
// PV length [ply]
static int pv_length[max_ply];

// PV table [ply][ply]
static int pv_table[max_ply][max_ply];

// follow PV & score PV move
static int follow_pv, score_pv;

// half move counter
static int ply;

// enable PV move scoring
inline void enable_pv_score(moves *move_list){
    // disable following PV
    follow_pv = 0;

    for (int count = 0; count < move_list->count; count++){
        // make sure PV move is hit
        if (pv_table[0][ply] == move_list->moves[count]){
            score_pv = 1;
            follow_pv = 1;
        }
    }
}

/*  =======================
         Move ordering
    =======================
    
    1. PV move
    2. Captures in MVV/LVA
    3. 1st killer move
    4. 2nd killer move
    5. History moves
    6. Unsorted moves
*/
inline int score_move(int move){
    // if PV move scoring is allowed
    if (score_pv){
        if (pv_table[0][ply] == move){
           score_pv = 0;
           // give PV move the highest score to search it first
           return 20000; 
        } 
    }
    
    // score capture moves
    if (get_move_capture(move)){
        int target_piece = P; // doesn't matter color of pawn for score perspective of mvv_lva

        int start_piece, end_piece;
        if (side^1) {start_piece = p; end_piece = k;}
        else {start_piece = P; end_piece = K;}

        for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++){
            // op_bit(bitboards[bb_piece], target_square); // we just eliminate everything, faster? no
            if (get_bit(bitboards[bb_piece], get_move_target(move))){
                target_piece = bb_piece;
                break;
            }
        }
        return mvv_lva[get_move_piece(move)][target_piece] + 10000;
    }else{
        // score 1st killer move
        if (killer_moves[0][ply] == move) return 9000;

        // score 2nd killer move
        else if (killer_moves[1][ply] == move) return 8000;

        // score hisroty move
        else return history_moves[get_move_piece(move)][get_move_target(move)];
    }
    // score quiet moves
    return 0;
}

inline void sort_moves(moves *move_list){
    std::sort(move_list->moves, move_list->moves + move_list->count,
              [](int move_1, int move_2) {return score_move(move_1) > score_move(move_2);});
}

inline int quiescence(int alpha, int beta){
    nodes++;
    
    int evaluation = evaluate();
    // fail-high beta cutoff
    if (evaluation >= beta){
        return beta; // node (move) fails high
    }
    // found better move
    if (evaluation > alpha){
        alpha = evaluation; // Principal Variation (PV) node (move)
    }
    
    moves move_list[1];
    generate_moves(move_list);
    sort_moves(move_list); // from 200 thousand to 1613 nodes, even more crazy

    for (int count = 0; count < move_list->count; count++){
        copy_board();

        // not a capture
        if (make_move(move_list->moves[count], only_captures) == 0){
            continue;
        }
        // capture
        ply++;
        int score = -quiescence(-beta, -alpha);
        
        take_back(); // ?
        ply--;
        
        // fail-high beta cutoff
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

constexpr int full_depth_moves = 4;
constexpr int reduction_limit = 3;
//constexpr int R = 2; // cannot call it R

/**
 * alpha -> maximazing player best socre
 * beta -> minimazing player best socre
*/
inline int negamax(int alpha, int beta, int depth){
    // define find PV node variable
    //bool found_pv = false;
    
    // init PV length
    pv_length[ply] = ply;
    
    if (depth == 0) return quiescence(alpha, beta);

    // too deep, overflow of array relying max ply constant
    if (ply > max_ply - 1) return evaluate();
    nodes++;

    int in_check = is_square_attacked((side^1) ? get_LSB(bitboards[K]) : get_LSB(bitboards[k]), side^1);
    int legal_moves = 0;

    // increase search depth if the king has been exposed to a check
    if (in_check) depth++;

    moves move_list[1];
    generate_moves(move_list);
    // if we are following PV line
    if (follow_pv) enable_pv_score(move_list); // enable PV move scoring
    sort_moves(move_list); // from 67 million to 200 thousand nodes in tricky_position, crazy (go quiescence)

    // number of moves searched in a move list
    int moves_searched = 0;

    for (int count = 0; count < move_list->count; count++){
        copy_board();

        // ilegal move
        if (make_move(move_list->moves[count], all_moves) == 0){
            continue;
        }
        // legal move
        ply++;
        legal_moves++;
        int score;
        // PVS (https://web.archive.org/web/20071030220825/http://www.brucemo.com/compchess/programming/pvs.htm)
        // LMR (https://web.archive.org/web/20150212051846/http://www.glaurungchess.com/lmr.html)
        if (moves_searched == 0) score = -negamax(-beta, -alpha, depth-1); // full search   
        else{ // reduced search (LMR)
            
            if( // condition to consider LMR
                moves_searched >= full_depth_moves &&
                depth >= reduction_limit &&
                in_check == 0 && 
                get_move_capture(move_list->moves[count]) == 0 &&
                get_move_promoted(move_list->moves[count]) == 0
              )
                score = -negamax(-alpha - 1, -alpha, depth - 2); // search current move with reduced depth:

            else score = alpha + 1; // hack to ensure that full-depth search is done
            
            // PVS
            score = -negamax(-alpha-1, -alpha, depth-1);
            if ((score > alpha) && (score < beta)){
                score = -negamax(-beta, -alpha, depth-1);
            }
        }
        
        take_back(); // ?
        ply--;
        moves_searched++;
        
        // fail-high beta cutoff
        if (score >= beta){
            if (get_move_capture(move_list->moves[count]) == 0){
                killer_moves[1][ply] = killer_moves[0][ply];
                killer_moves[0][ply] = move_list->moves[count];
            }

            return beta; // node (move) fails high
        }
        // found better move
        if (score > alpha){
            if (get_move_capture(move_list->moves[count]) == 0){
                history_moves[get_move_piece(move_list->moves[count])][get_move_target(move_list->moves[count])] += depth;
            }
            alpha = score; // Principal Variation (PV) node (move)
            //found_pv = true;

            pv_table[ply][ply] = move_list->moves[count];
            // copy move from deeper ply into current ply's line
            for (int next_ply = ply+1; next_ply < pv_length[ply+1]; next_ply++){
                pv_table[ply][next_ply] = pv_table[ply+1][next_ply];
            }
            // adjust PV length
            pv_length[ply] = pv_length[ply+1];
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
    int score;
    
    // reset nodes counter
    nodes = 0;
    // reset follow PV flags
    follow_pv = 0;
    score_pv = 0;
    
    // clear helper data structures for search
    std::memset(killer_moves, 0, sizeof(killer_moves));
    std::memset(history_moves, 0, sizeof(history_moves));
    std::memset(pv_table, 0, sizeof(pv_table));
    std::memset(pv_length, 0, sizeof(pv_length));

    // iterative deepening
    for (int current_depth = 1; current_depth <= depth; current_depth++){
        // enable follow PV flag
        follow_pv = 1;
        
        score = negamax(-50000, 50000, current_depth);
        std::cout << "info score cp " << score << " depth " << current_depth << " nodes " << nodes << " pv ";
        for (int count = 0; count < pv_length[0]; count++){
            // print PV move
            print_move(pv_table[0][count]);
            std::cout << ' ';
        }
        std::cout << '\n';
    }
    
    std::cout << "bestmove ";
    print_move(pv_table[0][0]);
    std::cout << "\n";
}

#endif