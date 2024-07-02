#ifndef POSITION_H
#define POSITION_H

#include "types.h"
#include "bitboard.h"

class Position {
public:
    static void init();

    Position()                           = default;
    Position(const Position&)            = delete;
    Position& operator=(const Position&) = delete;
};

#endif // POSITION_H