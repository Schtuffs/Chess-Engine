#include "BoardManager.h"

// ----- Creation -----

BoardManager::BoardManager(GLenum fillStyle, GLenum boardColourStyle) {
    if (fillStyle == BOARD_FILL_AUTOMATIC) {
        this->resetBoard();
    }

    // Selects board colouring
    this->setColour(boardColourStyle);
}

// ----- Read -----

void BoardManager::show() {
    this->m_renderer.background(this->m_dark);
    
    // Render board
    this->board();

    // Render each piece
    for (Piece& piece : this->m_pieces) {
        this->m_renderer.render(piece);
    }
}

// ----- Read ----- Hidden functions

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

void BoardManager::add(Piece& piece) {
    this->m_pieces.push_back(piece);
}

void BoardManager::add(GLenum type, GLenum colour, GLint x, GLint y) {
    Piece piece(type, colour, x, y);
    this->m_pieces.push_back(piece);
}

void BoardManager::resetBoard() {
    // Remove all old pieces
    this->m_pieces.clear();

    // Manual board setup

    // White pawns
    this->add(PIECE_PAWN, PIECE_WHITE, 1, 2);
    this->add(PIECE_PAWN, PIECE_WHITE, 2, 2);
    this->add(PIECE_PAWN, PIECE_WHITE, 3, 2);
    this->add(PIECE_PAWN, PIECE_WHITE, 4, 2);
    this->add(PIECE_PAWN, PIECE_WHITE, 5, 2);
    this->add(PIECE_PAWN, PIECE_WHITE, 6, 2);
    this->add(PIECE_PAWN, PIECE_WHITE, 7, 2);
    this->add(PIECE_PAWN, PIECE_WHITE, 8, 2);

    // White pieces
    this->add(PIECE_KNIGHT, PIECE_WHITE, 2, 1);
    this->add(PIECE_KNIGHT, PIECE_WHITE, 7, 1);
    this->add(PIECE_BISHOP, PIECE_WHITE, 3, 1);
    this->add(PIECE_BISHOP, PIECE_WHITE, 6, 1);
    this->add(PIECE_ROOK,   PIECE_WHITE, 1, 1);
    this->add(PIECE_ROOK,   PIECE_WHITE, 8, 1);
    this->add(PIECE_QUEEN,  PIECE_WHITE, 4, 1);
    this->add(PIECE_KING,   PIECE_WHITE, 5, 1);

    // Black pawns
    this->add(PIECE_PAWN, PIECE_BLACK, 1, 7);
    this->add(PIECE_PAWN, PIECE_BLACK, 2, 7);
    this->add(PIECE_PAWN, PIECE_BLACK, 3, 7);
    this->add(PIECE_PAWN, PIECE_BLACK, 4, 7);
    this->add(PIECE_PAWN, PIECE_BLACK, 5, 7);
    this->add(PIECE_PAWN, PIECE_BLACK, 6, 7);
    this->add(PIECE_PAWN, PIECE_BLACK, 7, 7);
    this->add(PIECE_PAWN, PIECE_BLACK, 8, 7);

    // Black pieces
    this->add(PIECE_KNIGHT, PIECE_BLACK, 2, 8);
    this->add(PIECE_KNIGHT, PIECE_BLACK, 7, 8);
    this->add(PIECE_BISHOP, PIECE_BLACK, 3, 8);
    this->add(PIECE_BISHOP, PIECE_BLACK, 6, 8);
    this->add(PIECE_ROOK,   PIECE_BLACK, 1, 8);
    this->add(PIECE_ROOK,   PIECE_BLACK, 8, 8);
    this->add(PIECE_QUEEN,  PIECE_BLACK, 4, 8);
    this->add(PIECE_KING,   PIECE_BLACK, 5, 8);
}

//  ----- Destruction -----

BoardManager::~BoardManager() {
    for (Piece& piece : this->m_pieces) {
        DeleteBufferObjects(piece);
    }
}

