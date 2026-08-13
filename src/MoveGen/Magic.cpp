#include "MoveGen/Magic.h"

#include <array>
#include <bit>
#include <cmath>
#include <print>

#include "Convert.h"
#include "Utils.h"

// #define SHUSH

// ----- Preparation -----

typedef struct MagicStruct {
    u64 mask;
    u64 magic;
    u64 shift;
    u64 offset;
} MagicStruct;

static constexpr u64 GenMask(i32 index, bool isRook)
{
    u64 mask = 0ull;
    i32 rank = index / 8;
    i32 file = index % 8;

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

static constexpr u64 SetBlockers(i32 index, i32 maskBits, u64 mask)
{
    u64 blockers = 0;

    for (i32 i = 0; i < maskBits; i++) {
        i32 square = std::countr_zero(mask);
        mask &= mask - 1;

        if (index & (1ull << i)) {
            blockers |= (1ull << square);
        }
    }

    return blockers;
}

static constexpr u64 CalculateAttacks(Index index, u64 pieces, bool isRook)
{
    u64 attacks = 0ull;
    i32 rank    = (i32)index / 8;
    i32 file    = (i32)index % 8;

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
            i32 targetIndex = targetRank * 8 + targetFile;
            attacks |= (1ull << targetIndex);

            if (pieces & (1ull << targetIndex)) {
                break;
            }

            targetRank += dRank[i];
            targetFile += dFile[i];
        }
    }

    return attacks;
}

// clang-format off

constexpr std::array<u64, 64> MAGIC_BISHOP_NUMBERS = {
    0xE51EBB94FBE45BFFull, 0xC7B9F567ED8FFE7Full, 0x19A8282157800224ull, 0x04D41401923C73BEull,
    0x480404A14244000Dull, 0x340E01FEA0C933AFull, 0xFFC3F989D57FE9ECull, 0xF7FF3FDD6EFBFFFFull,

    0xE7F974F4F9D9F7F5ull, 0x222161180311C580ull, 0x62003808704081A0ull, 0x70425C0408830360ull,
    0x66E5E110419250CDull, 0x8420220834154952ull, 0xEDE7F5ADF8FDFFFDull, 0x7F7EEA5F3D59BF5Eull,

    0x4478024050810631ull, 0x2A22030490025601ull, 0xD0E4044848002500ull, 0x518C08C801212289ull,
    0x1002004402111108ull, 0x5858102901009007ull, 0x2230A1C412051005ull, 0x142B042E41082700ull,

    0x882011AE8850A508ull, 0x274320C508181108ull, 0x080090048E040014ull, 0x508C00C01C0100B2ull,
    0x424300102B004000ull, 0x0490144022080230ull, 0x20C280B11C020814ull, 0x00020600C42C9212ull,

    0xF3BFC5C66B10122Full, 0xA25801B000C42434ull, 0x500144A208900400ull, 0x6080140400780120ull,
    0x124C0B40100C0100ull, 0x7048004100909018ull, 0xE8781EC402008A01ull, 0x4D221A0600807284ull,

    0xED4FF5C4EA9B2418ull, 0x6D9FE6F7B7EFDEB4ull, 0x433D610048044041ull, 0x00086A2018040300ull,
    0x214B14110C006200ull, 0x60D2241106001C0Aull, 0x2B7F1018F2EBFDCCull, 0x783438008E252100ull,

    0xEFBFFD71EEAD7FFFull, 0xD597FE7D435F79FFull, 0x9FFFF7FB3D9C7377ull, 0x8910144620981000ull,
    0xE40D01A0208A40F9ull, 0xA63240B31425025Cull, 0x9EFFEBF61DD769FBull, 0x877FCE36D752FA8Eull,

    0xEB5FFD776D5FDFBEull, 0xD7FDFFFEBCF2DEFFull, 0xD01801C04208900Full, 0x830639EF5720980Aull,
    0xC9C018202182C400ull, 0x893B2D4094880E8Cull, 0xFBBEFDF552EB5AE6ull, 0xFEFFFBFB7BDFDDFBull,
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

    for (Index sq = 0; sq < 64; sq++) {
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
            table.bishopAttacks[table.bishopMagic[sq].offset + hash] =
                CalculateAttacks(sq, blockers, false);
        }
        curBishopOffset += bishopVar;
    }

    return table;
}

constexpr u64 INVALID_HASH = 0xff'ff'ff'ff'ff'ff'ff'ff;

// ----- Bishops -----

static constexpr bool ValidBishopIndex(Index bishop, Index king)
{
    i32 kr = king / 8;
    i32 kf = king % 8;

    i32 br = bishop / 8;
    i32 bf = bishop % 8;

    i32 rise = (Utils::Max(kr, br) - Utils::Min(kr, br));
    i32 run  = (Utils::Max(kf, bf) - Utils::Min(kf, bf));

    return (rise == run);
}

static constexpr u64 BishopTableHash(Index bishop, Index king)
{
    if (!ValidBishopIndex(bishop, king)) {
        return INVALID_HASH;
    }

    i32 kr = king / 8;
    i32 kc = king % 8;

    i32 br = bishop / 8;
    i32 bc = bishop % 8;

    i32 nw = Utils::Min(kr, kc);
    i32 ne = Utils::Min(kr, 7 - kc);
    i32 sw = Utils::Min(7 - kr, kc);
    // i32 se = Utils::Min(7 - kr, 7 - kc);

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

static constexpr BitBoard CalculateBishopAttacks(Index bishop, Index king)
{
    BitBoard bb = 0;

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

    Index end = std::abs(kf - bf);
    for (Index i = 0; i < end; i++) {
        bb |= Convert::IndexToBitBoard((offset * i) + bishop);
    }

    return bb;
}

// ----- Rooks -----

static constexpr bool ValidRookIndex(Index rook, Index king)
{
    // Vert
    Index pFile = rook % 8;
    Index kFile = king % 8;

    if (pFile == kFile) {
        return true;
    }

    // Horz
    Index pRank = rook / 8;
    Index kRank = king / 8;

    if (pRank == kRank) {
        return true;
    }

    return false;
}

static constexpr u64 RookTableHash(Index rook, Index king)
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

static constexpr BitBoard CalculateRookAttacks(Index rook, Index king)
{
    BitBoard bb = 0;

    // Horizontal
    if ((rook / 8) == (king / 8)) {
        Index start = Utils::Min(rook, king);
        Index end   = Utils::Max(rook, king);

        for (Index i = start; i < end; i++) {
            bb |= Convert::IndexToBitBoard(i);
        }
    }
    // Vertical
    else if ((rook % 8) == (king % 8)) {
        Index start = Utils::Min(rook, king);
        Index end   = Utils::Max(rook, king);

        for (Index i = start; i < end; i += 8) {
            bb |= Convert::IndexToBitBoard(i);
        }
    }

    return bb;
}

// ----- Magic -----

constexpr KingTable CreateKingTable()
{
    KingTable table{};

    for (Index sq = 0; sq < 64; sq++) {
        for (Index king = 0; king < 64; king++) {
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

#ifndef SHUSH
inline constexpr MagicTable magics = CreateMagicTable();
#endif

inline constexpr KingTable kingAttacks = CreateKingTable();

// ----- Secrets -----

#ifndef SHUSH
static constexpr BitBoard GetBishopAttacks(Index index, BitBoard blockers)
{
    blockers &= magics.bishopMagic[index].mask;
    u32 hash = (blockers * magics.bishopMagic[index].magic) >> magics.bishopMagic[index].shift;
    return magics.bishopAttacks[magics.bishopMagic[index].offset + hash];
}

static_assert(GetBishopAttacks(54, 0x58'd8'00'00'00'00'ef'ff) == 0xa0'00'a0'10'08'04'02'00);

static constexpr BitBoard GetRookAttacks(Index index, BitBoard blockers)
{
    blockers &= magics.rookMagic[index].mask;
    u32 hash = (blockers * magics.rookMagic[index].magic) >> magics.rookMagic[index].shift;
    return magics.rookAttacks[magics.rookMagic[index].offset + hash];
}
#endif

static constexpr BitBoard GetBishopKingAttacks(Index bishop, Index king)
{
    u64 hash = BishopTableHash(bishop, king);
    if (hash == INVALID_HASH) {
        return 0;
    }
    return kingAttacks.bishop[hash];
}

static constexpr BitBoard GetRookKingAttacks(Index rook, Index king)
{
    u64 hash = RookTableHash(rook, king);
    if (hash == INVALID_HASH) {
        return 0;
    }
    return kingAttacks.rook[hash];
}

// ----- Public Functions -----

#ifndef SHUSH
BitBoard Magic::GetSlidingAttacks(Index index, BitBoard blockers, bool isRook)
{
    return (isRook ? GetRookAttacks(index, blockers) : GetBishopAttacks(index, blockers));
}
#else
BitBoard Magic::GetSlidingAttacks(Index index, BitBoard blockers, bool isRook)
{
    (void)index;
    (void)blockers;
    (void)isRook;
    return 0;
}
#endif

BitBoard Magic::GetKingAttacks(Index piece, Index king, bool isRook)
{
    return (isRook ? GetRookKingAttacks(piece, king) : GetBishopKingAttacks(piece, king));
}
