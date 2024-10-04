#pragma once

#include <iostream>
#include "RenderManager.h"
#include "Piece.h"

class Pawn : public virtual Piece{
private:
    bool moved;

public:
    Pawn(int x = 1, int y = 1);

    GLuint getVBO();
    POINT getPos();

    void move();

    void render();
};

