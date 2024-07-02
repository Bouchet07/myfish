#ifndef SEARCH_H
#define SEARCH_H

#include "moves.hpp"
#include "eval.hpp"
#include "misc.hpp"
#include "tt.hpp"

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
struct Tree {
    // killer moves [id][ply]
    int killer_moves[2][max_ply];
    // history moves [piece][square]
    int history_moves[12][64];
    // PV length [ply]
    int pv_length[max_ply];
    // PV table [ply][ply]
    int pv_table[max_ply][max_ply];
    // follow PV & score PV move
    int follow_pv, score_pv;
    // half move counter
    int ply;
    // leaf tree.nodes (number of positions reached during the test of the move generator at a given depth)
    U64 nodes;
};



// enable PV move scoring
inline void enable_pv_score(Tree &tree, moves &move_list){
    // disable following PV
    tree.follow_pv = 0;

    for (int count = 0; count < move_list.count; count++){
        // make sure PV move is hit
        if (tree.pv_table[0][tree.ply] == move_list.moves[count]){
            tree.score_pv = 1;
            tree.follow_pv = 1;
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
inline int score_move(Board &board, Tree &tree, int move){
    // if PV move scoring is allowed
    if (tree.score_pv){
        if (tree.pv_table[0][tree.ply] == move){
           tree.score_pv = 0;
           // give PV move the highest score to search it first
           return 20000; 
        } 
    }
    
    // score capture moves
    if (get_move_capture(move)){
        int target_piece = P; // doesn't matter color of pawn for score perspective of mvv_lva

        int start_piece, end_piece;
        if (board.side^1) {start_piece = p; end_piece = k;}
        else {start_piece = P; end_piece = K;}

        for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++){
            // op_bit(bitboards[bb_piece], target_square); // we just eliminate everything, faster? no
            if (get_bit(board.bitboards[bb_piece], get_move_target(move))){
                target_piece = bb_piece;
                break;
            }
        }
        return mvv_lva[get_move_piece(move)][target_piece] + 10000;
    }else{
        // score 1st killer move
        if (tree.killer_moves[0][tree.ply] == move) return 9000;

        // score 2nd killer move
        else if (tree.killer_moves[1][tree.ply] == move) return 8000;

        // score hisroty move
        else return tree.history_moves[get_move_piece(move)][get_move_target(move)];
    }
    // score quiet moves
    return 0;
}

inline void sort_moves(Board &board, Tree &tree, moves &move_list){
    std::sort(move_list.moves, move_list.moves + move_list.count,
              [&board, &tree](int move_1, int move_2) {return score_move(board, tree, move_1) > score_move(board, tree, move_2);});
}

inline int quiescence(Board &board, Tree &tree, Time &time, int alpha, int beta){
    if ((tree.nodes & 2047) == 0) communicate(time); // every 2047 nodes we listen to GUI input
    
    tree.nodes++;
    
    int evaluation = evaluate(board);
    // fail-high beta cutoff
    if (evaluation >= beta){
        return beta; // node (move) fails high
    }
    // found better move
    if (evaluation > alpha){
        alpha = evaluation; // Principal Variation (PV) node (move)
    }
    
    moves move_list;
    generate_moves(board, move_list);
    sort_moves(board, tree, move_list); // from 200 thousand to 1613 nodes, even more crazy

    for (int count = 0; count < move_list.count; count++){
        Board copy_board = board;

        // not a capture
        if (make_move(board, move_list.moves[count], only_captures) == 0){
            continue;
        }
        // capture
        tree.ply++;
        int score = -quiescence(board, tree, time, -beta, -alpha);
        
        board = copy_board;
        tree.ply--;
        
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
inline int negamax(Board &board, Tree &tree, Time &time, int alpha, int beta, int depth){
    int score;
    int hashf = hashfALPHA; // by default alpha
    if ((score = read_hash_entry(board, alpha, beta, depth)) != no_hash_entry){return score;}
    if ((tree.nodes & 2047) == 0) communicate(time); // every 2047 nodes we listen to GUI input
    
    // init PV length
    tree.pv_length[tree.ply] = tree.ply;
    
    if (depth == 0) return quiescence(board, tree, time, alpha, beta);

    // too deep, overflow of array relying max ply constant
    if (tree.ply > max_ply - 1) return evaluate(board);
    tree.nodes++;

    int in_check = is_square_attacked(board, (board.side^1) ? get_LSB(board.bitboards[K]) : get_LSB(board.bitboards[k]), board.side^1);
    int legal_moves = 0;

    // increase search depth if the king has been exposed to a check
    if (in_check) depth++;

    // null move pruning (when no pieces besides pawns and kings)
    if (depth >= 3 && in_check == 0 && tree.ply){
        if (board.bitboards[N] | board.bitboards[B] | board.bitboards[Q] |
            board.bitboards[n] | board.bitboards[b] | board.bitboards[q]){
            Board copy_board = board;
            // switch the side, literally giving opponent an extra move to make
            board.side ^= 1;
            board.hash_key ^= side_key;
            // reset enpassant capture square
            if (board.enpassant != no_sq) board.hash_key ^= enpassant_keys[board.enpassant];
            board.enpassant = no_sq;
            // search moves with reduced depth to find beta cutoffs
            // depth - 1 - R where R is a reduction limit 
            score = -negamax(board, tree, time, -beta, -beta + 1, depth - 1 - 2);
            board = copy_board;

            // fail-hard beta cutoff
            if (score >= beta)
                // node (move) fails high
                return beta;

            if(time.stopped == 1) return 0; // reutrn 0 if time is up (is here the ideal position?)
        }
    }

    moves move_list;
    generate_moves(board, move_list);
    // if we are following PV line
    if (tree.follow_pv) enable_pv_score(tree, move_list); // enable PV move scoring
    sort_moves(board, tree, move_list); // from 67 million to 200 thousand nodes in tricky_position, crazy (go quiescence)

    // number of moves searched in a move list
    int moves_searched = 0;

    for (int count = 0; count < move_list.count; count++){
        Board copy_board = board;

        // ilegal move
        if (make_move(board, move_list.moves[count], all_moves) == 0){
            continue;
        }
        // legal move
        tree.ply++;
        legal_moves++;
        // PVS (https://web.archive.org/web/20071030220825/http://www.brucemo.com/compchess/programming/pvs.htm)
        // LMR (https://web.archive.org/web/20150212051846/http://www.glaurungchess.com/lmr.html)
        if (moves_searched == 0) score = -negamax(board, tree, time, -beta, -alpha, depth-1); // full search   
        else{ // reduced search (LMR)
            
            if( // condition to consider LMR
                moves_searched >= full_depth_moves &&
                depth >= reduction_limit &&
                in_check == 0 && 
                get_move_capture(move_list.moves[count]) == 0 &&
                get_move_promoted(move_list.moves[count]) == 0
              )
                score = -negamax(board, tree, time, -alpha - 1, -alpha, depth - 2); // search current move with reduced depth:

            else score = alpha + 1; // hack to ensure that full-depth search is done
            
            /* // PVS
            score = -negamax(board, tree, time, -alpha-1, -alpha, depth-1);
            if ((score > alpha) && (score < beta)){
                score = -negamax(board, tree, time, -beta, -alpha, depth-1);
            } */
            if (score > alpha){
                score = -negamax(board, tree, time, -alpha-1, -alpha, depth-1);
                if ((score > alpha) && (score < beta)){
                    score = -negamax(board, tree, time, -beta, -alpha, depth-1);
                }
            }
        }
        
        board = copy_board;
        tree.ply--;
        if(time.stopped == 1) return 0; // reutrn 0 if time is up
        moves_searched++;
        
        // fail-high beta cutoff
        if (score >= beta){
            if (get_move_capture(move_list.moves[count]) == 0){
                tree.killer_moves[1][tree.ply] = tree.killer_moves[0][tree.ply];
                tree.killer_moves[0][tree.ply] = move_list.moves[count];
            }
            write_hash_entry(board, beta, depth, hashfBETA);
            return beta; // node (move) fails high
        }
        // found better move
        if (score > alpha){
            if (get_move_capture(move_list.moves[count]) == 0){
                tree.history_moves[get_move_piece(move_list.moves[count])][get_move_target(move_list.moves[count])] += depth;
            }
            alpha = score; // Principal Variation (PV) node (move)
            //found_pv = true;

            tree.pv_table[tree.ply][tree.ply] = move_list.moves[count];
            // copy move from deeper ply into current ply's line
            for (int next_ply = tree.ply+1; next_ply < tree.pv_length[tree.ply+1]; next_ply++){
                tree.pv_table[tree.ply][next_ply] = tree.pv_table[tree.ply+1][next_ply];
            }
            // adjust PV length
            tree.pv_length[tree.ply] = tree.pv_length[tree.ply+1];
            hashf = hashfEXACT; // switch hash flag from storing score for fail-low node to storing score for PV node
        }
    }
    if (legal_moves == 0){
        if (in_check) return -49000 + tree.ply; // +ply is important to prioritize shortest checkmate
        return 0; // stalemate
    }
    write_hash_entry(board, alpha, depth, hashf);
    // node (move) fails low
    return alpha;
}

inline int rmove(Board &board){
    //srand(time(NULL));
    moves move_list;
    generate_moves(board, move_list);
    int random = std::rand() % move_list.count + 1;
    while (make_move(board, move_list.moves[random], all_moves) == 0){
        random = rand() % move_list.count + 1;
    }
    make_move(board, move_list.moves[random], all_moves);
    return move_list.moves[random];

}

inline void search_position(Board &board, Time &time, int depth){
    int score = 0;
    
    Tree tree; // New tree
    std::memset(&tree, 0, sizeof(tree)); // ensure 0;

    time.stopped = 0; // reset "time is up" flag

    // define initial alpha beta bounds
    int alpha = -50000;
    int beta = 50000;

    // iterative deepening
    for (int current_depth = 1; current_depth <= depth; current_depth++){
        if(time.stopped == 1) break; // // stop calculating and return best move so far if time is up
        
        // enable follow PV flag
        tree.follow_pv = 1;
        
        score = negamax(board, tree, time, alpha, beta, current_depth);
        // Aspiration window (https://web.archive.org/web/20071031095918/http://www.brucemo.com/compchess/programming/aspiration.htm)
        // we fell outside the window, so try again with a full-width window (and the same depth)
        if ((score <= alpha) || (score >= beta)) {
            alpha = -50000;    
            beta = 50000;      
            continue;
        }
        // set up the window for the next iteration
        alpha = score - 50;
        beta = score + 50;

        std::cout << "info score cp " << score << " depth " << current_depth << " nodes " << tree.nodes << " pv ";
        for (int count = 0; count < tree.pv_length[0]; count++){
            // print PV move
            print_move(tree.pv_table[0][count]);
            std::cout << ' ';
        }
        std::cout << '\n';
    }
    
    std::cout << "bestmove ";
    print_move(tree.pv_table[0][0]);
    std::cout << "\n";
}

#endif