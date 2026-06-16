/*------------------------------

===== TODO =====

Renderer:
- Texture resizing for FixSize

Board:
- MakeMove needs to validate
- Needs piece movement
- Move calculation logic

------------------------------*/

#include <cstdio>

#include "raylib.h"

#include "Board.h"
#include "Button.h"
#include "Constants.h"
#include "Renderer.h"
#include "Utils.h"

constexpr Color BOARD_SQUARE_LIGHT      = {175, 150, 120, 255};
constexpr Color BOARD_SQUARE_DARK       = {100, 75, 60, 255};

void RenderMainMenu(Enums::Screen& screen)
{
    constexpr Color DEFAULT_COLOUR          = WHITE;
    constexpr Color DEFAULT_COLOUR_HOVER    = GRAY;
    static Renderer board;
    static Button btnNewGame("Start new game", {GetFontDefault(), BLACK, Utils::max(GetScreenWidth() / 100, 10)}, SCREEN_PERCENTAGES_4(0.1, 0.1375, 0.8, 0.1), DEFAULT_COLOUR);
    if (IsWindowResized()) {
        btnNewGame
            .Dimension(SCREEN_PERCENTAGES_4(0.1, 0.1375, 0.8, 0.1))
            .Thickness(3.f)
            .Font({GetFontDefault(), BLACK, Utils::max(GetScreenWidth() / 100, 10)});
    }
    if (btnNewGame.IsHovered()) {
        btnNewGame.ColourInside(DEFAULT_COLOUR_HOVER);
    } else {
        btnNewGame.ColourInside(DEFAULT_COLOUR);
    }
    if (btnNewGame.IsClicked()) {
        screen = Enums::Screen::NewGame;
    }

    board.RenderBoard(BOARD_SQUARE_DARK, BOARD_SQUARE_LIGHT);
    btnNewGame.Render();
}

void RenderNewGameMenu(Enums::Screen& screen)
{
    constexpr Color DEFAULT_COLOUR          = WHITE;
    constexpr Color DEFAULT_COLOUR_HOVER    = GRAY;
    static Button btnNewGame("Start new game", {GetFontDefault(), BLACK, Utils::max(GetScreenWidth() / 100, 10)}, SCREEN_PERCENTAGES_4(0.1, 0.1375, 0.8, 0.1), DEFAULT_COLOUR);
    if (IsWindowResized()) {
        btnNewGame
            .Dimension(SCREEN_PERCENTAGES_4(0.1, 0.1375, 0.8, 0.1))
            .Thickness(3.f)
            .Font({GetFontDefault(), BLACK, Utils::max(GetScreenWidth() / 100, 10)});
    }
    if (btnNewGame.IsHovered()) {
        btnNewGame.ColourInside(DEFAULT_COLOUR_HOVER);
    } else {
        btnNewGame.ColourInside(DEFAULT_COLOUR);
    }
    if (btnNewGame.IsClicked()) {
        screen = Enums::Screen::Game;
    }
    btnNewGame.Render();
}

void RenderGame(Enums::Screen& screen)
{
    // Prepare states
    static Renderer renderer;
    static Board board(DEFAULT_FEN);
    static bool isWhitePerspective = true;
    if (IsKeyPressed(KEY_F)) {
        std::printf("Heyo!\n");
        isWhitePerspective = !isWhitePerspective;
    }
    
    renderer.RenderBoard(BOARD_SQUARE_DARK, BOARD_SQUARE_LIGHT);
    renderer.RenderPieces(DEFAULT_FEN, isWhitePerspective);
    (void)screen;
}

void RenderSettings(Enums::Screen& screen)
{
    (void)screen;
}

int main(void)
{
    // Prepare window
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(120, 120, "Chess Engine");
    SetWindowMinSize(200, 200);
    // ToggleBorderlessWindowed();
    SetTargetFPS(60);

    // Main loop
    Enums::Screen currentScreen = Enums::Screen::Menu;
    bool shouldExitGame = false, inDebugMode = false;
    while (!WindowShouldClose() && !shouldExitGame) {
        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);
        
        switch(currentScreen) {
            case Enums::Screen::Menu: {
                RenderMainMenu(currentScreen);
                break;
            }
            case Enums::Screen::NewGame: {
                RenderNewGameMenu(currentScreen);
                break;
            }
            case Enums::Screen::Game: {
                RenderGame(currentScreen);
                break;
            }
            case Enums::Screen::Settings: {
                RenderSettings(currentScreen);
                break;
            }
            default: {
                char text[30]{};
                snprintf(text, sizeof(text), "Invalid screen selected: %d", currentScreen);
                int fontSize = Utils::max(GetScreenWidth() / 100, 20);

                Font font = GetFontDefault();
                Vector2 pos = Utils::CenterText(text, font, fontSize, {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f});

                // Calculate horizontal center
                // int textWidth = MeasureText(text, fontSize);
                // int posX = (GetScreenWidth() / 2) - (textWidth / 2);
                // int posY = GetScreenHeight() / 2;

                DrawText(text, pos.x, pos.y, fontSize, WHITE);
                break;
            }
        }

        if (inDebugMode) {
            DrawFPS(0, 0);
        }

        EndDrawing();
    }
    
    // Cleanup
    CloseWindow();
    
    return 0;
}

