#include "BoardManager.h"

#include "WindowManager.h"
#include "Piece.h"

// ----- Creation -----

BoardManager::BoardManager(GLenum boardColourStyle, const std::string& FEN) {
    // Setup board with FEN string
    this->setBoard(FEN);

    // Selects board colouring
    this->setBoardColour(boardColourStyle);

    // Setting
    this->m_heldPiece = 0;
    this->m_heldPieceOriginPos = -1;
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

    // Renders held piece so its on top
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

    // Loop through each grid index
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            INDEX index = y * GRID_SIZE + x;

            // Render move squares with a slightly changed colour mask
            COLOUR mask = { 0.0f, 0.0f, 0.0f};
            if (m_heldPiece) {
                auto moves = this->m_moveManager.getMoves();
                for (int move : moves) {
                    move &= MASK_MIN_FLAGS;
                    if (index == move) {
                        mask.r = 0.3f;
                        mask.g = -0.3f;
                        mask.b = -0.3f;
                    }
                }
            }

            // Render piece square as gold
            if (index == this->m_heldPieceOriginPos) {
                mask.r = 0.5f;
                mask.g = 0.5f;
                mask.b = -0.5f;
            }

            // Render colour based on evenness
            if ((x + y) % 2 == 0) {
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

void BoardManager::check(INDEX index) {
    // If a piece is held, try to release it
    if (this->m_heldPiece) {
        // Store piece incase of valid
        PIECE piece = this->m_moveManager.isValidMove(index);
        if (piece) {
            // Only switch turn if piece was placed elsewhere
            if (this->m_heldPieceOriginPos != index) {
                this->m_currentTurn = (this->m_currentTurn == PIECE_WHITE ? PIECE_BLACK : PIECE_WHITE);
                this->release(index, piece, true);
            }
            else {
                this->release(index, piece, false);
            }
        }
        return;
    }

    // Selects piece from grid position
    if (0 <= index && index < GRID_SIZE * GRID_SIZE) {
        if (this->m_grid[index]) {
            // Stores held piece
            if (this->hold(index)) {
                this->m_moveManager.calculateMoves(index, this->m_heldPiece, this->m_grid);
            }
        }
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
    this->m_heldPieceOriginPos = -1;

    // Phantom
    this->m_phantomAttack = -1;
    this->m_phantomLocation = -1;
}

void BoardManager::setBoard(const std::string& FEN) {
    this->clearBoard();

    std::string pieceFEN = FEN.substr(0, FEN.find(' '));
    std::string metadata = FEN.substr(FEN.find(' ') + 1);

    // Load metadata
    this->getMetadata(metadata);

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
            FLAG pieceColour = (isupper(c) ? PIECE_WHITE : PIECE_BLACK);
            FLAG pieceType;
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
    this->setMetadata();
}

// ----- Update ----- Hidden -----

void BoardManager::getMetadata(std::string& metadata) {
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

void BoardManager::setMetadata() {
    // Adds metadata to pieces on board
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        FLAG pieceType = Piece::getFlag(this->m_grid[i], MASK_TYPE);
        FLAG pieceColour = Piece::getFlag(this->m_grid[i], MASK_COLOUR);

        switch (pieceType) {
        case PIECE_PAWN: {
                // White pawn on start square
                int y = i / GRID_SIZE;
                if (y == 1 && pieceColour == PIECE_WHITE) {
                    Piece::addFlag(&this->m_grid[i], MASK_PAWN_FIRST_MOVE);
                }
                else if (y == 6 && pieceColour == PIECE_BLACK) {
                    Piece::addFlag(&this->m_grid[i], MASK_PAWN_FIRST_MOVE);
                }
            }
            break;
        case PIECE_KNIGHT:
            break;
        case PIECE_BISHOP:
            break;
        case PIECE_ROOK:
            // Queen side castling
            if (i % GRID_SIZE == 0) {
                Piece::addFlag(&this->m_grid[i], MASK_ROOK_CAN_CASTLE);
            }
            // King side castling
            if (i % GRID_SIZE == GRID_SIZE - 1) {
                Piece::addFlag(&this->m_grid[i], MASK_ROOK_CAN_CASTLE);
            }
            break;
        case PIECE_QUEEN:
            break;
        case PIECE_KING:
            // White meta
            if (pieceColour == PIECE_WHITE) {
                if (this->m_castling[BOARD_CASTLING_WHITE_KING]) {
                    Piece::addFlag(&this->m_grid[i], MASK_KING_CASTLE_KING);
                }
                if (this->m_castling[BOARD_CASTLING_WHITE_QUEEN]) {
                    Piece::addFlag(&this->m_grid[i], MASK_KING_CASTLE_QUEEN);
                }
            }
            // Black meta
            else {
                if (this->m_castling[BOARD_CASTLING_BLACK_KING]) {
                    Piece::addFlag(&this->m_grid[i], MASK_KING_CASTLE_KING);
                }
                if (this->m_castling[BOARD_CASTLING_BLACK_QUEEN]) {
                    Piece::addFlag(&this->m_grid[i], MASK_KING_CASTLE_QUEEN);
                }
            }
            break;
        default:
            break;

        }
    }
}

bool BoardManager::hold(INDEX index) {
    // Checks piece colour
    PIECE piece = this->m_grid[index];
    if (this->m_currentTurn != Piece::getFlag(piece, MASK_COLOUR)) {
        return false;
    }

    this->m_heldPiece = piece;
    Piece::addFlag(&this->m_heldPiece, MASK_HELD);
    
    this->m_heldPieceOriginPos = index;
    this->m_grid[index] = 0;

    return true;
}

void BoardManager::release(INDEX index, PIECE piece, bool moved) {
    // Put held piece down on specified square
    Piece::removeFlag(&piece, MASK_HELD);
    this->m_grid[index] = piece;
    

    this->m_heldPiece = 0;
    this->m_heldPieceOriginPos = -1;

    // Deal with phantom
    this->managePhantom(index, piece);

    if (moved) {
        Piece::removeFlags(index, this->m_grid);
    }
}

void BoardManager::managePhantom(INDEX index, PIECE piece) {
    // Check if pawn in on phantom square
    FLAG type = Piece::getFlag(this->m_grid[index], MASK_TYPE);
    if (type == PIECE_PAWN && index == this->m_phantomLocation) {
        this->m_grid[m_phantomAttack] = 0;
    }
    
    // Remove phantom always
    if (0 <= this->m_phantomLocation && this->m_phantomLocation < GRID_SIZE * GRID_SIZE) {
        if (this->m_grid[this->m_phantomLocation] == PIECE_PHANTOM){
            this->m_grid[m_phantomLocation] = 0;
            this->m_phantomLocation = -1;
            this->m_phantomAttack = -1;
        }
    }

    // Check if pawn
    if (Piece::getFlag(this->m_grid[index], MASK_TYPE) == PIECE_PAWN) {
        // Check if phantom should be created
        FLAG enPassent = Piece::getFlag(piece, MASK_PAWN_EN_PASSENT);
        if (enPassent) {
            FLAG colour = Piece::getFlag(piece, MASK_COLOUR);
            // White side phantom
            if (colour == PIECE_WHITE) {
                this->m_phantomLocation = index - GRID_SIZE;
            }
            // Black side phantom
            else {
                this->m_phantomLocation = index + GRID_SIZE;
            }
            this->m_phantomAttack = index;
            this->m_grid[m_phantomLocation] = PIECE_PHANTOM;
        }
    }
}

//  ----- Destruction -----

BoardManager::~BoardManager() {
    // Nothing todo
}

