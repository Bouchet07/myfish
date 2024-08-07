#ifndef UCI_H
#define UCI_H

#include "types.h"
#include "bitboard.h"
#include "tt.h"
#include "3rd-party/BS_thread_pool.hpp"

class UCI {
public:
    void init();
    void loop(int argc, char* argv[]);
};

void parse_go(Board &board, TimeControl &time, RT &rt, std::string_view command);
void parse_options(std::string_view command);

#endif