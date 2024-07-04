#ifndef UCI_H
#define UCI_H

#include "types.h"
#include "bitboard.h"

class UCI {
public:
    void init();
    void loop(int argc, char* argv[]);
};

void parse_go(Board &board, TimeControl &time, std::string_view command);

#endif