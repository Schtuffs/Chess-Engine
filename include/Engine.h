#pragma once

#include <atomic>
#include <string>
#include <string_view>

#include "Board.h"
#include "Constants.h"

/**
 * @brief The main calculation engine.
 * @class Engine
 * @date 2026-06-20
 */
class Engine {
public:
    // Creation / Destruction

    /**
     * @brief Creates base engine.
     * @date 2026-06-20
     */
    Engine();

    /**
     * @brief Cleanup.
     * @date 2026-06-20
     */
    ~Engine();

    // Read

    /**
     * @brief Get engines name.
     * @return Engines name.
     * @date 2026-06-20
     */
    std::string Name() const noexcept;

    /**
     * @brief Get authors name.
     * @return Authors name.
     * @date 2026-06-20
     */
    std::string Author() const noexcept;

    // Returns printable state of game.
    std::string GetState() const noexcept;

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

    /**
     * @brief Get authors name.
     * @param data Contains the FEN and move data.
     * @date 2026-06-20
     */
    bool SetState(std::string_view data);

    std::string Search();

    /**
     * @brief Get authors name.
     * @date 2026-06-20
     */
    void Stop();

private:
    std::string m_name, m_author;
    bool        m_isWhiteTurn;
    u16         m_halfMoves;
    Board       m_board;
};
