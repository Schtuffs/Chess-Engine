#include <stdint.h>

#include "raylib.h"

#include "Constants.h"
#include "Utils.h"
#include "Renderer.h"

constexpr Color BOARD_SQUARE_LIGHT      = {175, 150, 120, 255};
constexpr Color BOARD_SQUARE_DARK       = {100, 75, 60, 255};

int main(void)
{
    // Prepare window
    SetTraceLogLevel(LOG_WARNING);
    // SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(800, 800, "Chess Engine");
    ToggleBorderlessWindowed();
    SetTargetFPS(60);

    // Prepare states
    Renderer renderer;
    
    // Main loop
    bool perspective = true;
    while (!WindowShouldClose()) {
        // Flips board
        if (IsKeyPressed(KEY_F)) {
            perspective = !perspective;
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);
        renderer.RenderBoard(BOARD_SQUARE_DARK, BOARD_SQUARE_LIGHT);
        renderer.RenderPieces(DEFAULT_FEN, perspective);
        DrawFPS(0, 0);
        EndDrawing();
    }
    
    // Cleanup
    CloseWindow();
    
    return 0;
}

