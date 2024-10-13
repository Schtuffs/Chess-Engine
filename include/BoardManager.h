#pragma once

#include <glad/glad.h>
#include <vector>

#include "RenderManager.h"
#include "MoveManager.h"
#include "Defines.h"

// Manages pieces on the board and controlling some of its rendering
class BoardManager {
private:
    // To deal with all rendering functions
    RenderManager m_renderer;

    // Calculates moves
    MoveManager m_moveManager;

    // Reset FEN
    std::string m_resetFEN;

    // Store pieces and their information
    PIECE m_grid[GRID_SIZE * GRID_SIZE];
    INDEX m_heldPieceIndex;
    INDEX m_phantomLocation, m_phantomAttack;
    std::vector<INDEX> m_validMoves;

    // Pawn promotion data
    INDEX m_promotionIndex;

    // Castling data
    bool m_castling[4];
    
    // Stores whos turn it is to move
    int m_currentTurn, m_totalTurns, m_50moveRule;

    // Stores colours for rendering values
    COLOUR m_dark, m_light;

    // ----- Read -----

    // Renders promotion items to the screen
    void showPromotionOptions();

    // ----- Update -----

    // Sets metadata for board upon loading
    void getMetadata(std::string& metadata);

    // Adds the metadata from FEN string to board
    void setMetadata();

    // Determines which option was selected from the promotion screen
    void promotionSelection(INDEX index);

    // Grabs a piece and holds it
    bool hold(INDEX index);

    // Releases the piece
    void release(MOVE move, bool moved);

    // Deals with phantom piece
    void managePhantom(INDEX index, PIECE piece);

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
    void check(INDEX index);

    // ----- Update -----
    
    // Selects colours for board rendering
    void setBoardColour(GLuint boardColourStyle);

    // Sets board up with FEN string
    void resetBoard();

    // Clears everything off of board
    void clearBoard();

    // Sets an index where pawn promoted
    void setPromotion(INDEX index);

    // ----- Destruction -----

    // Frees all of the boards variables
    ~BoardManager();
};

