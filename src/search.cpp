#include "search.h"

#include <iostream>

#include "io.h"


// half move counter
int ply;

int best_move;

int rmove(){
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

void search_position(int depth){
    int score = negamax(-50000, 50000, depth);
    std::cout << "bestmove ";
    print_move(best_move);
    std::cout << "\n";
}