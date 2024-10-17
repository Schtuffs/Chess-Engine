#pragma once

#include <iostream>
#include <glad/glad.h>

#include "Defines.h"
#include "Move.h"

typedef struct colourHolder {
    GLfloat r, g, b;
} COLOUR;

typedef struct point {
    int x, y;
} POINT;

// Mostly namespace, however stores next texture slot
class Library {
private:
    static GLuint texSlot;

public:
    // Generates TOTAL_TEXTURES texture buffers
    // Order of least to most valuable pieces
    // All white piece textures, then black
    static void genTex(GLuint* buffer);

    // Quick math functions
    static GLfloat min(int x, int y);
    static GLfloat min(POINT pos);
    static int charToInt(char c);

    // Grid conversions
    static POINT physToGrid(POINT phys);
    static INDEX gridToIndex(POINT grid);
    static INDEX physToIndex(POINT phys);
    static INDEX flipIndex(INDEX index);
    
    static GLfloat map(GLfloat value, GLfloat currentMin, GLfloat currentMax, GLfloat newMin, GLfloat newMax);
};

bool operator==(POINT& p1, POINT& p2);
bool operator==(Move&  m1, Move&  m2);
bool operator!=(POINT& p1, POINT& p2);
COLOUR operator+(COLOUR& c1, COLOUR& c2);

