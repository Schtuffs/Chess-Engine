#pragma once

#include <cstdint>
#include <string_view>

#include "raylib.h"

#include "Constants.h"

/**
 * @brief Handles the rendering to the screen.
 * @class Renderer
 * @author Kyle Wagler
 * @date 2026-06-11
 */
class Renderer {
public:

    // ----- Creation / Destruction -----

    /**
     * @brief Loads `Texture2D` from resources.
     * @author Kyle Wagler
     * @date 2026-06-11
     */
    Renderer();
    /**
     * @brief Cleanup loaded `Texture2D`.
     * @author Kyle Wagler
     * @date 2026-06-11
     */
    ~Renderer();
    
    // ----- Read -----

    /**
     * @brief Renders the board for the background.
     * @param dark The colour for the dark tiles.
     * @param light The colour for the light tiles.
     * @author Kyle Wagler
     * @date 2026-06-11
     */
    void RenderBoard(Color dark, Color light);
    
    /**
     * @brief Renders the pieces based on a fen.
     * @param fen The fen state to render the board in.
     * @param isWhitePerspective Determines if rendering pieces from whites perspective or blacks perspective.
     * @author Kyle Wagler
     * @date 2026-06-11
     */
    void RenderPieces(std::string_view fen, bool isWhitePerspective);
    
private:
    Texture2D m_textures[12];
    int m_textureSize, m_startX, m_startY;
    
    /**
     * @brief Determines a pieces colour.
     * @param cur The current char in question.
     * @return The colour of the piece;
     * @author Kyle Wagler
     * @date 2026-06-11
     */
    int CheckColour(char cur);
    
    /**
     * @brief Determines a pieces type.
     * @param cur The current char in question.
     * @return The type of the piece
     * @author Kyle Wagler
     * @date 2026-06-11
     */
    int CheckType(char cur);
    
    /**
     * @brief Fixes window size.
     * @author Kyle Wagler
     * @date 2026-06-15
     */
    void FixSize();
    
    /**
     * @brief Renders a piece to the board.
     * @param texture The `Texture2D` to render.
     * @param pos The `Vec2` position of the piece.
     * @author Kyle Wagler
     * @date 2026-06-11
     */
    void RenderPiece(Texture2D texture, Vec2<int> pos);
};

