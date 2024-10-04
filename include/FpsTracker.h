#pragma once

#include <iostream>
#include <chrono>
#include <ctime>

class FpsTracker {
private:
    unsigned long long m_fps, m_average, m_total, m_count;
    clock_t m_prev_clock;

public:
    FpsTracker();

    void fps();
    static void showFPS();
    
    ~FpsTracker();
};

