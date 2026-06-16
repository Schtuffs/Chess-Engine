#pragma once

#include "raylib.h"

#include "Constants.h"

/**
 * @brief The main piece class.
 * 
 * @class Piece
 * @author Kyle Wagler
 * @date 2026-06-08
 */
class Piece {
public:
    // ----- Creation / Destruction -----
    
    /**
     * @brief Creates a piece with a given type and colour.
     * @param `Colour` The `Colour` of the piece.
     * @param `Type` The `Type` of the piece.
     * @author Kyle Wagler
     * @date 2026-06-08
     */
    Piece(Enums::Colour colour, Enums::Type type);
    
    /**
     * @brief Frees any allocated memory associated with the piece.
     * @author Kyle Wagler
     * @date 2026-06-08
     */
    ~Piece();

private:
    Enums::Colour m_colour; 
    Enums::Type m_type;
    bool m_isSetup;
};

