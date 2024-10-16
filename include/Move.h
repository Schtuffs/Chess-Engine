#pragma once

#include <iostream>
#include <vector>

#include "Library.h"

class Move {
private:
    // Stores the data for a move
    unsigned short m_moveData;

public:
    Move(INDEX start = 0, INDEX target = 0, FLAG flags = 0);

    // Returns start index of move
    INDEX getStart();

    // Returns target index (AKA where move is going)
    INDEX getTarget();

    // Returns flag data
    FLAG getFlags();

    // Returns if this move exists or not
    bool isMove();

    ~Move();
};

