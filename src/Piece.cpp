#include "Piece.h"

#include "BoardManager.h"
#include "EventManager.h"
    
namespace {

    void removePawnFlags(INDEX index, PIECE* grid) {
        // Removes first move mask
        Piece::removeFlag(&grid[index], MASK_PAWN_FIRST_MOVE);
        // Remove en passent
        Piece::removeFlag(&grid[index], MASK_PAWN_EN_PASSENT);

        // Check if pawn has reached the edge of the board
            // White Pawn                             // Black Pawn
        if (((index / GRID_SIZE) == GRID_SIZE - 1) || ((index / GRID_SIZE) == 0)) {
            // Adds promotion event
            EventManager::eventPromotion(index);
        }
    }

    void removeKnightFlags(INDEX index, PIECE* grid){ 

    }

    void removeBishopFlags(INDEX index, PIECE* grid) {

    }

    void removeRookFlags(INDEX index, PIECE* grid) {
        Piece::removeFlag(&grid[index], MASK_ROOK_CAN_CASTLE);
    }

    void removeQueenFlags(INDEX index, PIECE* grid) {

    }

    void removeKingFlags(INDEX index, PIECE* grid) {
    // Check if king can castle
    if(!Piece::getFlag(grid[index], MASK_KING_CASTLING)) {
        // No castling, return
        return;
    }

    // King starts on the right, halfway across the board
    INDEX kingStartX = GRID_SIZE / 2;
    int movedSquares = kingStartX - (index % GRID_SIZE);
    // Detecs if king moved more that 1 square
    if (abs(movedSquares) > 1) {
        // Negative (King side rook), king has castling rights on this side
        if (movedSquares < 0 && Piece::getFlag(grid[index], MASK_KING_CASTLE_KING)) {
            INDEX rookIndex = (index + movedSquares) + ((GRID_SIZE - 1) / 2);
            grid[index - 1] = grid[rookIndex];
            grid[rookIndex] = 0;
        }
        // Positive (Queen side rook), king has castling rights on this side
        else if (movedSquares > 0 && Piece::getFlag(grid[index], MASK_KING_CASTLE_QUEEN)) {
            INDEX rookIndex = (index + movedSquares) - (GRID_SIZE / 2);
            grid[index + 1] = grid[rookIndex];
            grid[rookIndex] = 0;
        }
    }

    // Finally, remove all castling rights always
    Piece::removeFlag(&grid[index], MASK_KING_CASTLE_KING);
    Piece::removeFlag(&grid[index], MASK_KING_CASTLE_QUEEN);
}

}

void Piece::addFlag(PIECE* piece, FLAG flag) {
    *piece |= flag;
}

void Piece::removeFlag(PIECE* piece, FLAG flag) {
    if ((*piece & flag)) {
        // If the flag has multiple attributes, all have to be added to ensure proper removal
        *piece |= flag;
        *piece ^= flag;
    }
}

bool Piece::hasFlag(PIECE piece, FLAG flag) {
    return ((piece & flag));
}

FLAG Piece::getFlag(PIECE piece, FLAG flag) {
    return (piece & flag);
}

void Piece::removeFlags(INDEX index, PIECE* grid) {
    // Piece information
    FLAG type = Piece::getFlag(grid[index], MASK_TYPE);
    switch (type) {
    case PIECE_PAWN:
        ::removePawnFlags(index, grid);
        break;
    case PIECE_KNIGHT:
        ::removeKnightFlags(index, grid);
        break;
    case PIECE_BISHOP:
        ::removeBishopFlags(index, grid);
        break;
    case PIECE_ROOK:
        ::removeRookFlags(index, grid);
        break;
    case PIECE_QUEEN:
        ::removeQueenFlags(index, grid);
        break;
    case PIECE_KING:
        ::removeKingFlags(index, grid);
        break;
    }
}

void Piece::Debug(PIECE piece) {
    // Piece type
    FLAG type = Piece::getFlag(piece, MASK_TYPE);
    std::cout << "Type: ";
    switch (type) {
    case PIECE_PAWN:
        std::cout << "Pawn" << std::endl;
        break;
    case PIECE_KNIGHT:
        std::cout << "Knight" << std::endl;
        break;
    case PIECE_BISHOP:
        std::cout << "Bishop" << std::endl;
        break;
    case PIECE_ROOK:
        std::cout << "Rook" << std::endl;
        break;
    case PIECE_QUEEN:
        std::cout << "Queen" << std::endl;
        break;
    case PIECE_KING:
        std::cout << "King" << std::endl;
        break;
    default:
        std::cout << "Unkown" << std::endl;
        break;
    }

    // Piece colour
    FLAG colour = Piece::getFlag(piece, MASK_COLOUR);
    std::cout << "Colour: " << (colour == PIECE_WHITE ? "White" : (colour == PIECE_BLACK) ? "Black" : "Unknown") << std::endl;

    // Held flag
    FLAG held = Piece::getFlag(piece, MASK_HELD);
    std::cout << "Held: " << (held ? "True" : "False") << std::endl;

    // If piece has first flag
    FLAG flag1 = Piece::getFlag(piece, MASK_FLAG_1);
    std::cout << "Flag1: " << (flag1 ? "True" : "False") << std::endl;

    // If piece has second flag
    FLAG flag2 = Piece::getFlag(piece, MASK_FLAG_2);
    std::cout << "Flag2: " << (flag2 ? "True" : "False") << std::endl;
}

