#pragma once

#include <glad/glad.h>
#include <vector>

#include "RenderManager.h"
#include "MoveManager.h"

// Board Defines

#define BOARD_BLACK_WHITE           0x30
#define BOARD_BROWN_BROWN           0x31
#define BOARD_GREEN_CREAM           0x32
#define BOARD_DBLUE_LBLUE           0x33
#define BOARD_RED_GOLD              0x34

#define TURN_WHITE                  0x101
#define TURN_BLACK                  0x102

#define BOARD_CASTLING_BLACK_KING   0x0
#define BOARD_CASTLING_BLACK_QUEEN  0x1
#define BOARD_CASTLING_WHITE_KING   0x2
#define BOARD_CASTLING_WHITE_QUEEN  0x3

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

#define MASK_BLACK              0b000010000
#define MASK_WHITE              0b000001000
#define MASK_TYPE               0b000000111
#define MASK_COLOUR             (MASK_BLACK | MASK_WHITE)
#define MASK_HELD               0b000100000
#define MASK_MIN_FLAGS          (MASK_HELD | MASK_COLOUR | MASK_TYPE)
#define MASK_PINNED             0b001000000
#define MASK_LOCATION           0b000111111

// Piece specific masks
#define MASK_PAWN_FIRST_MOVE    0b010000000
#define MASK_KING_CASTLE_KING   0b010000000
#define MASK_KING_CASTLE_QUEEN  0b100000000
#define MASK_CASTLING           (MASK_KING_CASTLE_KING | MASK_KING_CASTLE_QUEEN)

// Manages pieces on the board and controlling some of its rendering
class BoardManager {
private:
    // To deal with all rendering functions
    RenderManager m_renderer;

    // Calculates moves
    MoveManager m_moveManager;

    // Store pieces and their information
    int m_grid[GRID_SIZE * GRID_SIZE];
    int m_heldPiece, m_phantomLocation;
    POINT m_heldPieceOriginPos;
    std::vector<int> m_validMoves;

    // Castling data
    bool m_castling[4];
    
    // Stores whos turn it is to move
    int m_currentTurn, m_totalTurns, m_50moveRule;

    // Stores colours for rendering values
    COLOUR m_dark, m_light;

    // For board to know if it needs to check positioning
    static POINT s_mouse;
    static bool s_isClicked;

    // ----- Update -----

    // Sets metadata for board upon loading
    void getMetadata(std::string& metadata);

    // Adds the metadata from FEN string to board
    void setMetadata();

    // Grabs a piece and holds it
    void hold(POINT& gridPos);

    // Releases the piece
    void release(const POINT& gridPos, int piece, bool moved);

    // Removes any flags from piece
    void removeFlags(int index);

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

    static void clicked(const POINT& mousePos);

    // ----- Destruction -----

    // Frees all of the boards variables
    ~BoardManager();
};

