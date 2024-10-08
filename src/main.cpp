#include <iostream>
#include <glad/glad.h>
#include <vector>

#include "WindowManager.h"
#include "BoardManager.h"
#include "Piece.h"
#include "FpsTracker.h"

int main(void) {
    // Window initialization functions
    WindowManager::init();
    WindowManager::initCallbacks();

    // Create board manager
    BoardManager boardManager(BOARD_FILL_AUTOMATIC, BOARD_DBLUE_LBLUE);

    // Main window loop
    while(!WindowManager::shouldClose()) {
        // FPS counter so I can optimize a few things
        FpsTracker::fps();

        // Show board and pieces
        boardManager.show();

        // OpenGL functions
        WindowManager::swap();
        WindowManager::poll();
    }

    WindowManager::close();

    return 0;
}

