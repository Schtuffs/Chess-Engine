#pragma once

#include <glad/glad.h>
#include <vector>

#include "Piece.h"
#include "RenderManager.h"

#define BOARD_FILL_MANUAL       0x20
#define BOARD_FILL_AUTOMATIC    0x21

#define BOARD_BLACK_WHITE       0x30
#define BOARD_BROWN_BROWN       0x31
#define BOARD_GREEN_CREAM       0x32
#define BOARD_DBLUE_LBLUE       0x33

// Manages pieces on the board and controlling some of its rendering
class BoardManager {
private:
    // To deal with all rendering functions
    RenderManager m_renderer;

    // Store pieces
    std::vector<Piece> m_pieces;

    // Stores colours for rendering values
    COLOUR m_dark, m_light;

public:
    // ----- Creation -----

    // Setup board
    // fillStyle - BOARD_FILL_MANUAL or BOARD_FILL_AUTOMATIC
    // boardColourStyle - select colours to render board with
    BoardManager(GLenum fillStyle, GLenum boardColourStyle);

    // ----- Read -----

    // Render board and pieces
    void show();

    // Renders the board squares
    // Can be done alone to show grid colours selection
    void board();

    // ----- Update -----
    
    // Selects colours for board rendering
    void setColour(GLuint boardColourStyle);

    // Add an already created piece to board
    void add(Piece& piece);

    // Create and add a piece to the board
    void add(GLenum type, GLenum colour, GLint x, GLint y);
    
    // Reset board to base position
    void resetBoard();

    // ----- Destruction -----

    // Frees all of the boards variables
    ~BoardManager();
};

