#pragma once

#include <glad/glad.h>
#include <vector>

#include "RenderManager.h"
#include "MoveManager.h"
#include "Defines.h"
#include "Player.h"

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
    INDEX m_whiteKing, m_blackKing;

    // Pawn promotion data
    INDEX m_promotionIndex;

    // Castling data
    bool m_castling[4];
    
    // Stores move data
    int m_totalTurns, m_50moveRule;
    // Stores if game is checkmate
    bool m_checkmate;
    // Stores if game is in stalemate
    bool m_stalemate;

    // Stores colours for rendering values
    COLOUR m_dark, m_light;

    // Stores board looking state
    bool m_flipBoard;
    bool m_whitePerspective;

    // Stores player info
    Player m_whitePlayer, m_blackPlayer;
    Player* m_currentPlayer;



    // ----- Read -----

    // Renders promotion items to the screen
    void showPromotionOptions();

    // Checks if last move is checkmate
    void checkCheckmate(Move& move);

    // ----- Update -----

    // Sets metadata for board upon loading
    void readMetadata(std::string& metadata);

    // Adds the metadata from FEN string to board
    void setMetadata();

    // Determines which option was selected from the promotion screen
    void promotionSelection(INDEX index);

    // Grabs a piece and holds it
    void hold(INDEX index);

    // Releases the piece
    void release(Move& move);

    // Deals with phantom piece
    void managePhantom(Move move);

public:
    // ----- Creation -----

    // Setup board
    // fillStyle - BOARD_FILL_MANUAL or BOARD_FILL_AUTOMATIC
    // boardColourStyle - select colours to render board with
    BoardManager(Player& white, Player& black, GLenum boardColourStyle, bool flipBoard = true, const std::string& FEN = startFEN);

    // ----- Read -----

    // Render board and pieces
    void show();

    // Renders the board squares
    // Can be done alone to show grid colours selection
    void showBoard();

    // Checks if there are any actions the board needs to take before rendering
    void ManageInput(INDEX index);

    // ----- Update -----

    // Allows a player to make a move
    bool makeMove(Move& move);
    
    // Selects colours for board rendering
    void setBoardColour(GLuint boardColourStyle);

    // Sets board up with FEN string
    void resetBoard();

    // Clears everything off of board
    void clearBoard();

    // Sets an index where pawn promoted
    void setPromotion(INDEX index);

    // Allows changing of board flip state on the fly
    void changeFlip();

    // Allows user to force perspective change
    void changePerspective();

    // ----- Destruction -----

    // Frees all of the boards variables
    ~BoardManager();
};

