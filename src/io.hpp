#ifndef IO_H
#define IO_H

#include <iostream>
#include <cstring>

#include "types.hpp"
#include "bitboard.hpp"
#include "attacks.hpp"
#include "moves.hpp"
#include "search.hpp"

// FEN dedug positions
static const char* const empty_board = "8/8/8/8/8/8/8/8 w - - ";
static const char* const start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
static const char* const tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
static const char* const killer_position = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
static const char* const cmk_position = "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 ";

using namespace std;

inline void print_bitboard(U64 bitboard){
    cout << "\n";

    for (int rank = 0; rank < 8; rank++){
        for (int file = 0; file < 8; file++){

            int square = rank * 8 + file;
            if (!file) cout << "  " << 8 - rank << " ";
            
            cout << " " << (get_bit(bitboard, square) ? 1 : 0);
            
        }
        cout << "\n";
    }
    cout << "\n     a b c d e f g h\n\n";

    // print bitboard as unsigned decimal number
    cout << "     Bitboard: " << bitboard << "d\n\n";
    //printf("     Bitboard: %llud\n\n", bitboard);
}

inline void print_board(){
    cout << "\n";
    
    for (int rank = 0; rank < 8; rank++){
        for (int file = 0; file < 8; file++){

            int square = rank * 8 + file;
            if (!file) cout << "  " << 8 - rank << " ";

            int piece = -1;
            
            // loop over all piece bitboards
            for (int bb_piece = P; bb_piece <= k; bb_piece++){
                if (get_bit(bitboards[bb_piece], square)) piece = bb_piece;
                    
            }
            #ifdef WIN64
                cout << ' ' << ((piece == -1) ? '.' : ascii_pieces[piece]);
            #else
                cout << ' ' << ((piece == -1) ? '.' : unicode_pieces[piece]);
            #endif
            
            
        }
        cout << "\n";
    }
    cout << "\n     a b c d e f g h\n\n";
    cout << "     Side:     " << (side^1 ? "white" : "black") << "\n";
    cout << "     Enpassant:   " << (enpassant != no_sq ? square_to_coordinates[enpassant] : "no") << "\n";
    cout << "     Castling:  " << (castle & wk ? 'K' : '-') <<
                                  (castle & wq ? 'Q' : '-') <<
                                  (castle & bk ? 'k' : '-') <<
                                  (castle & bq ? 'q' : '-') << "\n\n";
}

#define _is_letter(fen) (((fen) >= 'a' && (fen) <= 'z') || ((fen) >= 'A' && (fen) <= 'Z'))
#define _is_number(fen) ((fen) >= '0' && (fen) <= '8')

inline void parse_fen(const char *fen){
    std::memset(bitboards, 0ULL, sizeof(bitboards));
    std::memset(occupancies, 0ULL, sizeof(occupancies));
    enpassant = no_sq;
    castle = 0;

    // board
    for (int rank = 0; rank < 8; rank++){
        for (int file = 0; file < 8; file++){
            int piece_down = 0;
            int square = rank * 8 + file;

            if (_is_letter(*fen)){
                int piece = char_pieces[*fen];
                set_bit(bitboards[piece], square);
                fen++;
                piece_down = 1;
            }
            if (_is_number(*fen)){
                int offset = *fen - '0';
                file += (!file && !piece_down) ? offset - 1: offset;
                //file += (piece_down && !file) ? offset - 1 : offset;
                fen++;
            }
            if (*fen == '/') *fen++;

        }
    }
    // side to move
    fen++; // skip initial space
    side = (*fen == 'w') ? white : black;

    // castling rights
    fen += 2; // skip 2 spaces
    while (*fen != ' '){
        switch (*fen){
            case 'K' : castle |= wk; break;
            case 'Q' : castle |= wq; break;
            case 'k' : castle |= bk; break;
            case 'q' : castle |= bq; break;
            case '-' : break;
        }
        fen++;
    }
    // enpassant square
    fen++; //skip initial space
    if (*fen != '-'){
        int file = fen[0] - 'a'; // fen[0] = *fen
        int rank = 8 - (fen[1] - '0');
        enpassant = rank * 8 + file;
    }else{
        enpassant = no_sq;
    }

    // occupancies
    for (int piece = P; piece <= K; piece++){
        occupancies[white] |= bitboards[piece];
    }
    for (int piece = p; piece <= k; piece++){
        occupancies[black] |= bitboards[piece];
    }
    occupancies[both] = occupancies[white] | occupancies[black];

    //printf("fen: %s\n", fen);
}

inline void print_attacked_squares(int side){
    cout << "\n";
    for (int rank = 0; rank < 8; rank++){
        for (int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            if (!file) printf("  %d ", 8 - rank);

            cout << ' ' << is_square_attacked(square, side);
        }
        cout << "\n";
    }
    cout << "\n     a b c d e f g h\n\n";  
}
/* inline void print_move(int move){
    if (get_move_promoted(move)){
        cout << square_to_coordinates[get_move_source(move)]
             << square_to_coordinates[get_move_target(move)]
             << promoted_pieces[get_move_promoted(move)];

    }else{
        cout << square_to_coordinates[get_move_source(move)]
             << square_to_coordinates[get_move_target(move)];
    }
    
} */
inline void print_move_list(moves &move_list){
    cout << "\n     move    piece     capture   double    enpass    castling\n\n";
    for (int move_count = 0; move_count < move_list.count; move_count++){
        int move = move_list.moves[move_count];
        #ifdef WIN64
            // print move
            cout << "     " << square_to_coordinates[get_move_source(move)] << square_to_coordinates[get_move_target(move)]
                 << (get_move_promoted(move) ? promoted_pieces[get_move_promoted(move)] : ' ')
                 << "   " << ascii_pieces[get_move_piece(move)]
                 << "         " << (get_move_capture(move) ? 1 : 0)
                 << "         " << (get_move_double(move) ? 1 : 0)
                 << "         " << (get_move_enpassant(move) ? 1 : 0)
                 << "         " << (get_move_castling(move) ? 1 : 0) << '\n';

        #else
            // print move
            cout << "     " << square_to_coordinates[get_move_source(move)] << square_to_coordinates[get_move_target(move)]
                 << (get_move_promoted(move) ? promoted_pieces[get_move_promoted(move)] : ' ')
                 << "   " << unicode_pieces[get_move_piece(move)]
                 << "         " << (get_move_capture(move) ? 1 : 0)
                 << "         " << (get_move_double(move) ? 1 : 0)
                 << "         " << (get_move_enpassant(move) ? 1 : 0)
                 << "         " << (get_move_castling(move) ? 1 : 0) << '\n';
        #endif
    }
    cout << "\n\n     Total number of moves: " << move_list.count << "\n\n";
}

void print_move_scores(moves &move_list)
{
    cout << "     Move scores:\n\n";
        
    // loop over moves within a move list
    for (int count = 0; count < move_list.count; count++)
    {
        cout << "     move: ";
        print_move(move_list.moves[count]);
        cout << " score: " << score_move(move_list.moves[count]) << '\n';
    }
}

/* inline void search_position(int depth){
    int score;
    
    // reset nodes counter
    nodes = 0;
    // reset follow PV flags
    follow_pv = 0;
    score_pv = 0;
    
    // clear helper data structures for search
    std::memset(killer_moves, 0, sizeof(killer_moves));
    std::memset(history_moves, 0, sizeof(history_moves));
    std::memset(pv_table, 0, sizeof(pv_table));
    std::memset(pv_length, 0, sizeof(pv_length));

    // iterative deepening
    for (int current_depth = 1; current_depth <= depth; current_depth++){
        // enable follow PV flag
        follow_pv = 1;
        
        score = negamax(-50000, 50000, current_depth);
        std::cout << "info score cp " << score << " depth " << current_depth << " nodes " << nodes << " pv ";
        for (int count = 0; count < pv_length[0]; count++){
            // print PV move
            print_move(pv_table[0][count]);
            std::cout << ' ';
        }
        std::cout << '\n';
    }
    
    std::cout << "bestmove ";
    print_move(pv_table[0][0]);
    std::cout << "\n";
} */

#endif