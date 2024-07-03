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
    TimeControl time;
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
            time.stop = false;
            s = std::thread(parse_go, std::ref(board), std::ref(time), line);
        }
        else if (token == "stop"){
            time.stop = true;
            if (s.joinable()) {
                s.join();
            }
        }
        else if (token == "position"){
            parse_position(board, line);
        }
        else if (token == "quit"){
            time.stop = true;
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


void parse_go(Board &board, TimeControl &time, std::string_view command){
    int depth = -1;

    if (auto argument = command.find("infinite"); argument != std::string_view::npos) {
        // Handle infinite search
    }
    if (auto argument = command.find("binc"); argument != std::string_view::npos) {
        if (board.side == BLACK){
            time.inc = std::atoi(command.substr(argument + 5).data());
        }
    }
    if (auto argument = command.find("winc"); argument != std::string_view::npos) {
        if (board.side == WHITE){
            time.inc = std::atoi(command.substr(argument + 5).data());
        }
    }
    if (auto argument = command.find("wtime"); argument != std::string_view::npos) {
        if (board.side == WHITE){
            time.time = std::atoi(command.substr(argument + 6).data());
        }
    }
    if (auto argument = command.find("btime"); argument != std::string_view::npos) {
        if (board.side == BLACK){
            time.time = std::atoi(command.substr(argument + 6).data());
        }
    }
    if (auto argument = command.find("movestogo"); argument != std::string_view::npos) {
        time.moves_to_go = std::atoi(command.substr(argument + 10).data());
    }
    if (auto argument = command.find("movetime"); argument != std::string_view::npos) {
        time.move_time = std::atoi(command.substr(argument + 9).data());
    }
    if (auto argument = command.find("depth"); argument != std::string_view::npos) {
        depth = std::atoi(command.substr(argument + 6).data());
    }
    
    if(time.move_time != -1){ // if move time is not available
        time.time = time.move_time; // set time equal to move time
        time.moves_to_go = 1; // set moves to go to 1
    }
    
    time.start_time = get_time_ms(); // init start time

    
    if(time.time != -1){ // if time control is available
        time.timeset = 1; // flag we're playing with time control

        // set up timing
        time.time /= time.moves_to_go;
        time.time -= 50;
        time.stop_time = time.start_time + time.time + time.inc;
    }       
    // if depth is not available
    if(depth == -1) depth = 64; // set depth to 64 plies (takes ages to complete...)
    
    /* std::cout << "time: " << time.time << " start: " << time.starttime << " stop: " << time.stoptime
              << " depth: " << depth << " timeset: " << time.timeset << '\n'; */

    search_position(board, time, depth);
}