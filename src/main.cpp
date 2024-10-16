#include <iostream>
#include <glad/glad.h>
#include <vector>

#include "BoardManager.h"
#include "Player.h"
#include "WindowManager.h"
#include "EventManager.h"
#include "FpsTracker.h"

// Need to add
// Stalemate with only knight/bishop - insufficient materials
// 50 move rule implementation
// 3-fold repitition

int main(void) {
    // Window initialization functions
    WindowManager::init(WINDOW_SIZE_REGULAR);
    WindowManager::initCallbacks();

    // Create players for board
    Player white(PLAYER_COLOUR_WHITE);
    Player black(PLAYER_COLOUR_BLACK);

    // Create board manager
    BoardManager board(white, black, BOARD_GREEN_CREAM);
    WindowManager::setBoard(board);

    // Create EventManager
    EventManager events;
    events.setBoard(&board);

    // Main window loop
    while(!WindowManager::shouldClose()) {
        // FPS counter so I can optimize a few things
        FpsTracker::fps();

        // Checks for any actions needed to be taken
        events.manageEvents();

        // OpenGL functions
        WindowManager::show();
        WindowManager::poll();
    }

    WindowManager::close();

    return 0;
}

