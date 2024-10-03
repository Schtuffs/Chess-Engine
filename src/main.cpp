#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "WindowManager.h"

int main(void) {
    // Window initialization functions
    GLFWwindow* window = WindowManager::init();
    WindowManager::initCallbacks(window);

    // Main window loop
    while(!glfwWindowShouldClose(window)) {
        WindowManager::background(1.0, 1.0, 1.0, 1.0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Free all alocated memory
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

