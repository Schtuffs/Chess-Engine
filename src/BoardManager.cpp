#include "BoardManager.h"

#include "WindowManager.h"
#include "MoveManager.h"

// ----- Creation -----

BoardManager::BoardManager(GLenum boardColourStyle, const std::string& FEN) {
    // Setup board with FEN string
    this->setBoard(FEN);

    // Selects board colouring
    this->setBoardColour(boardColourStyle);

    // Setting
    this->m_heldPiece = 0;
    this->m_heldPieceOriginPos = 0;
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
            int index = y * GRID_SIZE + x;

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

            // Render colour based on evenness
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

void BoardManager::check(int index) {
    // If a piece is held, try to release it
    if (this->m_heldPiece) {
        // Store piece incase of valid
        int piece = this->m_moveManager.isValidMove(index);
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
    this->m_heldPieceOriginPos = 0;
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
    // Adds king metadata
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        int pieceType = this->m_grid[i] & MASK_TYPE;
        int pieceColour = this->m_grid[i] & MASK_COLOUR;

        // King metadata
        if (pieceType == PIECE_KING) {
            // White meta
            if (pieceColour == PIECE_WHITE) {
                if (this->m_castling[BOARD_CASTLING_WHITE_KING]) {
                    this->m_grid[i] |= MASK_KING_CASTLE_KING;
                }
                else if (this->m_castling[BOARD_CASTLING_WHITE_QUEEN]) {
                    this->m_grid[i] |= MASK_KING_CASTLE_KING;
                }
            }
            // Black meta
            else {
                if (this->m_castling[BOARD_CASTLING_BLACK_KING]) {
                    this->m_grid[i] |= MASK_KING_CASTLE_KING;
                }
                else if (this->m_castling[BOARD_CASTLING_BLACK_QUEEN]) {
                    this->m_grid[i] |= MASK_KING_CASTLE_KING;
                }
            }
        }

        // Pawn metadata
        else if (pieceType == PIECE_PAWN) {
            // White pawn on start square
            int y = i / GRID_SIZE;
            if (y == 1 && pieceColour == PIECE_WHITE) {
                this->m_grid[i] |= MASK_PAWN_FIRST_MOVE;
            }
            else if (y == 6 && pieceColour == PIECE_BLACK) {
                this->m_grid[i] |= MASK_PAWN_FIRST_MOVE;
            }
        }
    }
}

bool BoardManager::hold(int index) {
    // Checks piece colour
    int piece = this->m_grid[index];
    if (this->m_currentTurn != (piece & MASK_COLOUR)) {
        return false;
    }

    this->m_heldPiece = MASK_HELD | piece;
    this->m_heldPieceOriginPos = index;
    this->m_grid[index] = 0;

    return true;
}

void BoardManager::release(int index, int piece, bool moved) {
    // Check if pawn
    if ((this->m_heldPiece & MASK_TYPE) == PIECE_PAWN) {
        // Check if phantom
        if (this->m_grid[index] == PIECE_PHANTOM){ 
            this->m_grid[m_phantomAttack] = 0;
        }
    }

    // Put held piece down on specified square
    this->m_grid[index] = piece ^ MASK_HELD;
    this->m_heldPiece = 0;
    if (moved) {
        this->removeFlags(index);
    }
}

void BoardManager::removeFlags(int index) {
    // Remove phantom
    if (this->m_grid[m_phantomLocation] == PIECE_PHANTOM) {
        this->m_grid[m_phantomLocation] = 0;
        this->m_phantomLocation = -1;
    }
    
    // Piece information
    int type = this->m_grid[index] & MASK_TYPE;
    switch (type) {
    case PIECE_PAWN:
        this->removePawnFlags(index);
        break;
    case PIECE_ROOK:
        this->removeRookFlags(index);
        break;
    case PIECE_KING:
        this->removeKingFlags(index);
        break;
    }
}

void BoardManager::removePawnFlags(int index) {
    // Piece information
    int piece = this->m_grid[index];
    int colour = piece & MASK_COLOUR;

    // Remove first move flag
    if ((piece & MASK_PAWN_FIRST_MOVE)) {
        // Removes first move mask
        this->m_grid[index] = (piece ^ MASK_PAWN_FIRST_MOVE);

        if (this->m_grid[index] & MASK_PAWN_EN_PASSENT) {
            // Remove en passent
            this->m_grid[index] ^= MASK_PAWN_EN_PASSENT;
            
            // Add phantom piece for white
            if (colour == PIECE_WHITE) {
                this->m_grid[index - GRID_SIZE] = PIECE_PHANTOM;
                this->m_phantomLocation = index - GRID_SIZE;
            }
            // Add phantom piece for black
            else {
                this->m_grid[index + GRID_SIZE] = PIECE_PHANTOM;
                this->m_phantomLocation = index + GRID_SIZE;
            }
            // Set phantom attack
            this->m_phantomAttack = index;
        }
    }
}

void BoardManager::removeKnightFlags(int index){ 

}

void BoardManager::removeBishopFlags(int index) {

}

void BoardManager::removeRookFlags(int index) {
    // Piece information
    int colour = this->m_grid[index] & MASK_COLOUR;
    
    // Remove castling rights from the king
    if (colour == PIECE_WHITE) {
        // Default white king square
        if ((this->m_grid[4] & MASK_TYPE) == PIECE_KING) {
            // Remove queenside from castling rights
            if ((this->m_grid[0] & MASK_TYPE) != PIECE_ROOK) {
                // Check if bit should be removed
                if ((this->m_grid[4] & MASK_KING_CASTLE_QUEEN) != 0) {
                    // Unflip
                    this->m_grid[4] ^= MASK_KING_CASTLE_QUEEN;
                }
            }
            // Remove kingside from castling rights
            if ((this->m_grid[7] & MASK_TYPE) != PIECE_ROOK) {
                // Check if bit should be removed
                if ((this->m_grid[4] & MASK_KING_CASTLE_KING) != 0) {
                    // Unflip
                    this->m_grid[4] ^= MASK_KING_CASTLE_KING;
                }
            }
        }
    }
    else {
        // Default black king square
        if ((this->m_grid[60] & MASK_TYPE) == PIECE_KING) {
            // Remove queenside from castling rights
            if ((this->m_grid[55] & MASK_TYPE) != PIECE_ROOK) {
                // Check if bit should be removed
                if ((this->m_grid[60] & MASK_KING_CASTLE_QUEEN) != 0) {
                    // Unflip
                    this->m_grid[60] ^= MASK_KING_CASTLE_QUEEN;
                }
            }
            // Remove kingside from castling rights
            if ((this->m_grid[63] & MASK_TYPE) != PIECE_ROOK) {
                // Check if bit should be removed
                if ((this->m_grid[60] & MASK_KING_CASTLE_KING) != 0) {
                    // Unflip
                    this->m_grid[60] ^= MASK_KING_CASTLE_KING;
                }
            }
        }
    }
}

void BoardManager::removeQueenFlags(int index) {

}

void BoardManager::removeKingFlags(int index) {
    // Remove castling if moved
    if ((this->m_grid[index] & MASK_KING_CASTLE_KING) != 0) {
        // Unflip
        this->m_grid[index] ^= MASK_KING_CASTLE_KING;
    }
    if ((this->m_grid[index] & MASK_KING_CASTLE_QUEEN) != 0) {
        // Unflip
        this->m_grid[index] ^= MASK_KING_CASTLE_QUEEN;
    }
}

//  ----- Destruction -----

BoardManager::~BoardManager() {
    // Nothing todo
}

