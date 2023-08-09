#include "uci.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "moves.h"
#include "io.h"
#include "search.h"


int parse_move(const char *move_string){
    moves move_list[1];
    generate_moves(move_list);

    int source_square = (move_string[0] - 'a') + 8*(8 - (move_string[1] - '0'));
    int target_square = (move_string[2] - 'a') + 8*(8 - (move_string[3] - '0'));


    for (int move_count = 0; move_count < move_list->count; move_count++){
        int move = move_list->moves[move_count];

        if (source_square == get_move_source(move) && target_square == get_move_target(move)){
            int promoted_piece = get_move_promoted(move);
            if (promoted_piece){
                if ((promoted_piece == Q || promoted_piece == q) && move_string[4] == 'q') return move;
                if ((promoted_piece == R || promoted_piece == r) && move_string[4] == 'r') return move;
                if ((promoted_piece == B || promoted_piece == b) && move_string[4] == 'b') return move;
                if ((promoted_piece == N || promoted_piece == n) && move_string[4] == 'n') return move;
                continue;
            }
            return move;
        }
    }
    return 0;

}

void parse_position(const char *command){
    command += 9; // skip position word
    const char *current_char = command;

    if (strncmp(command, "startpos", 8) == 0){
        parse_fen(start_position);
    }else{
        current_char = strstr(command, "fen");
        //current_char += 4; // skip fen
        if (current_char == NULL){
            parse_fen(start_position);
        }else{
            current_char += 4;
            parse_fen(current_char);
        }

    }
    current_char = strstr(command, "moves");
    if (current_char != NULL){
        current_char += 6;
        while (*current_char){
            int move = parse_move(current_char);
            if (move == 0) break;
            make_move(move, all_moves);
            if (get_move_promoted(move)){
                current_char += 6;
            }else{
                current_char += 5;
            }
        }
    }
    print_board();
}

void parse_go(const char *command){
    int depth = -1;

    char *current_depth = NULL;

    if (current_depth = strstr(command, "depth")){
        depth = std::atoi(current_depth + 6);
    }else{
        depth = 6;
    }
    search_position(depth);
}

using namespace std;

void uci_loop(){
    // reset stdin & stdout buffers
    std::setbuf(stdin, NULL);
    std::setbuf(stdout, NULL);

    char input[2000];
    
    cout << "id name myfish\nuciok\n";

    while(1){
        memset(input, 0, sizeof(input));
        // make sure ouput reaches the GUI
        fflush(stdout);
        // get GUI input
        if (!fgets(input, 2000, stdin)) continue;
        // make sure input is available
        if (input[0] == '\n') continue;

        if (strncmp(input, "isready", 7) == 0){
            cout << "readyok\n";
            continue;
        }
        else if (strncmp(input, "position", 8) == 0){
            parse_position(input);
            continue;
        }
        else if (strncmp(input, "ucinewgame", 10) == 0){
            parse_position("position startpos");
            continue;
        }
        else if (strncmp(input, "go", 2) == 0){
            parse_go(input);
            continue;
        }
        else if (strncmp(input, "quit", 4) == 0){
            break;
        }
        else if (strncmp(input, "uci", 3) == 0){
            cout << "id name myfish\nuciok\n";
        }
    }
}