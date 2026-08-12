#pragma once

#include <array>
#include <atomic>
#include <string>
#include <string_view>
#include <utility>

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

    void Search(std::string_view data);

    /**
     * @brief Get authors name.
     * @date 2026-06-20
     */
    void Stop();

private:
    static constexpr u64 INVALID_VALUE = 0xff'ff'ff'ff'ff'ff'ff'ff;
    // clang-format off
    static constexpr std::array<std::pair<std::string_view, u16>, 12> SEARCH_PARAMS = {
        std::pair{"wtime",       0x00'01},
        std::pair{"btime",       0x00'02},
        std::pair{"winc",        0x00'04},
        std::pair{"binc",        0x00'08},
        std::pair{"movestogo",   0x00'10},
        std::pair{"movetime",    0x00'20},

        std::pair{"depth",       0x00'40},
        std::pair{"nodes",       0x00'80},
        std::pair{"mate",        0x01'00},
        std::pair{"infinite",    0x02'00},

        std::pair{"ponder",      0x04'00},
        std::pair{"searchmoves", 0x08'00},
    };
    // clang-format on

    std::string m_name, m_author;
    bool        m_isWhiteTurn;
    u16         m_halfMoves;
    Board       m_board;

    std::atomic<bool>                   m_isSearching; // Search algorithm can change this when done
    std::atomic<bool>                   m_stopSearching; // Main thread can call this to end engine
    std::pair<u16, std::array<u64, 12>> m_searchParams;

    u64 GetSearchParamKey(std::string_view str) const noexcept;
    u64 GetSearchParamValue(std::string_view str) const noexcept;
    bool ValidValue(u64 val) const noexcept;
    u64 KeyIndex(u64 key) const noexcept;
};
