#include "RenderManager.h"

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

void RenderManager::rect(int x, int y, int width, int height) {
    GLfloat fx = this->map<GLfloat>(x, 0, GRID_SIZE, -1, 1);
    GLfloat fy = this->map<GLfloat>(y, 0, GRID_SIZE, -1, 1);
    GLfloat fw = this->map<GLfloat>(width, 0, GRID_SIZE, 0, 2);
    GLfloat fh = this->map<GLfloat>(height, 0, GRID_SIZE, 0, 2);

    GLfloat vertices[] = {
        fx,      fy,
        fx + fw, fy,
        fx,      fy + fh,
        fx + fw, fy + fh
    };

    GLuint indices[] = {
        0, 1, 2,
        1, 2, 3
    };

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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

    // Compile fragment shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(fragShader);

    // Bind the shaders to the shading program
    this->shaderID = glCreateProgram();
    glAttachShader(this->shaderID, vertShader);
    glAttachShader(this->shaderID, fragShader);
    glLinkProgram(this->shaderID);

    // Delete the unneccessary items
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    // Activate created program
    glUseProgram(this->shaderID);

    // Create VAO for all renderings
    glGenVertexArrays(1, &this->VAO);

    // Since everything is a square, I can save and use 1 EBO
    glGenBuffers(1, &this->EBO);

    // Create VBO for drawing board
    glGenBuffers(1, &this->VBO);
    
    // For error checking after object creation
    this->created = true;
}

bool RenderManager::is_created() {
    return this->created;
}

GLuint RenderManager::genVBO() {
    GLuint VBO;
    glGenBuffers(1, &VBO);
    return VBO;
}

void RenderManager::board() {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if ((x + y) % 2 == 0) {
                this->rect(x, y, 1, 1);
            }
        }
    }
}

void RenderManager::render(Piece& piece) {
    // Get piece points and 
    POINT pos = piece.getPos();
    pos.x--;
    pos.y--;

    this->rect(pos.x, pos.y, 1, 1);
    return;
}

RenderManager::~RenderManager() {
    glDeleteProgram(this->shaderID);
}

