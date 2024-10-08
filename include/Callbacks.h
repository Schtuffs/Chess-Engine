#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "FpsTracker.h"
#include "WindowManager.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void character_callback(GLFWwindow* window, GLuint codepoint);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xPos, double yPos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

