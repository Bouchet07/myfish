#include "io.h"

#include <iostream>
#include <cstring>

#include "bitboard.h"
#include "attacks.h"

using namespace std;

const char *square_to_coordinates[64] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", 
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", 
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};

// ASCII pieces
const char ascii_pieces[13] = "PNBRQKpnbrqk"; // 13 including null terminator \0

// unicode pieces
const char *unicode_pieces[12] = {
    "♙", "♘", "♗", "♖", "♕", "♔",
    "♟", "♞", "♝", "♜", "♛", "♚"
};

// convert ASCII char pieces to encoded constants

std::unordered_map<char, int> char_pieces = {
        {'P', P}, {'N', N}, {'B', B}, {'R', R}, {'Q', Q}, {'K', K},
        {'p', p}, {'n', n}, {'b', b}, {'r', r}, {'q', q}, {'k', k}
};

std::unordered_map<int, char> promoted_pieces = {
        {Q, 'q'}, {R, 'r'}, {B, 'b'}, {N, 'n'},
        {q, 'q'}, {r, 'r'}, {b, 'b'}, {n, 'n'}
};

const char* empty_board = "8/8/8/8/8/8/8/8 w - - ";
const char* start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
const char* tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
const char* killer_position = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
const char* cmk_position = "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 ";

void print_bitboard(U64 bitboard){
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

void print_board(){
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

void parse_fen(const char *fen){
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

void print_attacked_squares(int side){
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

void print_move(int move){
    if (get_move_promoted(move)){
        cout << square_to_coordinates[get_move_source(move)]
             << square_to_coordinates[get_move_target(move)]
             << promoted_pieces[get_move_promoted(move)];

    }else{
        cout << square_to_coordinates[get_move_source(move)]
             << square_to_coordinates[get_move_target(move)];
    }
    
}

void print_move_list(moves *move_list){
    cout << "\n     move    piece     capture   double    enpass    castling\n\n";
    for (int move_count = 0; move_count < move_list->count; move_count++){
        int move = move_list->moves[move_count];
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
    cout << "\n\n     Total number of moves: " << move_list->count << "\n\n";
}