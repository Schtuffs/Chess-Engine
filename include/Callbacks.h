#pragma once
#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "FpsTracker.h"
#include "RenderManager.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void character_callback(GLFWwindow* window, GLuint codepoint);
void cursor_position_callback(GLFWwindow* window, double xPos, double yPos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

