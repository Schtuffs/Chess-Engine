#pragma once

#include <iostream>
#include <vector>

#include "Library.h"
#include "Defines.h"
#include "Move.h"

class MoveManager {
private:
    std::vector<Move> m_validMoves, m_legalMoves;
    
    // ----- Move ----- Calculation ----- Functions -----

    // Calculates and determines if enemy captured king
    void calculateLegalMoves(INDEX colour, const PIECE* grid);
    
    // Calculates moves for king
    bool calculateKingMoves(INDEX startIndex, const PIECE* grid);

    // Calculates the potential castling moves for the king
    void calculateKingCastling(INDEX startIndex, const PIECE* grid);

    // Cardinal movement generation
    // Returns true if no moves capture a king
    // Returns false immediately upon king capture, should not happen and must be dealt with
    bool calculateCardinalMoves(INDEX startIndex, const PIECE* grid);

    // Diagonal movement generation
    // Returns true if no moves capture a king
    // Returns false immediately upon king capture, should not happen and must be dealt with
    bool calculateDiagonalMoves(INDEX startIndex, const PIECE* grid);

    // Calculates moves for knight hops
    // Returns true if no moves capture a king
    // Returns false immediately upon king capture, should not happen and must be dealt with
    bool calculateKnightMoves(INDEX startIndex, const PIECE* grid);

    // Calculates moves for pawns
    // Returns true if no moves capture a king
    // Returns false immediately upon king capture, should not happen and must be dealt with
    bool calculatePawnMoves(INDEX startIndex, const PIECE* grid);

    // Calculates the attacking move for pawns
    // Needed for preventing king from walking into pawn attack
    bool calculatePawnAttackMoves(INDEX start, const PIECE* grid);



    // ----- Move ----- List ----- Functions -----

    // Call this function to add a move to the move list
    FLAG add(INDEX start, INDEX target, FLAG flags, const PIECE* grid);

    // Pawn is a special case, has its own add logic
    FLAG addPawn(INDEX start, INDEX target, FLAG flags, const PIECE* grid);

    // Called by add function, it decides which list to add to
    void addValid(Move move);

    // Called by add function, it decides which list to add to
    void addLegal(Move& move);

    void clearValid();
    void clearLegal();

public:
    // ----- Creation -----

    MoveManager();

    // ----- Read -----

    // Returns a move if the move is legal
    Move isLegal(INDEX index);
    bool isLegal(Move& move);

    // Gets unchecked valid moves
    std::vector<Move> getValidMoves();
    
    // Gets calculated legal moves
    std::vector<Move> getLegalMoves();

    // ----- Update -----
    
    // Calculates all valid moves for given piece
    bool calculateMoves(INDEX startIndex, const PIECE* grid, bool calculateEnemyMoves = false);

    // Clears moves after piece is moved
    void clear();

    // ----- Destruction -----

    ~MoveManager();
};

