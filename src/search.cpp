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

void bench_perft(Board &board, int depth){
    auto start = std::chrono::high_resolution_clock::now();
    uint64_t nodes = perft(board, depth);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Nodes: " << nodes << '\n';
    std::cout << "Time taken: " << elapsed.count() << " milliseconds\n";
    std::cout << "MNodes/s: " << (nodes / (elapsed.count() / 1000))/1000000 << '\n';
}




Value negamax(Board &board, Tree &tree, Value alpha, Value beta, int depth){
    if(depth == 0){
        tree.visited_nodes++; // is this visited counting?
        return evaluate(board);
    }
    tree.visited_nodes++;
    bool in_check = is_square_attacked(board, get_LSB(board.bitboards[make_index_piece(board.side, KING)]), ~board.side);
    uint16_t legal_moves = 0;

    MoveList moves = generate_moves(board);
    Value score;
    for(uint8_t i = 0; i < moves.count; i++){
        Board board_copy = board;
        if(make_move(board, moves.moves[i], ALL_MOVES)==false){
            continue;
        }
        tree.ply++;
        legal_moves++;
        score = -negamax(board, tree, -beta, -alpha, depth - 1);
        board = board_copy;
        tree.ply--;
        // maybe remove later on
        if (tree.best_move == 0 && tree.ply == 0){
            tree.best_move = moves.moves[i];
        }
        // fail hard beta-cutoff
        if(score >= beta){
            return beta; // fails high
        }
        if(score > alpha){
            alpha = score;
            if (tree.ply == 0){
                tree.best_move = moves.moves[i]; // PV node
            }
        }
    }
    if (legal_moves==0){
        if(in_check){
            return mated_in(tree.ply);
        } else {
            return VALUE_DRAW; // stalemate
        }
    }
    return alpha; // fails low
}

void search_position(Board &board, int depth){
    Tree tree;
    Value score = negamax(board,tree, -VALUE_NONE, VALUE_NONE, depth);
    std::cout << "info score cp " << score << " depth " << depth << " nodes " << tree.visited_nodes << '\n';
    std::cout << "bestmove ";
    print_move(tree.best_move);
    std::cout << '\n';
    //std::cout << " Score: " << best_score << '\n';
}