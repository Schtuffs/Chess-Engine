#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "WindowManager.h"
#include "RenderManager.h"
#include "Pawn.h"
#include "FpsTracker.h"

int main(void) {
    // Window initialization functions
    GLFWwindow* window = WindowManager::init();
    WindowManager::initCallbacks(window);

    // Create renderer
    RenderManager renderManager;
    if (!renderManager.is_created()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // Main window loop
    FpsTracker fps;
    while(!glfwWindowShouldClose(window)) {
        // FPS counter so I can optimize a few things
        fps.fps();
        
        // Set background and draw board
        WindowManager::background(1.0, 1.0, 1.0, 1.0);
        renderManager.board();
        

        // OpenGL functions
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Free all alocated memory
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

