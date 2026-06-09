#include <stdint.h>

#include "raylib.h"

#include "Constants.h"
#include "Utils.h"
#include "Piece.h"

constexpr Color BOARD_SQUARE_LIGHT      = {200, 150, 120, 255};
constexpr Color BOARD_SQUARE_DARK       = {100, 75, 60, 255};
constexpr const char* START_FEN         = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr uint64_t BOARD_SQUARE_COUNT   = 8;

void DrawBoard(int width, int height) {
    // Determine size of squares
    uint32_t size = Utils::min(width, height);
    size = size / BOARD_SQUARE_COUNT;
    Dimensions::TILE_SIZE = size;

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

void DrawPieces(const char* fen, uint64_t width, uint64_t height) {
    // Ignore the obvious performance issues with everything in this function
    // Also the globals are attrocious
    static Piece pieces[12] = {
        { Enums::Colour::Black, Enums::Type::Bishop },  { Enums::Colour::White, Enums::Type::Bishop },
        { Enums::Colour::Black, Enums::Type::King },    { Enums::Colour::White, Enums::Type::King },
        { Enums::Colour::Black, Enums::Type::Knight },  { Enums::Colour::White, Enums::Type::Knight },
        { Enums::Colour::Black, Enums::Type::Pawn },    { Enums::Colour::White, Enums::Type::Pawn },
        { Enums::Colour::Black, Enums::Type::Queen },   { Enums::Colour::White, Enums::Type::Queen },
        { Enums::Colour::Black, Enums::Type::Rook },    { Enums::Colour::White, Enums::Type::Rook },
    };
    (void)fen;
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

    // Black pieces
    pieces[10 + 0].Render(startX + Dimensions::TILE_SIZE * 0, startY + Dimensions::TILE_SIZE * 0);
    pieces[ 4 + 0].Render(startX + Dimensions::TILE_SIZE * 1, startY + Dimensions::TILE_SIZE * 0);
    pieces[ 0 + 0].Render(startX + Dimensions::TILE_SIZE * 2, startY + Dimensions::TILE_SIZE * 0);
    pieces[ 8 + 0].Render(startX + Dimensions::TILE_SIZE * 3, startY + Dimensions::TILE_SIZE * 0);
    pieces[ 2 + 0].Render(startX + Dimensions::TILE_SIZE * 4, startY + Dimensions::TILE_SIZE * 0);
    pieces[ 0 + 0].Render(startX + Dimensions::TILE_SIZE * 5, startY + Dimensions::TILE_SIZE * 0);
    pieces[ 4 + 0].Render(startX + Dimensions::TILE_SIZE * 6, startY + Dimensions::TILE_SIZE * 0);
    pieces[10 + 0].Render(startX + Dimensions::TILE_SIZE * 7, startY + Dimensions::TILE_SIZE * 0);
    pieces[ 6 + 0].Render(startX + Dimensions::TILE_SIZE * 0, startY + Dimensions::TILE_SIZE * 1);
    pieces[ 6 + 0].Render(startX + Dimensions::TILE_SIZE * 1, startY + Dimensions::TILE_SIZE * 1);
    pieces[ 6 + 0].Render(startX + Dimensions::TILE_SIZE * 2, startY + Dimensions::TILE_SIZE * 1);
    pieces[ 6 + 0].Render(startX + Dimensions::TILE_SIZE * 3, startY + Dimensions::TILE_SIZE * 1);
    pieces[ 6 + 0].Render(startX + Dimensions::TILE_SIZE * 4, startY + Dimensions::TILE_SIZE * 1);
    pieces[ 6 + 0].Render(startX + Dimensions::TILE_SIZE * 5, startY + Dimensions::TILE_SIZE * 1);
    pieces[ 6 + 0].Render(startX + Dimensions::TILE_SIZE * 6, startY + Dimensions::TILE_SIZE * 1);
    pieces[ 6 + 0].Render(startX + Dimensions::TILE_SIZE * 7, startY + Dimensions::TILE_SIZE * 1);
    
    // White pieces
    pieces[ 6 + 1].Render(startX + Dimensions::TILE_SIZE * 0, startY + Dimensions::TILE_SIZE * 6);
    pieces[ 6 + 1].Render(startX + Dimensions::TILE_SIZE * 1, startY + Dimensions::TILE_SIZE * 6);
    pieces[ 6 + 1].Render(startX + Dimensions::TILE_SIZE * 2, startY + Dimensions::TILE_SIZE * 6);
    pieces[ 6 + 1].Render(startX + Dimensions::TILE_SIZE * 3, startY + Dimensions::TILE_SIZE * 6);
    pieces[ 6 + 1].Render(startX + Dimensions::TILE_SIZE * 4, startY + Dimensions::TILE_SIZE * 6);
    pieces[ 6 + 1].Render(startX + Dimensions::TILE_SIZE * 5, startY + Dimensions::TILE_SIZE * 6);
    pieces[ 6 + 1].Render(startX + Dimensions::TILE_SIZE * 6, startY + Dimensions::TILE_SIZE * 6);
    pieces[ 6 + 1].Render(startX + Dimensions::TILE_SIZE * 7, startY + Dimensions::TILE_SIZE * 6);
    pieces[10 + 1].Render(startX + Dimensions::TILE_SIZE * 0, startY + Dimensions::TILE_SIZE * 7);
    pieces[ 4 + 1].Render(startX + Dimensions::TILE_SIZE * 1, startY + Dimensions::TILE_SIZE * 7);
    pieces[ 0 + 1].Render(startX + Dimensions::TILE_SIZE * 2, startY + Dimensions::TILE_SIZE * 7);
    pieces[ 8 + 1].Render(startX + Dimensions::TILE_SIZE * 3, startY + Dimensions::TILE_SIZE * 7);
    pieces[ 2 + 1].Render(startX + Dimensions::TILE_SIZE * 4, startY + Dimensions::TILE_SIZE * 7);
    pieces[ 0 + 1].Render(startX + Dimensions::TILE_SIZE * 5, startY + Dimensions::TILE_SIZE * 7);
    pieces[ 4 + 1].Render(startX + Dimensions::TILE_SIZE * 6, startY + Dimensions::TILE_SIZE * 7);
    pieces[10 + 1].Render(startX + Dimensions::TILE_SIZE * 7, startY + Dimensions::TILE_SIZE * 7);
}

int main(void) {
    // Prepare window
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(800, 800, "Chess Engine");
    SetTargetFPS(60);
    ToggleBorderlessWindowed();

    // Prepare globals
    uint64_t monitor = GetCurrentMonitor();
    Dimensions::SCREEN_WIDTH  = GetMonitorWidth (monitor);
    Dimensions::SCREEN_HEIGHT = GetMonitorHeight(monitor);
    
    // Main loop
    while (!WindowShouldClose()) {
        ClearWindowState(FLAG_WINDOW_TOPMOST);
        uint64_t width = GetMonitorWidth(monitor);
        uint64_t height = GetMonitorHeight(monitor);

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);
        DrawBoard(width, height);
        DrawPieces(START_FEN, width, height);
        DrawFPS(0, 0);
        EndDrawing();
    }
    
    // Cleanup
    CloseWindow();
    
    return 0;
}

