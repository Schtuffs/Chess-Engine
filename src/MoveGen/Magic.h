#pragma once

#include "Constants.h"

namespace Magic {
Bitboard GetKingAttacks(Index index, Index king, bool isRook);
Bitboard GetSlidingAttacks(Index index, Bitboard blockers, bool isRook);
} // namespace Magic
