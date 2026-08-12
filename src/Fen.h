#pragma once

#include <span>
#include <string>
#include <string_view>

#include "Constants.h"
#include "Piece.h"

/**
 * @brief Helpful utilities for dealing with `Fen` items.
 * @namespace Fen
 * @date 2026-07-01
 */
namespace Fen {
/**
 * @brief Checks if given fen is valid.
 * @param data The fen data to check.
 * @return `true` on valid fen.
 * @date 2026-07-01
 */
bool IsValidFen(const char* data);

/**
 * @brief Generates fen string from given data.
 * @param pieces The `Piece` list to generate board `Piece` locations.
 * @param player The current player, either 'w' or 'b'.
 * @param castling The castling rights.
 * @param enPassant The current en passant square.
 * @param halfMoves Total half moves in the game.
 * @param fullMoves Total full moves in the game.
 * @return The generated fen or an empty string on invalid data.
 * @date 2026-07-01
 */
std::string GenerateFen(std::span<const Piece, 64> pieces, char player, std::string_view castling,
                        std::string_view enPassant, u32 halfMoves, u32 fullMoves);
} // namespace Fen
