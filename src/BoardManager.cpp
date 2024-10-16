#include "BoardManager.h"

#include "WindowManager.h"
#include "Piece.h"
#include "Move.h"

// ----- Creation -----

BoardManager::BoardManager(Player& white, Player& black, GLenum boardColourStyle, bool flipBoard, const std::string& FEN) : m_whitePlayer(white), m_blackPlayer(black) {
    // Setup FEN for setting board
    this->m_resetFEN = FEN;

    // Selects board colouring
    this->setBoardColour(boardColourStyle);
    
    // Setup board with FEN string
    this->resetBoard();
    
    // Setting
    this->m_heldPieceIndex = CODE_INVALID;
    this->m_promotionIndex = CODE_INVALID;
    this->m_flipBoard = flipBoard;
    this->m_whitePerspective = true;
    this->m_calculated = false;
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
        INDEX index = i;
        // Only flip is player is human, its blacks turn, and board should flip
        if (!this->m_whitePerspective) {
            index = Library::flipIndex(index);
        }
        // If there is a piece on the grid square
        if (this->m_grid[i] && !Piece::getFlag(this->m_grid[i], MASK_HELD)) {
            this->m_renderer.render(this->m_grid[i], index % GRID_SIZE, index / GRID_SIZE);
        }
    }

    // Renders held piece so its on top
    if (this->m_heldPieceIndex != CODE_INVALID) {
        POINT mousePos = WindowManager::cursorPos();
        PIECE held = this->m_grid[m_heldPieceIndex];
        this->m_renderer.render(held, mousePos.x, mousePos.y);
    }

    // Render promotion screen if promotion is valid
    if (this->m_promotionIndex != CODE_INVALID) {
        this->showPromotionOptions();
    }

    // If board is in a state of checkmate
    if (this->m_checkmate) {
        // Render checkmate screen
    }
}

void BoardManager::showBoard() { 
    // Scales board based on smaller side so it stays square
    GLfloat scale = Library::min(WindowManager::winSize()) / GRID_SIZE;

    // Loop through each grid index
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            INDEX index = y * GRID_SIZE + x;

            // Flip board if specified
            if (!this->m_whitePerspective) {
                index = Library::flipIndex(index);
            }

            // Render move squares with a slightly changed colour mask
            COLOUR mask = { 0.0f, 0.0f, 0.0f};
            if (this->m_heldPieceIndex != CODE_INVALID) {
                // Render piece origin square as gold
                if (index == this->m_heldPieceIndex) {
                    COLOUR gold = { 0.85f, 0.75f, 0.4f };
                    this->m_renderer.rect(gold, x * scale, y * scale, scale, scale);
                    continue;
                }

                // Check for moves and render as red
                auto moves = this->m_moveManager.getMoves(this->m_heldPieceIndex);
                for (Move& move : moves) {
                    if (index == move.Target()) {
                        mask.r = 0.3f;
                        mask.g = -0.3f;
                        mask.b = -0.3f;
                        break;
                    }
                }
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

void BoardManager::ManageInput(INDEX index) {
    // Do nothing if board is in checkmate or stalemate
    if (this->m_checkmate || this->m_stalemate) {
        return;
    }

    // If piece is black and a human, flip inputs
    if (!this->m_whitePerspective) {
        index = Library::flipIndex(index);
    }

    // First, check if promotion is happening
    if (this->m_promotionIndex != CODE_INVALID) {
        // Determine which option was hit
        this->promotionSelection(index);

        // When promoting, don't allow other functions to happen
        return;
    }
    
    // If a piece is held, try to release it
    if (this->m_heldPieceIndex != CODE_INVALID) {
        // Store piece incase of valid
        Move move(this->m_heldPieceIndex, index, 0);
        bool isMove = this->m_moveManager.isLegal(move);
        // Only switch turn if piece was placed elsewhere
        if (index != this->m_heldPieceIndex && isMove) {
            this->release(move);
            this->m_currentPlayer = (this->m_currentPlayer->Colour() == PLAYER_COLOUR_WHITE ? &this->m_blackPlayer : &this->m_whitePlayer);
            // Allows board to flip
            if (this->m_flipBoard) {
                this->m_whitePerspective = (this->m_currentPlayer->Colour() == PLAYER_COLOUR_WHITE ? true : false);
            }
        }
        else if (isMove) {
            this->release(move);
        }
        return;
    }

    // Selects piece from grid position
    if (0 <= index && index < GRID_SIZE * GRID_SIZE) {
        if (this->m_grid[index]) {
            // Stores held piece
            this->hold(index);
        }
    }
}

// ----- Read ----- Hidden -----

void BoardManager::showPromotionOptions() {
    // Holds value for showing next index
    int nextPiece = ((this->m_promotionIndex / GRID_SIZE) == 0 ? GRID_SIZE : (-GRID_SIZE));

    // Render over other squares
    GLfloat scale = Library::min(WindowManager::winSize()) / GRID_SIZE;
    FLAG colour = Piece::getFlag(this->m_grid[this->m_promotionIndex], MASK_COLOUR);

    // Holds pieces
    PIECE pieces[] = { PIECE_QUEEN, PIECE_ROOK, PIECE_BISHOP, PIECE_KNIGHT };
    
    for (int i = 0; i < 4; i++) {
        // Render square

        // Variable setup for rendering
        int x = (this->m_promotionIndex % GRID_SIZE);
        int y = (this->m_promotionIndex / GRID_SIZE) + (i * nextPiece / GRID_SIZE);

        // Render a golden square under promotion options
        COLOUR gold = { 0.85f, 0.75f, 0.4f };
        this->m_renderer.rect(gold, x * scale, y * scale, scale, scale);

        PIECE piece = pieces[i] | colour;
        this->m_renderer.render(piece, x, y);
    }
    
}

void BoardManager::checkCheckmate(Move& move) {
    // Calculates if move put king into check
    this->m_moveManager.clear();
    this->m_moveManager.calculateMoves(this->m_currentPlayer->Colour(), this->m_grid, false);
    auto moves = this->m_moveManager.getMoves();
    if (moves.size() == 1 && (moves[0].Flags(MOVE_CHECK) == MOVE_CHECK)) {
        if (this->m_currentPlayer->Colour() == PLAYER_COLOUR_WHITE) {
            // Check black king if white moves
            Piece::addFlag(&this->m_grid[this->m_blackKing], MOVE_CHECK);
        }
        else {
            // Check white king if black moved
            Piece::addFlag(&this->m_grid[this->m_whiteKing], MOVE_CHECK);
        }
    }
    
    // Determine if there are any moves than can prevent checkmate
    FLAG colour = (this->m_currentPlayer->Colour() == PLAYER_COLOUR_WHITE ? PLAYER_COLOUR_BLACK : PLAYER_COLOUR_WHITE);
    this->m_moveManager.calculateMoves(colour, this->m_grid, true);
    moves = this->m_moveManager.getMoves();
    

    int totalPieces = 0;
    // Determine how many total pieces there are on the board
    for (INDEX i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        if (Piece::getFlag(this->m_grid[i], MASK_COLOUR) == colour) {
            totalPieces++;
        }
    }
    // Determine if legal moves is greater than pieces
    if ((int)this->m_moveManager.getMoves().size() > totalPieces) {
        // More moves than pieces, theres a legal move somewhere
        return;
    }

    // King is in check with no moves
    if (Piece::getFlag(this->m_grid[this->m_blackKing], MOVE_CHECK) ||
        Piece::getFlag(this->m_grid[this->m_whiteKing], MOVE_CHECK)) {
            // CHECKMATE
            this->m_checkmate = true;
            std::cout << "CHECKMATE" << std::endl;
    }
    else {
        // King is not in check with no moves
        this->m_stalemate = true;
        std::cout << Piece::getFlag(this->m_grid[this->m_whiteKing], MOVE_CHECK) << std::endl;;
        std::cout << "STALEMATE" << std::endl;
    }
}

// ----- Update -----

bool BoardManager::makeMove(Move& move) {
    if (this->m_moveManager.isLegal(move)) {
        this->release(move);
        return true;
    }
    return false;
}

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
    this->m_currentPlayer = &this->m_whitePlayer;

    // Unholds piece and clears moves
    this->m_heldPieceIndex = CODE_INVALID;
    this->m_moveManager.clear();

    // Phantom
    this->m_phantomAttack = CODE_INVALID;
    this->m_phantomLocation = CODE_INVALID;

    // Reset checkmate and stalemate
    this->m_checkmate = false;
    this->m_stalemate = false;
}

void BoardManager::resetBoard() {
    this->clearBoard();

    std::string pieceFEN = this->m_resetFEN.substr(0, this->m_resetFEN.find(' '));
    std::string metadata = this->m_resetFEN.substr(this->m_resetFEN.find(' ') + 1);

    // Load metadata
    this->readMetadata(metadata);

    // Loop through each index
    // Starts from top left, goes to bottom right
    int x = 0, y = GRID_SIZE - 1;
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
            FLAG pieceType = 0;
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

void BoardManager::setPromotion(INDEX index) {
    if (0 <= index && index < GRID_SIZE * GRID_SIZE)
        this->m_promotionIndex = index;
    else
        this->m_promotionIndex = -1;
}

void BoardManager::changeFlip() {
    this->m_flipBoard = !this->m_flipBoard;
}

void BoardManager::changePerspective() {
    this->m_whitePerspective = !this->m_whitePerspective;
}

// ----- Update ----- Hidden -----

void BoardManager::readMetadata(std::string& metadata) {
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
            this->m_currentPlayer = (metadata[i] == 'w' ? &this->m_whitePlayer : &this->m_blackPlayer);
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
                    Piece::addFlag(&this->m_grid[i], MOVE_PAWN_FIRST_MOVE);
                }
                else if (y == (GRID_SIZE - 2) && pieceColour == PIECE_BLACK) {
                    Piece::addFlag(&this->m_grid[i], MOVE_PAWN_FIRST_MOVE);
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
                Piece::addFlag(&this->m_grid[i], MOVE_ROOK_CAN_CASTLE);
            }
            // King side castling
            if (i % GRID_SIZE == GRID_SIZE - 1) {
                Piece::addFlag(&this->m_grid[i], MOVE_ROOK_CAN_CASTLE);
            }
            break;
        case PIECE_QUEEN:
            break;
        case PIECE_KING:
            // White meta
            if (pieceColour == PIECE_WHITE) {
                this->m_whiteKing = i;
                if (this->m_castling[BOARD_CASTLING_WHITE_KING]) {
                    Piece::addFlag(&this->m_grid[i], MOVE_KING_CASTLE_KING);
                }
                if (this->m_castling[BOARD_CASTLING_WHITE_QUEEN]) {
                    Piece::addFlag(&this->m_grid[i], MOVE_KING_CASTLE_QUEEN);
                }
            }
            // Black meta
            else {
                this->m_blackKing = i;
                if (this->m_castling[BOARD_CASTLING_BLACK_KING]) {
                    Piece::addFlag(&this->m_grid[i], MOVE_KING_CASTLE_KING);
                }
                if (this->m_castling[BOARD_CASTLING_BLACK_QUEEN]) {
                    Piece::addFlag(&this->m_grid[i], MOVE_KING_CASTLE_QUEEN);
                }
            }
            break;
        default:
            break;

        }
    }
}

void BoardManager::promotionSelection(INDEX index) {
    // Holds value for checking next index
    int checkValue = ((this->m_promotionIndex / GRID_SIZE) == 0 ? GRID_SIZE : (-GRID_SIZE));

    // Determine if valid option was clicked
    FLAG colour = Piece::getFlag(this->m_grid[this->m_promotionIndex], MASK_COLOUR);
    // Top/Bottom - Queen
    if (index == this->m_promotionIndex) {
        this->m_grid[this->m_promotionIndex] = PIECE_QUEEN | colour;
        this->m_promotionIndex = CODE_INVALID;
    }
    // Second - Rook
    else if (index == this->m_promotionIndex + checkValue) {
        this->m_grid[this->m_promotionIndex] = PIECE_ROOK | colour;
        this->m_promotionIndex = CODE_INVALID;
    }
    // Third - Bishop
    else if (index == this->m_promotionIndex + (2 * checkValue)) {
        this->m_grid[this->m_promotionIndex] = PIECE_BISHOP | colour;
        this->m_promotionIndex = CODE_INVALID;
    }
    // Bottom/Top - Knight
    else if (index == this->m_promotionIndex + (3 * checkValue)) {
        this->m_grid[this->m_promotionIndex] = PIECE_KNIGHT | colour;
        this->m_promotionIndex = CODE_INVALID;
    }
}

void BoardManager::hold(INDEX index) {
    // Checks piece colour
    PIECE piece = this->m_grid[index];
    if (this->m_currentPlayer->Colour() != Piece::getFlag(piece, MASK_COLOUR)) {
        return;
    }

    this->m_heldPieceIndex = index;
    Piece::addFlag(&this->m_grid[m_heldPieceIndex], MASK_HELD);
    if (!this->m_calculated) {
        this->m_moveManager.calculateMoves(this->m_currentPlayer->Colour(), this->m_grid, true);
        this->m_calculated = true;
    }
}

void BoardManager::release(Move& move) {
    // Put held piece down on specified square
    PIECE piece = this->m_grid[this->m_heldPieceIndex];
    Piece::removeFlag(&piece, MASK_HELD);
    this->m_grid[move.Target()] = piece;
    
    // Make sure not to delete piece if it was not moved
    if (move.Start() != move.Target()) {
        // First, unchecks kings
        Piece::removeFlag(&this->m_grid[this->m_whiteKing], MOVE_CHECK);
        Piece::removeFlag(&this->m_grid[this->m_blackKing], MOVE_CHECK);
        this->m_grid[move.Start()] = PIECE_INVALID;
    }

    // Move king position
    if (Piece::getFlag(piece, MASK_TYPE) == PIECE_KING) {
        if (Piece::getFlag(piece, MASK_COLOUR) == PIECE_WHITE) {
            this->m_whiteKing = move.Target();
        }
        else {
            this->m_blackKing = move.Target();
        }
    }
    
    // Deal with phantom
    this->managePhantom(move);

    // Check if move put king into check
    if (move.Start() != move.Target()) {
        this->checkCheckmate(move);
    }
    
    // Clear old data
    this->m_heldPieceIndex = CODE_INVALID;
    if (move.Start() != move.Target()) {
        this->m_moveManager.clear();
        this->m_calculated = false;
    }

    if (move.Start() != move.Target()) {
        Piece::removeFlags(move.Target(), this->m_grid);
    }
}

void BoardManager::managePhantom(Move move) {
    INDEX target = move.Target();

    // Check if pawn in on phantom square
    FLAG type = Piece::getFlag(this->m_grid[target], MASK_TYPE);
    if (type == PIECE_PAWN && target == this->m_phantomLocation) {
        this->m_grid[m_phantomAttack] = PIECE_INVALID;
    }
    
    // Remove phantom always
    if (this->m_phantomLocation != CODE_INVALID) {
        if (this->m_grid[this->m_phantomLocation] == PIECE_PHANTOM) {
            this->m_grid[m_phantomLocation] = PIECE_INVALID;
        }
        this->m_phantomLocation = CODE_INVALID;
        this->m_phantomAttack = CODE_INVALID;
    }

    // Check if pawn
    if (Piece::getFlag(this->m_grid[target], MASK_TYPE) == PIECE_PAWN) {
        // Check if phantom should be created
        FLAG moveTwo = (move.Flags() & MOVE_PAWN_MOVE_TWO);
        if (moveTwo) {
            FLAG colour = Piece::getFlag(this->m_grid[target], MASK_COLOUR);
            // White side phantom
            if (colour == PIECE_WHITE) {
                this->m_phantomLocation = target - GRID_SIZE;
            }
            // Black side phantom
            else {
                this->m_phantomLocation = target + GRID_SIZE;
            }
            this->m_phantomAttack = target;
            this->m_grid[m_phantomLocation] = PIECE_PHANTOM;
        }
    }
}

//  ----- Destruction -----

BoardManager::~BoardManager() {
    // Nothing todo
}

