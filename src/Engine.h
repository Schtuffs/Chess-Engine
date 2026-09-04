#pragma once

#include <array>
#include <atomic>
#include <string>
#include <string_view>
#include <utility>

#include "Types/Position.h"
#include "Utils/Constants.h"

namespace Engine {

// Holds data for search engine to use to find moves
struct SearchParams {
    u64 wtime;
    u64 btime;
    u64 winc;
    u64 binc;
    u64 movestogo;
    u64 movetime;

    u64 depth;
    u64 nodes;
    u64 mate;
    u64 infinite;

    u64 ponder;
    u64 searchmoves;
};

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

}; // namespace Engine
