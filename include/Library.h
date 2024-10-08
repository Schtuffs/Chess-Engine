#pragma once

#include <iostream>
#include <glad/glad.h>

#include "Piece.h"

#define GRID_SIZE   8

typedef struct colourHolder {
    GLfloat r, g, b;
}COLOUR;

// Mostly namespace, however stores next texture slot
class Library {
private:
    static GLuint texSlot;

public:
    // OpenGL-based functions
    static GLuint genTex(GLuint type, GLuint colour);
    static GLuint genVAO();
    static GLuint genVBO();
    static GLuint genEBO();

    // Quick math functions
    static GLfloat min(int x, int y);
    static GLfloat min(POINT& pos);
    
    static GLfloat map(GLfloat value, GLfloat currentMin, GLfloat currentMax, GLfloat newMin, GLfloat newMax);
};

