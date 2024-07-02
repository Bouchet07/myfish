#ifndef EVAL_H
#define EVAL_H

#include "types.h"
#include "bitboard.h"


constexpr Value material_score(PieceType piece){
    switch (piece){
        case PAWN:   return PawnValue;
        case KNIGHT: return KnightValue;
        case BISHOP: return BishopValue;
        case ROOK:   return RookValue;
        case QUEEN:  return QueenValue;
        //case KING:   return VALUE_INFINITE;
        default:     return 0;
    }
}

// pawn positional score (black side)
constexpr Value pawn_score[64] = 
{
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  30,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
     5,   5,  10,  20,  20,   5,   5,   5,
     0,   0,   0,   5,   5,   0,   0,   0,
     0,   0,   0, -10, -10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score (black side)
constexpr Value knight_score[64] = 
{
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,  10,  10,   0,   0,  -5,
    -5,   5,  20,  20,  20,  20,   5,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,   5,  20,  10,  10,  20,   5,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5, -10,   0,   0,   0,   0, -10,  -5
};

// bishop positional score (black side)
constexpr Value bishop_score[64] = 
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,  10,   0,   0,   0,   0,  10,   0,
     0,  30,   0,   0,   0,   0,  30,   0,
     0,   0, -10,   0,   0, -10,   0,   0

};

// rook positional score (black side)
constexpr Value rook_score[64] =
{
    50,  50,  50,  50,  50,  50,  50,  50,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,   0,  20,  20,   0,   0,   0

};

// king positional score (black side)
constexpr Value king_score[64] = 
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   0,   5,  10,  10,   5,   0,   0,
     0,   5,   5,  -5,  -5,   0,   5,   0,
     0,   0,   5,   0, -15,   0,  10,   0
};

// mirror positional score tables for opposite side
constexpr int mirror_score[128] =
{
	SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1
};

//int evaluate(Board &board);

constexpr Value evaluate(Board &board){
    int score = 0;
    Bitboard bitboard = 0;
    Square square = SQUARE_ZERO;

    for (PieceType bb_piece = PAWN; bb_piece < PIECE_TYPE_NB; ++bb_piece){
        bitboard = board.bitboards[make_index_piece(WHITE, bb_piece)];
        while(bitboard){
            square = get_LSB(bitboard);

            score += material_score(bb_piece);
            switch (bb_piece){
                case PAWN:   score += pawn_score[mirror_score[square]];   break;
                case KNIGHT: score += knight_score[mirror_score[square]]; break;
                case BISHOP: score += bishop_score[mirror_score[square]]; break;
                case ROOK:   score += rook_score[mirror_score[square]];   break;
                case KING:   score += king_score[mirror_score[square]];   break;
                default: break;
            }

            pop_LSB(bitboard);
        }
        bitboard = board.bitboards[make_index_piece(BLACK, bb_piece)];
        while(bitboard){
            square = get_LSB(bitboard);

            score -= material_score(bb_piece);
            switch (bb_piece){
                case PAWN:   score -= pawn_score[square];   break;
                case KNIGHT: score -= knight_score[square]; break;
                case BISHOP: score -= bishop_score[square]; break;
                case ROOK:   score -= rook_score[square];   break;
                case KING:   score -= king_score[square];   break;
                default: break;
            }

            pop_LSB(bitboard);
        }

    }
    return (board.side==WHITE) ? score : -score;
}

#endif // EVAL_H