#pragma once

#include <array>
#include <atomic>
#include <string>
#include <string_view>
#include <utility>

#include "Types/Position.h"
#include "Utils/Constants.h"

namespace Engine {
// Read

/**
 * @brief Get engines name.
 * @return Engines name.
 * @date 2026-06-20
 */
std::string Name();

/**
 * @brief Get authors name.
 * @return Authors name.
 * @date 2026-06-20
 */
std::string Author();

// Returns printable state of game.
std::string GetState();

// Check if engine should be searching right now or not.
bool IsSearching();

// Update

// UCI

/**
 * @brief Flips the current player.
 * @date 2026-06-20
 */
void Flip();

/**
 * @brief Blocking call for waiting on engine task completion.
 * @date 2026-06-21
 */
void Ready();

// Setup the board state with given data.
bool SetState(std::string_view data);

// Begin searching for best move with given params.
void Search(std::string_view data);

// Allows search to send move to make.
void MakeMove(std::string_view data);

// Stops engine ASAP.
void Stop();
}; // namespace Engine
