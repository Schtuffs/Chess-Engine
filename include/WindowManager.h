#pragma once
#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Callbacks.h"

namespace WindowManager {
    GLFWwindow* init();
    void background(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void initCallbacks(GLFWwindow* window);
}

