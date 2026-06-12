#pragma once

#include "raylib.h"

#include "Constants.h"

/**
 * @brief Utility functions.
 * @namespace Utils
 * @author Kyle Wagler
 * @date 2026-06-06
 */
namespace Utils {
    /**
     * @brief Calculate the minimum of 2 values of type `T`.
     * @param a The first value.
     * @param b The second value.
     * @return The smaller of the 2. Returns value `a` on equivalent.
     * @author Kyle Wagler
     * @date 2026-06-06
     */
    template <typename T>
    T min(T a, T b) {
        return (b < a) ? b : a;
    }
    
    /**
     * @brief Calculate the maximum of 2 values of type `T`.
     * @param a The first value.
     * @param b The second value.
     * @return The larger of the 2. Returns value `a` on equivalent.
     * @author Kyle Wagler
     * @date 2026-06-06
     */
    template <typename T>
    T max(T a, T b) {
        return (b > a) ? b : a;
    }
    
    /**
     * @brief Loads a `Texture2D` to the GPU.
     * @param `Enums::Colour` The `Piece` colour.
     * @param `Enums::Type` The `Piece` type.
     * @return The loaded `Texture2D`. Check with `IsTextureValid(Texture2D)`.
     * @author Kyle Wagler
     * @date 2026-06-06
     */
    Texture2D LoadTexture(Enums::Colour colour, Enums::Type type, int size);
    
    /**
     * @brief Unloads a `Texture2D` from the GPU.
     * @param `Texture2D` The texture to unload.
     * @param `Enums::Colour` The `Piece` colour.
     * @param `Enums::Type` The `Piece` type.
     * @author Kyle Wagler
     * @date 2026-06-06
     */
    void UnloadTexture(Texture2D& texture, Enums::Colour colour, Enums::Type type);
}

