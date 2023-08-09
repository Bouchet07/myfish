#ifndef IO_H
#define IO_H

#include <unordered_map>

#include "types.h"
#include "moves.h"

extern const char *square_to_coordinates[64];
extern const char ascii_pieces[13];
extern const char *unicode_pieces[12];
extern std::unordered_map<char, int> char_pieces;
extern std::unordered_map<int, char> promoted_pieces;

void print_bitboard(U64 bitboard);
void print_board();

// FEN dedug positions
extern const char* empty_board;
extern const char* start_position;
extern const char* tricky_position;
extern const char* killer_position;
extern const char* cmk_position;

#define _is_letter(fen) (((fen) >= 'a' && (fen) <= 'z') || ((fen) >= 'A' && (fen) <= 'Z'))
#define _is_number(fen) ((fen) >= '0' && (fen) <= '8')

void parse_fen(const char *fen);

void print_attacked_squares(int side);
void print_move(int move);
void print_move_list(moves *move_list);

#endif