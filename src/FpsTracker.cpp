#include "FpsTracker.h"

namespace FpsTracker {
    // Setup variables
    static int s_fps = 0, s_average = 0, s_count = 0;
    static unsigned long long s_total = 0;
    static clock_t s_prev_clock = 0;
    static bool showFps = false;

    void fps() {
        // Return if off
        if (!showFps) {
            return;
        }

        // Determine if it has been enough clocks since last display
        s_fps++;
        clock_t currentClock = clock();
        if (currentClock - s_prev_clock >= CLOCKS_PER_SEC) {
            // Prevent first FPS of 1 to affect the average
            if (s_fps == 1 && s_average == 0) {
                s_prev_clock = currentClock;
                return;
            }

            // Display the total fps
            s_prev_clock = currentClock;
            std::cout << "FPS: " << s_fps;

            // Calculate out average fps over program runtime
            s_total += s_fps;
            s_count++;
            s_average = s_total / s_count;
            std::cout << ", Average: " << s_average << std::endl;

            // Reset FPS count
            s_fps = 0;
        }
    }

    void showFPS() {
        showFps = !showFps;
    }
}

