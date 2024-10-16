#include "MoveManager.h"

#include "BoardManager.h"
#include "Piece.h"

// ----- Creation -----

MoveManager::MoveManager() {
    // Nothing todo
}

// ----- Read -----

Move MoveManager::isLegal(INDEX index) {
    for (Move& move : this->m_legalMoves) {
        // Tries to find specified move
        // If found, returns it
        if (index == move.getTarget()) {
            return move;
        }
    }
    // Returns an empty move if move not found
    return Move();
}

bool MoveManager::isLegal(Move& move) {
    for(Move& legal : this->m_legalMoves) {
        // Checks positions are the same
        if(move.getStart() != legal.getStart()) {
            continue;
        }
        if (move.getTarget() != legal.getTarget()) {
            continue;
        }
        // Adds flags from other moves
        move.addFlags(legal.getFlags());
        return true;
    }
    return false;
}

std::vector<Move> MoveManager::getValidMoves() {
    return this->m_validMoves;
}

std::vector<Move> MoveManager::getLegalMoves() {
    return this->m_legalMoves;
}

// ----- Update -----

bool MoveManager::calculateMoves(INDEX startIndex, const PIECE* grid, bool calculateEnemyMoves) {
    
    // Clears old moves and add start index
    this->clearValid();

    // Add start index to moves
    if (calculateEnemyMoves) {
        this->add(startIndex, startIndex, 0, grid);
    }
    
    bool capturedKing = this->calculateCardinalMoves(startIndex, grid);
    capturedKing |= this->calculateDiagonalMoves(startIndex, grid);
    capturedKing |= this->calculateKnightMoves(startIndex, grid);
    if (calculateEnemyMoves)
        capturedKing |= this->calculatePawnMoves(startIndex, grid);
    else
        capturedKing |= this->calculatePawnAttackMoves(startIndex, grid);
    capturedKing |= this->calculateKingMoves(startIndex, grid);

    // Calculated moves for enemies, removes moves that would result in capture of king
    if (calculateEnemyMoves) {
        this->calculateLegalMoves(startIndex, grid);
    }

    return capturedKing;
}

void MoveManager::clear() {
    this->clearValid();
    this->clearLegal();
}

void MoveManager::clearValid() {
    this->m_validMoves.clear();
}

void MoveManager::clearLegal() {
    this->m_legalMoves.clear();
}

// ----- Move ----- Calculation ----- Functions -----

void MoveManager::calculateLegalMoves(INDEX index, const PIECE* grid) {
    FLAG colour = Piece::getFlag(grid[index], MASK_COLOUR);
    auto moves = this->m_validMoves;

    // Copy over grid
    PIECE editGrid[GRID_SIZE * GRID_SIZE];
    for (INDEX i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        editGrid[i] = grid[i];
    }

    // Loop through each move and play it on the board
    for (Move& move : moves) {
        // Place pawn to block moves
        editGrid[move.getTarget()] = grid[move.getStart()];
        editGrid[move.getStart()] = 0;

        // Loop through each grid index
        bool capturedKing = false;
        for (INDEX i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
            // Checks colours aren't the same
            FLAG targetColour = Piece::getFlag(editGrid[i], MASK_COLOUR);
            if (colour != targetColour && targetColour) {
                // See if king is captured after the move
                if (this->calculateMoves(i, editGrid, false) == MOVE_KING_CAPTURED) {
                    // King was captured, break out of loop since move is illegal
                    capturedKing = true;
                    break;
                }
            }
        }

        if (!capturedKing) {
            // King not captured, move is legal
            this->addLegal(move);
        }

        // Place whatever was here back
        editGrid[move.getTarget()] = grid[move.getTarget()];
        editGrid[move.getStart()] = grid[move.getStart()];
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
            
            if (this->add(startIndex, target, 0, grid) == MOVE_CAPTURE_KING) {
                capturedKing = MOVE_KING_CAPTURED;
            }
        }
    }

    // Prevent castling moves from being calculated in check
    if (!Piece::getFlag(grid[startIndex], MOVE_CHECK)) {
        this->calculateKingCastling(startIndex, grid);
    }

    return capturedKing;
}

void MoveManager::calculateKingCastling(INDEX startIndex, const PIECE* grid) { 
    // Check castling kingside
    PIECE piece = grid[startIndex];
    
    bool canCastle = true;
    if (Piece::getFlag(piece, MOVE_KING_CASTLE_KING)) {
        for (INDEX i = startIndex + 1; i % GRID_SIZE < GRID_SIZE; i++) {
            // Checks if piece at index is a rook
            if (Piece::getFlag(grid[i], MASK_TYPE) == PIECE_ROOK) {
                // Check if rook has castling ability flag
                if (Piece::getFlag(grid[i], MOVE_ROOK_CAN_CASTLE)) {
                    break;
                }
            }
            // If piece detected, cannot castle
            if (grid[i]) {
                canCastle = false;
                break;
            }
        }
        // Add castling move to valid moves
        if (canCastle) {
            this->add(startIndex, startIndex + 2, MOVE_KING_CASTLE_KING, grid);
        }
    }
    // Check castling queensize
    canCastle = true;
    if (Piece::getFlag(piece, MOVE_KING_CASTLE_QUEEN)) {
        for (INDEX i = startIndex - 1; i % GRID_SIZE >= 0; i--) {
            // Checks if piece at index is a rook
            if (Piece::getFlag(grid[i], MASK_TYPE) == PIECE_ROOK) {
                // Check if rook has castling ability flag
                if (Piece::getFlag(grid[i], MOVE_ROOK_CAN_CASTLE)) {
                    break;
                }
            }
            // If piece detected, cannot castle
            if (grid[i]) {
                canCastle = false;
                break;
            }
        }
        // Add castling move to valid moves
        if (canCastle) {
            this->add(startIndex, startIndex - 2, MOVE_KING_CASTLE_QUEEN, grid);
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
                flag = this->add(startIndex, target, 0, grid);
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
                    flag = this->add(startIndex, target, 0, grid);
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
        (INDEX)(startIndex + (2 * GRID_SIZE) - 1),
        (INDEX)(startIndex + (2 * GRID_SIZE) + 1),
        (INDEX)(startIndex + (2) + GRID_SIZE),
        (INDEX)(startIndex + (2) - GRID_SIZE),
        (INDEX)(startIndex - (2 * GRID_SIZE) - 1),
        (INDEX)(startIndex - (2 * GRID_SIZE) + 1),
        (INDEX)(startIndex - (2) + GRID_SIZE),
        (INDEX)(startIndex - (2) - GRID_SIZE)
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
            int addResult = this->add(startIndex, movement[i], 0, grid);
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
    int addResult = this->add(startIndex, startIndex + indexCheck, 0, grid);
    if (addResult != MOVE_END) {
        // Move was not blocked, check double move
        if (Piece::hasFlag(grid[startIndex], MOVE_PAWN_FIRST_MOVE)) {
            this->add(startIndex, startIndex + (2 * indexCheck), MOVE_PAWN_MOVE_TWO, grid);
        }
    }

    // Left attack check
    addResult = this->add(startIndex, startIndex + indexCheck - 1, MOVE_PAWN_ATTACK, grid);
    if (addResult == MOVE_CAPTURE_KING) {
        capturedKing = MOVE_KING_CAPTURED;
    }

    // Right attack check
    addResult = this->add(startIndex, startIndex + indexCheck + 1, MOVE_PAWN_ATTACK, grid);
    if (addResult == MOVE_CAPTURE_KING) {
        capturedKing = MOVE_KING_CAPTURED;
    }

    return capturedKing;
}

bool MoveManager::calculatePawnAttackMoves(INDEX start, const PIECE* grid) {
    FLAG colour = Piece::getFlag(grid[start], MASK_COLOUR);
    INDEX indexCheck = (colour == PIECE_WHITE ? GRID_SIZE : (-GRID_SIZE));

    // Left attack check
    bool capturedKing = MOVE_KING_NOT_CAPTURED;
    Move moveleft(start, start + indexCheck - 1, MOVE_PAWN_ATTACK);
    this->m_validMoves.push_back(moveleft);
    if (Piece::getFlag(grid[moveleft.getTarget()], MASK_TYPE) == PIECE_KING) {
        capturedKing = MOVE_KING_CAPTURED;
    }

    // Right attack check
    Move moveright(start, start + indexCheck + 1, MOVE_PAWN_ATTACK);
    this->m_validMoves.push_back(moveright);
    if (Piece::getFlag(grid[moveright.getTarget()], MASK_TYPE) == PIECE_KING) {
        capturedKing = MOVE_KING_CAPTURED;
    }

    return capturedKing;
}



// ----- Move ----- List ----- Functions -----

FLAG MoveManager::add(INDEX start, INDEX target, FLAG flags, const PIECE* grid) {
    // Ensure index is valid
    if (0 > start || start >= GRID_SIZE * GRID_SIZE) {
        return MOVE_END;
    }
    if (0 > target || target >= GRID_SIZE * GRID_SIZE) {
        return MOVE_END;
    }

    // Always add start index as legal move
    if (start == target) {
        Move move(start, target, flags);
        this->addLegal(move);
        return MOVE_CONTINUE;
    }

    FLAG thisType = Piece::getFlag(grid[start], MASK_TYPE);
    FLAG targetType = Piece::getFlag(grid[target], MASK_TYPE);
    
    // Pawns are special with their capturing
    if (thisType == PIECE_PAWN) {
        return this->addPawn(start, target, flags, grid);
    }

    FLAG thisColour = Piece::getFlag(grid[start], MASK_COLOUR);
    FLAG targetColour = Piece::getFlag(grid[target], MASK_COLOUR);

    // No piece, add move
    if (grid[target] == 0 || grid[target] == PIECE_PHANTOM) {
        this->addValid(Move(start, target, flags));
        return MOVE_CONTINUE;
    }

    // Determine if different colour colour

    if (thisColour != targetColour) {
        // Check if move is attacking a king
        if (targetType == PIECE_KING) {
            // Attacking king, add flag
            this->addValid(Move(start, target, flags | MOVE_CHECK));
            return MOVE_CAPTURE_KING;
        }
        // Pieces are not the same colour, add move
        this->addValid(Move(start, target, flags));
    }
    return MOVE_END;
}

FLAG MoveManager::addPawn(INDEX start, INDEX target, FLAG flags, const PIECE* grid) {
    FLAG thisColour = Piece::getFlag(grid[start], MASK_COLOUR);
    FLAG targetColour = Piece::getFlag(grid[target], MASK_COLOUR);

    // Checks for pawn attack flag
    if ((flags & MOVE_PAWN_ATTACK)) {
        // Check colours are not equal and that the target has a piece
        // Alternatively, checks if the piece is a phantom
        if ((thisColour != targetColour && targetColour) || (grid[target] == PIECE_PHANTOM)) {
            this->addValid(Move(start, target, flags));
        }
        // Colours not equal, or no piece
        return MOVE_END;
    }
    // Checks theres no piece
    if (grid[target] == 0) {
        this->addValid(Move(start, target, flags));
        return MOVE_CONTINUE;
    }
    return MOVE_END;
}

void MoveManager::addValid(Move move) {
    m_validMoves.push_back(move);
}

void MoveManager::addLegal(Move& move) {
    this->m_legalMoves.push_back(move);
}

// ----- Destruction -----

MoveManager::~MoveManager() {
    // Nothing todo
}

