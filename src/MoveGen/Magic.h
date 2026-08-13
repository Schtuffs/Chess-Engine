#pragma once

#include "Constants.h"

namespace Magic {
BitBoard GetKingAttacks(Index index, Index king, bool isRook);
BitBoard GetSlidingAttacks(Index index, BitBoard blockers, bool isRook);
} // namespace Magic
