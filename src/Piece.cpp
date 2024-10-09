#include "Piece.h"
#include "Library.h"
#include "WindowManager.h"

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
        this->m_gridPos.x = x;
    else
        this->m_gridPos.x = 0;
    
    if (0 < y && y <= GRID_SIZE)
        this->m_gridPos.y = y;
    else
        this->m_gridPos.y = 0;

    // Generate rendering variables for this object
    this->m_vao = Library::genVAO();
    this->m_vbo = Library::genVBO();
    this->m_ebo = Library::genEBO();
    this->m_texID = Library::genTex(this->m_type, this->m_colour);

    // Other variable setting
    this->m_isSelected = false;
    this->m_physPos = { 0, 0 };
}

// Getters

POINT Piece::GridPos() {
    return this->m_gridPos;
}

POINT Piece::PhysPos() {
    return this->m_physPos;
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

int Piece::Type() {
    return this->m_type;
}

bool Piece::isHeld() {
    return this->m_isSelected;
}

// Usability functions

void Piece::move() {
    double x, y;
    WindowManager::cursorPos(x, y);
    this->m_physPos = {(int)x, WindowManager::winSize().y - (int)y};

}

void Piece::pickup() {
    this->m_isSelected = true;
}

void Piece::putdown() {
    this->m_isSelected = false;
}

bool Piece::setGridPos(POINT pos) {
    if (1 > pos.x || pos.x > GRID_SIZE) {
        return false;
    }

    if (1 > pos.y || pos.y > GRID_SIZE) {
        return false;
    }
    
    this->m_gridPos = { pos.x, pos.y };
    return true;
}

// Destructors

Piece::~Piece() {
    // Nothing to-do
}

