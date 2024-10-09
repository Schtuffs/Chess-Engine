#include "BoardManager.h"

// ----- Creation -----

bool BoardManager::s_isClicked = false;
POINT BoardManager::s_mouse = {0, 0};

BoardManager::BoardManager(GLenum boardColourStyle, const std::string& FEN) {
    // Setup board with FEN string
    this->setBoard(FEN);

    // Selects board colouring
    this->setColour(boardColourStyle);

    this->m_heldPiece = nullptr;
    this->m_doesHeldPieceExists = false;
}

// ----- Read -----

void BoardManager::show() {
    // Set background, uses dark colour
    this->m_renderer.background(this->m_dark);
    
    // Render board
    this->board();

    // Render each piece
    for (Piece& piece : this->m_pieces) {
        if (piece.isHeld()) {
            piece.move();
        }
        this->m_renderer.render(piece);
    }
}

void BoardManager::board() {
    // Scales board based on smaller side so it stays square
    POINT winSize = WindowManager::winSize();
    GLfloat scale = Library::min(winSize);
    scale /= GRID_SIZE;

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if ((x + y) % 2 == 0) {
                this->m_renderer.rect(this->m_light, x * scale, y * scale, scale, scale);
            }
        }
    }
}

void BoardManager::check() {
    // For dealing with clicks
    if (!s_isClicked) {
        return;
    }

    s_isClicked = false;

    // Determine in which square was clicked
    POINT winSize = WindowManager::winSize();
    GLfloat scale = Library::min(winSize);
    scale /= GRID_SIZE;
    POINT mPos = { (int)(1 + s_mouse.x / scale), (int)(1 + GRID_SIZE - (s_mouse.y / scale)) };

    // Determine if a piece or square was clicked
    for (Piece& piece : this->m_pieces) {
        POINT pPos = piece.GridPos();

        // Piece is clicked
        if (this->m_heldPiece == nullptr && pPos.x == mPos.x && pPos.y == mPos.y) {
            this->m_heldPiece = &piece;
            this->m_heldPiece->pickup();
            this->m_doesHeldPieceExists = true;
            break;
        }
        // Square is clicked
        else if (m_doesHeldPieceExists) {
            // Checks if a valid square is selected before releasing the piece
            if (this->m_heldPiece->setGridPos(mPos)) {
                this->m_heldPiece->putdown();
                this->m_heldPiece = nullptr;
                this->m_doesHeldPieceExists = false;
                break;
            }
        }
    }
}

// ----- Update -----

void BoardManager::setColour(GLuint boardColourStyle) {
    switch (boardColourStyle) {
    case BOARD_BLACK_WHITE:
        this->m_dark.r = 0.f;
        this->m_dark.g = 0.f;
        this->m_dark.b = 0.f;

        this->m_light.r = 1.f;
        this->m_light.g = 1.f;
        this->m_light.b = 1.f;
        break;
    case BOARD_BROWN_BROWN:
        this->m_dark.r = 0.7f;
        this->m_dark.g = 0.43f;
        this->m_dark.b = 0.25f;

        this->m_light.r = 1.f;
        this->m_light.g = 0.84f;
        this->m_light.b = 0.6f;
        break;
    case BOARD_GREEN_CREAM:
        this->m_dark.r = 0.46f;
        this->m_dark.g = 0.59f;
        this->m_dark.b = 0.34f;

        this->m_light.r = 0.93f;
        this->m_light.g = 0.93f;
        this->m_light.b = 0.82f;
        break;
    case BOARD_DBLUE_LBLUE:
        this->m_dark.r = 0.0f;
        this->m_dark.g = 0.27f;
        this->m_dark.b = 0.45f;

        this->m_light.r = 0.6f;
        this->m_light.g = 0.8f;
        this->m_light.b = 1.0f;
        break;
    // Defaults black and white
    default:
        this->m_dark.r = 0.f;
        this->m_dark.g = 0.f;
        this->m_dark.b = 0.f;

        this->m_light.r = 1.f;
        this->m_light.g = 1.f;
        this->m_light.b = 1.f;
        break;
    }
}

void BoardManager::add(GLenum type, GLenum colour, GLint x, GLint y) {
    Piece piece(type, colour, x, y);
    this->m_pieces.push_back(piece);
}

void BoardManager::setBoard(const std::string& FEN) {
    this->deletePieces();

    // Loop through each index
    // Starts from top left, goes to bottom right
    int x = 1, y = 8;
    for (char c : FEN) {
        // Checks for rank change key
        if (c == '/') {
            x = 1;
            y--;
            continue;
        }

        // If the char is a number
        if (isdigit(c)) {
            x += c - '0';
        }
        // Determine what char it is
        else {
            int colour = (isupper(c) ? PIECE_WHITE : PIECE_BLACK);
            char type = tolower(c);
            this->add(type, colour, x, y);
            x++;
        }
    }
}

void BoardManager::clicked() {
    s_isClicked = true;
    double x, y;
    WindowManager::cursorPos(x, y);
    s_mouse = { (int)x, (int)y };
}

//  ----- Destruction -----

void BoardManager::deletePieces() {
    for (Piece& piece : this->m_pieces) {
        DeleteBufferObjects(piece);
    }
    this->m_pieces.clear();
}

BoardManager::~BoardManager() {
    this->deletePieces();
}

