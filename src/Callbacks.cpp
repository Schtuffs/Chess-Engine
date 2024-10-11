#include "Callbacks.h"

#include "EventManager.h"
#include "FpsTracker.h"
#include "WindowManager.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Close on escape
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Display FPS on backtick
    if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
        FpsTracker::showFPS();
    }

    // Reset on r
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        WindowManager::reset();
    }
}

void character_callback(GLFWwindow* window, GLuint codepoint) {}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        EventManager::eventClick(WindowManager::cursorPos());
    }
}

void cursor_position_callback(GLFWwindow* window, double xPos, double yPos) {}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    WindowManager::resize(width, height);
}

void window_refresh_callback(GLFWwindow* window) {
    // Resizes the window, then renders screen again to update during resizing
    glfwGetFramebufferSize(window, NULL, NULL);
    WindowManager::show();
}

