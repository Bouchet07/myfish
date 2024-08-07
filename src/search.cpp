#include "3rd-party/BS_thread_pool.hpp"
#include "search.h"
#include <iostream>
#include <chrono>
#include <thread>

uint64_t perft(Board &board, uint8_t depth){
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

void perft_test(Board &board, uint8_t depth, bool Use_UTF8){
    print_board(board, Use_UTF8);
    std::cout << "\nStarting Perft Test to depth: " << (int)depth << '\n';
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

void bench_perft(Board &board, uint8_t depth){
    auto start = std::chrono::high_resolution_clock::now();
    uint64_t nodes = perft(board, depth);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Nodes: " << nodes << '\n';
    std::cout << "Time taken: " << elapsed.count() << " milliseconds\n";
    std::cout << "MNodes/s: " << (nodes / (elapsed.count() / 1000))/1000000 << '\n';
}

void bench_go(Board &board, TimeControl &time, RT &rt, uint8_t depth){
    auto start = std::chrono::high_resolution_clock::now();
    search_position(board, time, rt, depth);
    pool.wait();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Time taken: " << elapsed.count() << " milliseconds\n";
}

int64_t get_time_ms(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

Value quiescence(Board &board, Tree &tree, TimeControl &time, RT &rt, Value alpha, Value beta){
    if ((tree.visited_nodes & 4096)==0){
        if (time.timeset && get_time_ms() > time.stop_time){
            time.stop = true;
        }
    }
    tree.visited_nodes++;
    //if (rt.is_repetition()) return VALUE_DRAW;
    Value evaluation = evaluate(board);
    if(evaluation >= beta){
        return beta;
    }
    if(evaluation > alpha){
        alpha = evaluation;
    }
    if (tree.ply >= MAX_PLY){   // too deep, overflow
        return evaluation;
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
        rt[++rt.index] = board.hash_key;
        score = -quiescence(board, tree, time, rt, -beta, -alpha);
        board = board_copy;
        tree.ply--;
        rt.index--;

        if(score >= beta){
            return beta;
        }
        if(score > alpha){
            alpha = score;
        }
        if (time.stop){
            return VALUE_ZERO;
        }
    }
    return alpha;
}

Value negamax(Board &board, Tree &tree, TimeControl &time, RT &rt, Value alpha, Value beta, uint8_t depth){
    if ((tree.visited_nodes & 4096)==0){
        if (time.timeset && get_time_ms() > time.stop_time){
            time.stop = true;
        }
        if (time.nodes && tree.visited_nodes > time.nodes){
            time.stop = true;
        }
    }
    if ((rt.is_repetition()||board.fifty >= 50) && tree.ply) {return VALUE_DRAW;}
    Value score;
    TT_flag hashf = hashfALPHA; // by default alpha

    bool pv_node = beta-alpha > 1; // If the window is closed, we are not in a PV node

    if ((tree.ply) && !pv_node && ((score = read_hash_entry(board, alpha, beta, depth, tree.ply)) != no_hash_entry)){return score;}
    
    if (tree.ply >= MAX_PLY){   // too deep, overflow
        return evaluate(board);
    }
    if(depth == 0){
        return quiescence(board, tree, time, rt, alpha, beta);
    } 
    tree.visited_nodes++;
    bool in_check = is_square_attacked(board, get_LSB(board.bitboards[make_index_piece(board.side, KING)]), ~board.side);
    if (in_check) depth++;

    uint16_t legal_moves = 0;

    // Null move pruning
    Board board_copy;
    if (depth >= 4 && !in_check && tree.ply){
        board_copy = board;
        tree.ply++;
        make_move(board, NULL_MOVE, ALL_MOVES);
        rt[++rt.index] = board.hash_key;
        score = -negamax(board, tree, time, rt, -beta, -beta + 1, depth - 4);
        board = board_copy;
        tree.ply--;
        rt.index--;
        if (score >= beta){
            return beta;
        }
    }

    MoveList moves = generate_moves(board);
    sort_moves(moves, tree, board);

    for(uint8_t i = 0; i < moves.size(); i++){
        board_copy = board;
        if(make_move(board, moves[i].move, ALL_MOVES)==false){
            continue;
        }
        tree.ply++;
        rt[++rt.index] = board.hash_key;
        if (legal_moves!=0){ // PVS and LMR
            // condition to consider LMR
            if(
                legal_moves >= FULL_DEPTH_MOVES &&
                depth >= REDUCTION_LIMIT &&
                in_check == 0 && 
                decode_move_capture(moves[i].move) == false &&
                decode_move_promoted(moves[i].move) == NO_PIECE_TYPE
              ){
                score = -negamax(board, tree, time, rt, -alpha - 1, -alpha, depth - 2);
              }else{
                score = alpha + 1; // hack to ensure that full-depth search is done
              }
            if (score > alpha){ // PVS
                score = -negamax(board, tree, time, rt, -alpha - 1, -alpha, depth - 1);
                if ((score > alpha) && (score < beta)){ // we messed up, research
                    score = -negamax(board, tree, time, rt, -beta, -alpha, depth - 1);
                }
            }
            
        } else {
            score = -negamax(board, tree, time, rt, -beta, -alpha, depth - 1);
        }
        board = board_copy;
        tree.ply--;
        rt.index--;
        if (time.stop){
            return VALUE_ZERO;
        }
        // fail hard beta-cutoff
        if(score >= beta){
            if (!decode_move_capture(moves[i].move)){  // quiet move
                tree.killer_moves[1][tree.ply] = tree.killer_moves[0][tree.ply];
                tree.killer_moves[0][tree.ply] = moves[i].move;
            }
            write_hash_entry(board, beta, depth, hashfBETA, tree.ply);
            return beta; // fails high
        }
        
        if(score > alpha){
            if (!decode_move_capture(moves[i].move)){ // quiet move
                tree.history_moves[make_index_piece(decode_move_piece(moves[i].move))][decode_move_target(moves[i].move)] += depth;
            }
            alpha = score;
            
            tree.pv.store(tree.ply, moves[i].move, board.hash_key);
            tree.pv.copy_up(tree.ply);
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
    write_hash_entry(board, alpha, depth, hashf, tree.ply);
    return alpha; // fails low
}

void iterative_deepening(Board board, TimeControl &time, RT rt, uint8_t depth, uint16_t num_thread){
    Tree tree;
    auto pv_backup = tree.pv;
    Value alpha = -VALUE_NONE;
    Value beta = VALUE_NONE;
    // iterative deepening
    Value score;
    for(int d = 1; d <= depth; d++){
        tree.visited_nodes = 0; // should be reseted it?

        score = negamax(board, tree, time, rt, alpha, beta, d);
        if (d >= 4){
            // Aspiration window
            if ((score <= alpha) || (score >= beta)) { // messed up, reset window and research
                alpha = -VALUE_NONE;    
                beta = VALUE_NONE;
                // restore previous pv
                tree.pv = pv_backup;
                if (time.stop){ // if there is no more time
                    break;
                }else{
                    tree.visited_nodes = 0; // should be reseted it?
                    score = negamax(board, tree, time, rt, alpha, beta, d);
                }
            }else{
                alpha = score - 50;
                beta = score + 50;
                // backup pv
                pv_backup = tree.pv;
            }
        }
        if (time.stop){ // only print complete search
            break;
        }
        if (num_thread != 1) continue; // only first thread prints
        mtx.lock();
        if      (score >=  VALUE_MATE_IN_MAX_PLY) std::cout << "info score mate " <<  (VALUE_MATE - score)/2 + 1;
        else if (score <= -VALUE_MATE_IN_MAX_PLY) std::cout << "info score mate " << -(VALUE_MATE + score)/2;
        else                                      std::cout << "info score cp "   << score;

        std::cout << " depth " << d << " nodes "
                << tree.visited_nodes
                << " time " << get_time_ms() - time.start_time
                << " hashfull " << tt.hashfull() << " pv";
        uint8_t i = 0;
        while(tree.pv.offset(0,i).first != NULL_MOVE){
            std::cout << ' ';
            print_move(tree.pv.offset(0,i++).first);
        }
        std::cout << '\n';
        mtx.unlock();
    }
    if (num_thread != 1) return; // only first thread prints
    mtx.lock();
    std::cout << "bestmove ";
    print_move(tree.pv[0].first);
    std::cout << '\n';
    mtx.unlock();
}


void search_position(Board &board, TimeControl &time, RT &rt, uint8_t depth){
    tt.age++;
    auto it_dep_index = std::bind(iterative_deepening, board, std::ref(time), rt, depth, std::placeholders::_1);
    pool.detach_sequence<uint16_t>(1, pool.get_thread_count()+1, it_dep_index);
}

/* void search_position(Board &board, TimeControl &time, RT &rt, uint8_t depth){
    // create num_threads threads
    //tt.age++;
    std::vector<std::thread> threads(num_threads);
    for (uint8_t th_num = 0; th_num < num_threads; th_num++){
        threads[th_num] = std::thread(iterative_deepening, board, std::ref(time), rt, depth, th_num+1);
        //std::cout << "Thread " << (int)th_num+1 << " started\n";
    }

    for (auto& thread : threads) {
        thread.join();
        //std::cout << "Thread joined\n";
    }
} */