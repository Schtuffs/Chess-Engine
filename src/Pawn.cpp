#include "Pawn.h"

Pawn::Pawn(GLuint team, int x, int y) {
    // Default white, change to black if set
    this->colour = PIECE_WHITE;
    if (team == PIECE_BLACK) {
        this->colour = PIECE_BLACK;
    }

    this->value = 1;
    this->type = PIECE_PAWN;
    this->pos = {x, y};

    this->vao = RenderManager::genVAO();
    this->vbo = RenderManager::genVBO();
    this->ebo = RenderManager::genEBO();
    this->texID = RenderManager::genTex(this->type, this->colour);
}

POINT Pawn::getPos() {
    return this->pos;
}

GLuint Pawn::VAO() {
    return this->vao;
}

GLuint Pawn::VBO() {
    return this->vbo;
}

GLuint Pawn::EBO() {
    return this->ebo;
}

GLuint Pawn::TexID() {
    return this->texID;
}

void Pawn::move() {
    
}

Pawn::~Pawn() {
    glDeleteTextures(1, &this->texID);
    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
    glDeleteBuffers(1, &this->ebo);
}

