#pragma once

#include <iostream>
#include <fstream>
#include <glad/glad.h>

#include "Piece.h"
#include "Library.h"
#include "BindManager.h"
#include "WindowManager.h"

// Manages the rendering of items to the screen, such as squares or pieces
class RenderManager {
private:
    GLuint m_shaderTexID, m_shaderColID, m_vao, m_vbo, m_ebo;
    bool m_created;

    // ----- Creation -----

    // Reads the .vert and .frag files and returns their contents
    std::string read(const std::string& filename);

    // Checks that shader files and shader program compiler properly
    void compileErrors(GLuint id, GLuint type);

public:
    // ----- Creation -----
    RenderManager(const std::string& vertFile = "../lib/default.vert", const std::string& fragTexFile = "../lib/defaultTex.frag", const std::string& fragColFile = "../lib/defaultCol.frag");

    // For checking that shader files were properly read
    // Returns if they were
    bool is_created();

    // ----- Render -----

    // Renders background with specified colour
    void background(COLOUR& colour);

    // Creates a square at given pixel coordinates with specified height
    void rect(COLOUR& colour, int x, int y, int width, int height);

    // Renders a piece to the screen
    void render(Piece& piece);

    // ----- Destruction -----

    // Deletes this objects rendering buffers
    ~RenderManager();
};

// Deletes rendering buffers of a piece
void DeleteBufferObjects(Piece& piece);

