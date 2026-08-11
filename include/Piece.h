#pragma once

#include <string>

#include "Constants.h"

/**
 * @brief The main piece class.
 * @class Piece
 * @date 2026-06-08
 */
class Piece {
public:
    // ----- Creation / Destruction -----

    /**
     * @brief Creates an invalid `Piece`.
     * @date 2026-06-28
     */
    Piece();

    /**
     * @brief Creates an en passant `Piece`.
     * @param index The `Index` of the en passant square.
     * @date 2026-06-28
     */
    Piece(Index index);

    /**
     * @brief Creates a `Piece` with a given `Enums::Type` and `Enums::Colour`.
     * @param `Colour` The `Colour` of the `Piece`.
     * @param `Type` The `Type` of the `Piece`.
     * @param index The `Index` of the `Piece`.
     * @date 2026-06-08
     */
    Piece(Enums::Colour colour, Enums::Type type, Index index);

    // Convert a char to a `Piece` with given `Index`.
    static Piece FromChar(char c, Index i);

    /**
     * @brief Frees any allocated memory associated with the `Piece`.
     * @date 2026-06-08
     */
    ~Piece();

    /**
     * @brief Set the position of the `Piece`.
     * @param index The index of the `Piece`.
     * @date 2026-06-08
     */
    void Position(Index index);

    /**
     * @brief Get the `Enums::Colour` of the `Piece`.
     * @return The `Enums::Colour` of the `Piece`.
     * @date 2026-06-29
     */
    Enums::Colour Colour() const noexcept;

    /**
     * @brief Check if `Piece` is an en passant placeholder.
     * @return `true` on en passant `Piece`.
     * @date 2026-07-08
     */
    bool IsEnPassant() const noexcept;

    /**
     * @brief Check if `Piece` is valid.
     * @return `true` on valid `Piece`.
     * @date 2026-06-29
     */
    bool IsValid() const noexcept;

    /**
     * @brief Get the `Index` position of the `Piece`.
     * @return The `Index` of the `Piece`.
     * @date 2026-06-29
     */
    Index Position() const noexcept;

    /**
     * @brief Get the `Enums::Type` of the `Piece`.
     * @return The `Enums::Type` of the `Piece`.
     * @date 2026-06-29
     */
    Enums::Type Type() const noexcept;

    /**
     * @brief Get the char representation of the `Piece`.
     * @return The char representation of the `Piece`.
     * @date 2026-06-29
     */
    char AsChar() const noexcept;

    /**
     * @brief Get the string representation of the `Piece`.
     * @return The string representation of the `Piece`.
     * @date 2026-06-29
     */
    std::string ToString() const noexcept;

private:
    Enums::Colour m_colour;
    Enums::Type   m_type;
    Index         m_pos;
    bool          m_isValid;
};
