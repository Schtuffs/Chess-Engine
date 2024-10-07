#include "RenderManager.h"

// Creation functions

GLuint RenderManager::texSlot = GL_TEXTURE0;
POINT RenderManager::winSize = { 800, 800 };

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

RenderManager::RenderManager(const std::string& vertFile, const std::string& fragFile) {
    // Read files
    std::string vertCode, fragCode;
    try {
        vertCode = this->read(vertFile);
        fragCode = this->read(fragFile);
    } catch (int e) {
        std::cout << "Could not read a file..." << std::endl;
        this->created = false;
        return;
    }

    // Convert to char* to use
    const char* vertSource = vertCode.c_str();
    const char* fragSource = fragCode.c_str();

    // Compile vertex shader
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSource, NULL);
    glCompileShader(vertShader);
    this->compileErrors(vertShader, GL_COMPILE_STATUS);

    // Compile fragment shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(fragShader);
    this->compileErrors(vertShader, GL_COMPILE_STATUS);

    // Bind the shaders to the shading program
    this->shaderID = glCreateProgram();
    glAttachShader(this->shaderID, vertShader);
    glAttachShader(this->shaderID, fragShader);
    glLinkProgram(this->shaderID);
    this->compileErrors(this->shaderID, GL_LINK_STATUS);

    // Delete the unneccessary items
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    // Create VAO for own renderings
    glGenVertexArrays(1, &this->vao);

    // Create EBO for own renderings
    glGenBuffers(1, &this->ebo);

    // Create VBO for drawing board
    glGenBuffers(1, &this->vbo);
    
    // For error checking after object creation
    this->created = true;
}

void RenderManager::compileErrors(GLuint id, GLuint type) {
    char error[1024];
    GLsizei read;
    GLint status = GL_TRUE;
    
    // For the vertex and fragment shaders
    if (type == GL_COMPILE_STATUS) {
        glGetShaderiv(id, type, &status);
        if (status != GL_TRUE) {
            glGetShaderInfoLog(id, 1024, &read, error);
            std::cout << "Compile error: " << error << std::endl;
        }
        return;
    }

    // For linking the program
    glGetProgramiv(id, type, &status);
    if (status != GL_TRUE) {
        glGetProgramInfoLog(id, 1024, &read, error);
        std::cout << "Link error: " << error << std::endl;
    }
}

// Render functions

GLuint RenderManager::id() {
    return this->shaderID;
}

void RenderManager::rect(int x, int y, int width, int height) {
    GLfloat fx = this->map<GLfloat>(x, 0, winSize.x, -1, 1);
    GLfloat fy = this->map<GLfloat>(y, 0, winSize.y, -1, 1);
    GLfloat fw = this->map<GLfloat>(width, 0, winSize.x, 0, 2);
    GLfloat fh = this->map<GLfloat>(height, 0, winSize.y, 0, 2);

    GLfloat vertices[] = {
        fx,      fy,     
        fx + fw, fy,     
        fx,      fy + fh,
        fx + fw, fy + fh
    };

    GLuint indices[] = {0, 1, 2, 1, 2, 3};

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void RenderManager::board() {
    glUseProgram(this->shaderID);

    // Scales board based on smaller side so it stays square
    GLfloat scale = this->min(winSize);
    scale /= GRID_SIZE;

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if ((x + y) % 2 == 0) {
                this->rect(x * scale, y * scale, scale, scale);
            }
        }
    }

    glUseProgram(0);
}

void RenderManager::render(Piece& piece) {
    GLfloat scale = this->min(winSize);
    scale /= GRID_SIZE;

    POINT pos = piece.getPos();
    GLfloat x, y;
    if (!piece.is_moving()) {
        pos.x--;
        pos.y--;
        x = this->map<GLfloat>((int)(pos.x * scale), 0, winSize.x, -1, 1);
        y = this->map<GLfloat>((int)(pos.y * scale), 0, winSize.y, -1, 1);
    }
    else {
        x = this->map<GLfloat>(pos.x, 0, winSize.x, -1, 1);
        y = this->map<GLfloat>(pos.y, 0, winSize.y, -1, 1);
    }
    
    GLfloat sx = this->map<GLfloat>((int)(scale), 0, winSize.x, 0, 2);
    GLfloat sy = this->map<GLfloat>((int)(scale), 0, winSize.y, 0, 2);


    GLfloat vertices[] = {
        // positions            // texture coords
        x,      y,      0.0f,   0.0f, 1.0f,   // top left 
        x + sx, y,      0.0f,   1.0f, 1.0f,   // top right
        x,      y + sy, 0.0f,   0.0f, 0.0f,   // bottom left
        x + sx, y + sy, 0.0f,   1.0f, 0.0f,   // bottom right
    };

    GLuint indices[] = {0, 1, 2, 1, 2, 3};

    // Start rendering process
    // Binding objects
    glBindVertexArray(piece.VAO());

    glBindBuffer(GL_ARRAY_BUFFER, piece.VBO());
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, piece.EBO());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    // Binding attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Unbind all after changes
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Rendering
    glUseProgram(this->shaderID);
    glBindTexture(GL_TEXTURE_2D, piece.TexID());
    glBindVertexArray(piece.VAO());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

// Other functions

template <typename t>
t RenderManager::map(t value, t currentMin, t currentMax, t newMin, t newMax) {
    value -= currentMin;

    t currentRange = currentMax - currentMin;
	if (currentRange != 0)
		value /= currentRange;

	t newRange = newMax - newMin;
	value *= newRange;

	value += newMin;

	return value;
}

GLfloat RenderManager::min(int x, int y) {
    GLfloat small = x;
    if (small > y)
        small = y;

    return small;
}

GLfloat RenderManager::min(POINT pos) {
    return this->min(pos.x, pos.y);
}

// Generation functions

GLuint RenderManager::genTex(GLuint type, GLuint colour) {
    // Create string for loading image
    std::string path = "../lib/pieces/";
    switch (type) {
    case PIECE_PAWN:
        path += "pawn";
        break;
    case PIECE_KNIGHT:
        path += "knight";
        break;
    case PIECE_BISHOP:
        path += "bishop";
        break;
    case PIECE_ROOK:
        path += "rook";
        break;
    case PIECE_QUEEN:
        path += "queen";
        break;
    case PIECE_KING:
        path += "king";
        break;
    }
    path += "_";

    switch (colour) {
    case PIECE_BLACK:
        path += "black.png";
        break;
    case PIECE_WHITE:
        path += "white.png";
        break;
    }
    
    GLuint texture;
    
    // Generating texture
    glGenTextures(1, &texture);

    // Set texture slot
    glActiveTexture(texSlot);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Setup MipMap function choices
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // load and generate the texture
    int width, height, channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    // Free and unbind
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

GLuint RenderManager::genVAO() {
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    return VAO;
}

GLuint RenderManager::genVBO() {
    GLuint VBO;
    glGenBuffers(1, &VBO);
    return VBO;
}

GLuint RenderManager::genEBO() {
    GLuint EBO;
    glGenBuffers(1, &EBO);
    return EBO;
}

// Useful extra functions

void RenderManager::resize(int width, int height) {
    winSize.x = width;
    winSize.y = height;
}

int RenderManager::winSizeX() {
    return winSize.x;
}

int RenderManager::winSizeY() {
    return winSize.y;
}

bool RenderManager::is_created() {
    return this->created;
}

RenderManager::~RenderManager() {
    glDeleteProgram(this->shaderID);
    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
    glDeleteBuffers(1, &this->ebo);
}

