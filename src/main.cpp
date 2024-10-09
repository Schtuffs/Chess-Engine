#include <iostream>
#include <glad/glad.h>
#include <vector>

#include "BoardManager.h"
#include "WindowManager.h"
#include "FpsTracker.h"

int main(void) {
    // Window initialization functions
    WindowManager::init();
    WindowManager::initCallbacks();

    // Create board manager
    BoardManager boardManager(BOARD_RED_GOLD);

    // Main window loop
    while(!WindowManager::shouldClose()) {
        // FPS counter so I can optimize a few things
        FpsTracker::fps();

        // Checks for any actions needed to be taken
        boardManager.check();
        
        // Show board and pieces
        boardManager.show();

        // OpenGL functions
        WindowManager::swap();
        WindowManager::poll();
    }

    WindowManager::close();

    return 0;
}

