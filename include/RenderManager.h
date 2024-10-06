#pragma once

#include <iostream>
#include <fstream>

#include "Piece.h"

#define GRID_SIZE   8

class RenderManager {
private:
    GLuint shaderID, vao, vbo, ebo;
    bool created;

    // Automatically deals with texture ids
    static GLuint texSlot;
    static POINT winSize;

    std::string read(const std::string& filename);
    void compileErrors(GLuint id, GLuint type);
    
    template <typename t>
    t map(t value, t currentMin, t currentMax, t newMin, t newMax);
    GLfloat min(int x, int y);
    GLfloat min(POINT pos);
    void rect(int x, int y, int width, int height);

public:
    RenderManager(const std::string& vertFile = "../lib/default.vert", const std::string& fragFile = "../lib/default.frag");
    bool is_created();

    static GLuint genTex(GLuint type, GLuint colour);
    static GLuint genVAO();
    static GLuint genVBO();
    static GLuint genEBO();
    static void resize(int width, int height);

    void board();
    void render(Piece& piece);
    GLuint id();
    
    ~RenderManager();
};

