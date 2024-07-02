#include <iostream>
#include <sstream>
#include <cstring>
#include <thread>


#include "uci.h"
#include "bitboard.h"
#include "search.h"
#include "moves.h"

void UCI::init(){
    std::cout << "id name " << ENGINE_NAME << " " << ENGINE_VERSION <<std::endl;
    std::cout << "id author " << ENGINE_AUTHOR << std::endl;
    std::cout << COUNT_BITS_METHOD << std::endl;
    std::cout << GET_LSB_METHOD << std::endl;
}

void UCI::loop(){
    Board board;
    std::string line;
	std::string token;
    std::thread s;

    // Make sure that the outputs are sent straight away to the GUI
	std::cout.setf (std::ios::unitbuf);

    while (std::getline(std::cin, line)) {
        std::istringstream is(line);
        token.clear();
        is >> std::skipws >> token;
        if (token == "uci") {
            std::cout << "id name " << ENGINE_NAME << " " << ENGINE_VERSION <<std::endl;
            std::cout << "id author " << ENGINE_AUTHOR << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (token == "isready") {
            std::cout << "readyok" << std::endl;
        }
        else if (token == "ucinewgame") {
            parse_position(board, "position startpos");
            continue;
        }
        else if (token == "go"){
            if (s.joinable()) {
                s.join();
            }
            s = std::thread(parse_go, std::ref(board), line.c_str());
            //parse_go(board, line.c_str());
        }
        else if (token == "stop"){
            if (s.joinable()) {
                s.join();
            }
        }
        else if (token == "position"){
            parse_position(board, line.c_str());
        }
        else if (token == "quit"){
            if (s.joinable()) {
                s.join();
            }
            std::exit(0);
        }
        else if (token == "d"){
            print_board(board);
        }
    }
}

void parse_go(Board &board, const char *command){
    int depth = -1;
    // print command
    std::cout << "Command: " << command << '\n';

    char *argument = NULL;

    if ((argument = strstr(command, "infinite"))) {} // infinite search
    /* if ((argument = strstr(command, "binc")) && board.side == black) time.inc = atoi(argument + 5); // parse black time increment
    if ((argument = strstr(command, "winc")) && board.side == white) time.inc = atoi(argument + 5); // parse white time increment
    if ((argument = strstr(command, "wtime")) && board.side == white) time.time = atoi(argument + 6); // parse white time limit
    if ((argument = strstr(command, "btime")) && board.side == black) time.time = atoi(argument + 6); // parse black time limit
    if ((argument = strstr(command, "movestogo"))) time.movestogo = atoi(argument + 10); // parse number of moves to go
    if ((argument = strstr(command, "movetime"))) time.movetime = atoi(argument + 9); // parse amount of time allowed to spend to make a move */
    if ((argument = strstr(command, "depth"))) depth = atoi(argument + 6); // parse search depth
    
    /* if(time.movetime != -1){ // if move time is not available
        time.time = time.movetime; // set time equal to move time
        time.movestogo = 1; // set moves to go to 1
    } */
    
    //time.starttime = get_time_ms(); // init start time
    depth = depth; //?

    
    /* if(time.time != -1){ // if time control is available
        time.timeset = 1; // flag we're playing with time control

        // set up timing
        time.time /= time.movestogo;
        time.time -= 50;
        time.stoptime = time.starttime + time.time + time.inc;
    } */       
    // if depth is not available
    if(depth == -1) depth = 5; // set depth to 64 plies (takes ages to complete...)
    
    /* std::cout << "time: " << time.time << " start: " << time.starttime << " stop: " << time.stoptime
              << " depth: " << depth << " timeset: " << time.timeset << '\n'; */

    search_position(board, depth);
}