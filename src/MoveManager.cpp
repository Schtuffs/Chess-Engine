#include "MoveManager.h"

#include "BoardManager.h"

// ----- Creation -----

MoveManager::MoveManager() {
    // Nothing todo
}

// ----- Read -----

void MoveManager::calculateMoves(POINT startPos, int piece, const int* grid) {
    // Init data
    this->m_startPos = startPos;
    this->m_piece = piece;
    
    // Copy grid data
    this->m_grid = grid;
    // Clear old moves
    this->m_validMoves.clear();
    
    // Add starting position no matter what
    this->addMove(this->m_startPos.y * GRID_SIZE + this->m_startPos.x);

    int type = this->m_piece & MASK_TYPE;
    if (type == PIECE_KING) {
        std::vector<int> enemyMoves = this->calculateEnemyMoves(piece & MASK_COLOUR);
        this->calculateKingMoves(enemyMoves);
        return;
    }

    if (type == PIECE_ROOK || type == PIECE_QUEEN) {
        this->calculateCardinalMoves();
    }

    if (type == PIECE_BISHOP || type == PIECE_QUEEN) {
        this->calculateDiagonalMoves();
        return;
    }

    if (type == PIECE_KNIGHT) {
        this->calculateKnightMoves();
        return;
    }

    if (type == PIECE_PAWN) {
        this->calculatePawnMoves();
        return;
    }
}

int MoveManager::isValidMove(int testPos) {
    for (int pos : this->m_validMoves) {
        if (testPos == (pos & MASK_LOCATION)) {
            return this->m_piece;
        }
    }
    return 0;
}

int MoveManager::isValidMove(const POINT& testPos) {
    return this->isValidMove(testPos.y * GRID_SIZE + testPos.x);
}

std::vector<int> MoveManager::getMoves() {
    return this->m_validMoves;
}

// ----- Read ----- Hidden -----

int MoveManager::addMove(int index) {
    // No piece, add move
    if (this->m_grid[index] == 0 || this->m_grid[index] == PIECE_PHANTOM) {
        this->m_validMoves.push_back(index);
        return MOVE_CONTINUE;
    }

    // Piece detected
    // Different colour
    int thisColour = this->m_piece & MASK_COLOUR;
    int testColour = this->m_grid[index] & MASK_COLOUR;
    if (thisColour != testColour) {
        // Pieces are not the same colour, add move
        this->m_validMoves.push_back(index);
    }
    return MOVE_END;
}

// ----- Update ----- Hidden -----

void MoveManager::calculateKingMoves(std::vector<int>& enemyMoves) {
    // Top left to bottom right search
    for (int y = 1; y >= -1; y--) {
        for (int x = -1; x <= 1; x++) {
            int index = (this->m_startPos.y * GRID_SIZE + this->m_startPos.x) + (y * GRID_SIZE) + x;

            // Don't add start position
            if (y == 0 && x == 0) {
                continue;
            }

            // Top side check
            if ((this->m_startPos.y == GRID_SIZE - 1) && (y == 1)) {
                // Increase y to remove out of out of bounds error
                break;
            }
            // Bottom side check
            if ((this->m_startPos.y == 0) && (y == -1)) {
                // All points have been checked, return
                return;
            }
            // Right Side check
            if ((this->m_startPos.x == GRID_SIZE - 1) && (x == 1)) {
                continue;
            }
            // Left side check
            if ((this->m_startPos.x == 0) && (x == -1)) {
                continue;
            }

            // Makes sure move is not being attacked
            bool validMove = true;
            for (int move : enemyMoves) {
                if (move == index) {
                    validMove = false;
                    break;
                }
            }
            // If move is still valid, add it
            if (validMove) {
                this->addMove(index);
            }
        }
    }
}

std::vector<int> MoveManager::calculateEnemyMoves(int colour) {
    std::vector<int> moves;
    
    // Loop through grid
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        // Colours don't match, calculate moves
        if (!this->m_grid[i] || (this->m_grid[i] & MASK_COLOUR) == colour) {
            continue;
        }
        // Call a movemanager for current piece
        MoveManager enemy;
        POINT start = { i % GRID_SIZE, i / GRID_SIZE };
        std::vector<int> calculated;

        // Special case for pawn
        if ((this->m_grid[i] & MASK_TYPE) == PIECE_PAWN) {
            enemy.calculatePawnAttackingMoves(this->m_grid[i], i);
            calculated = enemy.getMoves();
        }
        // Special case for king
        else if ((this->m_grid[i] & MASK_TYPE) == PIECE_KING) {
            calculated.clear();
            enemy.calculateKingAttackingMoves(this->m_grid[i], i);
            calculated = enemy.getMoves();
        }
        // Calculates moves for all other pieces
        else {
            enemy.calculateMoves(start, this->m_grid[i], this->m_grid);
            calculated = enemy.getMoves();
        }

        // Loop through each calculated value
        for (int calc : calculated) {
            // Loop through each previously calculated value
            for (int move : moves) {
                // If the numbers are equal, no need to add
                if (calc == move) {
                    calc = -1;
                    break;
                }
            }
            // If move is not already in list, add it
            if (calc != -1) {
                moves.push_back(calc);
            }
        }
    }
    return moves;
}

void MoveManager::calculateCardinalMoves() {
    // Each direction separately
    int index = this->m_startPos.y * GRID_SIZE + this->m_startPos.x;
    // Up
    for (int i = index + GRID_SIZE; i < GRID_SIZE * GRID_SIZE; i += GRID_SIZE) {
        if (this->addMove(i) == MOVE_END) {
            break;
        }
    }

    // Down
    for (int i = index - GRID_SIZE; i >= 0; i -= GRID_SIZE) {
        if (this->addMove(i) == MOVE_END) {
            break;
        }
    }

    // Right
    for (int i = index + 1; (GRID_SIZE * GRID_SIZE > i) && (i % GRID_SIZE != 0); i++) {
        if (this->addMove(i) == MOVE_END) {
            break;
        }
    }

    // Left
    for (int i = index - 1; (0 <= i) && (i % GRID_SIZE != GRID_SIZE - 1); i--) {
        if (this->addMove(i) == MOVE_END) {
            break;
        }
    }
}

void MoveManager::calculateDiagonalMoves() {
    // Each direction separately
    int index = this->m_startPos.y * GRID_SIZE + this->m_startPos.x;
    // Up left
    for (int i = index + GRID_SIZE - 1; (i < GRID_SIZE * GRID_SIZE) && (i % GRID_SIZE != GRID_SIZE - 1); i += GRID_SIZE - 1) {
        if (this->addMove(i) == MOVE_END) {
            break;
        }
    }

    // Up right
    for (int i = index + GRID_SIZE + 1; (i >= 0) && (i % GRID_SIZE != 0); i += GRID_SIZE + 1) {
        if (this->addMove(i) == MOVE_END) {
            break;
        }
    }

    // Down left
    for (int i = index - GRID_SIZE - 1; (GRID_SIZE * GRID_SIZE > i) && (i % GRID_SIZE != GRID_SIZE - 1); i-= GRID_SIZE + 1) {
        if (this->addMove(i) == MOVE_END) {
            break;
        }
    }

    // Down right
    for (int i = index - GRID_SIZE + 1; (0 <= i) && (i % GRID_SIZE != 0); i -= GRID_SIZE - 1) {
        if (this->addMove(i) == MOVE_END) {
            break;
        }
    }
}

void MoveManager::calculateKnightMoves() {
    int index = this->m_startPos.y * GRID_SIZE + this->m_startPos.x;
    
    // Add move return value is unimportant since knights don't keep going in same direction

    // Up
    // Checks that move will not wrap around
    if (index % GRID_SIZE != 0) {
        // Left
        this->addMove(index + (2 * GRID_SIZE) - 1);
    }
    if (index % GRID_SIZE != GRID_SIZE - 1) {
        // Right
        this->addMove(index + (2 * GRID_SIZE) + 1);
    }

    // Right
    // Checks that move will not wrap around
    if (index % GRID_SIZE < GRID_SIZE - 2) {
        // Up
        this->addMove(index + (2) + GRID_SIZE);
        // Down
        this->addMove(index + (2) - GRID_SIZE);
    }

    // Down
    // Checks that move will not wrap around
    if (index % GRID_SIZE != 0) {
        // Left
        this->addMove(index - (2 * GRID_SIZE) - 1);
    }
    if (index % GRID_SIZE != GRID_SIZE - 1) {
        // Right
        this->addMove(index - (2 * GRID_SIZE) + 1);
    }
    
    // Left
    // Checks that move will not wrap around
    if (index % GRID_SIZE > 1) {
        // Up
        this->addMove(index - (2) + GRID_SIZE);
        // Down
        this->addMove(index - (2) - GRID_SIZE);
    }
}

void MoveManager::calculatePawnMoves() {
    int colour = this->m_piece & MASK_COLOUR;

    // White piece
    int indexCheck = this->m_startPos.y * GRID_SIZE + this->m_startPos.x + GRID_SIZE;
    if (colour == PIECE_WHITE) {
        // Single move check
        if (this->m_grid[indexCheck] == 0) {
            this->m_validMoves.push_back(indexCheck);

            // Double move check
            if ((this->m_piece & MASK_PAWN_FIRST_MOVE)) {
                if (this->m_grid[indexCheck + GRID_SIZE] == 0) {
                    this->m_validMoves.push_back(indexCheck + GRID_SIZE);
                }
            }
        }
        // Capture checks
        // Left
        int otherColour = this->m_grid[indexCheck - 1] & MASK_COLOUR;
        if (indexCheck % GRID_SIZE != 0) {
            if (this->m_grid[indexCheck - 1] != 0 && colour != otherColour) {
                this->m_validMoves.push_back(indexCheck - 1);
            }
        }
        // Right
        otherColour = this->m_grid[indexCheck + 1] & MASK_COLOUR;
        if (indexCheck % GRID_SIZE != GRID_SIZE - 1) {
            if (this->m_grid[indexCheck + 1] != 0 && colour != otherColour) {
                this->m_validMoves.push_back(indexCheck + 1);
            }
        }
    }
    else {
        indexCheck = this->m_startPos.y * GRID_SIZE + this->m_startPos.x - GRID_SIZE;
        // Single move check
        if (this->m_grid[indexCheck] == 0) {
            this->m_validMoves.push_back(indexCheck);

            // Double move check
            if ((this->m_piece & MASK_PAWN_FIRST_MOVE)) {
                if (this->m_grid[indexCheck - GRID_SIZE] == 0)
                    this->m_validMoves.push_back(indexCheck - GRID_SIZE);
            }
        }
        // Capture checks
        // Left
        int otherColour = this->m_grid[indexCheck - 1] & MASK_COLOUR;
        if (indexCheck % GRID_SIZE != 0) {
            if (this->m_grid[indexCheck - 1] != 0 && colour != otherColour) {
                this->m_validMoves.push_back(indexCheck - 1);
            }
        }
        // Right
        otherColour = this->m_grid[indexCheck + 1] & MASK_COLOUR;
        if (indexCheck % GRID_SIZE != GRID_SIZE - 1) {
            if (this->m_grid[indexCheck + 1] != 0 && colour != otherColour) {
                this->m_validMoves.push_back(indexCheck + 1);
            }
        }
    }
}

// Attacking move functions

void MoveManager::calculatePawnAttackingMoves(int piece, int index) {
    int colour = piece & MASK_COLOUR;

    // White piece
    if (colour == PIECE_WHITE) {
        int indexCheck = index + GRID_SIZE;
        // Left
        this->m_validMoves.push_back(indexCheck - 1);
        // Right
        this->m_validMoves.push_back(indexCheck + 1);
    }
    // Black piece
    else {
        int indexCheck = index - GRID_SIZE;
        // Left
        this->m_validMoves.push_back(indexCheck - 1);
        // Right
        this->m_validMoves.push_back(indexCheck + 1);
    }
}

void MoveManager::calculateKingAttackingMoves(int piece, int index) {
    for (int y = 1; y >= -1; y--) {
        for (int x = -1; x <= 1; x++) {
            this->m_validMoves.push_back(index + (y * GRID_SIZE) + x);
        }
    }
}

// ----- Destruction -----

MoveManager::~MoveManager() {
    // Nothing todo
}

