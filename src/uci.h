#ifndef UCI_H
#define UCI_H

#include "types.h"
#include "bitboard.h"

class UCI {
public:
    void init();
    void loop();
};

void parse_go(Board &board, const char *command);

#endif