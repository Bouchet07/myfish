#ifndef ATTACKS_H
#define ATTACKS_H

#include "types.h"
#include "bitboard.h"

// Pawn attack table [color][square]
extern U64 pawn_attacks[2][64];

// Knight attack table [square]
extern U64 knight_attacks[64];

// King attack table [square]
extern U64 king_attacks[64];

// Returns the mask of all pawn attacks for a given color and square
U64 mask_pawn_attacks(int color, int square);

// Returns the mask of all knights attacks for a given square
U64 mask_knight_attacks(int square);

// Returns the mask of all king attacks for a given square
U64 mask_king_attacks(int square);

// Initializes the leapers (pawns, king, knights) attack table
void init_leapers_attacks();

// Returns the mask of all bishop attacks for a given square (Relevant)
U64 mask_bishop_attacks(int square);

// Returns the mask of all rook possible attacks for a given square (Relevant)
U64 mask_rook_attacks(int square);

// Returns the mask of all bishop possible attacks for a given square and block
U64 bishop_attacks_on_the_fly(int square, U64 block);

// Returns the mask of all rook possible attacks for a given square and block
U64 rook_attacks_on_the_fly(int square, U64 block);

/* extern U64 bishop_masks[64];
extern U64 rook_masks[64];
extern U64 bishop_attacks[64][512];
extern U64 rook_attacks[64][4096]; */

// Relevant occupancy bit count
constexpr uint8_t bishop_relevant_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

// Relevant occupancy bit count
constexpr uint8_t rook_relevant_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

/** 
    \param[in] index possible combination of occupancy
    \param[in] bits_in_mask number of relevant bits
    \param[in] attacks_mask mask of attacks (Relevant)

    \returns  Returns for a given index, the occupancy bitboard
*/
U64 set_occupancy(uint16_t index, uint8_t bits_in_mask, U64 attack_mask);

// function will guess by relevant bits if is bishop or rook
U64 find_magic_number(uint16_t square, uint8_t relevant_bits);

constexpr U64 bishop_magic_numbers[64] = {
    0x40040822862081,
    0x40810a4108000,
    0x2008008400920040,
    0x61050104000008,
    0x8282021010016100,
    0x41008210400a0001,
    0x3004202104050c0,
    0x22010108410402,
    0x60400862888605,
    0x6311401040228,
    0x80801082000,
    0x802a082080240100,
    0x1860061210016800,
    0x401016010a810,
    0x1000060545201005,
    0x21000c2098280819,
    0x2020004242020200,
    0x4102100490040101,
    0x114012208001500,
    0x108000682004460,
    0x7809000490401000,
    0x420b001601052912,
    0x408c8206100300,
    0x2231001041180110,
    0x8010102008a02100,
    0x204201004080084,
    0x410500058008811,
    0x480a040008010820,
    0x2194082044002002,
    0x2008a20001004200,
    0x40908041041004,
    0x881002200540404,
    0x4001082002082101,
    0x8110408880880,
    0x8000404040080200,
    0x200020082180080,
    0x1184440400114100,
    0xc220008020110412,
    0x4088084040090100,
    0x8822104100121080,
    0x100111884008200a,
    0x2844040288820200,
    0x90901088003010,
    0x1000a218000400,
    0x1102010420204,
    0x8414a3483000200,
    0x6410849901420400,
    0x201080200901040,
    0x204880808050002,
    0x1001008201210000,
    0x16a6300a890040a,
    0x8049000441108600,
    0x2212002060410044,
    0x100086308020020,
    0x484241408020421,
    0x105084028429c085,
    0x4282480801080c,
    0x81c098488088240,
    0x1400000090480820,
    0x4444000030208810,
    0x1020142010820200,
    0x2234802004018200,
    0xc2040450820a00,
    0x2101021090020
};

constexpr U64 rook_magic_numbers[64] = {
    0x8a80104000800020,
    0x140002000100040,
    0x2801880a0017001,
    0x100081001000420,
    0x200020010080420,
    0x3001c0002010008,
    0x8480008002000100,
    0x2080088004402900,
    0x800098204000,
    0x2024401000200040,
    0x100802000801000,
    0x120800800801000,
    0x208808088000400,
    0x2802200800400,
    0x2200800100020080,
    0x801000060821100,
    0x80044006422000,
    0x100808020004000,
    0x12108a0010204200,
    0x140848010000802,
    0x481828014002800,
    0x8094004002004100,
    0x4010040010010802,
    0x20008806104,
    0x100400080208000,
    0x2040002120081000,
    0x21200680100081,
    0x20100080080080,
    0x2000a00200410,
    0x20080800400,
    0x80088400100102,
    0x80004600042881,
    0x4040008040800020,
    0x440003000200801,
    0x4200011004500,
    0x188020010100100,
    0x14800401802800,
    0x2080040080800200,
    0x124080204001001,
    0x200046502000484,
    0x480400080088020,
    0x1000422010034000,
    0x30200100110040,
    0x100021010009,
    0x2002080100110004,
    0x202008004008002,
    0x20020004010100,
    0x2048440040820001,
    0x101002200408200,
    0x40802000401080,
    0x4008142004410100,
    0x2060820c0120200,
    0x1001004080100,
    0x20c020080040080,
    0x2935610830022400,
    0x44440041009200,
    0x280001040802101,
    0x2100190040002085,
    0x80c0084100102001,
    0x4024081001000421,
    0x20030a0244872,
    0x12001008414402,
    0x2006104900a0804,
    0x1004081002402
};

void init_sliders_attacks(uint8_t bishop);

U64 get_bishop_attacks(uint8_t square, U64 occupancy);

U64 get_rook_attacks(uint8_t square, U64 occupancy);

U64 get_queen_attacks(uint8_t square, U64 occupancy);

#endif // ATTACKS_H