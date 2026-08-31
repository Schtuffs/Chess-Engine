#pragma once

#include <sstream>
#include <string>
#include <string_view>

#include "Types/ShortTypes.h"

// clang-format off
enum Square : u8 {
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_NONE  = 0,
    SQ_BAD   = 65,
    SQ_TOTAL = 64

};

// clang-format off
constexpr Square& operator++(Square& sq)        { return (sq = (Square)((int)(sq) + 1)); }
constexpr Square& operator--(Square& sq)        { return (sq = (Square)((int)(sq) - 1)); }
constexpr Square  operator++(Square& sq, int)   { Square tmp = sq; ++sq; return tmp; }
constexpr Square  operator--(Square& sq, int)   { Square tmp = sq; ++sq; return tmp; }
constexpr Square  operator +(Square  sq, int i) { return (Square)((int)sq + i); }
constexpr void    operator+=(Square& sq, int i) { sq = (Square)((int)sq + i); }
constexpr Square  operator /(Square  sq, int i) { return (Square)((int)sq / i); }
constexpr Square  operator %(Square  sq, int i) { return (Square)((int)sq % i); }
// clang-format on

enum Colour : u8 { WHITE, BLACK, COLOUR_TOTAL = 2 };
constexpr Colour operator~(const Colour& col) { return (Colour)(col ^ BLACK); }

enum Direction : i8 {
    NORTH = 8,
    SOUTH = -8,
    EAST  = 1,
    WEST  = -1,

    NORTH_EAST = NORTH + EAST,
    NORTH_WEST = NORTH + WEST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
};

constexpr Direction operator+(Direction lhs, Direction rhs) { return Direction(i8(lhs) + i8(rhs)); }

constexpr Square operator+(Square sq, Direction dir) { return (Square)((i8)sq + (i8)dir); }
constexpr Square operator-(Square sq, Direction dir) { return (Square)((i8)sq - (i8)dir); }

enum PieceType : u8 { TYPE_NONE = 0, KNIGHT, BISHOP, ROOK, QUEEN, PAWN, KING, TYPE_TOTAL = 8 };

enum Piece : u8 {
    PIECE_NONE   = 0,
    WHITE_KNIGHT = KNIGHT,
    WHITE_BISHOP,
    WHITE_ROOK,
    WHITE_QUEEN,
    WHITE_PAWN,
    WHITE_KING,
    BLACK_KNIGHT = KNIGHT + 8,
    BLACK_BISHOP,
    BLACK_ROOK,
    BLACK_QUEEN,
    BLACK_PAWN,
    BLACK_KING,
    PIECE_TOTAL = 16
};
// clang-format on

class Move {
public:
    constexpr Move(u16 data = 0) : m_data(data) {}
    constexpr Move(const Move& move) : m_data(move.m_data) {}
    constexpr Move(const Move&& move) : m_data(move.m_data) {}
    static constexpr Move Make(Square from, Square to)
    {
        return Move((from << SHIFT_FROM) | (to << SHIFT_TO));
    }
    static constexpr Move MakePromo(Square from, Square to, PieceType type)
    {
        return Move((from << SHIFT_FROM) | (to << SHIFT_TO) | (((type - 1) & 0x3) << SHIFT_PROMO) |
                    MASK_PROMO);
    }
    static constexpr Move MakeCastle(Square from, Square to)
    {
        return Move((from << SHIFT_FROM) | (to << SHIFT_TO) | MASK_CASTLE);
    }
    static constexpr Move MakeEnPassant(Square from, Square to)
    {
        return Move((from << SHIFT_FROM) | (to << SHIFT_TO) | MASK_PASSANT);
    }

    // clang-format off
    constexpr bool IsValid() const noexcept { return m_data; }
    constexpr Square    To() const noexcept { return (Square)((m_data >> SHIFT_TO)   & MASK_POSITION); }
    constexpr Square  From() const noexcept { return (Square)((m_data >> SHIFT_FROM) & MASK_POSITION); }
    
    constexpr bool       IsCastle() const noexcept { return (m_data & MASK_FLAGS) == MASK_CASTLE; }
    constexpr bool    IsEnPassant() const noexcept { return (m_data & MASK_FLAGS) == MASK_PASSANT; }
    constexpr bool        IsPromo() const noexcept { return (m_data & MASK_PROMO); }
    constexpr PieceType Promotion() const noexcept { return (PieceType)(((m_data >> SHIFT_PROMO) & 0x3) + 1); }
    // clang-format on

    std::string Str() const noexcept;

    constexpr void operator=(Move other) { m_data = other.m_data; }

private:
    static constexpr u8 SHIFT_FROM  = 6;
    static constexpr u8 SHIFT_TO    = 0;
    static constexpr u8 SHIFT_PROMO = 13;

    static constexpr u16 MASK_POSITION = 0b00000000'00111111;
    static constexpr u16 MASK_PROMO    = 0b00010000'00000000;
    static constexpr u16 MASK_CASTLE   = 0b00100000'00000000;
    static constexpr u16 MASK_PASSANT  = 0b01000000'00000000;
    static constexpr u16 MASK_FLAGS    = 0b01110000'00000000;

    // Bit counts:
    // 0-5:   To
    // 6-11:  From
    // 12:    Promotion
    // 12-14: Promotion type
    // 13:    Castle
    // 14:    En Passant
    // 15:    Unused
    u16 m_data;
};

inline std::string Move::Str() const noexcept
{
    std::stringstream str;
    str << "From: " << (i32)From();
    str << ", To: " << (i32)To();
    str << ", P: " << IsPromo();
    str << ", C: " << IsCastle();
    str << ", E: " << IsEnPassant();
    return str.str();
}
