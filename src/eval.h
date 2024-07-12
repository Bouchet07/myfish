#ifndef EVAL_H
#define EVAL_H

#include "types.h"
#include "bitboard.h"
#include "attacks.h"


constexpr Value material_score(PieceType piece){
    switch (piece){
        case PAWN:   return 100;//PawnValue;
        case KNIGHT: return 300;//KnightValue;
        case BISHOP: return 350;//BishopValue;
        case ROOK:   return 500;//RookValue;
        case QUEEN:  return 1000;//QueenValue;
        case KING:   return 10000;//VALUE_INFINITE;
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
constexpr Square mirror_score[128] =
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

constexpr Value piece_score(Piece p, Square square){
    switch (type_of(p)){
        case KNIGHT: return knight_score[relative_square(~color_of(p), square)];
        case BISHOP: return bishop_score[relative_square(~color_of(p), square)];
        case ROOK:   return rook_score  [relative_square(~color_of(p), square)];
        case KING:   return king_score  [relative_square(~color_of(p), square)];
        default  :   return 0;
    }
    
}

constexpr Value double_pawn_penalty = -10;
constexpr Value isolated_pawn_penalty = -10;
constexpr Value passed_pawn_bonus[RANK_NB] = {0, 5, 10, 20, 35, 60, 100, 200};
constexpr Value semi_open_file_bonus = 10;
constexpr Value open_file_bonus = 15;

//int evaluate(Board &board);

constexpr Value evaluate_pawns(Board &board){
    Bitboard w_pawn = board[W_PAWN];
    Bitboard b_pawn = board[B_PAWN];
    uint8_t w_pawn_count = popcnt(w_pawn);
    uint8_t b_pawn_count = popcnt(b_pawn);

    Value score = w_pawn_count * material_score(PAWN) -
                  b_pawn_count * material_score(PAWN);

    score += popcnt(w_pawn & shift<NORTH>(w_pawn)) * double_pawn_penalty;
    score -= popcnt(b_pawn & shift<SOUTH>(b_pawn)) * double_pawn_penalty;

    while (w_pawn){
        Square square = get_LSB(w_pawn);
        score += pawn_score[relative_square(~WHITE, square)];
        if (!(isolated_file_bb(square) & board[W_PAWN]))
            score += isolated_pawn_penalty;
        if (!(passed_pawn_mask(WHITE, square) & board[B_PAWN]))
            score += passed_pawn_bonus[relative_rank(WHITE, square)];
        pop_LSB(w_pawn);
    }
    while (b_pawn){
        Square square = get_LSB(b_pawn);
        score -= pawn_score[relative_square(~BLACK, square)];
        if (!(isolated_file_bb(square) & board[B_PAWN]))
            score -= isolated_pawn_penalty;
        if (!(passed_pawn_mask(BLACK, square) & board[W_PAWN]))
            score -= passed_pawn_bonus[relative_rank(BLACK, square)];
        pop_LSB(b_pawn);
    }
    return score;
}

constexpr Value evaluate_bishop(Board &board){
    Bitboard w_bishop = board[W_BISHOP];
    Bitboard b_bishop = board[B_BISHOP];

    Value score = popcnt(w_bishop) * material_score(BISHOP) - 
                  popcnt(b_bishop) * material_score(BISHOP);

    while(w_bishop){
        Square square = get_LSB(w_bishop);
        score += piece_score(make_piece(WHITE, BISHOP), square);
        score += (get_bishop_attacks(square, 0) & board[B_KING]) ?  5 : 0; // x-ray attack
        score += (get_bishop_attacks(square, 0) & board[B_QUEEN]) ? 1 : 0; // x-ray attack
        score += popcnt(get_bishop_attacks(square, board[BOTH])) * 2; // bishop mobility
        pop_LSB(w_bishop);
    }
    while(b_bishop){
        Square square = get_LSB(b_bishop);
        score -= piece_score(make_piece(BLACK, BISHOP), square);
        score -= (get_bishop_attacks(square, 0) & board[W_KING]) ?  5 : 0; // x-ray attack
        score -= (get_bishop_attacks(square, 0) & board[W_QUEEN]) ? 1 : 0; // x-ray attack
        score -= popcnt(get_bishop_attacks(square, board[BOTH])) * 2; // bishop mobility
        pop_LSB(b_bishop);
    }
    return score;
}

constexpr Value evaluate_rook(Board &board){
    Bitboard w_rook = board[W_ROOK];
    Bitboard b_rook = board[B_ROOK];

    Value score = popcnt(w_rook) * material_score(ROOK) - 
                  popcnt(b_rook) * material_score(ROOK);

    while(w_rook){
        Square square = get_LSB(w_rook);
        score += piece_score(make_piece(WHITE, ROOK), square);

        if (!(file_bb(square) & board[W_PAWN])){
            if (!(file_bb(square) & board[B_PAWN])) score += open_file_bonus;
            else                                    score += semi_open_file_bonus;
        }
        score += (get_rook_attacks(square, 0) & board[B_KING]) ?  5 : 0; // x-ray attack
        score += (get_rook_attacks(square, 0) & board[B_QUEEN]) ? 1 : 0; // x-ray attack
        
        pop_LSB(w_rook);
    }
    while(b_rook){
        Square square = get_LSB(b_rook);
        score -= piece_score(make_piece(BLACK, ROOK), square);

        if (!(file_bb(square) & board[B_PAWN])){
            if (!(file_bb(square) & board[W_PAWN])) score -= open_file_bonus;
            else                                    score -= semi_open_file_bonus;
        }
        score -= (get_rook_attacks(square, 0) & board[W_KING]) ?  5 : 0; // x-ray attack
        score -= (get_rook_attacks(square, 0) & board[W_QUEEN]) ? 1 : 0; // x-ray attack

        pop_LSB(b_rook);
    }
    return score;
}

constexpr Value evaluate_queen(Board &board){
    Bitboard w_queen = board[W_QUEEN];
    Bitboard b_queen = board[B_QUEEN];

    Value score = popcnt(w_queen) * material_score(QUEEN) - 
                  popcnt(b_queen) * material_score(QUEEN);

    while(w_queen){
        Square square = get_LSB(w_queen);
        score += piece_score(W_QUEEN, square);
        score += popcnt(get_queen_attacks(square, board[BOTH])) * 2; // queen mobility
        pop_LSB(w_queen);
    }
    while(b_queen){
        Square square = get_LSB(b_queen);
        score -= piece_score(B_QUEEN, square);
        score -= popcnt(get_queen_attacks(square, board[BOTH])) * 2; // queen mobility
        pop_LSB(b_queen);
    }
    return score;

}

constexpr Value evaluate_king(Board &board){
    Value score = 0;

    Square square = get_LSB(board[W_KING]);
    score += piece_score(W_KING, square);
    score += popcnt(king_attacks[square] & board[WHITE]) * 5; // king safety
    score += piece_score(W_KING, square);

    square = get_LSB(board[B_KING]);
    score -= piece_score(B_KING, square);
    score -= popcnt(king_attacks[square] & board[BLACK]) * 5; // king safety
    score -= piece_score(B_KING, square);

    return score;
}

constexpr Value evaluate_piece(Board &board, PieceType p){
    Bitboard w_piece = board[make_piece(WHITE, p)];
    Bitboard b_piece = board[make_piece(BLACK, p)];

    Value score = popcnt(w_piece) * material_score(p) - 
                  popcnt(b_piece) * material_score(p);
    
    while(w_piece){
        Square square = get_LSB(w_piece);
        score += piece_score(make_piece(WHITE, p), square);
        pop_LSB(w_piece);
    
    }
    while(b_piece){
        Square square = get_LSB(b_piece);
        score -= piece_score(make_piece(BLACK, p), square);
        pop_LSB(b_piece);
    
    }
    return score;
}

constexpr Value evaluate(Board &board){
    Value score = evaluate_pawns(board);
    score += evaluate_rook(board);
    score += evaluate_bishop(board);
    score += evaluate_king(board);
    score += evaluate_queen(board);
    score += evaluate_piece(board, KNIGHT);
    
    return (board.side==WHITE) ? score : -score;
}

#endif // EVAL_H