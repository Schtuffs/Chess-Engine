#include "Library.h"

#include "stb_image.h"

GLuint Library::texSlot = GL_TEXTURE0;

// OpenGL-based functions

void Library::genTex(GLuint* buffer) {
    // Holds filepath
    std::string path = "../lib/pieces/";
    std::string image[TOTAL_TEXTURES] = {
        "pawn_white.png",
        "knight_white.png",
        "bishop_white.png",
        "rook_white.png",
        "queen_white.png",
        "king_white.png",
        "pawn_black.png",
        "knight_black.png",
        "bishop_black.png",
        "rook_black.png",
        "queen_black.png",
        "king_black.png"
    };
    
    glGenTextures(TOTAL_TEXTURES, buffer);

    // Loop through buffer array and bind textures
    for (int i = 0; i < TOTAL_TEXTURES; i++) {
        // Set texture slot
        glActiveTexture(texSlot);
        glBindTexture(GL_TEXTURE_2D, buffer[i]);

        // Setup MipMap function choices
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // load and generate the texture
        int width, height, channels;
        std::string file = path + image[i];
        unsigned char *data = stbi_load(file.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture" << std::endl;
            // Reset buffer for not loading texture
            buffer[i] = 0;
        }

        // Free and unbind
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
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

