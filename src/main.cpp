#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <vector>

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

    // Manual board setup with pawns
    Pawn wpawn1(PIECE_WHITE, 1, 2);
    Pawn wpawn2(PIECE_WHITE, 2, 2);
    Pawn wpawn3(PIECE_WHITE, 3, 2);
    Pawn wpawn4(PIECE_WHITE, 4, 2);
    Pawn wpawn5(PIECE_WHITE, 5, 2);
    Pawn wpawn6(PIECE_WHITE, 6, 2);
    Pawn wpawn7(PIECE_WHITE, 7, 2);
    Pawn wpawn8(PIECE_WHITE, 8, 2);

    /*Knight wknight1(PIECE_WHITE, 100, 000);
    Knight wknight2(PIECE_WHITE, 600, 000);
    Bishop wbishop1(PIECE_WHITE, 200, 000);
    Bishop wbishop2(PIECE_WHITE, 500, 000);
    Rook wrook1    (PIECE_WHITE, 000, 000);
    Rook wrook2    (PIECE_WHITE, 700, 000);
    Queen wqueen   (PIECE_WHITE, 300, 000);
    King wking     (PIECE_WHITE, 400, 000);*/

    Pawn bpawn1(PIECE_BLACK, 1, 7);
    Pawn bpawn2(PIECE_BLACK, 2, 7);
    Pawn bpawn3(PIECE_BLACK, 3, 7);
    Pawn bpawn4(PIECE_BLACK, 4, 7);
    Pawn bpawn5(PIECE_BLACK, 5, 7);
    Pawn bpawn6(PIECE_BLACK, 6, 7);
    Pawn bpawn7(PIECE_BLACK, 7, 7);
    Pawn bpawn8(PIECE_BLACK, 8, 7);

    /*Knight bknight1(PIECE_BLACK, 100, 700);
    Knight bknight2(PIECE_BLACK, 600, 700);
    Bishop bbishop1(PIECE_BLACK, 200, 700);
    Bishop bbishop2(PIECE_BLACK, 500, 700);
    Rook brook1    (PIECE_BLACK, 000, 700);
    Rook brook2    (PIECE_BLACK, 700, 700);
    Queen bqueen   (PIECE_BLACK, 300, 700);
    King bking     (PIECE_BLACK, 400, 700);*/

    // Main window loop
    FpsTracker fps;
    while(!glfwWindowShouldClose(window)) {
        // FPS counter so I can optimize a few things
        fps.fps();
        
        // Set background and draw board
        WindowManager::background(1.0, 1.0, 1.0, 1.0);
        renderManager.board();
        
        // Rendering
        renderManager.render(wpawn1);
        renderManager.render(wpawn2);
        renderManager.render(wpawn3);
        renderManager.render(wpawn4);
        renderManager.render(wpawn5);
        renderManager.render(wpawn6);
        renderManager.render(wpawn7);
        renderManager.render(wpawn8);

        renderManager.render(bpawn1);
        renderManager.render(bpawn2);
        renderManager.render(bpawn3);
        renderManager.render(bpawn4);
        renderManager.render(bpawn5);
        renderManager.render(bpawn6);
        renderManager.render(bpawn7);
        renderManager.render(bpawn8);

        // OpenGL functions
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Free all alocated memory
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

