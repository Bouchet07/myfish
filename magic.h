#ifndef MAGIC_H
#define MAGIC_H

#include "bitboard.h"

extern unsigned int random_state;
unsigned int get_random_U32_number();
U64 get_random_U64_number();

U64 generate_magic_number();
U64 find_magic_number(int square, int relevant_bits, int bishop);

extern U64 bishop_magic_numbers[64];
extern U64 rook_magic_numbers[64];



#endif