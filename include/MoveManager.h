#pragma once

#include <iostream>
#include <vector>

#include "Library.h"

#define MOVE_CONTINUE   1
#define MOVE_END        2

class MoveManager {
private:
    POINT m_startPos;
    std::vector<int> m_validMoves;

    int m_piece;
    const int *m_grid;

    // ----- Read -----

    // Compares the values to determine if move should be added
    int addMove(int index);

    // ----- Update -----

    // Calculates moves for king
    void calculateKingMoves(std::vector<int>& enemyMoves);

    // Calculates enemy moves so king can't move into their squares
    std::vector<int> calculateEnemyMoves(int colour);

    // Cardinal movement generation
    void calculateCardinalMoves();

    // Diagonal movement generation
    void calculateDiagonalMoves();

    // Calculates moves for knight hops
    void calculateKnightMoves();

    // Calculates moves for pawns
    void calculatePawnMoves();

    // Used for calculating invalid king moves
    void calculatePawnAttackingMoves(int piece, int index);

    // Calculates all attacking moves for king
    void calculateKingAttackingMoves(int piece, int index);

public:
    // ----- Creation -----
    MoveManager();

    // ----- Read -----
    
    // Calculates all valid moves for given piece
    void calculateMoves(POINT startPos, int piece, const int* grid);

    // Returns piece if a given move is in the valid moves
    int isValidMove(const POINT& testPos);

    // Returns piece if a given move is in the valid moves
    int isValidMove(int testPos);

    std::vector<int> getMoves();

    // ----- Update -----

    // ----- Destruction -----
    ~MoveManager();
};

