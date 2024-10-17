#pragma once

#include <iostream>
#include <vector>

#include "Library.h"
#include "Defines.h"
#include "Move.h"

class MoveManager {
private:
    std::vector<Move> m_moves;

public:
    // ----- Creation -----

    MoveManager();

    // ----- Read -----

    // Returns if the move is legal
    bool isLegal(Move& move);

    std::vector<Move> getMoves(INDEX index);
    std::vector<Move> getMoves();

    // ----- Update -----
    
    // Calculates all valid moves for given piece
    void calculateMoves(FLAG colour, const PIECE* grid, bool calculateEnemyMoves = false);

    // Clears moves
    void clear();

    // ----- Destruction -----

    ~MoveManager();
};

