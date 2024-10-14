#pragma once

#include <iostream>
#include <vector>

#include "Library.h"

class Move {
private:
    std::vector<MOVE> m_validMoves, m_legalMoves;

    void p_addValid(INDEX startIndex, INDEX targetIndex, FLAG flags);
    int p_addPawn(INDEX startIndex, INDEX targetIndex, FLAG flags, const PIECE* grid);

public:
    Move();

    int addValid(INDEX startIndex, INDEX targetIndex, FLAG flags, const PIECE* grid);
    void addLegal(MOVE move);
    MOVE isLegal(INDEX testPos);

    std::vector<MOVE> getValidMoves();
    std::vector<MOVE> getLegalMoves();

    static INDEX getStart(MOVE move);
    static INDEX getTarget(MOVE move);
    static FLAG getFlags(MOVE move);
    static FLAG getFlag(MOVE move, FLAG flag);

    void clearValid();
    void clearLegal();

    ~Move();
};

