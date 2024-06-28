#ifndef MOVES_H
#define MOVES_H

#include <array>

#include "types.h"
#include "bitboard.h"
#include "attacks.h"

/**
 *        binary move bits                      description                         hexadecimal move bits
 * 
 * 0000 0000 0000 0000 0011 1111    source square (6 bits for 64 squares)           0x00003F
 * 0000 0000 0000 1111 1100 0000    target square (6 bits for 64 squares)           0x000FC0
 * 0000 0000 1111 0000 0000 0000    piece (4 bits for 12 pieces)                    0x00F000
 * 0000 1111 0000 0000 0000 0000    promoted piece (4 bits for 8 promoted pieces)   0x0F0000
 * 0001 0000 0000 0000 0000 0000    capture flag (1 bit)                            0x100000
 * 0010 0000 0000 0000 0000 0000    double push flag (1 bit)                        0x200000
 * 0100 0000 0000 0000 0000 0000    enpassant flag (1 bit)                          0x400000
 * 1000 0000 0000 0000 0000 0000    castling flag (1 bit)                           0x800000
*/
/* using Move = uint32_t; */

enum MoveType {
    NORMAL,
    PROMOTION  = 1 << 14,
    EN_PASSANT = 2 << 14,
    CASTLING   = 3 << 14
};

// A move needs 16 bits to be stored
//
// bit  0- 5: destination square (from 0 to 63)
// bit  6-11: origin square (from 0 to 63)
// bit 12-13: promotion piece type - 2 (from KNIGHT-2 to QUEEN-2)
// bit 14-15: special move flag: promotion (1), en passant (2), castling (3)
// NOTE: en passant bit is set only when a pawn can be captured
//
// Special cases are Move::none() and Move::null(). We can sneak these in because in
// any normal move destination square is always different from origin square
// while Move::none() and Move::null() have the same origin and destination square.
class Move {
   public:
    Move() = default;
    constexpr explicit Move(uint16_t d) :
        data(d) {}

    constexpr Move(Square from, Square to) :
        data((from << 6) + to) {}

    template<MoveType T>
    static constexpr Move make(Square from, Square to, PieceType pt = KNIGHT) {
        return Move(T + ((pt - KNIGHT) << 12) + (from << 6) + to);
    }

    constexpr Square from_sq() const {
        //assert(is_ok());
        return Square((data >> 6) & 0x3F);
    }

    constexpr Square to_sq() const {
        //assert(is_ok());
        return Square(data & 0x3F);
    }

    constexpr int from_to() const { return data & 0xFFF; }

    constexpr MoveType type_of() const { return MoveType(data & (3 << 14)); }

    constexpr PieceType promotion_type() const { return PieceType(((data >> 12) & 3) + KNIGHT); }

    constexpr bool is_ok() const { return none().data != data && null().data != data; }

    static constexpr Move null() { return Move(65); }
    static constexpr Move none() { return Move(0); }

    constexpr bool operator==(const Move& m) const { return data == m.data; }
    constexpr bool operator!=(const Move& m) const { return data != m.data; }

    constexpr explicit operator bool() const { return data != 0; }

    constexpr uint16_t raw() const { return data; }

    /* struct MoveHash {
        std::size_t operator()(const Move& m) const { return make_key(m.data); }
    }; */

   protected:
    uint16_t data;
};

/* constexpr Move encode_move(Square source, Square target, Piece piece,
                           PieceType promoted, bool capture,
                           bool double_push, bool enpassant, bool castling) {
    
    return (static_cast<uint32_t>(source)) | 
           (static_cast<uint32_t>(target) << 6) | 
           (static_cast<uint32_t>(piece) << 12) | 
           (static_cast<uint32_t>(promoted) << 16) |
           (static_cast<uint32_t>(capture) << 20) | 
           (static_cast<uint32_t>(double_push) << 21) | 
           (static_cast<uint32_t>(enpassant) << 22) | 
           (static_cast<uint32_t>(castling) << 23);
}

constexpr Square decode_move_source(Move move) {
    return Square(move & 0x3F);
}
constexpr Square decode_move_target(Move move) {
    return Square((move >> 6) & 0x3F);
}
constexpr Piece decode_move_piece(Move move) {
    return Piece((move >> 12) & 0xF);
}
constexpr PieceType decode_move_promoted(Move move) {
    return PieceType((move >> 16) & 0xF);
}
constexpr bool decode_move_capture(Move move) {
    return (move >> 20) & 1;
}
constexpr bool decode_move_double_push(Move move) {
    return (move >> 21) & 1;
}
constexpr bool decode_move_enpassant(Move move) {
    return (move >> 22) & 1;
}
constexpr bool decode_move_castling(Move move) {
    return (move >> 23) & 1;
} */

enum GenType {
    CAPTURES,
    QUIETS,
    QUIET_CHECKS,
    EVASIONS,
    NON_EVASIONS,
    LEGAL
};

struct ExtMove: public Move {
    int value;

    void operator=(Move m) { data = m.raw(); }

    // Inhibit unwanted implicit conversions to Move
    // with an ambiguity that yields to a compile error.
    operator float() const = delete;
};

inline bool operator<(const ExtMove& f, const ExtMove& s) { return f.value < s.value; }

template<GenType>
ExtMove* generate(const Board& board, ExtMove* moveList);

// The MoveList struct wraps the generate() function and returns a convenient
// list of moves. Using MoveList is sometimes preferable to directly calling
// the lower level generate() function.
template<GenType T>
struct MoveList {

    explicit MoveList(const Board& board) :
        last(generate<T>(board, moveList)) {}
    const ExtMove* begin() const { return moveList; }
    const ExtMove* end() const { return last; }
    size_t         size() const { return last - moveList; }
    bool           contains(Move move) const { return std::find(begin(), end(), move) != end(); }

   private:
    ExtMove moveList[MAX_MOVES], *last;
};

/* struct MoveList {
    std::array<Move, 256> moves = {};
    uint8_t count = 0;
};

void print_move_list(MoveList &move_list);


constexpr void add_move(MoveList &move_list, Move move){
    move_list.moves[move_list.count] = move;
    move_list.count++;
}
inline bool is_square_available(Board &board, Square s){
    return !(board.occupancies[BOTH] & s);
}

inline bool two_squares_available(Board &board, Square s1, Square s2){
    return !((board.occupancies[BOTH] & s1) && (board.occupancies[BOTH] & s2));
}
inline bool three_squares_available(Board &board, Square s1, Square s2, Square s3){
    return !((board.occupancies[BOTH] & s1) && (board.occupancies[BOTH] & s2) && (board.occupancies[BOTH] & s3));
}

MoveList generate_moves(Board &board);


constexpr bool _is_letter(const char fen){
    return (fen >= 'a' && fen <= 'z') || (fen >= 'A' && fen <= 'Z');
}

constexpr bool _is_number(const char fen){
    return (fen >= '0' && fen <= '8');
}

// FEN dedug positions
static const char* const empty_board = "8/8/8/8/8/8/8/8 w - - ";
static const char* const start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
static const char* const tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
static const char* const killer_position = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
static const char* const cmk_position = "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 ";

void parse_fen(Board &board, const char *fen);

enum MoveFlag : bool{
    ALL_MOVES = 0,
    CAPTURE_MOVES = 1,
};

bool make_move(Board &board, Move move, MoveFlag move_flag);

void print_move(Move move); */

Move parse_move(Board &board, const char* move_string);

void parse_position(Board &board, const char *command);

#endif // MOVES_H