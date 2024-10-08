#pragma once

#include <iostream>
#include <glad/glad.h>
#include <stb_image.h>

#define PIECE_INVALID   0x00
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
    GLuint m_type, m_colour, m_texID, m_vao, m_vbo, m_ebo, m_value;
    POINT m_pos;

public:
    Piece(GLenum type = PIECE_PAWN, GLenum colour = PIECE_WHITE, int x = 1, int y = 1);

    POINT Pos();
    GLuint VAO();
    GLuint VBO();
    GLuint EBO();
    GLuint TexID();

    // Move piece to position
    void move();

    ~Piece();
    friend void DeleteBufferObjects(Piece& piece);
};

