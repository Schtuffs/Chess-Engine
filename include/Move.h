#pragma once

#include <iostream>
#include <vector>

#include "Defines.h"

class Move {
private:
    // Stores the data for a move
    short m_moveData;

public:
    Move(INDEX start = -1, INDEX target = 0, FLAG flags = 0);

    // Returns start index of move
    INDEX Start();

    // Returns target index (AKA where move is going)
    INDEX Target();

    // Returns flag data
    FLAG Flags();
    // Returns specific flag data
    FLAG Flags(FLAG flag);

    // Adds a flag to the move
    void addFlags(FLAG flags);

    // Returns if this move exists or not
    bool isMove();

    ~Move();
};

