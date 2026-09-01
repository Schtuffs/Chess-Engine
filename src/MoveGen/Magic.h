#pragma once

#include "Types/BitBoard.h"
#include "Types/Types.h"

namespace Magic {
BitBoard GetKingAttacks(Square sq, Square king, bool isRook);
template <PieceType>
BitBoard GetAttacks(Square sq, BitBoard = 0, Colour player = COLOUR_TOTAL);
} // namespace Magic
