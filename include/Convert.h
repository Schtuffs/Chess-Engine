#pragma once

#include <cmath>
#include <string>
#include <string_view>

#include "Constants.h"
#include "Piece.h"

/**
 * @brief Conversion utility
 * @namespace Convert
 * @date 2026-07-03
 */
namespace Convert {

// Converts a letter to a piece type.
constexpr Enums::Type CharToType(char c)
{
    c = tolower(c);
    switch (c) {
    case 'b':
        return Enums::Type::Bishop;
    case 'k':
        return Enums::Type::King;
    case 'n':
        return Enums::Type::Knight;
    case 'p':
        return Enums::Type::Pawn;
    case 'q':
        return Enums::Type::Queen;
    case 'r':
        return Enums::Type::Rook;
    default:
        return Enums::Type::Invalid;
    }
}

/**
 * @brief Changes a `Bitboard` to an `Index`.
 * @param bb The `Bitboard` to convert.
 * @return The `Index`. Ex: 0x00'00'00'00'00'00'10'00 -> 12.
 * @date 2026-07-07
 */
constexpr Index BitboardToIndex(Bitboard bb) { return (Index)std::round(std::log2(bb)); }

/**
 * @brief Changes an `Index` to a `Bitboard`.
 * @param index The `Index` to convert.
 * @return The `Bitboard`. Ex: 12 -> 0x00'00'00'00'00'00'10'00.
 * @date 2026-07-07
 */
constexpr Bitboard IndexToBitboard(Index index) { return (Bitboard)1 << index; }

// Convert given castling move to actual move.
std::string_view CastleToMove(std::string_view move, Enums::Colour player);

/**
 * @brief Changes an `Index` to a string move.
 * @param index The `Index` to convert.
 * @return The move. Ex: 12 -> e2.
 * @date 2026-07-01
 */
std::string IndexToMove(Index index);

/**
 * @brief Changes a string move to an `Index`.
 * @param move The move to convert.
 * @return The `Index`. Ex: e2 -> 12.
 * @date 2026-07-01
 */
Index MoveToIndex(std::string_view move);

// Convert a fen to a printable string.
std::string FenToString(std::string_view fen);

/**
 * @brief Changes a `Bitboard` to a printable string.
 * @param bb The `Bitboard` to convert.
 * @param on  The char to use when 1 is detected.
 * @param off The char to use when 0 is detected.
 * @return The string representation.
 * @date 2026-07-07
 */
std::string BitboardToString(Bitboard bb, char on = 'X', char off = ' ');

/**
 * @brief Changes a `Piece` list to a printable string.
 * @param pieces The `Piece` list to convert. Must be 64 in size.
 * @return The string representation.
 * @date 2026-07-07
 */
std::string PiecesToString(const Piece* pieces);
} // namespace Convert
