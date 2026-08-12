#pragma once

#include "Engine.h"

/**
 * @brief Manages the UCI protocol.
 * @class UCI
 * @date 2026-06-20
 */
class UCI {
public:
    // Creation / Destruction

    /**
     * @brief Creates a UCI manager.
     * @param argc Just incase.
     * @param argv Just incase.
     * @date 2026-06-20
     */
    UCI(int argc, char** argv);

    /**
     * @brief Cleanup.
     * @date 2026-06-20
     */
    ~UCI();

    // Read

    // Update

    /**
     * @brief Main loop to manage UCI.
     * @date 2026-06-20
     */
    void Loop();

private:
    Engine m_engine;
};
