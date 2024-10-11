#include "MoveManager.h"

#include "BoardManager.h"
#include "Piece.h"

// ----- Creation -----

MoveManager::MoveManager() {
    // Nothing todo
}

// ----- Read -----

void MoveManager::calculateMoves(INDEX startIndex, PIECE piece, const PIECE* grid) {
    // Init data
    this->m_startIndex = startIndex;
    this->m_piece = piece;
    
    // Copy grid data
    this->m_grid = grid;
    // Clear old moves
    this->m_validMoves.clear();
    
    // Add starting position no matter what
    this->addMove(this->m_startIndex);

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

PIECE MoveManager::isValidMove(INDEX testPos) {
    for (INDEX pos : this->m_validMoves) {
        if (testPos == Piece::getFlag(pos, MASK_LOCATION)) {
            Piece::addFlag(&this->m_piece, Piece::getFlag(pos, MASK_FLAGS));
            return this->m_piece;
        }
    }
    return 0;
}

PIECE MoveManager::isValidMove(const POINT& testPos) {
    return this->isValidMove(testPos.y * GRID_SIZE + testPos.x);
}

std::vector<INDEX> MoveManager::getMoves() {
    return this->m_validMoves;
}

std::vector<INDEX> MoveManager::getDefendingMoves() {
    return this->m_defendingMoves;
}

// ----- Read ----- Hidden -----

int MoveManager::addMove(INDEX index) {
    // Ensure index is valid
    if (0 > index || index >= GRID_SIZE * GRID_SIZE) {
        return MOVE_END;
    }
    
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
    // Colours match, move is defending
    else {
        this->m_defendingMoves.push_back(index);
    }
    return MOVE_END;
}

// ----- Update ----- Hidden -----

void MoveManager::calculateKingMoves(std::vector<INDEX>& enemyMoves) {
    // Top left to bottom right search
    for (int y = 1; y >= -1; y--) {
        for (int x = -1; x <= 1; x++) {
            int index = (this->m_startIndex) + (y * GRID_SIZE) + x;

            // Don't add start position
            if (y == 0 && x == 0) {
                continue;
            }

            // Top side check
            if ((this->m_startIndex / GRID_SIZE == GRID_SIZE - 1) && (y == 1)) {
                // Increase y to remove out of out of bounds error
                break;
            }
            // Bottom side check
            if ((this->m_startIndex / GRID_SIZE == 0) && (y == -1)) {
                // All points have been checked, break
                break;
            }
            // Right Side check
            if ((this->m_startIndex % GRID_SIZE == GRID_SIZE - 1) && (x == 1)) {
                continue;
            }
            // Left side check
            if ((this->m_startIndex % GRID_SIZE == 0) && (x == -1)) {
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
    
    this->calculateKingCastling();
}

void MoveManager::calculateKingCastling() { 
    // Check castling kingside
    bool canCastle = true;
    if (Piece::getFlag(this->m_piece, MASK_KING_CASTLE_KING)) {
        for (int i = this->m_startIndex + 1; i % GRID_SIZE < GRID_SIZE; i++) {
            // Checks if piece at index is a rook
            if (Piece::getFlag(this->m_grid[i], MASK_TYPE) == PIECE_ROOK) {
                // Check if rook has castling ability flag
                if (Piece::getFlag(this->m_grid[i], MASK_ROOK_CAN_CASTLE)) {
                    break;
                }
            }
            // If piece detected, cannot castle
            if (this->m_grid[i]) {
                canCastle = false;
                break;
            }
        }
        if (canCastle) {
            this->addMove(this->m_startIndex + 2);
        }
    }
    // Check castling queensize
    canCastle = true;
    if (Piece::getFlag(this->m_piece, MASK_KING_CASTLE_QUEEN)) {
        for (int i = this->m_startIndex - 1; i % GRID_SIZE >= 0; i--) {
            // Checks if piece at index is a rook
            if (Piece::getFlag(this->m_grid[i], MASK_TYPE) == PIECE_ROOK) {
                // Check if rook has castling ability flag
                if (Piece::getFlag(this->m_grid[i], MASK_ROOK_CAN_CASTLE)) {
                    break;
                }
            }
            // If piece detected, cannot castle
            if (this->m_grid[i]) {
                canCastle = false;
                break;
            }
        }
        if (canCastle) {
            this->addMove(this->m_startIndex - 2);
        }
    }
}

std::vector<INDEX> MoveManager::calculateEnemyMoves(FLAG colour) {
    std::vector<INDEX> moves;
    
    // Loop through grid
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        // Colours don't match, calculate moves
        if (!this->m_grid[i] || Piece::getFlag(this->m_grid[i], MASK_COLOUR) == colour) {
            continue;
        }
        // Call a movemanager for current piece
        MoveManager enemy;
        std::vector<INDEX> calculated;

        // Case for pawn
        if (Piece::getFlag(this->m_grid[i], MASK_TYPE) == PIECE_PAWN) {
            enemy.calculatePawnAttackingMoves(this->m_grid[i], i);
            calculated = enemy.getMoves();
        }
        // Case for king
        else if (Piece::getFlag(this->m_grid[i], MASK_TYPE) == PIECE_KING) {
            enemy.calculateKingAttackingMoves(this->m_grid[i], i);
            calculated = enemy.getMoves();
        }
        // Calculates moves for all other pieces
        else {
            enemy.calculateMoves(i, this->m_grid[i], this->m_grid);
            calculated = enemy.getMoves();
            auto def = enemy.getDefendingMoves();
            calculated.insert(calculated.end(), def.begin(), def.end());
        }
        // Remove first index, which is the current piece position
        

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
    int index = this->m_startIndex;
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
    int index = this->m_startIndex;
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
    int index = this->m_startIndex;
    
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
    int indexCheck = this->m_startIndex + GRID_SIZE;
    if (colour == PIECE_WHITE) {
        // Single move check
        if (this->m_grid[indexCheck] == 0) {
            this->m_validMoves.push_back(indexCheck);

            // Double move check
            if (this->m_piece & MASK_PAWN_FIRST_MOVE) {
                if (this->m_grid[indexCheck + GRID_SIZE] == 0) {
                    this->m_validMoves.push_back((indexCheck | MASK_PAWN_EN_PASSENT) + GRID_SIZE);
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
        indexCheck = this->m_startIndex - GRID_SIZE;
        // Single move check
        if (this->m_grid[indexCheck] == 0) {
            this->m_validMoves.push_back(indexCheck);

            // Double move check
            if (this->m_piece & MASK_PAWN_FIRST_MOVE) {
                if (this->m_grid[indexCheck - GRID_SIZE] == 0) {
                    this->m_validMoves.push_back((indexCheck | MASK_PAWN_EN_PASSENT) - GRID_SIZE);
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
}

// Attacking move functions

void MoveManager::calculatePawnAttackingMoves(PIECE piece, INDEX index) {
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

void MoveManager::calculateKingAttackingMoves(PIECE piece, INDEX index) {
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

