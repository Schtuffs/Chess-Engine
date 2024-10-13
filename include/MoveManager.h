#pragma once

#include <iostream>
#include <vector>

#include "Library.h"
#include "Defines.h"
#include "Move.h"

class MoveManager {
private:
    Move m_move;
    
    // ----- Update -----

    // Calculates and determines if enemy captured king
    void calculateLegalMoves(INDEX colour, const PIECE* grid);
    
    // Calculates moves for king
    bool calculateKingMoves(INDEX startIndex, const PIECE* grid);

    // Calculates the potential castling moves for the king
    void calculateKingCastling(INDEX startIndex, const PIECE* grid);

    // Cardinal movement generation
    // Returns true if no moves capture a king
    // Returns false immediately upon king capture, should not happen and must be dealt with
    bool calculateCardinalMoves(INDEX startIndex, const PIECE* grid);

    // Diagonal movement generation
    // Returns true if no moves capture a king
    // Returns false immediately upon king capture, should not happen and must be dealt with
    bool calculateDiagonalMoves(INDEX startIndex, const PIECE* grid);

    // Calculates moves for knight hops
    // Returns true if no moves capture a king
    // Returns false immediately upon king capture, should not happen and must be dealt with
    bool calculateKnightMoves(INDEX startIndex, const PIECE* grid);

    // Calculates moves for pawns
    // Returns true if no moves capture a king
    // Returns false immediately upon king capture, should not happen and must be dealt with
    bool calculatePawnMoves(INDEX startIndex, const PIECE* grid);

public:
    // ----- Creation -----

    MoveManager();

    // ----- Read -----

    // Returns a move if the move is legal
    MOVE isLegal(INDEX index);

    // Gets calculated legal moves
    std::vector<MOVE> getMoves();

    // ----- Update -----
    
    // Calculates all valid moves for given piece
    bool calculateMoves(INDEX startIndex, const PIECE* grid, bool calculateEnemyMoves = false);

    // Clears moves after piece is moved
    void clear();

    // ----- Destruction -----

    ~MoveManager();
};

