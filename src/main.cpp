#include <iostream>
#include <glad/glad.h>
#include <vector>

#include "Piece.h"
#include "BoardManager.h"

int main(void) {
    // Window initialization functions
    WindowManager::init();
    WindowManager::initCallbacks();

    // Create board manager
    BoardManager boardManager(BOARD_BROWN_BROWN);

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

