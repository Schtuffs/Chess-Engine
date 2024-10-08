#pragma once

#include <iostream>
#include <chrono>
#include <ctime>

// Counts clocks/second for program
namespace FpsTracker {
    // Calculates and shows current and average FPS
    void fps();

    // Changes state of FPS counter to show FPS
    void showFPS();
};

