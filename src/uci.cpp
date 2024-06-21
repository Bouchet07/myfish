#include <iostream>
#include <sstream>

#include "uci.h"
#include "bitboard.h"

void UCI::init(){
    std::cout << "id name " << ENGINE_NAME << " " << ENGINE_VERSION <<std::endl;
    std::cout << "id author " << ENGINE_AUTHOR << std::endl;
    std::cout << COUNT_BITS_METHOD << std::endl;
    std::cout << GET_LSB_METHOD << std::endl;
}

void UCI::loop(){
    
    std::string line;
	std::string token;

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
        else if (token == "ucinewgame") {}
        else if (token == "stop"){
            std::cout << "bestmove " << "e7e5" << std::endl;
        }
    }
}