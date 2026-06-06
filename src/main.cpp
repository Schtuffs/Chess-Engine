#include <cstdint>

#include "raylib.h"

#include "Utils.h"

constexpr Color BOARD_SQUARE_LIGHT      = Color(200, 150, 120, 255);
constexpr Color BOARD_SQUARE_DARK       = Color(100, 75, 60, 255);
constexpr uint64_t BOARD_SQUARE_COUNT   = 8;

void DrawBoard(uint64_t width, uint64_t height) {
    // Determine size of squares
    uint32_t size = Utils::min(width, height);
    size = size / BOARD_SQUARE_COUNT;

    // Calculate start position
    uint32_t sizeX  = width  - size * BOARD_SQUARE_COUNT;
    uint32_t sizeY  = height - size * BOARD_SQUARE_COUNT;
    uint32_t startX = sizeX / 2;
    uint32_t startY = sizeY / 2;

    // Loop through grid squares
    for (uint64_t i = 0; i < BOARD_SQUARE_COUNT; i++) {
        for (uint64_t j = 0; j < BOARD_SQUARE_COUNT; j++) {
            Color colour = BOARD_SQUARE_DARK;
            if ((i + j) % 2 == 0) {
                colour = BOARD_SQUARE_LIGHT;
            }

            DrawRectangle(i * size + startX, j * size + startY, size, size, colour);
        }
    }
}

int main(void) {
    // Prepare window
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(800, 800, "Chess Engine");
    SetTargetFPS(60);
    ToggleBorderlessWindowed();
    
    // Main loop
    uint64_t monitor = GetCurrentMonitor();
    while (!WindowShouldClose()) {
        ClearWindowState(FLAG_WINDOW_TOPMOST);
        uint64_t width = GetMonitorWidth(monitor);
        uint64_t height = GetMonitorHeight(monitor);

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);
        DrawBoard(width, height);
        DrawFPS(0, 0);
        EndDrawing();
    }
    
    // Cleanup
    CloseWindow();
    
    return 0;
}

