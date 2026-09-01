#pragma once

#include <array>
#include <atomic>
#include <string>
#include <string_view>
#include <utility>

#include "Types/Position.h"
#include "Utils/Constants.h"

namespace Engine {

// ----- Update -----

// ----- UCI -----

// Allows engine to quit.
void Quit();

// Enables UCI.
void Uci();

// Synchronize the engine.
void IsReady();

// Set engine options.
void SetOption(std::stringstream ss);

// Setup the board state with given data.
void SetPosition(std::stringstream ss);

// Begin searching for best move with given params.
void Go(std::stringstream ss);

// Stops engine ASAP.
void Stop();

// Hits the ponder.
void PonderHit();

// ----- Custom -----

// Prints position state.
void D();

// Swap player to move.
void Flip();

// Help menu.
void Help();

// ----- Non UCI -----

// Check if engine should be searching right now or not.
bool IsSearching();

}; // namespace Engine
