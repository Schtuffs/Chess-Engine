#include "BindManager.h"

// Binding

void BindManager::Activate(GLuint shader) {
    glUseProgram(shader);
}

void BindManager::BindVAO(GLuint VAO) {
    glBindVertexArray(VAO);
}

void BindManager::BindVBO(GLuint VBO, GLfloat* vertices, GLsizeiptr size) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
}

void BindManager::BindEBO(GLuint EBO, GLuint* indices, GLsizeiptr size) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void BindManager::BindTEX(GLuint TEX) {
    glBindTexture(GL_TEXTURE_2D, TEX);
}

void BindManager::LinkAttrib(GLuint layout, GLuint components, GLenum type, GLsizeiptr stride, void* offset) {
    glVertexAttribPointer(layout, components, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
}

// Unbinding

void BindManager::Deactivate() {
    glUseProgram(0);
}

void BindManager::UnbindVAO() {
    glBindVertexArray(0);
}

void BindManager::UnbindVBO() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BindManager::UnbindEBO() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void BindManager::UnbindTEX() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void BindManager::UnbindAll() {
    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


