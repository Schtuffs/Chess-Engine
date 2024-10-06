#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stb_image.h>

#define PIECE_PAWN      0x01
#define PIECE_KNIGHT    0x02
#define PIECE_BISHOP    0x03
#define PIECE_ROOK      0x04
#define PIECE_QUEEN     0x05
#define PIECE_KING      0x06

#define PIECE_WHITE     0x10
#define PIECE_BLACK     0x11

typedef struct point {
    int x, y;
} POINT;

class Piece {
protected:
    GLuint type, colour, texID, vao, vbo, ebo, value;
    POINT pos;

public:
    virtual POINT getPos() = 0;
    virtual GLuint VAO() = 0;
    virtual GLuint VBO() = 0;
    virtual GLuint EBO() = 0;
    virtual GLuint TexID() = 0;

    // Move piece to position
    virtual void move() = 0;
};

