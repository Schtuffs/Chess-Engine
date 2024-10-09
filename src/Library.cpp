#include "Library.h"

GLuint Library::texSlot = GL_TEXTURE0;

// OpenGL-based functions

GLuint Library::genTex(GLuint type, GLuint colour) {
    // Error checking for incorrectly created pieces
    if (!type || !colour) {
        return 0;
    }

    // Create string for loading image
    std::string path = "../lib/pieces/";
    switch (type) {
    case PIECE_PAWN:
        path += "pawn";
        break;
    case PIECE_KNIGHT:
        path += "knight";
        break;
    case PIECE_BISHOP:
        path += "bishop";
        break;
    case PIECE_ROOK:
        path += "rook";
        break;
    case PIECE_QUEEN:
        path += "queen";
        break;
    case PIECE_KING:
        path += "king";
        break;
    }
    path += "_";

    switch (colour) {
    case PIECE_BLACK:
        path += "black.png";
        break;
    case PIECE_WHITE:
        path += "white.png";
        break;
    }

    GLuint texture;
    
    // Generating texture
    glGenTextures(1, &texture);

    // Set texture slot
    glActiveTexture(texSlot);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Setup MipMap function choices
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // load and generate the texture
    int width, height, channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
        return 0;
    }
    
    // Free and unbind
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

GLuint Library::genVAO() {
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    return VAO;
}

GLuint Library::genVBO() {
    GLuint VBO;
    glGenBuffers(1, &VBO);
    return VBO;
}

GLuint Library::genEBO() {
    GLuint EBO;
    glGenBuffers(1, &EBO);
    return EBO;
}

// Quick calculations

GLfloat Library::min(int x, int y) {
    GLfloat small = x;
    if (small > y)
        small = y;

    return small;
}

GLfloat Library::min(POINT& pos) {
    return Library::min(pos.x, pos.y);
}

GLfloat Library::map(GLfloat value, GLfloat currentMin, GLfloat currentMax, GLfloat newMin, GLfloat newMax) {
    value -= currentMin;

    GLfloat currentRange = currentMax - currentMin;
    if (currentRange != 0)
        value /= currentRange;

    GLfloat newRange = newMax - newMin;
    value *= newRange;

    value += newMin;

    return value;
}

