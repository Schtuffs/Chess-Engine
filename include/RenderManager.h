#pragma once

#include <iostream>
#include <fstream>

#include "Piece.h"

#define GRID_SIZE   8

class RenderManager {
private:
    GLuint shaderID, VAO, VBO, EBO;
    bool created;

    std::string read(const std::string& filename);
    template <typename t>
    t map(t value, t currentMin, t currentMax, t newMin, t newMax);
    void rect(int x, int y, int width, int height);

public:
    RenderManager(const std::string& vertFile = "../lib/default.vert", const std::string& fragFile = "../lib/default.frag");
    bool is_created();

    static GLuint genVBO();

    void board();
    void render(Piece& piece);
    
    ~RenderManager();
};

