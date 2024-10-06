#include "Callbacks.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
        FpsTracker::showFPS();
    }
}

void character_callback(GLFWwindow* window, GLuint codepoint) {}

void cursor_position_callback(GLFWwindow* window, double xPos, double yPos) {}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    RenderManager::resize(width, height);
} 

