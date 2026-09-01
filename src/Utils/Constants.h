#pragma once

#include <string_view>

#include "Types/ShortTypes.h"

extern bool inDebugMode;

/**
 * @brief Helpful for holding 2 variables of the same type in 1.
 * @tparam T The desired datatype.
 * @date 2026-06-11
 */
template <typename T>
struct Vec2 {
    T x, y;
};

constexpr std::string_view VERSION_MAJOR = "2";
constexpr std::string_view VERSION_MINOR = "0";

// Resources path.
constexpr std::string_view PATH_RESOURCES = "../resources";

/**
 * @brief Resource path for piece textures.
 * @date 2026-06-08
 */
constexpr std::string_view PATH_PIECES = "../resources/pieces";

/**
 * @brief Holds global enums.
 * @namespace Enums
 * @date 2026-06-08
 */
namespace Enums {
/**
 * @brief Holds information for castling rights.
 * @enum Castling
 * @date 2026-07-03
 */
namespace Castling {
constexpr u8 Black_King  = 1; /**< Black castling kingside  */
constexpr u8 Black_Queen = 2; /**< Black castling queenside */
constexpr u8 White_King  = 4; /**< White castling kingside  */
constexpr u8 White_Queen = 8; /**< White castling queenside */
} // namespace Castling

/**
 * @brief Screen state information.
 * @enum Screen
 * @date 2026-06-08
 */
enum class Screen {
    Quit,         /**< Quitting the game.        */
    Menu,         /**< Main menu screen.         */
    NewGame,      /**< New game options screen.  */
    GameCreation, /**< Playing a game screen.    */
    Game,         /**< Playing a game screen.    */
    Multiplayer,  /**< Setup multiplayer screen. */
    Settings,     /**< Settings screen.          */
};

/**
 * @brief Convert an enum variable to a string via indexing with enum value.
 * @namespace ToString
 * @date 2026-06-08
 */
namespace ToString {
/**
 * @brief Convert `Colour` to string.
 * @date 2026-06-08
 */
constexpr const char* Colour[] = {
    "WHITE",
    "BLACK",
    "COLOUR_TOTAL",
};

/**
 * @brief Convert `PieceType` to string.
 * @date 2026-06-08
 */
constexpr const char* Type[] = {"TYPE_NONE", "KNIGHT", "BISHOP", "ROOK",      "QUEEN",
                                "PAWN",      "KING",   "N/A",    "TYPE_TOTAL"};

/**
 * @brief Convert `Utils::Setting` to string.
 * @date 2026-07-01
 */
// clang-format off
constexpr const char* Setting[] = {
    "GAME_LOAD",       "GAME_FEN",          "GAME_MOVES",
    "ENGINE_WHITE_AI", "ENGINE_WHITE_PATH", "ENGINE_BLACK_AI",  "ENGINE_BLACK_PATH",
    "BOARD_TILE_DARK", "BOARD_TILE_LIGHT",  "BOARD_TILE_PROMO", "BOARD_TILE_LEGAL",
    "TOTAL_SETTINGS"
};

// clang-format on
constexpr const char* LogLevel[] = {"INFO", "DEBUG", "WARNING", "ERROR", "PRINT"};
} // namespace ToString
} // namespace Enums
