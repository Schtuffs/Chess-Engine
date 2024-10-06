#include "FpsTracker.h"

bool FpsTracker::showFps = false;

FpsTracker::FpsTracker() {
    // For main calculation
    this->m_fps = 0;
    this->m_prev_clock = 0;

    // For average calculation
    this->m_average = 0;
    this->m_total = 0;
    this->m_count = 0;
}

void FpsTracker::fps() {
    if (!this->showFps) {
        return;
    }

    this->m_fps++;
    clock_t currentClock = clock();
    if (currentClock - this->m_prev_clock >= CLOCKS_PER_SEC) {
        // Prevent first FPS of 1 to affect the average
        if (this->m_fps == 1 && this->m_average == 0) {
            this->m_prev_clock = currentClock;
            return;
        }

        // Display the total fps
        this->m_prev_clock = currentClock;
        std::cout << "FPS: " << this->m_fps;

        // Calculate out average fps over program runtime
        this->m_total += this->m_fps;
        this->m_count++;
        this->m_average = this->m_total / this->m_count;
        std::cout << ", Average: " << this->m_average << std::endl;
        
        // Reset FPS count
        this->m_fps = 0;
    }
}

void FpsTracker::showFPS() {
    showFps = !showFps;
}

FpsTracker::~FpsTracker() {
    // Nothing todo
}

