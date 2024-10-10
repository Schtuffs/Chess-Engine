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
    BoardManager board(BOARD_DBLUE_LBLUE);
    WindowManager::setBoard(board);

    // Main window loop
    while(!WindowManager::shouldClose()) {
        // FPS counter so I can optimize a few things
        FpsTracker::fps();

        // Checks for any actions needed to be taken
        board.check();

        // OpenGL functions
        WindowManager::show();
        WindowManager::poll();
    }

    WindowManager::close();

    return 0;
}

