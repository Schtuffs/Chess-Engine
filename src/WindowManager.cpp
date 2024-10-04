#include "WindowManager.h"

// Does simple initialization functions
// Will automatically end program if anything goes wrong
// Window created is put into current context automatically
// Glad is initialized as well
GLFWwindow* WindowManager::init() {
    // Init glfw
    if (!glfwInit()) {
        std::cout << "Could not initialize glfw..." << std::endl;
        exit(EXIT_FAILURE);
    }

    // GLFW v3.3, CORE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window
    GLFWwindow* window = glfwCreateWindow(800, 800, "Chess Engine", NULL, NULL);
    if (!window) {
        std::cout << "Could not create window..." << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, 800, 800);
    
    return window;
}

void WindowManager::background(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void WindowManager::initCallbacks(GLFWwindow* window) {
    glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetCharCallback(window, character_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

