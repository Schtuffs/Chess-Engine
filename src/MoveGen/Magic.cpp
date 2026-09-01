#include "MoveGen/Magic.h"

#include <array>
#include <bit>
#include <cmath>
#include <print>

// ----- Preparation -----

typedef struct MagicStruct {
    u64 mask;
    u64 magic;
    u64 shift;
    u64 offset;
} MagicStruct;

static constexpr u64 GenMask(i32 sq, bool isRook)
{
    u64 mask = 0ull;
    i32 rank = sq / 8;
    i32 file = sq % 8;

    // clang-format off
    if (isRook) {
        for (i32 i = rank + 1; i < 7; i++) { mask |= (1ull << (i * 8 + file)); }
        for (i32 i = rank - 1; i > 0; i--) { mask |= (1ull << (i * 8 + file)); }
        for (i32 i = file + 1; i < 7; i++) { mask |= (1ull << (rank * 8 + i)); }
        for (i32 i = file - 1; i > 0; i--) { mask |= (1ull << (rank * 8 + i)); }
    }
    else {
        for (i32 i = rank + 1, j = file + 1; i < 7 && j < 7; i++, j++) { mask |= (1ull << (i * 8 + j)); }
        for (i32 i = rank + 1, j = file - 1; i < 7 && j > 0; i++, j--) { mask |= (1ull << (i * 8 + j)); }
        for (i32 i = rank - 1, j = file + 1; i > 0 && j < 7; i--, j++) { mask |= (1ull << (i * 8 + j)); }
        for (i32 i = rank - 1, j = file - 1; i > 0 && j > 0; i--, j--) { mask |= (1ull << (i * 8 + j)); }
    }
    // clang-format on

    return mask;
}

static constexpr u64 SetBlockers(i32 sq, i32 maskBits, u64 mask)
{
    u64 blockers = 0;

    for (i32 i = 0; i < maskBits; i++) {
        i32 square = std::countr_zero(mask);
        mask &= mask - 1;

        if (sq & (1ull << i)) {
            blockers |= (1ull << square);
        }
    }

    return blockers;
}

static constexpr u64 CalculateAttacks(Square sq, BitBoard pieces, bool isRook)
{
    BitBoard attacks;
    i32      rank = (i32)sq / 8;
    i32      file = (i32)sq % 8;

    i32 dRank[4] = {0};
    i32 dFile[4] = {0};

    // clang-format off
    // Prepare offsets
    if (isRook) {
        dRank[0] = -1; dFile[0] =  0; // North
        dRank[1] =  1; dFile[1] =  0; // South
        dRank[2] =  0; dFile[2] = -1; // West
        dRank[3] =  0; dFile[3] =  1; // East
    } else {
        dRank[0] = -1; dFile[0] = -1; // North-West
        dRank[1] = -1; dFile[1] =  1; // North-East
        dRank[2] =  1; dFile[2] = -1; // South-West
        dRank[3] =  1; dFile[3] =  1; // South-East
    }
    // clang-format on

    // Looping through until a piece is found
    for (i32 i = 0; i < 4; i++) {
        i32 targetRank = rank + dRank[i];
        i32 targetFile = file + dFile[i];

        while (targetRank >= 0 && targetRank < 8 && targetFile >= 0 && targetFile < 8) {
            Square targetIndex = Square(targetRank * 8 + targetFile);
            attacks |= targetIndex;

            if (pieces & targetIndex) {
                break;
            }

            targetRank += dRank[i];
            targetFile += dFile[i];
        }
    }

    return attacks.raw();
}

// clang-format off

constexpr std::array<u64, 64> KING_MOVES = {
    0x00'00'00'00'00'00'03'03, 0x00'00'00'00'00'00'07'07, 0x00'00'00'00'00'00'0e'0e, 0x00'00'00'00'00'00'1c'1c,
    0x00'00'00'00'00'00'38'38, 0x00'00'00'00'00'00'70'70, 0x00'00'00'00'00'00'e0'e0, 0x00'00'00'00'00'00'c0'c0,

    0x00'00'00'00'00'03'03'03, 0x00'00'00'00'00'07'07'07, 0x00'00'00'00'00'0e'0e'0e, 0x00'00'00'00'00'1c'1c'1c,
    0x00'00'00'00'00'38'38'38, 0x00'00'00'00'00'70'70'70, 0x00'00'00'00'00'e0'e0'e0, 0x00'00'00'00'00'c0'c0'c0,

    0x00'00'00'00'03'03'03'00, 0x00'00'00'00'07'07'07'00, 0x00'00'00'00'0e'0e'0e'00, 0x00'00'00'00'1c'1c'1c'00,
    0x00'00'00'00'38'38'38'00, 0x00'00'00'00'70'70'70'00, 0x00'00'00'00'e0'e0'e0'00, 0x00'00'00'00'c0'c0'c0'00,

    0x00'00'00'03'03'03'00'00, 0x00'00'00'07'07'07'00'00, 0x00'00'00'0e'0e'0e'00'00, 0x00'00'00'1c'1c'1c'00'00,
    0x00'00'00'38'38'38'00'00, 0x00'00'00'70'70'70'00'00, 0x00'00'00'e0'e0'e0'00'00, 0x00'00'00'c0'c0'c0'00'00,

    0x00'00'03'03'03'00'00'00, 0x00'00'07'07'07'00'00'00, 0x00'00'0e'0e'0e'00'00'00, 0x00'00'1c'1c'1c'00'00'00,
    0x00'00'38'38'38'00'00'00, 0x00'00'70'70'70'00'00'00, 0x00'00'e0'e0'e0'00'00'00, 0x00'00'c0'c0'c0'00'00'00,

    0x00'03'03'03'00'00'00'00, 0x00'07'07'07'00'00'00'00, 0x00'0e'0e'0e'00'00'00'00, 0x00'1c'1c'1c'00'00'00'00,
    0x00'38'38'38'00'00'00'00, 0x00'70'70'70'00'00'00'00, 0x00'e0'e0'e0'00'00'00'00, 0x00'c0'c0'c0'00'00'00'00,

    0x03'03'03'00'00'00'00'00, 0x07'07'07'00'00'00'00'00, 0x0e'0e'0e'00'00'00'00'00, 0x1c'1c'1c'00'00'00'00'00,
    0x38'38'38'00'00'00'00'00, 0x70'70'70'00'00'00'00'00, 0xe0'e0'e0'00'00'00'00'00, 0xc0'c0'c0'00'00'00'00'00,

    0x03'03'00'00'00'00'00'00, 0x07'07'00'00'00'00'00'00, 0x0e'0e'00'00'00'00'00'00, 0x1c'1c'00'00'00'00'00'00,
    0x38'38'00'00'00'00'00'00, 0x70'70'00'00'00'00'00'00, 0xe0'e0'00'00'00'00'00'00, 0xc0'c0'00'00'00'00'00'00,
};

constexpr std::array<u64, 64> KNIGHT_MOVES = {
    0x00'00'00'00'00'02'04'00, 0x00'00'00'00'00'05'08'00, 0x00'00'00'00'00'0a'11'00, 0x00'00'00'00'00'14'22'00,
    0x00'00'00'00'00'28'44'00, 0x00'00'00'00'00'50'88'00, 0x00'00'00'00'00'a0'10'00, 0x00'00'00'00'00'40'20'00,

    0x00'00'00'00'02'04'00'04, 0x00'00'00'00'05'08'00'08, 0x00'00'00'00'0a'11'00'11, 0x00'00'00'00'14'22'00'22,
    0x00'00'00'00'28'44'00'44, 0x00'00'00'00'50'88'00'88, 0x00'00'00'00'a0'10'00'10, 0x00'00'00'00'40'20'00'20,

    0x00'00'00'02'04'00'04'02, 0x00'00'00'05'08'00'08'05, 0x00'00'00'0a'11'00'11'0a, 0x00'00'00'14'22'00'22'14,
    0x00'00'00'28'44'00'44'28, 0x00'00'00'50'88'00'88'50, 0x00'00'00'a0'10'00'10'a0, 0x00'00'00'40'20'00'20'40,

    0x00'00'02'04'00'04'02'00, 0x00'00'05'08'00'08'05'00, 0x00'00'0a'11'00'11'0a'00, 0x00'00'14'22'00'22'14'00,
    0x00'00'28'44'00'44'28'00, 0x00'00'50'88'00'88'50'00, 0x00'00'a0'10'00'10'a0'00, 0x00'00'40'20'00'20'40'00,

    0x00'02'04'00'04'02'00'00, 0x00'05'08'00'08'05'00'00, 0x00'0a'11'00'11'0a'00'00, 0x00'14'22'00'22'14'00'00,
    0x00'28'44'00'44'28'00'00, 0x00'50'88'00'88'50'00'00, 0x00'a0'10'00'10'a0'00'00, 0x00'40'20'00'20'40'00'00,

    0x02'04'00'04'02'00'00'00, 0x05'08'00'08'05'00'00'00, 0x0a'11'00'11'0a'00'00'00, 0x14'22'00'22'14'00'00'00,
    0x28'44'00'44'28'00'00'00, 0x50'88'00'88'50'00'00'00, 0xa0'10'00'10'a0'00'00'00, 0x40'20'00'20'40'00'00'00,

    0x04'00'04'02'00'00'00'00, 0x08'00'08'05'00'00'00'00, 0x11'00'11'0a'00'00'00'00, 0x22'00'22'14'00'00'00'00,
    0x44'00'44'28'00'00'00'00, 0x88'00'88'50'00'00'00'00, 0x10'00'10'a0'00'00'00'00, 0x20'00'20'40'00'00'00'00,

    0x00'04'02'00'00'00'00'00, 0x00'08'05'00'00'00'00'00, 0x00'11'0a'00'00'00'00'00, 0x00'22'14'00'00'00'00'00,
    0x00'44'28'00'00'00'00'00, 0x00'88'50'00'00'00'00'00, 0x00'10'a0'00'00'00'00'00, 0x00'20'40'00'00'00'00'00,
};

constexpr std::array<std::array<BitBoard, 64>, 2> PAWN_MOVES = {{
    // White
    {{
        0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull,
        0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull,

        0x00'00'00'00'00'02'01'00ull, 0x00'00'00'00'00'05'02'00ull, 0x00'00'00'00'00'0a'04'00ull, 0x00'00'00'00'00'14'08'00ull,
        0x00'00'00'00'00'28'10'00ull, 0x00'00'00'00'00'50'20'00ull, 0x00'00'00'00'00'a0'40'00ull, 0x00'00'00'00'00'40'80'00ull,

        0x00'00'00'00'02'01'00'00ull, 0x00'00'00'00'05'02'00'00ull, 0x00'00'00'00'0a'04'00'00ull, 0x00'00'00'00'14'08'00'00ull,
        0x00'00'00'00'28'10'00'00ull, 0x00'00'00'00'50'20'00'00ull, 0x00'00'00'00'a0'40'00'00ull, 0x00'00'00'00'40'80'00'00ull,

        0x00'00'00'02'01'00'00'00ull, 0x00'00'00'05'02'00'00'00ull, 0x00'00'00'0a'04'00'00'00ull, 0x00'00'00'14'08'00'00'00ull,
        0x00'00'00'28'10'00'00'00ull, 0x00'00'00'50'20'00'00'00ull, 0x00'00'00'a0'40'00'00'00ull, 0x00'00'00'40'80'00'00'00ull,

        0x00'00'02'01'00'00'00'00ull, 0x00'00'05'02'00'00'00'00ull, 0x00'00'0a'04'00'00'00'00ull, 0x00'00'14'08'00'00'00'00ull,
        0x00'00'28'10'00'00'00'00ull, 0x00'00'50'20'00'00'00'00ull, 0x00'00'a0'40'00'00'00'00ull, 0x00'00'40'80'00'00'00'00ull,

        0x00'02'01'00'00'00'00'00ull, 0x00'05'02'00'00'00'00'00ull, 0x00'0a'04'00'00'00'00'00ull, 0x00'14'08'00'00'00'00'00ull,
        0x00'28'10'00'00'00'00'00ull, 0x00'50'20'00'00'00'00'00ull, 0x00'a0'40'00'00'00'00'00ull, 0x00'40'80'00'00'00'00'00ull,

        0x02'01'00'00'00'00'00'00ull, 0x05'02'00'00'00'00'00'00ull, 0x0a'04'00'00'00'00'00'00ull, 0x14'08'00'00'00'00'00'00ull,
        0x28'10'00'00'00'00'00'00ull, 0x50'20'00'00'00'00'00'00ull, 0xa0'40'00'00'00'00'00'00ull, 0x40'80'00'00'00'00'00'00ull,

        0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull,
        0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull,
    }},

    // Black
    {{
        0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull,
        0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull,

        0x00'00'00'00'00'00'01'02ull, 0x00'00'00'00'00'00'02'05ull, 0x00'00'00'00'00'00'04'0aull, 0x00'00'00'00'00'00'08'14ull,
        0x00'00'00'00'00'00'10'28ull, 0x00'00'00'00'00'00'20'50ull, 0x00'00'00'00'00'00'40'a0ull, 0x00'00'00'00'00'00'80'40ull,

        0x00'00'00'00'00'01'02'00ull, 0x00'00'00'00'00'02'05'00ull, 0x00'00'00'00'00'04'0a'00ull, 0x00'00'00'00'00'08'14'00ull,
        0x00'00'00'00'00'10'28'00ull, 0x00'00'00'00'00'20'50'00ull, 0x00'00'00'00'00'40'a0'00ull, 0x00'00'00'00'00'80'40'00ull,

        0x00'00'00'00'01'02'00'00ull, 0x00'00'00'00'02'05'00'00ull, 0x00'00'00'00'04'0a'00'00ull, 0x00'00'00'00'08'14'00'00ull,
        0x00'00'00'00'10'28'00'00ull, 0x00'00'00'00'20'50'00'00ull, 0x00'00'00'00'40'a0'00'00ull, 0x00'00'00'00'80'40'00'00ull,

        0x00'00'00'01'02'00'00'00ull, 0x00'00'00'02'05'00'00'00ull, 0x00'00'00'04'0a'00'00'00ull, 0x00'00'00'08'14'00'00'00ull,
        0x00'00'00'10'28'00'00'00ull, 0x00'00'00'20'50'00'00'00ull, 0x00'00'00'40'a0'00'00'00ull, 0x00'00'00'80'40'00'00'00ull,

        0x00'00'01'02'00'00'00'00ull, 0x00'00'02'05'00'00'00'00ull, 0x00'00'04'0a'00'00'00'00ull, 0x00'00'08'14'00'00'00'00ull,
        0x00'00'10'28'00'00'00'00ull, 0x00'00'20'50'00'00'00'00ull, 0x00'00'40'a0'00'00'00'00ull, 0x00'00'80'40'00'00'00'00ull,

        0x00'01'02'00'00'00'00'00ull, 0x00'02'05'00'00'00'00'00ull, 0x00'04'0a'00'00'00'00'00ull, 0x00'08'14'00'00'00'00'00ull,
        0x00'10'28'00'00'00'00'00ull, 0x00'20'50'00'00'00'00'00ull, 0x00'40'a0'00'00'00'00'00ull, 0x00'80'40'00'00'00'00'00ull,

        0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull,
        0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull, 0x00'00'00'00'00'00'00'00ull,
    }}
}};

constexpr std::array<u64, 64> MAGIC_BISHOP_NUMBERS = {
    0x001024b002420160ull, 0x1008080140420021ull, 0x2012080041080024ull, 0x0c282601408c0802ull,
    0x2004042000000002ull, 0x0012021004022080ull, 0x0880414820100000ull, 0x4501002211044000ull,

    0x0020402222121600ull, 0x1081088a28022020ull, 0x01004c2810851064ull, 0x2040080841004918ull,
    0x1448020210201017ull, 0x4808110108400025ull, 0x0010504404054004ull, 0x0800010422092400ull,

    0x0040000870450250ull, 0x0402040408080518ull, 0x001000980a404108ull, 0x0001020804110080ull,
    0x0008200c02082005ull, 0x00040802009a0800ull, 0x0001000201012100ull, 0x0111080200820180ull,

    0x0904122104101024ull, 0x4008200405244084ull, 0x0044040002182400ull, 0x4804080004021002ull,
    0x6401004024004040ull, 0x0404010001300a20ull, 0x0428020200a20100ull, 0x0300460100420200ull,

    0x000404200c062000ull, 0x0022101400510141ull, 0x0104044400180031ull, 0x2040040400280211ull,
    0x0008020400401010ull, 0x20100110401a0040ull, 0x00100101005a2080ull, 0x001a008300042411ull,

    0x120a025004504000ull, 0x4001084242101000ull, 0x0a020202010a4200ull, 0x4000002018000100ull,
    0x0000080104000044ull, 0x1004009806004043ull, 0x100401080a000112ull, 0x1041012101000608ull,

    0x040400c250100140ull, 0x080a10460a100002ull, 0x2210030401240002ull, 0x06040aa108481b20ull,
    0x4009004050410002ull, 0x08106003420200e0ull, 0x1410500a08206000ull, 0x0092548802004000ull,

    0x0001040041241028ull, 0x0000120042025011ull, 0x0008060104054400ull, 0x20004404020a0a01ull,
    0x0040008010020214ull, 0x04000050209802c1ull, 0x0000208244210400ull, 0x0010140848044010ull,
};

constexpr std::array<u64, 64> MAGIC_ROOK_NUMBERS = {
    0x0880081080c00020ull, 0x210020c000308100ull, 0x0080082001100280ull, 0x01001000a0050108ull,
    0x0200041029600a00ull, 0x5100010008220400ull, 0x8280120001000d80ull, 0x1880012100014080ull,

    0x3040800340008020ull, 0x0400400050026003ull, 0x0021002000104902ull, 0x020900200a100100ull,
    0x000d800802840080ull, 0x0002808004000600ull, 0x0024001002110814ull, 0x2000800541000480ull,

    0x8000ee8002400080ull, 0x0024c04010002005ull, 0x822002401000c800ull, 0x2040808010000800ull,
    0x804080800c000802ull, 0x02a0080110402004ull, 0x201044000810010aull, 0x4080020004004483ull,

    0x4d84400180228000ull, 0x1406400880200880ull, 0x0000801200402203ull, 0x1080080280100084ull,
    0x0402140080080080ull, 0x0a880c0080020080ull, 0x0342000200080405ull, 0x20004a8200050044ull,

    0x8280c00020800889ull, 0x8002201000400940ull, 0x044a200101001542ull, 0x0088090021005000ull,
    0x3008004200c00400ull, 0x0284120080800400ull, 0x4462106804000201ull, 0x1008240382000061ull,

    0x0080400080208002ull, 0x0020100040004020ull, 0x4000802042020010ull, 0x040a002042120008ull,
    0x012a008820120004ull, 0x0006000408020010ull, 0x0002008405020008ull, 0x80100c0040820003ull,

    0x0002800100446100ull, 0x00a0982002400080ull, 0x09a0080010014040ull, 0x380c209200420a00ull,
    0x0c04008108000580ull, 0xc002008004002280ull, 0x002900842a000100ull, 0x040100008a004300ull,

    0x00010211800020c3ull, 0x0000a08412050242ull, 0x2001004010200489ull, 0x0a00081000210045ull,
    0x4512002810204402ull, 0x8c22000401102802ull, 0x0485000082005401ull, 0x00000100208400ceull,
};

constexpr std::array<u64, 64> MAGIC_BISHOP_BITS = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6,
};

constexpr std::array<u64, 64> MAGIC_ROOK_BITS = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12,
};

// clang-format on

typedef struct MagicTable {
    MagicStruct bishopMagic[64];
    MagicStruct rookMagic[64];

    BitBoard bishopAttacks[5248];
    BitBoard rookAttacks[102400];
} MagicTable;

typedef struct KingTable {
    BitBoard bishop[896]; // Max is 14 when in center, 64 * 14
    BitBoard rook[896];   // 64 * 14 (its always 14 for rooks)
} KingTable;

consteval MagicTable CreateMagicTable()
{
    MagicTable table;
    u32        curBishopOffset = 0;
    u32        curRookOffset   = 0;

    for (Square sq = SQ_A1; sq < 64; ++sq) {
        // Rooks
        table.rookMagic[sq].mask   = GenMask(sq, true);
        table.rookMagic[sq].magic  = MAGIC_ROOK_NUMBERS[sq];
        table.rookMagic[sq].shift  = 64 - MAGIC_ROOK_BITS[sq];
        table.rookMagic[sq].offset = curRookOffset;

        i32 rookVar = 1 << MAGIC_ROOK_BITS[sq];
        for (i32 i = 0; i < rookVar; i++) {
            u64 blockers = SetBlockers(i, MAGIC_ROOK_BITS[sq], table.rookMagic[sq].mask);
            u32 hash     = (blockers * table.rookMagic[sq].magic) >> table.rookMagic[sq].shift;
            table.rookAttacks[table.rookMagic[sq].offset + hash] =
                CalculateAttacks(sq, blockers, true);
        }
        curRookOffset += rookVar;

        // Bishops
        table.bishopMagic[sq].mask   = GenMask(sq, false);
        table.bishopMagic[sq].magic  = MAGIC_BISHOP_NUMBERS[sq];
        table.bishopMagic[sq].shift  = 64 - MAGIC_BISHOP_BITS[sq];
        table.bishopMagic[sq].offset = curBishopOffset;

        i32 bishopVar = 1 << MAGIC_BISHOP_BITS[sq];
        for (i32 i = 0; i < bishopVar; i++) {
            u64 blockers = SetBlockers(i, MAGIC_BISHOP_BITS[sq], table.bishopMagic[sq].mask);
            u32 hash     = (blockers * table.bishopMagic[sq].magic) >> table.bishopMagic[sq].shift;
            if (table.bishopAttacks[table.bishopMagic[sq].offset + hash].raw() != 0) {
            }
            table.bishopAttacks[table.bishopMagic[sq].offset + hash] =
                CalculateAttacks(sq, blockers, false);
        }
        curBishopOffset += bishopVar;
    }

    return table;
}

constexpr u64 INVALID_HASH = 0xff'ff'ff'ff'ff'ff'ff'ff;

// ----- Bishops -----

static constexpr bool ValidBishopIndex(Square bishop, Square king)
{
    i32 kr = king / 8;
    i32 kf = king % 8;

    i32 br = bishop / 8;
    i32 bf = bishop % 8;

    i32 rise = (std::max(kr, br) - std::min(kr, br));
    i32 run  = (std::max(kf, bf) - std::min(kf, bf));

    return (rise == run);
}

static constexpr u64 BishopTableHash(Square bishop, Square king)
{
    if (!ValidBishopIndex(bishop, king)) {
        return INVALID_HASH;
    }

    i32 kr = king / 8;
    i32 kc = king % 8;

    i32 br = bishop / 8;
    i32 bc = bishop % 8;

    i32 nw = std::min(kr, kc);
    i32 ne = std::min(kr, 7 - kc);
    i32 sw = std::min(7 - kr, kc);
    // i32 se = std::min(7 - kr, 7 - kc);

    u64 hash;

    // NW
    if (br < kr && bc < kc) {
        hash = kr - br - 1;
    }
    // NE
    else if (br < kr && bc > kc) {
        hash = nw + (kr - br - 1);
    }
    // SW
    else if (br > kr && bc < kc) {
        hash = nw + ne + (br - kr - 1);
    }
    // SE
    else {
        hash = nw + ne + sw + (br - kr - 1);
    }

    return (hash + (king * 14));
}

static constexpr BitBoard CalculateBishopAttacks(Square bishop, Square king)
{
    BitBoard bb;

    i32 kf = king % 8;
    i32 kr = king / 8;
    i32 bf = bishop % 8;
    i32 br = bishop / 8;

    i8 offset;
    if (bf < kf) {
        if (br < kr) {
            offset = 9;
        } else {
            offset = -7;
        }
    } else {
        if (br < kr) {
            offset = 7;
        } else {
            offset = -9;
        }
    }

    Square end = (Square)std::abs(kf - bf);
    for (Square i = SQ_A1; i < end; i++) {
        bb |= (Square)((offset * i) + bishop);
    }

    return bb;
}

// ----- Rooks -----

static constexpr bool ValidRookIndex(Square rook, Square king)
{
    // Vert
    Square pFile = rook % 8;
    Square kFile = king % 8;

    if (pFile == kFile) {
        return true;
    }

    // Horz
    Square pRank = rook / 8;
    Square kRank = king / 8;

    if (pRank == kRank) {
        return true;
    }

    return false;
}

static constexpr u64 RookTableHash(Square rook, Square king)
{
    if (!ValidRookIndex(rook, king)) {
        return INVALID_HASH;
    }

    i32 kr = king / 8;
    i32 kc = king % 8;
    i32 rr = rook / 8;
    i32 rc = rook % 8;

    i32 north = kr;
    i32 south = 7 - kr;
    i32 east  = 7 - kc;
    // i32 west  = kc;

    u64 hash;
    if (rc == kc) {    // Same file
        if (rr < kr) { // North
            hash = kr - rr - 1;
        } else { // South
            hash = north + (rr - kr - 1);
        }
    } else {           // Same rank
        if (rc > kc) { // East
            hash = north + south + (rc - kc - 1);
        } else { // West
            hash = north + south + east + (kc - rc - 1);
        }
    }
    return ((king * 14) + hash);
}

static constexpr BitBoard CalculateRookAttacks(Square rook, Square king)
{
    BitBoard bb;

    // Horizontal
    if ((rook / 8) == (king / 8)) {
        Square start = std::min(rook, king);
        Square end   = std::max(rook, king);

        for (Square i = start; i < end; i++) {
            bb |= i;
        }
    }
    // Vertical
    else if ((rook % 8) == (king % 8)) {
        Square start = std::min(rook, king);
        Square end   = std::max(rook, king);

        for (Square i = start; i < end; i += 8) {
            bb |= i;
        }
    }

    return bb;
}

// ----- Magic -----

consteval KingTable CreateKingTable()
{
    KingTable table{};

    for (Square sq = SQ_A1; sq < SQ_TOTAL; sq++) {
        for (Square king = SQ_A1; king < SQ_TOTAL; king++) {
            // No need for same square
            if (king == sq) {
                continue;
            }

            // Rook
            u64 hash = RookTableHash(sq, king);
            if (hash != INVALID_HASH) {
                table.rook[hash] = CalculateRookAttacks(sq, king);
            }

            // Bishop
            hash = BishopTableHash(sq, king);
            if (hash != INVALID_HASH) {
                table.bishop[hash] = CalculateBishopAttacks(sq, king);
            }
        }
    }

    return table;
}

inline constexpr MagicTable magics = CreateMagicTable();

inline constexpr KingTable kingAttacks = CreateKingTable();

// ----- Secrets -----

static constexpr BitBoard GetBishopAttacks(Square sq, BitBoard occupied)
{
    occupied &= magics.bishopMagic[sq].mask;
    u32 hash = (occupied * magics.bishopMagic[sq].magic) >> magics.bishopMagic[sq].shift;
    return magics.bishopAttacks[magics.bishopMagic[sq].offset + hash];
}

static constexpr BitBoard GetRookAttacks(Square sq, BitBoard occupied)
{
    occupied &= magics.rookMagic[sq].mask;
    u32 hash = (occupied * magics.rookMagic[sq].magic) >> magics.rookMagic[sq].shift;
    return magics.rookAttacks[magics.rookMagic[sq].offset + hash];
}

static constexpr BitBoard GetBishopKingAttacks(Square bishop, Square king)
{
    u64 hash = BishopTableHash(bishop, king);
    if (hash == INVALID_HASH) {
        return 0ull;
    }
    return kingAttacks.bishop[hash];
}

static constexpr BitBoard GetRookKingAttacks(Square rook, Square king)
{
    u64 hash = RookTableHash(rook, king);
    if (hash == INVALID_HASH) {
        return 0ull;
    }
    return kingAttacks.rook[hash];
}

// ----- Public Functions -----

template <PieceType type>
BitBoard Magic::GetAttacks(Square sq, BitBoard occupied, Colour player)
{
    if constexpr (type == BISHOP) {
        return GetBishopAttacks(sq, occupied);
    }
    if constexpr (type == KING) {
        return KING_MOVES[sq];
    }
    if constexpr (type == KNIGHT) {
        return KNIGHT_MOVES[sq];
    }
    if constexpr (type == PAWN) {
        return PAWN_MOVES[player][sq];
    }
    if constexpr (type == QUEEN) {
        return GetBishopAttacks(sq, occupied) | GetRookAttacks(sq, occupied);
    }
    if constexpr (type == ROOK) {
        return GetRookAttacks(sq, occupied);
    }
    return BitBoard(0ull);
}

BitBoard Magic::GetKingAttacks(Square sq, Square king, bool isRook)
{
    return (isRook ? GetRookKingAttacks(sq, king) : GetBishopKingAttacks(sq, king));
}

template BitBoard Magic::GetAttacks<BISHOP>(Square sq, BitBoard occupied, Colour player);
template BitBoard Magic::GetAttacks<KING>(Square sq, BitBoard occupied, Colour player);
template BitBoard Magic::GetAttacks<KNIGHT>(Square sq, BitBoard occupied, Colour player);
template BitBoard Magic::GetAttacks<PAWN>(Square sq, BitBoard occupied, Colour player);
template BitBoard Magic::GetAttacks<QUEEN>(Square sq, BitBoard occupied, Colour player);
template BitBoard Magic::GetAttacks<ROOK>(Square sq, BitBoard occupied, Colour player);
