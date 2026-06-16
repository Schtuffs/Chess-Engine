#pragma once

/**
 * @brief Helpful for holding 2 variables of the same type in 1.
 * @tparam T The desired datatype.
 * @author Kyle Wagler
 * @date 2026-06-11
 */
template <typename T>
struct Vec2 {
    T x, y;
};

/**
 * @brief Default fen string.
 * @author Kyle Wagler
 * @date 2026-06-11
 */
constexpr const char* DEFAULT_FEN   = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/**
 * @brief Grid size in files and ranks.
 * @author Kyle Wagler
 * @date 2026-06-11
 */
constexpr int GRID_SIZE             = 8;

/**
 * @brief Resource path for piece textures.
 * @author Kyle Wagler
 * @date 2026-06-08
 */
constexpr const char* PATH_PIECES   = "../resources/pieces";

/**
 * @brief Holds global enums.
 * @namespace Enums
 * @author Kyle Wagler
 * @date 2026-06-08
 */
namespace Enums {
    /**
     * @brief Holds information for the colour of the piece.
     * @enum Colour
     * @author Kyle Wagler
     * @date 2026-06-08
     */
    enum Colour {
        Black,  /**< Piece colour black */
        White,  /**< Piece colour white */
    };
    
    /**
     * @brief Screen state information.
     * @enum Screen
     * @author Kyle Wagler
     * @date 2026-06-08
     */
    enum Screen {
        Menu,           /**< Main menu screen */
        NewGame,        /**< New game options screen */
        Multiplayer,    /**< Setup multiplayer screen */
        Game,           /**< Playing a game screen */
        Settings,       /**< Settings screen */
    };
    
    /**
     * @brief Holds information for the type of piece.
     * @enum Type
     * @author Kyle Wagler
     * @date 2026-06-08
     */
    enum Type {
        Bishop, /**< Bishop type */
        King,   /**< King type */
        Knight, /**< Knight type */
        Pawn,   /**< Pawn type */
        Queen,  /**< Queen type */
        Rook,   /**< Rook type */
    };
    
    /**
     * @brief Convert an enum variable to a string via indexing with enum value.
     * @namespace ToString
     * @author Kyle Wagler
     * @date 2026-06-08
     */
    namespace ToString {
        /**
         * @brief Convert `Enums::Type` to string.
         * @author Kyle Wagler
         * @date 2026-06-08
         */
        constexpr const char* Type[] = {
            "bishop", "king", "knight",
            "pawn", "queen", "rook",
        };
    
        /**
         * @brief Convert `Enums::Colour` to string.
         * @author Kyle Wagler
         * @date 2026-06-08
         */
        constexpr const char* Colour[] = {
            "black", "white",
        };
    }
}

