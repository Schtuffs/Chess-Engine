#include "RenderManager.h"

#include "BindManager.h"
#include "WindowManager.h"
#include "BoardManager.h"
#include "Piece.h"

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
    glGenVertexArrays(1, &this->m_vao);

    // Create EBO for own renderings
    glGenBuffers(1, &this->m_vbo);

    // Create VBO for drawing board
    glGenBuffers(1, &this->m_ebo);

    Library::genTex(this->m_pieceTextures);
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

void RenderManager::render(PIECE piece, int x, int y) {
    // Check to not try to render phantom pieces
    if (piece >= PIECE_PHANTOM) {
        return;
    }
    
    FLAG held = Piece::getFlag(piece, MASK_HELD);
    // Gets piece information
    FLAG type = Piece::getFlag(piece, MASK_TYPE);
    
    // Colour will add 6 indexes to texture array if black
    FLAG colour = (Piece::getFlag(piece, MASK_COLOUR) == PIECE_BLACK ? 6 : 0);
    INDEX index = type + colour - 1;

    // For making pieces scale with screen size changes
    POINT winSize = WindowManager::winSize();
    GLfloat scale = Library::min(winSize);
    scale /= GRID_SIZE;

    GLfloat sx, sy, sw, sh;
    // Calculate render location
    // If a piece is not held, calculate based on grid position
    if (!held) {
        sx = Library::map((int)(x * scale), 0, winSize.x, -1, 1);
        sy = Library::map((int)(y * scale), 0, winSize.y, -1, 1);
        sw = Library::map((int)(scale), 0, winSize.x, 0, 2);
        sh = Library::map((int)(scale), 0, winSize.y, 0, 2);
    }
    // Piece is held, calculate off mouse position
    else {
        sx = Library::map((int)(  x  ), 0, winSize.x, -1, 1);
        sy = Library::map((int)(  y  ), 0, winSize.y, 1, -1);
        sw = Library::map((int)(scale), 0, winSize.x, 0, 2);
        sh = Library::map((int)(scale), 0, winSize.y, 0, 2);
        sx -= sw / 2;
        sy -= sh / 2;
    }

    GLfloat vertices[] = {
        // positions      // texture coords
        sx,      sy,      0.0f, 0.0f, 1.0f,   // top left 
        sx + sw, sy,      0.0f, 1.0f, 1.0f,   // top right
        sx,      sy + sh, 0.0f, 0.0f, 0.0f,   // bottom left
        sx + sw, sy + sh, 0.0f, 1.0f, 0.0f,   // bottom right
    };

    GLuint indices[] = {0, 1, 2, 1, 2, 3};

    // Binding render objects
    BindManager::BindVAO(this->m_vao);
    BindManager::BindVBO(this->m_vbo, vertices, sizeof(vertices));
    BindManager::BindEBO(this->m_ebo, indices, sizeof(indices));

    // Binding attributes
    BindManager::LinkAttrib(0, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void*)0);
    BindManager::LinkAttrib(2, 2, GL_FLOAT, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    // Rendering
    BindManager::Activate(this->m_shaderTexID);
    BindManager::BindTEX(this->m_pieceTextures[index]);
    BindManager::BindVAO(this->m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Unbind
    BindManager::UnbindAll();
}

// Destruction functions

RenderManager::~RenderManager() {
    // Deletes buffer objects
    glDeleteProgram(this->m_shaderTexID);
    glDeleteProgram(this->m_shaderColID);
    glDeleteVertexArrays(1, &this->m_vao);
    glDeleteBuffers(1, &this->m_vbo);
    glDeleteBuffers(1, &this->m_ebo);

    // Deletes texture buffers
    glDeleteTextures(TOTAL_TEXTURES, this->m_pieceTextures);
}

