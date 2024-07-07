#include "search.h"
#include <iostream>
#include <chrono>

uint64_t perft(Board &board, int depth){
    if(depth == 0){
        return 1;
    }
    MoveList moves = generate_moves(board);
    uint64_t nodes = 0;
    Board board_copy;
    for(uint8_t i = 0; i < moves.size(); i++){
        board_copy = board;
        if(make_move(board, moves[i].move, ALL_MOVES)==false){
            continue;
        }
        nodes += perft(board, depth - 1);
        board = board_copy;
    }
    return nodes;
}

void perft_test(Board &board, int depth, bool Use_UTF8){
    print_board(board, Use_UTF8);
    std::cout << "\nStarting Perft Test to depth: " << depth << '\n';
    MoveList moves = generate_moves(board);
    uint64_t nodes = 0, moves_count = 0;
    Board board_copy;
    for(uint8_t i = 0; i < moves.size(); i++){
        board_copy = board;
        if(make_move(board, moves[i].move, ALL_MOVES)==false){
            continue;
        }
        moves_count++;
        uint64_t n = perft(board, depth - 1);
        nodes += n;
        std::cout << "Move: ";
        print_move(moves[i].move);
        std::cout << " Nodes: " << n << '\n';
        board = board_copy;
    }
    std::cout << "Total Nodes: " << nodes << '\n' << "Total Moves: " << moves_count << '\n';
}

void bench_perft(Board &board, int depth){
    auto start = std::chrono::high_resolution_clock::now();
    uint64_t nodes = perft(board, depth);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Nodes: " << nodes << '\n';
    std::cout << "Time taken: " << elapsed.count() << " milliseconds\n";
    std::cout << "MNodes/s: " << (nodes / (elapsed.count() / 1000))/1000000 << '\n';
}

void bench_go(Board &board, TimeControl &time, int depth){
    auto start = std::chrono::high_resolution_clock::now();
    search_position(board, time, depth, tt_size(10));
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Time taken: " << elapsed.count() << " milliseconds\n";
}

int64_t get_time_ms(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void enable_pv_score(MoveList &moves, Tree &tree){
    tree.follow_pv = false;
    for (uint8_t i = 0; i < moves.size(); i++){
        if (moves[i].move == tree.pv[0][tree.ply]){
            tree.score_pv = true;
            tree.follow_pv = true;
            return;
        }
    }
}

Value quiescence(Board &board, Tree &tree, TimeControl &time, Value alpha, Value beta){
    if ((tree.visited_nodes & 4096)==0){
        if (time.timeset && get_time_ms() > time.stop_time){
            time.stop = true;
        }
    }
    tree.visited_nodes++;
    Value evaluation = evaluate(board);
    if(evaluation >= beta){
        return beta;
    }
    if(evaluation > alpha){
        alpha = evaluation;
    }
    MoveList moves = generate_moves(board);
    sort_moves(moves, tree, board);
    Board board_copy;
    Value score;
    for (uint8_t i = 0; i < moves.size(); i++){
        board_copy = board;
        if(make_move(board, moves[i].move, CAPTURE_MOVES)==false){
            continue;
        }
        tree.ply++;
        score = -quiescence(board, tree, time, -beta, -alpha);
        board = board_copy;
        tree.ply--;

        if (time.stop){
            return VALUE_ZERO;
        }

        if(score >= beta){
            return beta;
        }
        if(score > alpha){
            alpha = score;
        }
    }
    return alpha;
}

Value negamax(Board &board, Tree &tree, TimeControl &time, TT &tt, Value alpha, Value beta, int depth){
    if ((tree.visited_nodes & 4096)==0){
        if (time.timeset && get_time_ms() > time.stop_time){
            time.stop = true;
        }
    }
    Value score;
    int hashf = hashfALPHA; // by default alpha
    if ((score = read_hash_entry(board, tt, alpha, beta, depth)) != no_hash_entry){return score;}
    
    tree.pv_length[tree.ply] = tree.ply;

    if (tree.ply >= MAX_PLY){   // too deep, overflow
        return evaluate(board);
    }
    if(depth == 0){
        return quiescence(board, tree, time, alpha, beta);
    } 
    tree.visited_nodes++;
    bool in_check = is_square_attacked(board, get_LSB(board.bitboards[make_index_piece(board.side, KING)]), ~board.side);
    if (in_check) depth++;

    uint16_t legal_moves = 0;

    // Null move pruning
    Board board_copy;
    if (depth >= 3 && !in_check && tree.ply){
        board_copy = board;
        board.side = ~board.side; // pass the turn
        board.hash_key ^= zobrist_keys[side_index];
        if (board.enpassant != SQ_NONE) board.hash_key ^= zobrist_keys[ep_index];
        board.enpassant = SQ_NONE;
        score = -negamax(board, tree, time, tt, -beta, -beta + 1, depth - 3);
        board = board_copy;
        if (score >= beta){
            return beta;
        }
    }

    MoveList moves = generate_moves(board);
    if (tree.follow_pv){
        enable_pv_score(moves, tree);
    }
    sort_moves(moves, tree, board);

    for(uint8_t i = 0; i < moves.size(); i++){
        board_copy = board;
        if(make_move(board, moves[i].move, ALL_MOVES)==false){
            continue;
        }
        tree.ply++;
        
        if (legal_moves!=0){ // PVS and LMR
            // condition to consider LMR
            if(
                legal_moves >= FULL_DEPTH_MOVES &&
                depth >= REDUCTION_LIMIT &&
                in_check == 0 && 
                decode_move_capture(moves[i].move) == false &&
                decode_move_promoted(moves[i].move) == NO_PIECE_TYPE
              ){
                score = -negamax(board, tree, time, tt, -alpha - 1, -alpha, depth - 2);
              }else{
                score = alpha + 1; // hack to ensure that full-depth search is done
              }
            if (score > alpha){ // PVS
                score = -negamax(board, tree, time, tt, -alpha - 1, -alpha, depth - 1);
                if (score > alpha && score < beta){ // we messed up, research
                    score = -negamax(board, tree, time, tt, -beta, -alpha, depth - 1);
                }
            }
            
        } else {
            score = -negamax(board, tree, time, tt, -beta, -alpha, depth - 1);
        }
        board = board_copy;
        tree.ply--;

        if (time.stop){
            return VALUE_ZERO;
        }

        // fail hard beta-cutoff
        if(score >= beta){
            if (!decode_move_capture(moves[i].move)){  // quiet move
                tree.killer_moves[1][tree.ply] = tree.killer_moves[0][tree.ply];
                tree.killer_moves[0][tree.ply] = moves[i].move;
            }
            write_hash_entry(board, tt, beta, depth, hashfBETA);
            return beta; // fails high
        }
        if(score > alpha){
            if (!decode_move_capture(moves[i].move)){ // quiet move
                tree.history_moves[make_index_piece(decode_move_piece(moves[i].move))][decode_move_target(moves[i].move)] += depth;
            }
            alpha = score;

            tree.pv[tree.ply][tree.ply] = moves[i].move;
            for (int next = tree.ply + 1; next < tree.pv_length[tree.ply + 1]; next++){
                tree.pv[tree.ply][next] = tree.pv[tree.ply + 1][next];
            }
            tree.pv_length[tree.ply] = tree.pv_length[tree.ply + 1];
            hashf = hashfEXACT;
        }
        legal_moves++;
    }
    if (legal_moves==0){
        if(in_check){
            return mated_in(tree.ply);
        } else {
            return VALUE_DRAW; // stalemate
        }
    }
    write_hash_entry(board, tt, alpha, depth, hashf);
    return alpha; // fails low
}

void search_position(Board &board, TimeControl &time, int depth, size_t tt_size){
    Tree tree;
    TT tt = create_tt(tt_size);

    // iterative deepening
    for(int d = 1; d <= depth; d++){
        tree.visited_nodes = 0; // should be reseted it?
        tree.follow_pv = true;
        Value score = negamax(board, tree, time, tt, -VALUE_NONE, VALUE_NONE, d);
        std::cout << "info score cp " << score << " depth " << d << " nodes "
                  << tree.visited_nodes << " pv";
        for (int i = 0; i < tree.pv_length[0]; i++){
            std::cout << ' ';
            print_move(tree.pv[0][i]);
        }
        std::cout << '\n';
        if (time.stop){
            break;
        }
    }
    std::cout << "bestmove ";
    print_move(tree.pv[0][0]);
    std::cout << '\n';
    //std::cout << " Score: " << best_score << '\n';
}