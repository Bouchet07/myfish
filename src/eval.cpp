#include "eval.h"


int evaluate(Board &board){
    int score = 0;
    Bitboard bitboard;
    Square square;

    for (PieceType bb_piece = PAWN; bb_piece < PIECE_TYPE_NB; ++bb_piece){
        bitboard = board.bitboards[make_index_piece(WHITE, bb_piece)];
        while(bitboard){
            square = get_LSB(bitboard);

            score += material_score[make_index_piece(WHITE, bb_piece)];
            switch (bb_piece){
                case PAWN: score += pawn_score[mirror_score[square]];   break;
                case KNIGHT: score += knight_score[mirror_score[square]]; break;
                case BISHOP: score += bishop_score[mirror_score[square]]; break;
                case ROOK: score += rook_score[mirror_score[square]];   break;
                case KING: score += king_score[mirror_score[square]];   break;
                default: break;
            }

            pop_LSB(bitboard);
        }
        bitboard = board.bitboards[make_index_piece(BLACK, bb_piece)];
        while(bitboard){
            square = get_LSB(bitboard);

            score += material_score[make_index_piece(BLACK, bb_piece)];
            switch (bb_piece){
                case PAWN: score -= pawn_score[square];   break;
                case KNIGHT: score -= knight_score[square]; break;
                case BISHOP: score -= bishop_score[square]; break;
                case ROOK: score -= rook_score[square];   break;
                case KING: score -= king_score[square];   break;
                default: break;
            }

            pop_LSB(bitboard);
        }

    }
    return (board.side==WHITE) ? score : -score;
}