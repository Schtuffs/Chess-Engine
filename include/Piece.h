#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#define PIECE_PAWN      0x01
#define PIECE_KNIGHT    0x02
#define PIECE_BISHOP    0x03
#define PIECE_ROOK      0x04
#define PIECE_QUEEN     0x05
#define PIECE_KING      0x06

typedef struct point {
    int x, y;
} POINT;

class Piece {
protected:
    int type, imgID, VBO;
    POINT pos;

public:
    // Render piece to screen
    virtual void render() = 0;

    virtual GLuint getVBO() = 0;
    virtual POINT getPos() = 0;

    // Move piece to position
    virtual void move() = 0;
};

