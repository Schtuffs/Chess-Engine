#pragma once

#include <glad/glad.h>

#include "RenderManager.h"

// Board Defines
#define BOARD_FILL_MANUAL       0x20
#define BOARD_FILL_AUTOMATIC    0x21

#define BOARD_BLACK_WHITE       0x30
#define BOARD_BROWN_BROWN       0x31
#define BOARD_GREEN_CREAM       0x32
#define BOARD_DBLUE_LBLUE       0x33
#define BOARD_RED_GOLD          0x34

#define TURN_WHITE              0x101
#define TURN_BLACK              0x102

constexpr char startFEN[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Piece defines

#define PIECE_INVALID   0x0
#define PIECE_PAWN      0x1
#define PIECE_KNIGHT    0x2
#define PIECE_BISHOP    0x3
#define PIECE_ROOK      0x4
#define PIECE_QUEEN     0x5
#define PIECE_KING      0x6
#define PIECE_PHANTOM   0x7FFF

#define PIECE_WHITE     0x08
#define PIECE_BLACK     0x10

#define BLACK_MASK      0b010000
#define WHITE_MASK      0b001000
#define TYPE_MASK       0b000111
#define COLOUR_MASK     (BLACK_MASK | WHITE_MASK)
#define HELD_MASK       0b100000

// Manages pieces on the board and controlling some of its rendering
class BoardManager {
private:
    // To deal with all rendering functions
    RenderManager m_renderer;

    // Store pieces and their information
    int m_grid[GRID_SIZE * GRID_SIZE];
    int m_heldPiece;
    POINT m_heldPieceOriginPos;

    // Stores colours for rendering values
    COLOUR m_dark, m_light;

    // Stores whos turn it is to move
    int m_turn;

    // For board to know if it needs to check positioning
    static POINT s_mouse;
    static bool s_isClicked;

    // ----- Update -----

    // Grabs a piece and holds it
    void hold(POINT gridPos);

    // Determine if piece can be dropped in clicked location
    // Will put back to where picked up if location is invalid
    void release(POINT gridPos);

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
    void showBoard();

    // Checks if there are any actions the board needs to take before rendering
    void check();

    // ----- Update -----
    
    // Selects colours for board rendering
    void setBoardColour(GLuint boardColourStyle);

    // Sets board up with FEN string
    void setBoard(const std::string& FEN);

    // Clears everything off of board
    void clearBoard();

    static void clicked(POINT mousePos);

    // ----- Destruction -----

    // Frees all of the boards variables
    ~BoardManager();
};

