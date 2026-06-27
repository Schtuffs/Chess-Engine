#pragma once

#include <string>
#include <string_view>

#include "Constants.h"

/**
 * @brief The main calculation engine.
 * @class Engine
 * @author Kyle Wagler
 * @date 2026-06-20
 */
class Engine {
    public:
    
    // Creation / Destruction
    
    /**
     * @brief Creates base engine.
     * @author Kyle Wagler
     * @date 2026-06-20
     */
    Engine();

    /**
     * @brief Cleanup.
     * @author Kyle Wagler
     * @date 2026-06-20
     */
    ~Engine();
    
    // Read
    
    /**
     * @brief Get engines name.
     * @return Engines name.
     * @author Kyle Wagler
     * @date 2026-06-20
     */
    std::string Name();
    
    /**
     * @brief Get authors name.
     * @return Authors name.
     * @author Kyle Wagler
     * @date 2026-06-20
     */
    std::string Author();
    
    // Update
    
    // UCI
    
    /**
     * @brief Flips the current player.
     * @author Kyle Wagler
     * @date 2026-06-20
     */
    void Flip();
    
    /**
     * @brief Blocking call for waiting on engine task completion.
     * @author Kyle Wagler
     * @date 2026-06-21
     */
    void Ready();
    
    /**
     * @brief Get authors name.
     * @param data Contains the FEN and move data.
     * @author Kyle Wagler
     * @date 2026-06-20
     */
    bool SetState(std::string_view data);
    
    /**
     * @brief Get authors name.
     * @author Kyle Wagler
     * @date 2026-06-20
     */
    void Stop();

private:
    std::string m_name, m_author, m_fen;
    bool m_isWhite;
    bool m_toggles;
    u16 m_halfMoves;
};

