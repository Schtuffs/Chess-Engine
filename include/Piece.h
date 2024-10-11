#pragma once

#include "Defines.h"

namespace Piece {
    void removeFlags(INDEX index, PIECE* grid);
    void addFlag(PIECE* piece, FLAG flag);
    void removeFlag(PIECE* piece, FLAG flag);
    bool hasFlag(PIECE piece, FLAG flag);
    FLAG getFlag(PIECE piece, FLAG flag);

    void Debug(PIECE piece);
}

