#include "RenderManager.h"

// Creation functions

std::string RenderManager::read(const std::string& filename) {
    // Create file
    std::ifstream in(filename, std::ios::binary);
	if (!in.is_open()) {
		throw 1;
	}

	std::string contents;
	in.seekg(0, std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
	in.close();
	return contents;
}

RenderManager::RenderManager(const std::string& vertFile, const std::string& fragTexFile, const std::string& fragColFile) {
    // Read files
    std::string vertCode, fragTexCode, fragColCode;
    try {
        vertCode = this->read(vertFile);
        fragTexCode = this->read(fragTexFile);
        fragColCode = this->read(fragColFile);
    } catch (int e) {
        std::cout << "Could not read a file..." << std::endl;
        this->m_created = false;
        return;
    }
    // For error checking after object creation
    // Starts with true and will be changed to false later if shader compilation fails
    this->m_created = true;

    // Convert to char* to use
    const char* vertSource = vertCode.c_str();
    const char* fragTexSource = fragTexCode.c_str();
    const char* fragColSource = fragColCode.c_str();

    // Compile vertex shader
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSource, NULL);
    glCompileShader(vertShader);
    this->compileErrors(vertShader, GL_COMPILE_STATUS);

    // Compile texture fragment shader
    GLuint fragTexShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragTexShader, 1, &fragTexSource, NULL);
    glCompileShader(fragTexShader);
    this->compileErrors(fragTexShader, GL_COMPILE_STATUS);

    // Compile colour fragment shader
    GLuint fragColShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragColShader, 1, &fragColSource, NULL);
    glCompileShader(fragColShader);
    this->compileErrors(fragColShader, GL_COMPILE_STATUS);

    // Bind the shaders to the texture shading program
    this->m_shaderTexID = glCreateProgram();
    glAttachShader(this->m_shaderTexID, vertShader);
    glAttachShader(this->m_shaderTexID, fragTexShader);
    glLinkProgram(this->m_shaderTexID);
    this->compileErrors(this->m_shaderTexID, GL_LINK_STATUS);

    // Bind the shaders to the colour shading program
    this->m_shaderColID = glCreateProgram();
    glAttachShader(this->m_shaderColID, vertShader);
    glAttachShader(this->m_shaderColID, fragColShader);
    glLinkProgram(this->m_shaderColID);
    this->compileErrors(this->m_shaderColID, GL_LINK_STATUS);

    // Delete the unneccessary items
    glDeleteShader(vertShader);
    glDeleteShader(fragTexShader);
    glDeleteShader(fragColShader);

    // If any compilation failed, return now
    if (!this->m_created) {
        return;
    }

    // Create VAO for own renderings
    this->m_vao = Library::genVAO();

    // Create EBO for own renderings
    this->m_vbo = Library::genVBO();

    // Create VBO for drawing board
    this->m_ebo = Library::genEBO();
}

void RenderManager::compileErrors(GLuint id, GLuint type) {
    char error[1024];
    GLsizei read;
    GLint status = GL_TRUE;
    
    // For the vertex and fragment shaders
    glGetShaderiv(id, type, &status);
    if (status != GL_TRUE) {
        if (type == GL_COMPILE_STATUS)
            glGetShaderInfoLog(id, 1024, &read, error);
        else
            glGetProgramInfoLog(id, 1024, &read, error);
        std::cout << "Compile error: " << error << std::endl;
        this->m_created = false;
    }
}

bool RenderManager::is_created() {
    return this->m_created;
}

// Render functions

void RenderManager::background(COLOUR& colour) {
    glClearColor(colour.r, colour.g, colour.b, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderManager::rect(COLOUR& colour, int x, int y, int width, int height) {
    // Return if the renderer was not properly made
    if (!this->m_created) {
        std::cout << "Cannot render piece: renderer not properly initialized..." << std::endl;
        return;
    }

    // Calculating where the points are in vector-space instead of pixel-space
    POINT winSize = WindowManager::winSize();
    GLfloat fx = Library::map(x, 0, winSize.x, -1, 1);
    GLfloat fy = Library::map(y, 0, winSize.y, -1, 1);
    GLfloat fw = Library::map(width, 0, winSize.x, 0, 2);
    GLfloat fh = Library::map(height, 0, winSize.y, 0, 2);

    // Places points into array for rendering
    GLfloat vertices[] = {
        // Coordinates      // Colours
        fx,      fy,        colour.r, colour.g, colour.b,
        fx + fw, fy,        colour.r, colour.g, colour.b,
        fx,      fy + fh,   colour.r, colour.g, colour.b,
        fx + fw, fy + fh,   colour.r, colour.g, colour.b
    };

    GLuint indices[] = {0, 1, 2, 1, 2, 3};

    // Binding
    BindManager::BindVAO(this->m_vao);
    BindManager::BindVBO(this->m_vbo, vertices, sizeof(vertices));
    BindManager::BindEBO(this->m_ebo, indices, sizeof(indices));

    // Editing
    BindManager::LinkAttrib(0, 2, GL_FLOAT, 5 * sizeof(GLfloat), (void*)0);
    BindManager::LinkAttrib(1, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    // Unbind
    BindManager::UnbindAll();

    // Rendering
    BindManager::Activate(this->m_shaderColID);
    BindManager::BindVAO(this->m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    BindManager::UnbindAll();
}

void RenderManager::render(Piece& piece) {
    // Return if the renderer was not properly made
    if (!this->m_created) {
        std::cout << "Cannot render piece: renderer not properly initialized..." << std::endl;
        return;
    }

    // For making pieces scale with screen size changes
    POINT winSize = WindowManager::winSize();
    GLfloat scale = Library::min(winSize);
    scale /= GRID_SIZE;

    // For rendering in vector-space instead of pixel-space
    GLfloat x, y, sx, sy;
    if (!piece.isHeld()) {
        // Draws based on grid position
        POINT pos = piece.GridPos();
        x = Library::map((int)((pos.x - 1) * scale), 0, winSize.x, -1, 1);
        y = Library::map((int)((pos.y - 1) * scale), 0, winSize.y, -1, 1);
        sx = Library::map((int)(scale), 0, winSize.x, 0, 2);
        sy = Library::map((int)(scale), 0, winSize.y, 0, 2);
    }
    else {
        // Uses physical piece position on screen to draw
        POINT pos = piece.PhysPos();
        x =  Library::map((int)(pos.x), 0, winSize.x, -1, 1);
        y =  Library::map((int)(pos.y), 0, winSize.y, -1, 1);
        sx = Library::map((int)(scale), 0, winSize.x,  0, 2);
        sy = Library::map((int)(scale), 0, winSize.y,  0, 2);

        // Centers piece around the mouse
        x -= sx / 2;
        y -= sy / 2;
    }

    GLfloat vertices[] = {
        // positions          // texture coords
        x,      y,      0.0f, 0.0f, 1.0f,   // top left 
        x + sx, y,      0.0f, 1.0f, 1.0f,   // top right
        x,      y + sy, 0.0f, 0.0f, 0.0f,   // bottom left
        x + sx, y + sy, 0.0f, 1.0f, 0.0f,   // bottom right
    };

    GLuint indices[] = {0, 1, 2, 1, 2, 3};

    // Start rendering process
    // Binding objects
    BindManager::BindVAO(piece.VAO());
    BindManager::BindVBO(piece.VBO(), vertices, sizeof(vertices));
    BindManager::BindEBO(piece.EBO(), indices, sizeof(indices));

    // Binding attributes
    BindManager::LinkAttrib(0, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void*)0);
    BindManager::LinkAttrib(2, 2, GL_FLOAT, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    // Unbind all after changes
    BindManager::UnbindAll();

    // Rendering
    BindManager::Activate(this->m_shaderTexID);
    BindManager::BindTEX(piece.TexID());
    BindManager::BindVAO(piece.VAO());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Unbind
    BindManager::UnbindAll();
}

// Destruction functions

void DeleteBufferObjects(Piece& piece) {
    glDeleteTextures(1, &piece.m_texID);
    glDeleteVertexArrays(1, &piece.m_vao);
    glDeleteBuffers(1, &piece.m_vbo);
    glDeleteBuffers(1, &piece.m_ebo);
}

RenderManager::~RenderManager() {
    glDeleteProgram(this->m_shaderTexID);
    glDeleteProgram(this->m_shaderColID);
    glDeleteVertexArrays(1, &this->m_vao);
    glDeleteBuffers(1, &this->m_vbo);
    glDeleteBuffers(1, &this->m_ebo);
}

