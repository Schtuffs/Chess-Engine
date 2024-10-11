#include "Library.h"

#include "stb_image.h"
#include "WindowManager.h"

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

GLfloat Library::min(POINT pos) {
    return Library::min(pos.x, pos.y);
}

int Library::charToInt(char c) {
    return (tolower(c) - CHAR_TO_INT);

}

// Grid conversions

POINT Library::physToGrid(POINT phys) {
    // Determine in which square was clicked
    POINT winSize = WindowManager::winSize();
    GLfloat min = Library::min(winSize);
    GLfloat scale = min / GRID_SIZE;

    POINT gridPos = { (int)(phys.x / scale), (int)(GRID_SIZE - (phys.y / scale)) };
    return gridPos;
}

INDEX Library::gridToIndex(POINT grid) {
    return (grid.y * GRID_SIZE + grid.x);
}

INDEX Library::physToIndex(POINT phys) {
    phys = Library::physToGrid(phys);
    return Library::gridToIndex(phys);
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

bool operator==(POINT &p1, POINT &p2) {
    if (p1.x != p2.x)
        return false;
    if (p1.y != p2.y)
        return false;
    return true;
}

bool operator!=(POINT &p1, POINT &p2) {
    if (p1.x != p2.x)
        return true;
    if (p1.y != p2.y)
        return true;
    return false;
}

COLOUR operator+(COLOUR& c1, COLOUR& c2) {
    COLOUR c;
    c.r = c1.r + c2.r;
    c.g = c1.g + c2.g;
    c.b = c1.b + c2.b;
    return c;
}

