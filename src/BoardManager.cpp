#include "BoardManager.h"

#include "WindowManager.h"
#include "MoveManager.h"

// ----- Creation -----

bool BoardManager::s_isClicked = false;
POINT BoardManager::s_mouse = { 0, 0 };

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
    COLOUR bg = {1.f, 1.f, 1.f};
    this->m_renderer.background(bg);
    
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
            int index = y * GRID_SIZE + x;

            // Render move squares with a slightly changed colour mask
            COLOUR mask = { 0.0f, 0.0f, 0.0f};
            if (m_heldPiece) {
                auto moves = this->m_moveManager.getMoves();
                for (int move : moves) {
                    if (index == move) {
                        mask.r = 0.3f;
                        mask.g = -0.3f;
                        mask.b = -0.3f;
                    }
                }
            }

            if ((x + y) % 2) {
                COLOUR dark = this->m_dark + mask;
                this->m_renderer.rect(dark, x * scale, y * scale, scale, scale);
            }
            else {
                COLOUR light = this->m_light + mask;
                this->m_renderer.rect(light, x * scale, y * scale, scale, scale);
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
        if (this->canPutPieceDown(gridPos)) {
            // Only switch turn if piece was placed elsewhere
            this->release(gridPos);
            if (this->m_heldPieceOriginPos != gridPos) {
                this->m_currentTurn = (this->m_currentTurn == PIECE_WHITE ? PIECE_BLACK : PIECE_WHITE);
            }
        }
        return;
    }
    // Selects piece from grid position
    if (this->m_grid[gridPos.y * GRID_SIZE + gridPos.x]) {
        // Stores held piece
        this->hold(gridPos);
        this->m_moveManager.calculateMoves(gridPos, this->m_heldPiece, this->m_grid);
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
    // Empty all pieces
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        this->m_grid[i] = 0;
    }

    // Reset castling rights
    this->m_castling[BOARD_CASTLING_BLACK_KING] = false;
    this->m_castling[BOARD_CASTLING_BLACK_QUEEN] = false;
    this->m_castling[BOARD_CASTLING_WHITE_KING] = false;
    this->m_castling[BOARD_CASTLING_WHITE_QUEEN] = false;

    // Reset move counts
    this->m_totalTurns = 0;
    this->m_50moveRule = 0;
    this->m_currentTurn = TURN_WHITE;

    // Unholds piece
    this->m_heldPiece = 0;
    this->m_heldPieceOriginPos = { 0, 0 };
}

void BoardManager::setBoard(const std::string& FEN) {
    this->clearBoard();

    std::string pieceFEN = FEN.substr(0, FEN.find(' '));
    std::string metadata = FEN.substr(FEN.find(' ') + 1);

    // Load metadata
    this->setMetadata(metadata);

    // Loop through each index
    // Starts from top left, goes to bottom right
    int x = 0, y = 7;
    for (char c : pieceFEN) {
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

void BoardManager::clicked(const POINT& mousePos) {
    s_isClicked = true;
    s_mouse = mousePos;

    // Determines if height must be adjusted
    POINT winSize = WindowManager::winSize();
    if (winSize.y > winSize.x) {
        s_mouse.y -= (winSize.y - winSize.x);
    }
}

// ----- Update ----- Hidden -----

void BoardManager::setMetadata(std::string& metadata) {
    int currentField = 0;
    for (size_t i = 0; i < metadata.length(); i++) {
        // Increase field upon finding space
        if (metadata[i] == ' ') {
            currentField++;
            continue;
        }

        switch (currentField) {
        // Current move
        case 0:
            this->m_currentTurn = (metadata[i] == 'b' ? PIECE_BLACK : PIECE_WHITE);
            break;
        // Castling rights
        case 1:
            switch (metadata[i]) {
            // Black kingside
            case 'k':
                this->m_castling[BOARD_CASTLING_BLACK_KING] = true;
                break;
            // Black queensize
            case 'q':
                this->m_castling[BOARD_CASTLING_BLACK_QUEEN] = true;
                break;
            // White kingside
            case 'K':
                this->m_castling[BOARD_CASTLING_WHITE_KING] = true;
                break;
            // White queenside
            case 'Q':
                this->m_castling[BOARD_CASTLING_WHITE_QUEEN] = true;
                break;
            // No castling, continue
            case '-':
                // Reset castling rights
                this->m_castling[BOARD_CASTLING_BLACK_KING] = false;
                this->m_castling[BOARD_CASTLING_BLACK_QUEEN] = false;
                this->m_castling[BOARD_CASTLING_WHITE_KING] = false;
                this->m_castling[BOARD_CASTLING_WHITE_QUEEN] = false;
                break;
            default:
                break;
            }
            break;
        // En passent
        case 2:
            // If no en passent square
            if (metadata[i] == '-') {
                continue;
            }

            if (metadata.length() >= i + 1) {
                this->m_phantomLocation = metadata[i + 1] * GRID_SIZE + Library::charToInt(metadata[i]);
                this->m_grid[this->m_phantomLocation] = PIECE_PHANTOM;
            }

            // Increase i to account for extra positioning
            i++;
            break;
        // Special move, 50 move rule
        case 3:
            this->m_50moveRule = metadata[i] - '0';
            break;
        // Total moves
        case 4:
            this->m_totalTurns = metadata[i] - '0';
            break;
        default:
            break;
        }
    }
}

void BoardManager::hold(POINT& gridPos) {
    // Checks piece colour
    int piece = this->m_grid[gridPos.y * GRID_SIZE + gridPos.x];
    if (this->m_currentTurn != (piece & MASK_COLOUR)) {
        return;
    }

    // Extra error checking to not pull pieces out of the void
    if (s_mouse.y > 0) {
        this->m_heldPiece = MASK_HELD | piece;
        this->m_heldPieceOriginPos = { gridPos.x, gridPos.y };
        this->m_grid[gridPos.y * GRID_SIZE + gridPos.x] = 0;

        // Calculates valid moves

    }
}

void BoardManager::release(const POINT& gridPos) {
    // Put held piece down on specified square
    int gridIndex = gridPos.y * GRID_SIZE + gridPos.x;
    this->m_grid[gridIndex] = this->m_heldPiece ^ MASK_HELD;
    this->m_heldPiece = 0;
}

bool BoardManager::canPutPieceDown(POINT& gridPos) {
    // Check if the move is legal
    if (this->m_moveManager.isValidMove(gridPos)) {
        return true;
    }
    return false;
    // Check if piece is placed back where it came from
    int gridIndex = this->m_heldPieceOriginPos.y * GRID_SIZE + this->m_heldPieceOriginPos.x;
    int placeIndex = gridPos.y * GRID_SIZE + gridPos.x;
    if (placeIndex == gridIndex) {
        // Puts piece down, but will not swap turns
        this->release(gridPos);
        return false;
    }

    // Check if colours match
    int pieceColour = m_heldPiece & MASK_COLOUR;
    int potentialNewSquare = this->m_grid[gridPos.y * GRID_SIZE + gridPos.x];
    int potentialColour = potentialNewSquare & MASK_COLOUR;
    if (pieceColour == potentialColour) {
        return false;
    }

    // Check if click is within board parameters (in case of window size change)
    POINT winSize = WindowManager::winSize();
    int min = Library::min(winSize);
    if (s_mouse.x > min || s_mouse.y < 0) {
        return false;
    }

    
    

    // Square is valid, place piece
    return true;
}

//  ----- Destruction -----

BoardManager::~BoardManager() {
    // Nothing todo
}

