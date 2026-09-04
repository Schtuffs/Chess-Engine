#pragma once

#include "Engine.h"
#include "Types/Position.h"

namespace Search {
void Begin(const Position& position, Engine::SearchParams params);

// Stops search ASAP.
void Stop();
}; // namespace Search
