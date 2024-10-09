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

#define TURN_WHITE              0x101
#define TURN_BLACK              0x102

constexpr char startFEN[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Manages pieces on the board and controlling some of its rendering
class BoardManager {
private:
    // To deal with all rendering functions
    RenderManager m_renderer;

    // Store pieces
    std::vector<Piece> m_pieces;
    Piece* m_heldPiece;
    bool m_doesHeldPieceExists;

    // Stores colours for rendering values
    COLOUR m_dark, m_light;

    // Stores whos turn it is to move
    int m_turn;

    // For board to know if it needs to check positioning
    static POINT s_mouse;
    static bool s_isClicked;

    // ----- Destruction -----

    // Deletes all pieces from array
    void deletePieces();

public:
    // ----- Creation -----

    // Setup board
    // fillStyle - BOARD_FILL_MANUAL or BOARD_FILL_AUTOMATIC
    // boardColourStyle - select colours to render board with
    BoardManager(GLenum boardColourStyle, const std::string& FEN = startFEN);

    // ----- Read -----

    // Render board and pieces
    void show();

    // Renders the board squares
    // Can be done alone to show grid colours selection
    void board();

    // Checks if there are any actions the board needs to take before rendering
    void check();

    // ----- Update -----
    
    // Selects colours for board rendering
    void setColour(GLuint boardColourStyle);

    // Create and add a piece to the board
    void add(GLenum type, GLenum colour, GLint x, GLint y);

    // Sets board up with FEN string
    void setBoard(const std::string& FEN);

    static void clicked();

    // ----- Destruction -----

    // Frees all of the boards variables
    ~BoardManager();
};

