#include "MoveManager.h"

#include "BoardManager.h"
#include "MoveGen.h"
#include "Piece.h"

// ----- Creation -----

MoveManager::MoveManager() {
    // Nothing todo
}

// ----- Read -----

bool MoveManager::isLegal(Move& move) {
    for(Move& legal : this->m_moves) {
        // Checks positions are the same
        if(move.Start() != legal.Start()) {
            continue;
        }
        if (move.Target() != legal.Target()) {
            continue;
        }
        // Adds flags from other moves
        move.addFlags(legal.Flags());
        return true;
    }
    return false;
}

std::vector<Move> MoveManager::getMoves(INDEX start) {
    // Puts all moves with same start index into a vector and returns it
    std::vector<Move> moves;
    for (Move& move : this->m_moves) {
        if (move.Start() == start) {
            moves.push_back(move);
        }
    }
    return moves;
}

std::vector<Move> MoveManager::getMoves() {
    return this->m_moves;
}

// ----- Update -----

void MoveManager::calculateMoves(FLAG colour, const PIECE* grid, bool calculateEnemyMoves) {
    this->m_moves = MoveGen::generate(colour, grid, calculateEnemyMoves);
}

void MoveManager::clear() {
    this->m_moves.clear();
}

// ----- Destruction -----

MoveManager::~MoveManager() {
    // Nothing todo
}

