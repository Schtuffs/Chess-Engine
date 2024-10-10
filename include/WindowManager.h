#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Library.h"
#include "BoardManager.h"

namespace WindowManager {
    // ----- Creation -----

    // Does simple initialization functions
    // Will automatically end program if anything goes wrong
    // Window created is automatically put into main context
    // Glad is initialized as well
    void init();

    // ----- Reading -----

    // Returns if the window should close or not
    // Will return true when the window should close
    bool shouldClose();

    // Gives the dimensions of the window in pixels
    POINT winSize();

    // Swap buffers, renders the window
    void show();

    // Poll window events and process them
    void poll();

    // Gives data for current mouse position
    void cursorPos(double& x, double& y);

    // Returns current cursor position
    POINT cursorPos();

    // ----- Useful -----

    // Initialize callback functions for window
    // Allows processing of user input
    void initCallbacks();
    
    // Sets board for window to render
    void setBoard(BoardManager& board);

    // Stores the window size for easy access later
    void resize(int width, int height);

    // Allows quick reset of board
    void reset();

    // ----- Destruction -----

    // Closes OpenGL functions and frees the memory
    void close();
}

