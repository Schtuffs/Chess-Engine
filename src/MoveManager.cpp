#include "MoveManager.h"

#include "BoardManager.h"
#include "Piece.h"

// ----- Creation -----

MoveManager::MoveManager() {
    // Nothing todo
}

// ----- Read -----

MOVE MoveManager::isLegal(INDEX index) {
    return this->m_move.isLegal(index);
}

std::vector<MOVE> MoveManager::getMoves() {
    return this->m_move.getLegalMoves();
}

// ----- Update -----

bool MoveManager::calculateMoves(INDEX startIndex, const PIECE* grid, bool calculateEnemyMoves) {
    
    // Clears old moves and add start index
    m_move.clearValid();

    // Add start index to moves
    if (calculateEnemyMoves) {
        m_move.addValid(startIndex, startIndex, 0, grid);
    }
    
    bool capturedKing = this->calculateCardinalMoves(startIndex, grid);
    capturedKing |= this->calculateDiagonalMoves(startIndex, grid);
    capturedKing |= this->calculateKnightMoves(startIndex, grid);
    capturedKing |= this->calculatePawnMoves(startIndex, grid);
    capturedKing |= this->calculateKingMoves(startIndex, grid);

    // Calculated moves for enemies, removes moves that would result in capture of king
    if (calculateEnemyMoves) {
        this->calculateLegalMoves(startIndex, grid);
    }

    return capturedKing;
}

void MoveManager::clear() {
    this->m_move.clearValid();
    this->m_move.clearLegal();
}

// ----- Update ----- Hidden -----

void MoveManager::calculateLegalMoves(INDEX index, const PIECE* grid) {
    FLAG colour = Piece::getFlag(grid[index], MASK_COLOUR);
    auto moves = m_move.getValidMoves();

    // Copy over grid
    PIECE editGrid[GRID_SIZE * GRID_SIZE];
    for (INDEX i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        editGrid[i] = grid[i];
    }

    // Loop through each move and play it on the board
    for (MOVE move : moves) {
        // Place pawn to block moves
        editGrid[m_move.getTarget(move)] = grid[m_move.getStart(move)];
        editGrid[m_move.getStart(move)] = 0;

        // Loop through each grid index
        bool capturedKing = false;
        for (INDEX i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
            // Checks colours aren't the same
            FLAG targetColour = Piece::getFlag(editGrid[i], MASK_COLOUR);
            if (colour != targetColour && targetColour) {
                // See if king is captured after the move
                bool result = this->calculateMoves(i, editGrid, false);
                if (result == MOVE_KING_CAPTURED) {
                    // King was captured, break out of loop since move is illegal
                    capturedKing = true;
                    break;
                }
            }
        }

        if (!capturedKing) {
            // King not captured, move is legal
            m_move.addLegal(move);
        }

        // Place whatever was here back
        editGrid[m_move.getTarget(move)] = grid[m_move.getTarget(move)];
        editGrid[m_move.getStart(move)] = grid[m_move.getStart(move)];
    }
}

bool MoveManager::calculateKingMoves(INDEX startIndex, const PIECE* grid) {
    // Type must be king
    FLAG type = Piece::getFlag(grid[startIndex], MASK_TYPE);
    if (type != PIECE_KING) {
        return MOVE_KING_NOT_CAPTURED;
    }

    // Top left to bottom right search
    bool capturedKing = MOVE_KING_NOT_CAPTURED;
    for (int y = 1; y >= -1; y--) {
        // Top side check
        if ((startIndex / GRID_SIZE == GRID_SIZE - 1) && (y == 1)) {
            // Increase y to remove out of out of bounds error
            continue;
        }
        // Bottom side check
        if ((startIndex / GRID_SIZE == 0) && (y == -1)) {
            // All points have been checked, break
            break;
        }
        for (int x = -1; x <= 1; x++) {
            INDEX target = (startIndex) + (y * GRID_SIZE) + x;

            // Skip center square
            if (x == y && x == 0) {
                continue;
            }
            // Right Side check
            if ((startIndex % GRID_SIZE == GRID_SIZE - 1) && (x == 1)) {
                continue;
            }
            // Left side check
            if ((startIndex % GRID_SIZE == 0) && (x == -1)) {
                continue;
            }
            
            if (m_move.addValid(startIndex, target, 0, grid) == MOVE_CAPTURE_KING) {
                capturedKing = MOVE_KING_CAPTURED;
            }
        }
    }

    this->calculateKingCastling(startIndex, grid);

    return capturedKing;
}

void MoveManager::calculateKingCastling(INDEX startIndex, const PIECE* grid) { 
    // Check castling kingside
    PIECE piece = grid[startIndex];
    
    bool canCastle = true;
    if (Piece::getFlag(piece, MASK_KING_CASTLE_KING)) {
        for (INDEX i = startIndex + 1; i % GRID_SIZE < GRID_SIZE; i++) {
            // Checks if piece at index is a rook
            if (Piece::getFlag(grid[i], MASK_TYPE) == PIECE_ROOK) {
                // Check if rook has castling ability flag
                if (Piece::getFlag(grid[i], MASK_ROOK_CAN_CASTLE)) {
                    break;
                }
            }
            // If piece detected, cannot castle
            if (grid[i]) {
                canCastle = false;
                break;
            }
        }
        if (canCastle) {
            this->m_move.addValid(startIndex, startIndex + 2, MASK_KING_CASTLE_KING, grid);
        }
    }
    // Check castling queensize
    canCastle = true;
    if (Piece::getFlag(piece, MASK_KING_CASTLE_QUEEN)) {
        for (INDEX i = startIndex - 1; i % GRID_SIZE >= 0; i--) {
            // Checks if piece at index is a rook
            if (Piece::getFlag(grid[i], MASK_TYPE) == PIECE_ROOK) {
                // Check if rook has castling ability flag
                if (Piece::getFlag(grid[i], MASK_ROOK_CAN_CASTLE)) {
                    break;
                }
            }
            // If piece detected, cannot castle
            if (grid[i]) {
                canCastle = false;
                break;
            }
        }
        if (canCastle) {
            this->m_move.addValid(startIndex, startIndex - 2, MASK_KING_CASTLE_QUEEN, grid);
        }
    }
}

bool MoveManager::calculateCardinalMoves(INDEX startIndex, const PIECE* grid) {
    // Type must be rook or queen
    FLAG type = Piece::getFlag(grid[startIndex], MASK_TYPE);
    if (type != PIECE_ROOK && type != PIECE_QUEEN) {
        return MOVE_KING_NOT_CAPTURED;
    }

    INDEX movement[] = { GRID_SIZE, 1, (-GRID_SIZE), -1 };

    // Loops through each potential movement direction
    bool capturedKing = MOVE_KING_NOT_CAPTURED;
    for (int i = 0; i < 4; i++) {
        for (INDEX target = startIndex + movement[i]; 0 <= target && target < GRID_SIZE * GRID_SIZE; target += movement[i]) {
            FLAG flag = MOVE_END;
            // Specific compares for each direction
            if ((i == 0 && target < GRID_SIZE * GRID_SIZE) ||
                (i == 1 && target % GRID_SIZE != 0) ||
                (i == 2 && target >= 0) ||
                (i == 3 && target % GRID_SIZE != GRID_SIZE - 1)) {
                flag = m_move.addValid(startIndex, target, 0, grid);
            }
            // Can continue searching in this direction
            if (flag == MOVE_CONTINUE) {
                continue;
            }
            // End of column/row hit
            else if (flag == MOVE_END) {
                break;
            }
            // King was captured
            else if (flag == MOVE_CAPTURE_KING) {
                capturedKing = MOVE_KING_CAPTURED;
                break;
            }
        }
    }
    return capturedKing;
}

bool MoveManager::calculateDiagonalMoves(INDEX startIndex, const PIECE* grid) {
    // Type must be bishop or queen
    FLAG type = Piece::getFlag(grid[startIndex], MASK_TYPE);
    if (type != PIECE_BISHOP && type != PIECE_QUEEN) {
        return MOVE_KING_NOT_CAPTURED;
    }

    // Stores movement change for pieces
    INDEX movement[] = { (GRID_SIZE - 1), (GRID_SIZE + 1), (-GRID_SIZE - 1), (-GRID_SIZE + 1) };

    // Loops through each potential movement direction
    bool capturedKing = MOVE_KING_NOT_CAPTURED;
    for (int i = 0; i < 4; i++) {
        for (INDEX target = startIndex + movement[i]; 0 <= target && target < GRID_SIZE * GRID_SIZE; target += movement[i]) {
            FLAG flag = MOVE_END;
            if (0 <= target && target < GRID_SIZE * GRID_SIZE) {
                // Specific compares for each direction
                if (((i == 0 || i == 2) && (target % GRID_SIZE != GRID_SIZE - 1)) ||
                    ((i == 1 || i == 3) && (target % GRID_SIZE != 0))) {
                    flag = m_move.addValid(startIndex, target, 0, grid);
                }
            }
            // Can continue searching in this direction
            if (flag == MOVE_CONTINUE) {
                continue;
            }
            // End of column/row hit
            else if (flag == MOVE_END) {
                break;
            }
            // King was captured
            else if (flag == MOVE_CAPTURE_KING) {
                capturedKing = MOVE_KING_CAPTURED;
                break;
            }
        }
    }
    return capturedKing;
}

bool MoveManager::calculateKnightMoves(INDEX startIndex, const PIECE* grid) {
    // Type must be knight
    FLAG type = Piece::getFlag(grid[startIndex], MASK_TYPE);
    if (type != PIECE_KNIGHT) {
        return MOVE_KING_NOT_CAPTURED;
    }

    INDEX movement[] = {
        startIndex + (2 * GRID_SIZE) - 1,
        startIndex + (2 * GRID_SIZE) + 1,
        startIndex + (2) + GRID_SIZE,
        startIndex + (2) - GRID_SIZE,
        startIndex - (2 * GRID_SIZE) - 1,
        startIndex - (2 * GRID_SIZE) + 1,
        startIndex - (2) + GRID_SIZE,
        startIndex - (2) - GRID_SIZE
    };

    bool preChecks[] = {
        startIndex % GRID_SIZE != 0,
        startIndex % GRID_SIZE != GRID_SIZE - 1,
        startIndex % GRID_SIZE < GRID_SIZE - 2,
        startIndex % GRID_SIZE < GRID_SIZE - 2,
        startIndex % GRID_SIZE != 0,
        startIndex % GRID_SIZE != GRID_SIZE - 1,
        startIndex % GRID_SIZE > 1,
        startIndex % GRID_SIZE > 1
    };

    // Loop though all of the different moves and conditions
    bool capturedKing = MOVE_KING_NOT_CAPTURED;
    for (int i = 0; i < 8; i++) {
        if(preChecks[i]) {
            int addResult = m_move.addValid(startIndex, movement[i], 0, grid);
            if (addResult == MOVE_CAPTURE_KING) {
                capturedKing = MOVE_KING_CAPTURED;
            }
        }
    }

    return capturedKing;
}

bool MoveManager::calculatePawnMoves(INDEX startIndex, const PIECE* grid) {
    // Type must be pawn
    FLAG type = Piece::getFlag(grid[startIndex], MASK_TYPE);
    if (type != PIECE_PAWN) {
        return MOVE_KING_NOT_CAPTURED;
    }

    bool capturedKing = MOVE_KING_NOT_CAPTURED;
    
    FLAG colour = Piece::getFlag(grid[startIndex], MASK_COLOUR);
    INDEX indexCheck = (colour == PIECE_WHITE ? GRID_SIZE : (-GRID_SIZE));
    // Single move check
    int addResult = m_move.addValid(startIndex, startIndex + indexCheck, 0, grid);
    if (addResult != MOVE_END) {
        // Move was not blocked, check double move
        if (Piece::hasFlag(grid[startIndex], MASK_PAWN_FIRST_MOVE)) {
            m_move.addValid(startIndex, startIndex + (2 * indexCheck), MASK_PAWN_EN_PASSENT, grid);
        }
    }

    // Left attack check
    addResult = m_move.addValid(startIndex, startIndex + indexCheck - 1, 0, grid);
    if (addResult == MOVE_CAPTURE_KING) {
        capturedKing = MOVE_KING_CAPTURED;
    }

    // Right attack check
    addResult = m_move.addValid(startIndex, startIndex + indexCheck + 1, 0, grid);
    if (addResult == MOVE_CAPTURE_KING) {
        capturedKing = MOVE_KING_CAPTURED;
    }

    return capturedKing;
}

// ----- Destruction -----

MoveManager::~MoveManager() {
    // Nothing todo
}

