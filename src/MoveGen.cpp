#include "MoveGen.h"

#include "Piece.h"
#include "Library.h"

std::vector<Move> MoveGen::s_validMoves;
std::vector<Move> MoveGen::s_legalMoves;

std::vector<Move> MoveGen::generate(FLAG colour, const PIECE* grid, bool calculateLegal) {
    // Clears old moves
    s_validMoves.clear();

    bool capturedKing = false;
    for (INDEX i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        // Determine if piece colour matches to generate moves
        if (Piece::getFlag(grid[i], MASK_COLOUR) != colour) {
            continue;
        }

        // Add start square
        if (calculateLegal) {
            MoveGen::add(i, i, 0, grid);
        }

        capturedKing |= calculateCardinalMoves(i, grid);
        capturedKing |= calculateDiagonalMoves(i, grid);
        capturedKing |= calculateKnightMoves(i, grid);
        if (calculateLegal) {
            capturedKing |= calculatePawnMoves(i, grid);
        }
        else {
            capturedKing |= calculatePawnAttackMoves(i, grid);
        }
        capturedKing |= calculateKingMoves(i, grid);
        
        // If calculating legal moves and king is captured, return a single move with a check
        if (!calculateLegal && capturedKing) {
            Move move(0, 0, MOVE_CHECK);
            std::vector<Move> temp;
            temp.push_back(move);
            return temp;
        }
    }

    // Returns legal moves only for the original move generation call
    if (calculateLegal) {
        MoveGen::calculateLegalMoves(colour, grid);
        auto moves = s_legalMoves;
        s_legalMoves.clear();
        return moves;
    }

    return s_validMoves;
}

void MoveGen::calculateLegalMoves(FLAG colour, const PIECE* grid) {
    auto moves = s_validMoves;

    // Copy over grid
    PIECE editGrid[GRID_SIZE * GRID_SIZE];
    for (INDEX i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        editGrid[i] = grid[i];
    }

    // Loop through each move and play it on the board
    for (Move& move : moves) {
        // Play move to determine legality
        editGrid[move.Target()] = grid[move.Start()];
        editGrid[move.Start()] = 0;

        // Loop through each grid index
        for (INDEX i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
            // Checks colours aren't the same
            FLAG targetColour = Piece::getFlag(editGrid[i], MASK_COLOUR);
            if (colour != targetColour && targetColour) {
                auto returned = MoveGen::generate(targetColour, editGrid, false);
                // Checks if returned object was the check
                if (!(returned.size() == 1 && returned[0].Flags(MOVE_CHECK))) {
                    addLegal(move);
                }
            }
        }

        // Place whatever was here back
        editGrid[move.Target()] = grid[move.Target()];
        editGrid[move.Start()] = grid[move.Start()];
    }
}

bool MoveGen::calculateKingMoves(INDEX startIndex, const PIECE* grid) {
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
            if (MoveGen::add(startIndex, target, 0, grid) == MOVE_CAPTURE_KING) {
                capturedKing = MOVE_KING_CAPTURED;
            }
        }
    }

    // Prevent castling moves from being calculated in check
    if (Piece::getFlag(grid[startIndex], MOVE_CHECK) != MOVE_CHECK) {
        calculateKingCastling(startIndex, grid);
    }

    return capturedKing;
}

void MoveGen::calculateKingCastling(INDEX startIndex, const PIECE* grid) { 
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
            add(startIndex, startIndex + 2, MOVE_KING_CASTLE_KING, grid);
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
            add(startIndex, startIndex - 2, MOVE_KING_CASTLE_QUEEN, grid);
        }
    }
}

bool MoveGen::calculateCardinalMoves(INDEX startIndex, const PIECE* grid) {
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
                flag = add(startIndex, target, 0, grid);
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

bool MoveGen::calculateDiagonalMoves(INDEX startIndex, const PIECE* grid) {
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
                    flag = add(startIndex, target, 0, grid);
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

bool MoveGen::calculateKnightMoves(INDEX startIndex, const PIECE* grid) {
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
        (INDEX)(startIndex - (2 * GRID_SIZE) + 1),
        (INDEX)(startIndex - (2 * GRID_SIZE) - 1),
        (INDEX)(startIndex - (2) - GRID_SIZE),
        (INDEX)(startIndex - (2) + GRID_SIZE)
    };

    bool preChecks[] = {
        startIndex % GRID_SIZE != 0,
        startIndex % GRID_SIZE != GRID_SIZE - 1,
        startIndex % GRID_SIZE < GRID_SIZE - 2,
        startIndex % GRID_SIZE < GRID_SIZE - 2,
        startIndex % GRID_SIZE != GRID_SIZE - 1,
        startIndex % GRID_SIZE != 0,
        startIndex % GRID_SIZE > 1,
        startIndex % GRID_SIZE > 1
    };

    // Loop though all of the different moves and conditions
    bool capturedKing = MOVE_KING_NOT_CAPTURED;
    for (int i = 0; i < 8; i++) {
        if(preChecks[i]) {
            int addResult = add(startIndex, movement[i], 0, grid);
            if (addResult == MOVE_CAPTURE_KING) {
                capturedKing = MOVE_KING_CAPTURED;
            }
        }
    }

    return capturedKing;
}

bool MoveGen::calculatePawnMoves(INDEX startIndex, const PIECE* grid) {
    // Type must be pawn
    FLAG type = Piece::getFlag(grid[startIndex], MASK_TYPE);
    if (type != PIECE_PAWN) {
        return MOVE_KING_NOT_CAPTURED;
    }

    bool capturedKing = MOVE_KING_NOT_CAPTURED;
    
    FLAG colour = Piece::getFlag(grid[startIndex], MASK_COLOUR);
    INDEX indexCheck = (colour == PIECE_WHITE ? GRID_SIZE : (-GRID_SIZE));
    // Single move check
    int addResult = add(startIndex, startIndex + indexCheck, 0, grid);
    if (addResult != MOVE_END) {
        // Move was not blocked, check double move
        if (Piece::hasFlag(grid[startIndex], MOVE_PAWN_FIRST_MOVE)) {
            add(startIndex, startIndex + (2 * indexCheck), MOVE_PAWN_MOVE_TWO, grid);
        }
    }

    // Left attack check
    addResult = add(startIndex, startIndex + indexCheck - 1, MOVE_PAWN_ATTACK, grid);
    if (addResult == MOVE_CAPTURE_KING) {
        capturedKing = MOVE_KING_CAPTURED;
    }

    // Right attack check
    addResult = add(startIndex, startIndex + indexCheck + 1, MOVE_PAWN_ATTACK, grid);
    if (addResult == MOVE_CAPTURE_KING) {
        capturedKing = MOVE_KING_CAPTURED;
    }

    return capturedKing;
}

bool MoveGen::calculatePawnAttackMoves(INDEX start, const PIECE* grid) {
    // Type must be pawn
    FLAG type = Piece::getFlag(grid[start], MASK_TYPE);
    if (type != PIECE_PAWN) {
        return MOVE_KING_NOT_CAPTURED;
    }

    FLAG colour = Piece::getFlag(grid[start], MASK_COLOUR);
    INDEX indexCheck = (colour == PIECE_WHITE ? GRID_SIZE : (-GRID_SIZE));

    // Left attack check
    bool capturedKing = MOVE_KING_NOT_CAPTURED;
    Move moveleft(start, start + indexCheck - 1, MOVE_PAWN_ATTACK);
    s_validMoves.push_back(moveleft);
    if (Piece::getFlag(grid[moveleft.Target()], MASK_TYPE) == PIECE_KING) {
        capturedKing = MOVE_KING_CAPTURED;
    }

    // Right attack check
    Move moveright(start, start + indexCheck + 1, MOVE_PAWN_ATTACK);
    s_validMoves.push_back(moveright);
    if (Piece::getFlag(grid[moveright.Target()], MASK_TYPE) == PIECE_KING) {
        capturedKing = MOVE_KING_CAPTURED;
    }

    return capturedKing;
}

// ----- Move ----- List ----- Functions -----

FLAG MoveGen::add(INDEX start, INDEX target, FLAG flags, const PIECE* grid) {
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
        addLegal(move);
        return MOVE_CONTINUE;
    }

    FLAG thisType = Piece::getFlag(grid[start], MASK_TYPE);
    FLAG targetType = Piece::getFlag(grid[target], MASK_TYPE);
    
    // Pawns are special with their capturing
    if (thisType == PIECE_PAWN) {
        return addPawn(start, target, flags, grid);
    }

    // No piece, add move
    if (grid[target] == 0 || grid[target] == PIECE_PHANTOM) {
        addValid(Move(start, target, flags));
        return MOVE_CONTINUE;
    }

    // Determine if different colour colour
    FLAG thisColour = Piece::getFlag(grid[start], MASK_COLOUR);
    FLAG targetColour = Piece::getFlag(grid[target], MASK_COLOUR);
    if (thisColour != targetColour) {
        // Check if move is attacking a king
        if (targetType == PIECE_KING) {
            // Attacking king, add flag
            addValid(Move(start, target, flags | MOVE_CHECK));
            return MOVE_CAPTURE_KING;
        }
        // Pieces are not the same colour, add move
        addValid(Move(start, target, flags));
    }
    return MOVE_END;
}

FLAG MoveGen::addPawn(INDEX start, INDEX target, FLAG flags, const PIECE* grid) {
    FLAG thisColour = Piece::getFlag(grid[start], MASK_COLOUR);
    FLAG targetColour = Piece::getFlag(grid[target], MASK_COLOUR);

    // Checks for pawn attack flag
    if ((flags & MOVE_PAWN_ATTACK)) {
        // Check colours are not equal and that the target has a piece
        // Alternatively, checks if the piece is a phantom
        if ((thisColour != targetColour && targetColour) || (grid[target] == PIECE_PHANTOM)) {
            addValid(Move(start, target, flags));
        }
        // Colours not equal, or no piece
        return MOVE_END;
    }
    // Checks theres no piece
    if (grid[target] == 0) {
        addValid(Move(start, target, flags));
        return MOVE_CONTINUE;
    }
    return MOVE_END;
}

void MoveGen::addValid(Move move) {
    for (Move& valid : s_validMoves) {
        if (move == valid) {
            return;
        }
    }
    s_validMoves.push_back(move);
}

void MoveGen::addLegal(Move& move) {
    for (Move& legal : s_legalMoves) {
        if (move == legal) {
            return;
        }
    }
    s_legalMoves.push_back(move);
}



