#include "WindowManager.h"

#include "Callbacks.h"
#include "FpsTracker.h"

namespace WindowManager {
    static POINT s_winSize;
    static GLFWwindow* s_window;
    static double s_prevTime = 0;

    // Holds pointer to board for showing
    static BoardManager* s_board;

    // Creation

    void init(int winSizeParam) {
        // Sets window to defined size
        s_winSize = { winSizeParam, winSizeParam };

        // Init glfw
        if (!glfwInit()) {
            std::cout << "Could not initialize glfw..." << std::endl;
            exit(EXIT_FAILURE);
        }

        // GLFW v3.3, CORE
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // For debugging
        // glfwWindowHint(GLFW_FLOATING, true);

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
        glfwSetWindowRefreshCallback(s_window, window_refresh_callback);
    }

    // Reading

    bool shouldClose() {
        return glfwWindowShouldClose(s_window);
    }

    POINT winSize() {
        return s_winSize;
    }

    void show(bool updateAlways) {
        // Only renders and swaps buffers at WINDOW_MAX_FPS to save GPU
        // Allows for other calculations to happen more
        double current = clock();
        double deltaTime = ((double)CLOCKS_PER_SEC / (double)WINDOW_MAX_FPS) - (double)(current - s_prevTime);
        if ((0 >= deltaTime) || (updateAlways)) {
            // Accounts for extra offset of frame times
            // If the frame took an extra 0.01 clocks,
            // The current time is set back by that 0.01 to allow
            // For proper counting of frames
            // Ex: s_prevTime = 1800 + (-0.23);
            // s_prevTime = 1799.77
            s_prevTime = current + deltaTime;
            s_board->show();
            glfwSwapBuffers(s_window);
        }
    }

    void poll() {
        glfwPollEvents();
    }

    void cursorPos(double& x, double& y) {
        glfwGetCursorPos(s_window, &x, &y);
    }

    POINT cursorPos() {
        double x, y;
        glfwGetCursorPos(s_window, &x, &y);
        return POINT{ (int)x, (int)y };
    }
    
    // Useful functions

    void resize(int width, int height) {
        s_winSize.x = width;
        s_winSize.y = height;
    }

    void setBoard(BoardManager& board) {
        s_board = &board;
    }
    
    void reset() {
        s_board->resetBoard();
    }
    
    // Destruction

    void close() {
        glfwDestroyWindow(s_window);
        glfwTerminate();
    }
}

