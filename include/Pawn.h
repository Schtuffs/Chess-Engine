#pragma once

#include <iostream>
#include "RenderManager.h"
#include "Piece.h"

class Pawn : public virtual Piece{
private:
    bool moved;

public:
    Pawn(GLuint team, int x = 0, int y = 0);

    POINT getPos();
    GLuint VAO();
    GLuint VBO();
    GLuint EBO();
    GLuint TexID();

    void move();

    ~Pawn();
};

