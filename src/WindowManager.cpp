#include "WindowManager.h"

namespace WindowManager {
    static POINT s_winSize;
    static GLFWwindow* s_window;

    // Creation

    void init() {
        s_winSize = { 800, 800 };

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
        GLFWwindow* window = glfwCreateWindow(s_winSize.x, s_winSize.y, "Chess Engine", NULL, NULL);
        if (!window) {
            std::cout << "Could not create window..." << std::endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        s_window = window;

        glfwMakeContextCurrent(window);

        // Remove V-Sync
        glfwSwapInterval(0);

        // Glad initialization
        gladLoadGL();
        glViewport(0, 0, s_winSize.x, s_winSize.y);

        // Setting blendmode so .png have blank backgrounds
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    void initCallbacks() {
        glfwSetKeyCallback(s_window, key_callback);
    	glfwSetCursorPosCallback(s_window, cursor_position_callback);
        glfwSetMouseButtonCallback(s_window, mouse_button_callback);
    	glfwSetCharCallback(s_window, character_callback);
        glfwSetFramebufferSizeCallback(s_window, framebuffer_size_callback);
    }

    // Reading

    bool shouldClose() {
        return glfwWindowShouldClose(s_window);
    }

    POINT winSize() {
        return s_winSize;
    }

    void swap() {
        glfwSwapBuffers(s_window);
    }

    void poll() {
        glfwPollEvents();
    }

    void cursorPos(double& x, double& y) {
        glfwGetCursorPos(s_window, &x, &y);
    }

    // Useful functions

    void resize(int width, int height) {
        s_winSize.x = width;
        s_winSize.y = height;
    }

    // Destruction

    void close() {
        glfwDestroyWindow(s_window);
        glfwTerminate();
    }

}

