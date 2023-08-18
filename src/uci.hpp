#ifndef UCI_H
#define UCI_H

#include <iostream>

#include "types.hpp"
#include "moves.hpp"
#include "io.hpp"
#include "search.hpp"
#include "benchmark.hpp"


inline int parse_move(Board &board, const char* move_string){
    moves move_list;
    generate_moves(board, move_list);

    int source_square = (move_string[0] - 'a') + 8*(8 - (move_string[1] - '0'));
    int target_square = (move_string[2] - 'a') + 8*(8 - (move_string[3] - '0'));


    for (int move_count = 0; move_count < move_list.count; move_count++){
        int move = move_list.moves[move_count];

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

inline void parse_position(Board &board, const char *command){
    command += 9; // skip position word
    const char *current_char = command;

    if (strncmp(command, "startpos", 8) == 0){
        parse_fen(board, start_position);
    }else{
        current_char = strstr(command, "fen");
        //current_char += 4; // skip fen
        if (current_char == NULL){
            parse_fen(board, start_position);
        }else{
            current_char += 4;
            parse_fen(board, current_char);
        }

    }
    current_char = strstr(command, "moves");
    if (current_char != NULL){
        current_char += 6;
        while (*current_char){
            int move = parse_move(board, current_char);
            if (move == 0) break;
            make_move(board, move, all_moves);
            if (get_move_promoted(move)){
                current_char += 6;
            }else{
                current_char += 5;
            }
        }
    }
    //print_board(board);
}

inline void parse_go(Board &board, Time &time, const char *command){
    int depth = -1;

    char *argument = NULL;

    if ((argument = strstr(command, "infinite"))) {} // infinite search
    if ((argument = strstr(command, "binc")) && board.side == black) time.inc = atoi(argument + 5); // parse black time increment
    if ((argument = strstr(command, "winc")) && board.side == white) time.inc = atoi(argument + 5); // parse white time increment
    if ((argument = strstr(command, "wtime")) && board.side == white) time.time = atoi(argument + 6); // parse white time limit
    if ((argument = strstr(command, "btime")) && board.side == black) time.time = atoi(argument + 6); // parse black time limit
    if ((argument = strstr(command, "movestogo"))) time.movestogo = atoi(argument + 10); // parse number of moves to go
    if ((argument = strstr(command, "movetime"))) time.movetime = atoi(argument + 9); // parse amount of time allowed to spend to make a move
    if ((argument = strstr(command, "depth"))) depth = atoi(argument + 6); // parse search depth
    
    if(time.movetime != -1){ // if move time is not available
        time.time = time.movetime; // set time equal to move time
        time.movestogo = 1; // set moves to go to 1
    }
    
    time.starttime = get_time_ms(); // init start time
    depth = depth; //?

    
    if(time.time != -1){ // if time control is available
        time.timeset = 1; // flag we're playing with time control

        // set up timing
        time.time /= time.movestogo;
        time.time -= 50;
        time.stoptime = time.starttime + time.time + time.inc;
    }       
    // if depth is not available
    if(depth == -1) depth = 64; // set depth to 64 plies (takes ages to complete...)
    
    std::cout << "time: " << time.time << " start: " << time.starttime << " stop: " << time.stoptime
              << " depth: " << depth << " timeset: " << time.timeset << '\n';

    search_position(board, time, depth);
}

inline void uci_loop(){
    Board board;
    std::memset(&board, 0, sizeof(board)); // Ensure 0s
    Time time;
    
    // reset stdin & stdout buffers
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    // Make sure that the outputs are sent straight away to the GUI
	std::cout.setf (std::ios::unitbuf);

    char input[2000];
    
    std::cout << "Myfish by Diego Bouchet\n";

    while(true){
        std::memset(input, 0, sizeof(input));
        // make sure ouput reaches the GUI
        //fflush(stdout);
        
        // get GUI input
        if (!fgets(input, 2000, stdin)) continue;
        // make sure input is available
        if (input[0] == '\n') continue;

        if (strncmp(input, "isready", 7) == 0){
            std::cout << "readyok\n";
            continue;
        }
        else if (strncmp(input, "position", 8) == 0){
            parse_position(board, input);
            continue;
        }
        else if (strncmp(input, "ucinewgame", 10) == 0){
            parse_position(board, "position startpos");
            continue;
        }
        else if (strncmp(input, "go", 2) == 0){
            parse_go(board, time, input);
            continue;
        }
        else if (strncmp(input, "quit", 4) == 0){
            break;
        }
        else if (strncmp(input, "uci", 3) == 0){
            std::cout << "id name myfish\nid author Diego Bouchet\nuciok\n";
        }
        else if (strncmp(input, "d", 1) == 0){
            print_board(board);
        }
    }
}

#endif