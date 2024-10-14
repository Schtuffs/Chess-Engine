#include "Move.h"

#include "Piece.h"

Move::Move() {
    // Nothing todo
}

Move::~Move() {
    // Nothing todo
}

// Private functions

void Move::p_addValid(INDEX startIndex, INDEX targetIndex, FLAG flags) {
    bool firstInstance = true;
    MOVE adding = {startIndex | (targetIndex << 6) | flags};
    for (MOVE move : m_validMoves) {
        if (move.moveInfo == adding.moveInfo) {
            firstInstance = false;
            break;
        }
    }

    if (firstInstance) {
        m_validMoves.push_back(adding);
    }
}

int Move::p_addPawn(INDEX startIndex, INDEX targetIndex, FLAG flags, const PIECE* grid) {
    // Checks if start and target are in same x
    INDEX startX = startIndex % GRID_SIZE;
    INDEX targetX = targetIndex % GRID_SIZE;

    FLAG thisColour = Piece::getFlag(grid[startIndex], MASK_COLOUR);
    FLAG targetColour = Piece::getFlag(grid[targetIndex], MASK_COLOUR);

    // Checks for same x value movement
    if (startX == targetX) {
        // Checks theres no piece
        if (grid[targetIndex] == 0) {
            p_addValid(startIndex, targetIndex, flags);
            return MOVE_CONTINUE;
        }
        return MOVE_END;
    }
    // X values are not equal, this is a capture check

    // Check colours are not equal and that the target has a piece
    if (thisColour != targetColour && targetColour) {
        p_addValid(startIndex, targetIndex, flags);
    }
    // Colours not equal, or no piece
    return MOVE_END;
}



// Declared functions

int Move::addValid(INDEX startIndex, INDEX targetIndex, FLAG flags, const PIECE* grid) {
    // Ensure index is valid
    if (0 > startIndex || startIndex >= GRID_SIZE * GRID_SIZE) {
        return MOVE_END;
    }
    if (0 > targetIndex || targetIndex >= GRID_SIZE * GRID_SIZE) {
        return MOVE_END;
    }

    // Always add start index as legal move
    if (startIndex == targetIndex) {
        MOVE move = {startIndex | (targetIndex << 6) | flags};
        addLegal(move);
        return MOVE_CONTINUE;
    }

    FLAG thisType = Piece::getFlag(grid[startIndex], MASK_TYPE);
    FLAG targetType = Piece::getFlag(grid[targetIndex], MASK_TYPE);

    FLAG thisColour = Piece::getFlag(grid[startIndex], MASK_COLOUR);
    FLAG targetColour = Piece::getFlag(grid[targetIndex], MASK_COLOUR);

    // Pawns are special with their capturing
    if (thisType == PIECE_PAWN) {
        return p_addPawn(startIndex, targetIndex, flags, grid);
    }

    // No piece, add move
    if (grid[targetIndex] == 0 || grid[targetIndex] == PIECE_PHANTOM) {
        p_addValid(startIndex, targetIndex, flags);
        return MOVE_CONTINUE;
    }

    // Determine if different colour colour

    if (thisColour != targetColour) {
        // Check if move is attacking a king
        if (targetType == PIECE_KING) {
            // Attacking king, add flag
            p_addValid(startIndex, targetIndex, flags | MASK_KING_IN_CHECK);
            return MOVE_CAPTURE_KING;
        }
        // Pieces are not the same colour, add move
        p_addValid(startIndex, targetIndex, flags);
    }
    return MOVE_END;
}

void Move::addLegal(MOVE move) {
    this->m_legalMoves.push_back(move);
}

MOVE Move::isLegal(INDEX testPos) {
    auto moves = this->getLegalMoves();
    for (size_t i = 0; i < moves.size(); i++) {
        if (testPos == this->getTarget(moves[i])) {
            return moves[i];
        }
    }
    return { 0 };
}

std::vector<MOVE> Move::getValidMoves() {
    return m_validMoves;
}

std::vector<MOVE> Move::getLegalMoves(){ 
    return m_legalMoves;
}

INDEX Move::getStart(MOVE move) {
    return ((move.moveInfo & MASK_MOVE_START));
}

INDEX Move::getTarget(MOVE move) {
    return ((move.moveInfo & MASK_MOVE_TARGET) >> 6);
}

FLAG Move::getFlags(MOVE move) {
    return ((move.moveInfo & MASK_MOVE_FLAGS));
}

FLAG Move::getFlag(MOVE move, FLAG flag) {
    return (move.moveInfo & flag);
}

void Move::clearValid() {
    m_validMoves.clear();
}

void Move::clearLegal() {
    m_legalMoves.clear();
}

