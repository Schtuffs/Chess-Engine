#include "Pawn.h"

Pawn::Pawn(int x, int y) {
    this->type = PIECE_PAWN;
    this->VBO = RenderManager::genVBO();
    this->pos = {x, y};
}

GLuint Pawn::getVBO() {
    return this->VBO;
}

POINT Pawn::getPos() {
    return this->pos;
}

void Pawn::move() {
    

}
void Pawn::render() {

}

