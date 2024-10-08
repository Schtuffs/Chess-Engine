#pragma once

#include <glad/glad.h>

// Handles all of the functions for binding/unbinding OpenGL buffer objects
namespace BindManager {
    // ----- Binding -----

    // Activate shader program
    void Activate(GLuint shader);

    // Binds VAO
    void BindVAO(GLuint VAO);

    // Bind VBO with vertices
    void BindVBO(GLuint VBO, GLfloat* vertices, GLsizeiptr size);

    // Bind EBO with indices
    void BindEBO(GLuint EBO, GLuint* indices, GLsizeiptr size);

    // Binds a texture
    void BindTEX(GLuint TEX);

    // Adds attributes to VAO with VBO information
    // Layout - vertex shader layout option. Ex: 2
    // Components - how many indexes for the information in this layout. Ex: 2
    // Type - variable type. Ex: GL_FLOAT
    // Stride - how long until we are at the next instance of this data in the array. Ex: 5 * sizeof(GLfloat)
    // Offset - how far into vertices the layout information begins. Ex: (void*)(3 * sizeof(GLfloat))
    void LinkAttrib(GLuint layout, GLuint components, GLenum type, GLsizeiptr stride, void* offset);
    
    // ----- Unbinding -----

    // Deactivate shader program
    void Deactivate();

    // Unbinds VAO
    void UnbindVAO();

    // Unbinds VBO
    void UnbindVBO();
    
    // Unbinds EBO
    void UnbindEBO();

    // Unbinds Texture
    void UnbindTEX();

    // Unbinds shader program, VAO, VBO, EBO, and Texture
    void UnbindAll();
}

