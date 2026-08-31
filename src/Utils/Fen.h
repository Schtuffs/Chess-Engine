#pragma once

#include <span>
#include <string>
#include <string_view>

/**
 * @brief Helpful utilities for dealing with `Fen` items.
 * @namespace Fen
 * @date 2026-07-01
 */
namespace Fen {
constexpr std::string_view DEFAULT = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/**
 * @brief Checks if given fen is valid.
 * @param data The fen data to check.
 * @return `true` on valid fen.
 * @date 2026-07-01
 */
bool IsValidFen(const char* data);
} // namespace Fen
