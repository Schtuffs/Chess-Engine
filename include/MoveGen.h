#pragma once

#include <iostream>
#include <vector>

#include "Defines.h"
#include "Move.h"

class MoveGen {
private:
    static std::vector<Move> s_validMoves, s_legalMoves;

    // ----- Move ----- Calculation ----- Functions -----

    // Calculates and determines if enemy captured king
    static void calculateLegalMoves(FLAG colour, const PIECE* grid);
    
    // Calculates moves for king
    static bool calculateKingMoves(INDEX startIndex, const PIECE* grid);

    // Calculates the potential castling moves for the king
    static void calculateKingCastling(INDEX startIndex, const PIECE* grid);

    // Cardinal movement generation
    // Returns true if no moves capture a king
    static bool calculateCardinalMoves(INDEX startIndex, const PIECE* grid);

    // Diagonal movement generation
    // Returns true if no moves capture a king
    static bool calculateDiagonalMoves(INDEX startIndex, const PIECE* grid);

    // Calculates moves for knight hops
    // Returns true if no moves capture a king
    static bool calculateKnightMoves(INDEX startIndex, const PIECE* grid);

    // Calculates moves for pawns
    // Returns true if no moves capture a king
    static bool calculatePawnMoves(INDEX startIndex, const PIECE* grid);

    // Calculates the attacking move for pawns
    // Needed for preventing king from walking into pawn attack
    static bool calculatePawnAttackMoves(INDEX start, const PIECE* grid);

    // ----- Move ----- List ----- Functions -----

    // Call this function to add a move to the move list
    static FLAG add(INDEX start, INDEX target, FLAG flags, const PIECE* grid);
    static FLAG add(Move move, const PIECE* grid);

    // Pawn is a special case, has its own add logic
    static FLAG addPawn(INDEX start, INDEX target, FLAG flags, const PIECE* grid);

    // Called by add function, it decides which list to add to
    static void addValid(Move move);

    // Called by add function, it decides which list to add to
    static void addLegal(Move& move);

public:
    // Returns a single move with check flag if king captured when not calculating legal
    static std::vector<Move> generate(FLAG colour, const PIECE* grid, bool calculateLegal);
};

