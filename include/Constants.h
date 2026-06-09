#pragma once

#include <cstdint>

/**
 * @brief Resource path for piece textures.
 * @author Kyle Wagler
 * @date 2026-06-08
 */
constexpr const char* PATH_PIECES   = "../resources/pieces";

/**
 * @brief Holds different dimensions. To be improved later.
 * @namespace Dimensions
 * @author Kyle Wagler
 * @date 2026-06-08
 */
namespace Dimensions {
    /**
     * @brief Holds tile square size.
     * @author Kyle Wagler
     * @date 2026-06-08
     */
    extern uint64_t TILE_SIZE;

    /**
     * @brief Holds screen width.
     * @author Kyle Wagler
     * @date 2026-06-08
     */
    extern uint64_t SCREEN_WIDTH;

    /**
     * @brief Holds screen height.
     * @author Kyle Wagler
     * @date 2026-06-08
     */
    extern uint64_t SCREEN_HEIGHT;
}

/**
 * @brief Holds board information. Will be moved to class later.
 * @namespace Board
 * @author Kyle Wagler
 * @date 2026-06-08
 */
namespace Board {
    
}

/**
 * @brief Holds global enums.
 * @namespace Enums
 * @author Kyle Wagler
 * @date 2026-06-08
 */
namespace Enums {
    /**
     * @brief Holds information for the type of piece.
     * @enum Type
     * @author Kyle Wagler
     * @date 2026-06-08
     */
    enum Type {
        Bishop,
        King,
        Knight,
        Pawn,
        Queen,
        Rook,
    };

    /**
     * @brief Holds information for the colour of the piece.
     * @enum Colour
     * @author Kyle Wagler
     * @date 2026-06-08
     */
    enum Colour {
        Black,
        White,
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

