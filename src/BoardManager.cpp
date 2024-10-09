#include "BoardManager.h"

#include "WindowManager.h"

// ----- Creation -----

bool BoardManager::s_isClicked = false;
POINT BoardManager::s_mouse = {0, 0};

BoardManager::BoardManager(GLenum boardColourStyle, const std::string& FEN) {
    // Setup board with FEN string
    this->setBoard(FEN);

    // Selects board colouring
    this->setBoardColour(boardColourStyle);

    // Setting
    this->m_heldPiece = 0;
    this->m_heldPieceOriginPos = { 0, 0 };
}

// ----- Read -----

void BoardManager::show() {
    // Set background, uses dark colour
    this->m_renderer.background(this->m_dark);
    
    // Render board
    this->showBoard();

    // Render each piece
    //Render board
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        // If there is a piece on the grid square
        if (this->m_grid[i]) {
            this->m_renderer.render(this->m_grid[i], i % GRID_SIZE, i / GRID_SIZE);
        }
    }

    // Renders held piece
    if (this->m_heldPiece) {
        POINT mousePos = WindowManager::cursorPos();
        this->m_renderer.render(this->m_heldPiece, mousePos.x, mousePos.y);
    }
}

void BoardManager::showBoard() {
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
    GLfloat min = Library::min(winSize);
    GLfloat scale = min / GRID_SIZE;

    POINT gridPos = { (int)(s_mouse.x / scale), (int)(GRID_SIZE - (s_mouse.y / scale)) };

    // If a piece is held, try to release it
    if (this->m_heldPiece) {
        this->release(gridPos);
        return;
    }
    // Selects piece from grid position
    if (this->m_grid[gridPos.y * GRID_SIZE + gridPos.x]) {
        // Stores held piece
        this->hold(gridPos);
    }
}

// ----- Update -----

void BoardManager::setBoardColour(GLuint boardColourStyle) {
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
    case BOARD_RED_GOLD:
        this->m_dark.r = 0.8f;
        this->m_dark.g = 0.0f;
        this->m_dark.b = 0.0f;

        this->m_light.r = 0.8f;
        this->m_light.g = 0.6f;
        this->m_light.b = 0.3f;
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

void BoardManager::clearBoard() {
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        this->m_grid[i] = 0;
    }
}

void BoardManager::setBoard(const std::string& FEN) {
    this->clearBoard();

    // Loop through each index
    // Starts from top left, goes to bottom right
    int x = 0, y = 7;
    for (char c : FEN) {
        if (c == ' ') {
            return;
        }
        // Checks for rank change key
        if (c == '/') {
            x = 0;
            y--;
            continue;
        }

        // If the char is a number
        if (isdigit(c)) {
            x += c - '0';
        }
        // Determine what char it is
        else {
            int pieceColour = (isupper(c) ? PIECE_WHITE : PIECE_BLACK);
            int pieceType;
            switch(c) {
            case 'p':
            case 'P':
                pieceType = PIECE_PAWN;
                break;
            case 'n':
            case 'N':
                pieceType = PIECE_KNIGHT;
                break;
            case 'b':
            case 'B':
                pieceType = PIECE_BISHOP;
                break;
            case 'r':
            case 'R':
                pieceType = PIECE_ROOK;
                break;
            case 'q':
            case 'Q':
                pieceType = PIECE_QUEEN;
                break;
            case 'k':
            case 'K':
                pieceType = PIECE_KING;
                break;
            default:
                // If type is undetermined, still increase index but move on
                x++;
                continue;
            }
            this->m_grid[y * GRID_SIZE + x] = pieceColour | pieceType;
            x++;
        }
    }
}

void BoardManager::clicked(POINT mousePos) {
    s_isClicked = true;
    s_mouse = mousePos;

    // Determines if height must be adjusted
    POINT winSize = WindowManager::winSize();
    if (winSize.y > winSize.x) {
        std::cout << winSize.y - winSize.x << std::endl;
        s_mouse.y -= (winSize.y - winSize.x);
    }
}

// ----- Update ----- Hidden -----

void BoardManager::hold(POINT gridPos) {
    // Extra error checking to not pull pieces out of the void
    if (s_mouse.y > 0) {
        this->m_heldPiece = HELD_MASK | this->m_grid[gridPos.y * GRID_SIZE + gridPos.x];
        this->m_heldPieceOriginPos = { gridPos.x, gridPos.y };
        this->m_grid[gridPos.y * GRID_SIZE + gridPos.x] = 0;
    }
}

void BoardManager::release(POINT gridPos) {
    // Check if colours match
    int pieceColour = m_heldPiece & COLOUR_MASK;
    int potentialNewSquare = this->m_grid[gridPos.y * GRID_SIZE + gridPos.x];
    int potentialColour = potentialNewSquare & COLOUR_MASK;
    if (pieceColour == potentialColour) {
        // Put piece back to where it came from
        this->m_grid[this->m_heldPieceOriginPos.y * GRID_SIZE + this->m_heldPieceOriginPos.x] = this->m_heldPiece ^ HELD_MASK;
        this->m_heldPiece = 0;
        return;
    }

    // Check if click is within board parameters (in case of window size change)
    POINT winSize = WindowManager::winSize();
    int min = Library::min(winSize);
    if (s_mouse.x > min || s_mouse.y < 0) {
        // Put piece back to where it came from
        std::cout << winSize.y - min << std::endl;
        this->m_grid[this->m_heldPieceOriginPos.y * GRID_SIZE + this->m_heldPieceOriginPos.x] = this->m_heldPiece ^ HELD_MASK;
        this->m_heldPiece = 0;
        return;
    }

    // Colours are not the same, check if valid move
    // Not implemented yet

    // Square is valid, place piece
    this->m_grid[gridPos.y * GRID_SIZE + gridPos.x] = this->m_heldPiece ^ HELD_MASK;
    this->m_heldPiece = 0;
}

//  ----- Destruction -----

BoardManager::~BoardManager() {
    // Nothing todo
}

