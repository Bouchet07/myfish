#include <iostream>

#include "types.h"
#include "bitboard.h"
#include "magic.h"
#include "attacks.h"
#include "moves.h"
#include "io.h"

int main(){
    init_all();
    std::cout << COUNT_BITS_METHOD << "\n";
    std::cout << GET_LSB_METHOD << "\n";
    parse_fen(start_position);
    print_board();

    moves move_list[1];
    generate_moves(move_list);
    print_move_list(move_list);

}