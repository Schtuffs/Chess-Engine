#pragma once

#include <iostream>
#include <glad/glad.h>
#include <stb_image.h>

#define PIECE_INVALID   0x0
#define PIECE_PAWN      'p'
#define PIECE_KNIGHT    'n'
#define PIECE_BISHOP    'b'
#define PIECE_ROOK      'r'
#define PIECE_QUEEN     'q'
#define PIECE_KING      'k'

#define PIECE_WHITE     0x08
#define PIECE_BLACK     0x10

typedef struct point {
    int x, y;
} POINT;

class Piece {
protected:
    GLuint m_type, m_colour, m_texID, m_vao, m_vbo, m_ebo, m_value;
    POINT m_gridPos, m_physPos;

    bool m_isSelected;

public:
    // ----- Creation -----
    Piece(GLenum type = PIECE_INVALID, GLenum colour = PIECE_WHITE, int x = 1, int y = 1);

    // ----- Read ------
    POINT GridPos();
    POINT PhysPos();
    GLuint VAO();
    GLuint VBO();
    GLuint EBO();
    GLuint TexID();

    int Type();
    bool isHeld();

    // ----- Update -----

    // Move piece to position
    void move();

    // Lets piece be selected to follow mouse
    void pickup();

    // Puts piece down
    void putdown();

    // Sets piece down is new positino
    bool setGridPos(POINT pos);

    // ----- Destruction -----
    ~Piece();
    friend void DeleteBufferObjects(Piece& piece);
};

