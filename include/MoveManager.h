#pragma once

#include <iostream>
#include <vector>

#include "Library.h"
#include "Defines.h"

class MoveManager {
private:
    int m_startIndex;
    std::vector<INDEX> m_validMoves;
    std::vector<INDEX> m_defendingMoves;

    PIECE m_piece;
    const PIECE *m_grid;

    // ----- Read -----

    // Compares the values to determine if move should be added
    int addMove(INDEX index);

    // ----- Update -----

    // Calculates moves for king
    void calculateKingMoves(std::vector<INDEX>& enemyMoves);

    // Calculates the potential castling moves for the king
    void calculateKingCastling();

    // Calculates enemy moves so king can't move into their squares
    std::vector<INDEX> calculateEnemyMoves(FLAG colour);

    // Cardinal movement generation
    void calculateCardinalMoves();

    // Diagonal movement generation
    void calculateDiagonalMoves();

    // Calculates moves for knight hops
    void calculateKnightMoves();

    // Calculates moves for pawns
    void calculatePawnMoves();

    // Used for calculating invalid king moves
    void calculatePawnAttackingMoves(PIECE piece, INDEX index);

    // Calculates all attacking moves for king
    void calculateKingAttackingMoves(PIECE piece, INDEX index);

public:
    // ----- Creation -----

    MoveManager();

    // ----- Read -----
    
    // Calculates all valid moves for given piece
    void calculateMoves(INDEX startIndex, PIECE piece, const PIECE* grid);

    // Returns piece if a given move is in the valid moves
    PIECE isValidMove(const POINT& testPos);

    // Returns piece if a given move is in the valid moves
    PIECE isValidMove(INDEX testPos);

    std::vector<INDEX> getMoves();
    std::vector<INDEX> getDefendingMoves();

    // ----- Update -----

    // ----- Destruction -----

    ~MoveManager();
};

