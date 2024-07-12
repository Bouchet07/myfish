#ifndef EVAL_H
#define EVAL_H

#include "types.h"
#include "bitboard.h"
#include "attacks.h"

// forward declaration
constexpr int get_game_phase_score(const Board &board);

template<GamePhase GP>
constexpr Value material_score(const PieceType piece, const Board &board){
    if constexpr (GP == OPENING){
        switch (piece){
            case PAWN:   return 82;
            case KNIGHT: return 337;
            case BISHOP: return 365;
            case ROOK:   return 477;
            case QUEEN:  return 1025;
            case KING:   return 12000;
            default:     return 0;
        }
    }else if constexpr (GP == ENDGAME){
        switch (piece){
            case PAWN:   return 94;
            case KNIGHT: return 281;
            case BISHOP: return 297;
            case ROOK:   return 512;
            case QUEEN:  return 936;
            case KING:   return 12000;
            default:     return 0;
        } 
    }else{
        /*          
            Now in order to calculate interpolated score
            for a given game phase we use this formula
            (same for material and positional scores):
            
            (
                score_opening * game_phase_score + 
                score_endgame * (opening_phase_score - game_phase_score)
            ) / opening_phase_score
        
            E.g. the score for pawn on d4 at phase say 5000 would be
            interpolated_score = (12 * 5000 + (-7) * (6192 - 5000)) / 6192 = 8,342377261

            This is not a normal linear interpolation, but it is a good approximation
        */
        int game_phase_score = get_game_phase_score(board);
        return (
            material_score<OPENING>(piece, board) * game_phase_score +
            material_score<ENDGAME>(piece, board) * (opening_phase_score - game_phase_score)
        ) / opening_phase_score;
    }
    
}

/*
    The game phase score of the game is derived from the pieces
    (not counting pawns and kings) that are still on the board.
    The full material starting position game phase score is:
    
    4 * knight material score in the opening +
    4 * bishop material score in the opening +
    4 * rook material score in the opening +
    2 * queen material score in the opening
*/
constexpr int get_game_phase_score(const Board &board){
    int game_phase_score = 0;
    for (PieceType piece = KNIGHT; piece <= QUEEN; ++piece){
        game_phase_score += (popcnt(board[make_piece(WHITE, piece)]) + popcnt(board[make_piece(BLACK, piece)]))
                          * material_score<OPENING>(piece, board);
    }
    return game_phase_score;
}

constexpr GamePhase get_game_phase(const Board &board){
    int game_phase_score = get_game_phase_score(board);
    if (game_phase_score > opening_phase_score) return OPENING;
    if (game_phase_score > endgame_phase_score) return MIDDLEGAME;
    return ENDGAME;
}

// positional piece scores [game phase][piecetype][square] for black side
constexpr Value positional_score_table[2][6][64] =

// opening positional piece scores //
{
    //pawn
    0,   0,   0,   0,   0,   0,  0,   0,
    98, 134,  61,  95,  68, 126, 34, -11,
    -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
    0,   0,   0,   0,   0,   0,  0,   0,
    
    // knight
    -167, -89, -34, -49,  61, -97, -15, -107,
    -73, -41,  72,  36,  23,  62,   7,  -17,
    -47,  60,  37,  65,  84, 129,  73,   44,
    -9,  17,  19,  53,  37,  69,  18,   22,
    -13,   4,  16,  13,  28,  19,  21,   -8,
    -23,  -9,  12,  10,  19,  17,  25,  -16,
    -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23,
    
    // bishop
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
    -4,   5,  19,  50,  37,  37,   7,  -2,
    -6,  13,  13,  26,  34,  12,  10,   4,
    0,  15,  15,  15,  14,  27,  18,  10,
    4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
    
    // rook
    32,  42,  32,  51, 63,  9,  31,  43,
    27,  32,  58,  62, 80, 67,  26,  44,
    -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
    
    // queen
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
    -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
    -1, -18,  -9,  10, -15, -25, -31, -50,
    
    // king
    -65,  23,  16, -15, -56, -34,   2,  13,
    29,  -1, -20,  -7,  -8,  -4, -38, -29,
    -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
    1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,


    // Endgame positional piece scores //

    //pawn
    0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
    94, 100,  85,  67,  56,  53,  82,  84,
    32,  24,  13,   5,  -2,   4,  17,  17,
    13,   9,  -3,  -7,  -7,  -8,   3,  -1,
    4,   7,  -6,   1,   0,  -5,  -1,  -8,
    13,   8,   8,  10,  13,   0,   2,  -7,
    0,   0,   0,   0,   0,   0,   0,   0,
    
    // knight
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -29, -51, -23, -15, -22, -18, -50, -64,
    
    // bishop
    -14, -21, -11,  -8, -7,  -9, -17, -24,
    -8,  -4,   7, -12, -3, -13,  -4, -14,
    2,  -8,   0,  -1, -2,   6,   0,   4,
    -3,   9,  12,   9, 14,  10,   3,   2,
    -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
    
    // rook
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
    7,  7,  7,  5,  4,  -3,  -5,  -3,
    4,  3, 13,  1,  2,   1,  -1,   2,
    3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
    
    // queen
    -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
    3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
    
    // king
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
    10,  17,  23,  15,  20,  45,  44,  13,
    -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};

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

template<GamePhase GP, PieceType pt, Color c>
constexpr Value positional_score(const Square square, const Board &board){
    if constexpr (GP == MIDDLEGAME){
        int game_phase_score = get_game_phase_score(board);
        return (
            positional_score<OPENING, pt, c>(square, board) * game_phase_score +
            positional_score<ENDGAME, pt, c>(square, board) * (opening_phase_score - game_phase_score)
        ) / opening_phase_score;
    }else{
        return positional_score_table[GP][make_index_piece_type(pt)][relative_square(~c, square)];
    }
}

template<GamePhase GP, Piece p>
constexpr Value positional_score(const Square square, const Board &board){
    return positional_score<GP, type_of(p), color_of(p)>(square, board);
}


/* constexpr Value piece_score(Piece p, Square square){
    switch (type_of(p)){
        case KNIGHT: return knight_score[relative_square(~color_of(p), square)];
        case BISHOP: return bishop_score[relative_square(~color_of(p), square)];
        case ROOK:   return rook_score  [relative_square(~color_of(p), square)];
        case KING:   return king_score  [relative_square(~color_of(p), square)];
        default  :   return 0;
    }
    
} */

constexpr Value double_pawn_penalty = -10;
constexpr Value isolated_pawn_penalty = -10;
constexpr Value passed_pawn_bonus[RANK_NB] = { 0, 10, 30, 50, 75, 100, 150, 200 };
constexpr Value semi_open_file_bonus = 10;
constexpr Value open_file_bonus = 15;


template<GamePhase GP>
constexpr Value evaluate_pawns(const Board &board){
    Bitboard w_pawn = board[W_PAWN];
    Bitboard b_pawn = board[B_PAWN];
    uint8_t w_pawn_count = popcnt(w_pawn);
    uint8_t b_pawn_count = popcnt(b_pawn);

    Value score = w_pawn_count * material_score<GP>(PAWN, board) -
                  b_pawn_count * material_score<GP>(PAWN, board);
    //std::cout << "w_pawn_count: " << (int)w_pawn_count << '\n';
    //score += popcnt(w_pawn & shift<NORTH>(w_pawn)) * double_pawn_penalty;
    //score -= popcnt(b_pawn & shift<SOUTH>(b_pawn)) * double_pawn_penalty;
    int doubled_pawns = 0;
    while (w_pawn){
        Square square = get_LSB(w_pawn);
        score += positional_score<GP, PAWN, WHITE>(square, board);
        //score += pawn_score[relative_square(~WHITE, square)];
        //std::cout << "position score " << score << '\n';
        if (!(isolated_file_bb(square) & board[W_PAWN])){
            score += isolated_pawn_penalty;
            //std::cout << "isolated pawn penalty " << score << '\n';
        }
        if (!(passed_pawn_mask(WHITE, square) & board[B_PAWN])){
            score += passed_pawn_bonus[relative_rank(WHITE, square)];
            //std::cout << "passed pawn bonus " << score << '\n';
        }
        
        doubled_pawns = popcnt(board[W_PAWN] & file_bb(square));
        if (doubled_pawns > 1){
            score += doubled_pawns * double_pawn_penalty;
            //std::cout << "double pawn penalty " << score << '\n';
        }

        pop_LSB(w_pawn);
    }
    doubled_pawns = 0;
    while (b_pawn){
        Square square = get_LSB(b_pawn);
        score -= positional_score<GP, PAWN, BLACK>(square, board);
        //score -= pawn_score[relative_square(~BLACK, square)];
        if (!(isolated_file_bb(square) & board[B_PAWN]))
            score -= isolated_pawn_penalty;
        if (!(passed_pawn_mask(BLACK, square) & board[W_PAWN]))
            score -= passed_pawn_bonus[relative_rank(BLACK, square)];

        doubled_pawns = popcnt(board[W_PAWN] & file_bb(square));
        if (doubled_pawns > 1) score += doubled_pawns * double_pawn_penalty;

        pop_LSB(b_pawn);
    }
    return score;
}

template<GamePhase GP>
constexpr Value evaluate_bishop(const Board &board){
    Bitboard w_bishop = board[W_BISHOP];
    Bitboard b_bishop = board[B_BISHOP];

    Value score = popcnt(w_bishop) * material_score<GP>(BISHOP, board) - 
                  popcnt(b_bishop) * material_score<GP>(BISHOP, board);

    while(w_bishop){
        Square square = get_LSB(w_bishop);
        score += positional_score<GP, BISHOP, WHITE>(square, board);
        //score += piece_score(make_piece(WHITE, BISHOP), square);
        //score += (get_bishop_attacks(square, 0) & board[B_KING]) ?  5 : 0; // x-ray attack
        //score += (get_bishop_attacks(square, 0) & board[B_QUEEN]) ? 1 : 0; // x-ray attack
        score += popcnt(get_bishop_attacks(square, board[BOTH])) * 1; // bishop mobility
        pop_LSB(w_bishop);
    }
    while(b_bishop){
        Square square = get_LSB(b_bishop);
        score -= positional_score<GP, BISHOP, BLACK>(square, board);
        //score -= piece_score(make_piece(BLACK, BISHOP), square);
        //score -= (get_bishop_attacks(square, 0) & board[W_KING]) ?  5 : 0; // x-ray attack
        //score -= (get_bishop_attacks(square, 0) & board[W_QUEEN]) ? 1 : 0; // x-ray attack
        score -= popcnt(get_bishop_attacks(square, board[BOTH])) * 1; // bishop mobility
        pop_LSB(b_bishop);
    }
    return score;
}

template<GamePhase GP>
constexpr Value evaluate_rook(const Board &board){
    Bitboard w_rook = board[W_ROOK];
    Bitboard b_rook = board[B_ROOK];

    Value score = popcnt(w_rook) * material_score<GP>(ROOK, board) - 
                  popcnt(b_rook) * material_score<GP>(ROOK, board);

    while(w_rook){
        Square square = get_LSB(w_rook);
        score += positional_score<GP, ROOK, WHITE>(square, board);
        //score += piece_score(make_piece(WHITE, ROOK), square);

        if (!(file_bb(square) & board[W_PAWN])){
            if (!(file_bb(square) & board[B_PAWN])) score += open_file_bonus + semi_open_file_bonus;
            else                                    score += semi_open_file_bonus;
        }
        //score += (get_rook_attacks(square, 0) & board[B_KING]) ?  5 : 0; // x-ray attack
        //score += (get_rook_attacks(square, 0) & board[B_QUEEN]) ? 1 : 0; // x-ray attack
        
        pop_LSB(w_rook);
    }
    while(b_rook){
        Square square = get_LSB(b_rook);
        score -= positional_score<GP, ROOK, BLACK>(square, board);
        //score -= piece_score(make_piece(BLACK, ROOK), square);

        if (!(file_bb(square) & board[B_PAWN])){
            if (!(file_bb(square) & board[W_PAWN])) score -= open_file_bonus + semi_open_file_bonus;
            else                                    score -= semi_open_file_bonus;
        }
        //score -= (get_rook_attacks(square, 0) & board[W_KING]) ?  5 : 0; // x-ray attack
        //score -= (get_rook_attacks(square, 0) & board[W_QUEEN]) ? 1 : 0; // x-ray attack

        pop_LSB(b_rook);
    }
    return score;
}

template<GamePhase GP>
constexpr Value evaluate_queen(const Board &board){
    Bitboard w_queen = board[W_QUEEN];
    Bitboard b_queen = board[B_QUEEN];

    Value score = popcnt(w_queen) * material_score<GP>(QUEEN, board) - 
                  popcnt(b_queen) * material_score<GP>(QUEEN, board);

    while(w_queen){
        Square square = get_LSB(w_queen);
        score += positional_score<GP, QUEEN, WHITE>(square, board);
        //score += piece_score(W_QUEEN, square);
        score += popcnt(get_queen_attacks(square, board[BOTH])) * 1; // queen mobility
        pop_LSB(w_queen);
    }
    while(b_queen){
        Square square = get_LSB(b_queen);
        score -= positional_score<GP, QUEEN, BLACK>(square, board);
        //score -= piece_score(B_QUEEN, square);
        score -= popcnt(get_queen_attacks(square, board[BOTH])) * 1; // queen mobility
        pop_LSB(b_queen);
    }
    return score;

}

template<GamePhase GP>
constexpr Value evaluate_king(const Board &board){
    Value score = 0;
    if (board[W_KING]){
        Square square = get_LSB(board[W_KING]);
        score += positional_score<GP, KING, WHITE>(square, board);
        //score += piece_score(W_KING, square);
        //score += popcnt(king_attacks[square] & board[WHITE]) * 5; // king safety
        /* if (!(file_bb(square) & board[W_PAWN])){
            if (!(file_bb(square) & board[B_PAWN])) score -= open_file_bonus + semi_open_file_bonus;
            else                                    score -= semi_open_file_bonus;
        } */
    }
    
    if (board[B_KING]){  
        Square square = get_LSB(board[B_KING]);
        score -= positional_score<GP, KING, BLACK>(square, board);
        //score -= piece_score(B_KING, square);
        //score -= popcnt(king_attacks[square] & board[BLACK]) * 5; // king safety
        /* if (!(file_bb(square) & board[B_PAWN])){
            if (!(file_bb(square) & board[W_PAWN])) score += open_file_bonus + semi_open_file_bonus;
            else                                    score += semi_open_file_bonus;
        } */
    }

    return score;
}

template<GamePhase GP, PieceType p>
constexpr Value evaluate_piece(const Board &board){
    Bitboard w_piece = board[make_piece(WHITE, p)];
    Bitboard b_piece = board[make_piece(BLACK, p)];

    Value score = popcnt(w_piece) * material_score<GP>(p, board) - 
                  popcnt(b_piece) * material_score<GP>(p, board);
    
    while(w_piece){
        Square square = get_LSB(w_piece);
        score += positional_score<GP, p, WHITE>(square, board);
        //score += piece_score(make_piece(WHITE, p), square);
        pop_LSB(w_piece);
    
    }
    while(b_piece){
        Square square = get_LSB(b_piece);
        score -= positional_score<GP, p, BLACK>(square, board);
        //score -= piece_score(make_piece(BLACK, p), square);
        pop_LSB(b_piece);
    
    }
    return score;
}

template<GamePhase GP>
constexpr Value evaluate_dispatch(Board &board){
    Value score = evaluate_pawns<GP>(board);
    score += evaluate_rook<GP>(board);
    score += evaluate_bishop<GP>(board);
    score += evaluate_king<GP>(board);
    score += evaluate_queen<GP>(board);
    score += evaluate_piece<GP, KNIGHT>(board);
    
    return (board.side==WHITE) ? score : -score;
}

constexpr Value evaluate(Board &board){
    GamePhase GP = get_game_phase(board);
    switch (GP){
        case OPENING: return evaluate_dispatch<OPENING>(board);
        case ENDGAME: return evaluate_dispatch<ENDGAME>(board);
        default: return evaluate_dispatch<MIDDLEGAME>(board);
    }
}

#endif // EVAL_H