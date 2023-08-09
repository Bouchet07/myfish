#include "types.h"

// pieces bitboards
U64 bitboards[12];
// occupancies bitboards
U64 occupancies[3];
// side to move
int side;
// enpassant square
int enpassant = no_sq;
// castling rights
int castle;
// leaf nodes (number of positions reached during the test of the move generator at a given depth)
long nodes;