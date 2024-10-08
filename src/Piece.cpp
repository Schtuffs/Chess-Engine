#include "Piece.h"
#include "Library.h"

// Creators

Piece::Piece(GLenum type, GLenum colour, int x, int y) {
    // Default white, change to black if set
    this->m_colour = PIECE_WHITE;
    if (colour == PIECE_BLACK) {
        this->m_colour = PIECE_BLACK;
    }

    // Checks type is valid
    switch(type) {
    case PIECE_PAWN:
        this->m_value = 1;
        this->m_type = type;
        break;
    case PIECE_KNIGHT:
        this->m_value = 3;
        this->m_type = type;
        break;
    case PIECE_BISHOP:
        this->m_value = 3;
        this->m_type = type;
        break;
    case PIECE_ROOK:
        this->m_value = 5;
        this->m_type = type;
        break;
    case PIECE_QUEEN:
        this->m_value = 9;
        this->m_type = type;
        break;
    case PIECE_KING:
        this->m_value = 0xFFFFFFFF;
        this->m_type = type;
        break;
    default:
        this->m_value = 0;
        this->m_type = PIECE_INVALID;
        break;
    }

    // Spot checking
    if (0 < x && x <= GRID_SIZE)
        this->m_pos.x = x;
    else
        this->m_pos.x = 0;
    
    if (0 < y && y <= GRID_SIZE)
        this->m_pos.y = y;
    else
        this->m_pos.y = 0;

    // Generate rendering variables for this object
    this->m_vao = Library::genVAO();
    this->m_vbo = Library::genVBO();
    this->m_ebo = Library::genEBO();
    this->m_texID = Library::genTex(this->m_type, this->m_colour);
}

// Getters

POINT Piece::Pos() {
    return this->m_pos;
}

GLuint Piece::VAO() {
    return this->m_vao;
}

GLuint Piece::VBO() {
    return this->m_vbo;
}

GLuint Piece::EBO() {
    return this->m_ebo;
}

GLuint Piece::TexID() {
    return this->m_texID;
}

// Usability functions

void Piece::move() {
    
}

// Destructors

Piece::~Piece() {
    // Nothing to-do
}

