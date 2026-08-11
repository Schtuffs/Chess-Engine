#include "Board.h"

#include "Convert.h"
#include "Fen.h"
#include "Utils.h"

constexpr Index INVALID_ENPASSANT = 64;

// ----- Creation ----- Destruction -----

Board::Board(std::string_view fen)
    : m_fen(fen), m_castling(0), m_enPassant(INVALID_ENPASSANT),
      m_playerColour(Enums::Colour::White)
{
    if (!Fen::IsValidFen(m_fen.c_str())) {
        ErrorPrintln("Board::Board: Invalid fen: {}", m_fen);
        m_fen = DEFAULT_FEN;
    }
    DebugPrintln("Board::Board: Fen: {}", m_fen);

    // Get pieces
    u8 index = 0;
    for (u64 rank = 8 - 1; rank < 8; rank--) {
        for (u64 file = 0; file < 8; file++) {
            u64  i = rank * 8 + file;
            char c = m_fen[index++];

            if (isdigit(c)) {
                file += c - '0' - 1;
                continue;
            }

            if (c == '/') {
                rank++;
                break;
            }

            m_pieces[i] = Piece::FromChar(c, i);
        }
    }

    // Get player
    m_playerColour = (m_fen[index + 1] == 'w' ? Enums::Colour::White : Enums::Colour::Black);

    // Get castling
    std::string_view castling = m_fen.substr(index + 3);

    char c;
    index = 0;
    while ((c = castling[index++]) != ' ') {
        switch (c) {
        case 'K':
            m_castling |= static_cast<u8>(Enums::Castling::White_King);
            break;
        case 'Q':
            m_castling |= static_cast<u8>(Enums::Castling::White_Queen);
            break;
        case 'k':
            m_castling |= static_cast<u8>(Enums::Castling::Black_King);
            break;
        case 'q':
            m_castling |= static_cast<u8>(Enums::Castling::Black_Queen);
            break;
        }
    }

    // Get en passant
    std::string_view enPassant = castling.substr(index);
    if (enPassant[0] != '-') {
        m_enPassant           = Convert::MoveToIndex(enPassant);
        m_pieces[m_enPassant] = Piece(m_enPassant);
    }

    // Get move counts
    std::string_view halfMoves = enPassant.substr(enPassant.find(' ') + 1);
    std::string_view fullMoves = halfMoves.substr(halfMoves.find(' ') + 1);
    if (fullMoves.length() == 1 && fullMoves[0] == '1' && m_playerColour == Enums::Colour::White) {
        m_fen[m_fen.length() - 1] = '0';
    }
}

Board::~Board() {}

// ----- Read -----

u8 Board::Castling() const noexcept { return m_castling; }

std::span<const Piece, 64> Board::Pieces() const noexcept { return m_pieces; }

std::string_view Board::Fen() const noexcept { return m_fen; }

// ----- Update -----

bool Board::MakeMove(std::string_view move)
{
    // Validate input
    move = Convert::CastleToMove(move, m_playerColour);
    if (move.length() < 4) {
        return false;
    }

    // Attempt to play the move
    if (!ValidateMove(move)) {
        InfoPrintln("Board::MakeMove: Could not play move: {}", move);
        return false;
    }

    // Perform specific piece based functions
    MoveEnPassant(move);
    MoveCastling(move);

    // Play the move and swap turns
    Index startPos = Convert::MoveToIndex(move);
    Index endPos   = Convert::MoveToIndex(move.substr(2));
    Piece piece    = m_pieces[startPos];
    Piece other    = m_pieces[endPos];
    MovePiece(move);
    piece = m_pieces[endPos];
    bool captureOrPawn =
        (piece.Type() == Enums::Type::Pawn) | (other.IsValid() | other.IsEnPassant());
    m_playerColour = Utils::SwapColour(m_playerColour);
    m_fen          = RecalculateFen(captureOrPawn);
    DebugPrintln("Board::MakeMove: Fen: {}", m_fen);

    return true;
}

// ----- Update ----- Hidden -----

bool Board::ValidateMove(std::string_view move)
{
    Index start = Convert::MoveToIndex(move);
    Index end   = Convert::MoveToIndex(move.substr(2));

    // Check indexes
    if (!Utils::IsValidIndex(start) || !Utils::IsValidIndex(end)) {
        WarningPrintln("Board::ValidateMove: Invalid start or end pos: {}, {}", start, end);
        return false;
    }

    // Piece returned to start square
    if (start == end) {
        InfoPrintln("Board::ValidateMove: Piece returned to starting position.");
        return false;
    }

    // Check piece validity
    const Piece& piece = m_pieces[start];
    if (piece.Colour() != m_playerColour) {
        WarningPrintln("Board::ValidateMove: Invalid piece selected at start position: {}",
                       piece.ToString());
        return false;
    }

    if (piece.Type() == Enums::Type::Pawn) {
        if (!ValidatePromotion(move)) {
            return false;
        }
    }

    return true;
}

bool Board::ValidatePromotion(std::string_view move)
{
    // Gather information
    Index  start = Convert::MoveToIndex(move);
    Index  end   = Convert::MoveToIndex(move.substr(2));
    Piece& piece = m_pieces[start];

    // Must be pawn to check promo
    if (piece.Type() != Enums::Type::Pawn) {
        return true;
    }

    // Check if promoting
    Index rank = end / 8;
    if (piece.Colour() == Enums::Colour::White && rank != 7) {
        return true;
    }
    if (piece.Colour() == Enums::Colour::Black && rank != 0) {
        return true;
    }

    // Must have promo info
    if (move.length() < 5) {
        return false;
    }

    // Turn the piece into promo
    char promo = tolower(move[4]);
    switch (promo) {
    case 'q':
        piece = Piece(piece.Colour(), Enums::Type::Queen, piece.Position());
        break;
    case 'r':
        piece = Piece(piece.Colour(), Enums::Type::Rook, piece.Position());
        break;
    case 'b':
        piece = Piece(piece.Colour(), Enums::Type::Bishop, piece.Position());
        break;
    case 'n':
        piece = Piece(piece.Colour(), Enums::Type::Knight, piece.Position());
        break;
    default:
        WarningPrintln("Board::ValidatePromotion: Invalid promotion type: {}", promo);
        return false;
    }
    
    return true;
}

void Board::MovePiece(std::string_view move)
{
    Index start = Convert::MoveToIndex(move);
    Index end   = Convert::MoveToIndex(move.substr(2));

    Piece piece = m_pieces[start];

    // Swap pieces
    piece.Position(end);
    m_pieces[end]   = piece;
    m_pieces[start] = Piece();
}

void Board::MoveEnPassant(std::string_view move)
{
    // Reset en passant
    Index enPassant = m_enPassant;
    m_enPassant     = INVALID_ENPASSANT;

    // Remove old en passant
    if (enPassant != INVALID_ENPASSANT) {
        m_pieces[enPassant] = Piece();
    }

    // Calculate information
    Index        start = Convert::MoveToIndex(move);
    Index        end   = Convert::MoveToIndex(move.substr(2));
    const Piece& piece = m_pieces[start];

    // Ensure piece is pawn
    if (piece.Type() != Enums::Type::Pawn) {
        return;
    }

    // Update en passant square (if necessary)
    i8 offset = (piece.Colour() == Enums::Colour::White ? (i8)8 : -(i8)8);
    if (start + (offset * 2) == end) {
        m_enPassant           = start + offset;
        m_pieces[m_enPassant] = Piece(m_enPassant);
    }

    // Must be taking en passant square
    if (end == enPassant) {
        // Eliminate the peasant
        m_pieces[enPassant - offset] = Piece();
    }
}

void Board::MoveCastling(std::string_view move)
{
    if (move.length() < 4) {
        ErrorPrintln("Board::MoveCastling: move ({}) was too short: {}", move, move.length());
        return;
    }

    Index start = Convert::MoveToIndex(move);
    Index end   = Convert::MoveToIndex(move.substr(2));

    Piece& piece = m_pieces[start];
    if (piece.Type() == Enums::Type::King) {
        // Short castle
        if (start + 2 == end) {
            Piece& rook = m_pieces[end + 1];
            rook.Position(end - 1);
            m_pieces[end - 1] = rook;
            m_pieces[end + 1] = Piece();
        }
        // Long castle
        else if (start - 2 == end) {
            Piece& rook = m_pieces[end - 2];
            rook.Position(end + 1);
            m_pieces[end + 1] = rook;
            m_pieces[end - 2] = Piece();
        }
    }
}

// ----- Fen -----

std::string Board::RecalculateFen()
{
    char        player    = RecalculatePlayer();
    std::string castling  = RecalculateCastling();
    std::string enPassant = RecalculateEnPassant();
    u32         halfMoves = RecalculateHalfMoves(false) - 1;
    u32 fullMoves = RecalculateFullMoves() - (m_playerColour == Enums::Colour::White ? 1 : 0);
    if (m_playerColour == Enums::Colour::White) {
        fullMoves--;
    }
    return Fen::GenerateFen(m_pieces, player, castling, enPassant, halfMoves, fullMoves);
}

std::string Board::RecalculateFen(bool isCaptureOrPawn)
{
    char        player    = RecalculatePlayer();
    std::string castling  = RecalculateCastling();
    std::string enPassant = RecalculateEnPassant();
    u32         halfMoves = RecalculateHalfMoves(isCaptureOrPawn);
    u32         fullMoves = RecalculateFullMoves();
    return Fen::GenerateFen(m_pieces, player, castling, enPassant, halfMoves, fullMoves);
}

char Board::RecalculatePlayer() { return (m_playerColour == Enums::Colour::White ? 'w' : 'b'); }

std::string Board::RecalculateCastling()
{
    std::string castle = "";

    // Remove all castling if king moved
    if (m_pieces[4].Type() != Enums::Type::King) {
        m_castling &= ~(static_cast<u8>(Enums::Castling::White_King) |
                        static_cast<u8>(Enums::Castling::White_Queen));
    }
    if (m_pieces[60].Type() != Enums::Type::King) {
        m_castling &= ~(static_cast<u8>(Enums::Castling::Black_King) |
                        static_cast<u8>(Enums::Castling::Black_Queen));
    }

    // Individual castling checks
    if (m_pieces[7].Type() != Enums::Type::Rook) {
        m_castling &= ~(static_cast<u8>(Enums::Castling::White_King));
    }
    if (m_pieces[0].Type() != Enums::Type::Rook) {
        m_castling &= ~(static_cast<u8>(Enums::Castling::White_Queen));
    }
    if (m_pieces[63].Type() != Enums::Type::Rook) {
        m_castling &= ~(static_cast<u8>(Enums::Castling::Black_King));
    }
    if (m_pieces[56].Type() != Enums::Type::Rook) {
        m_castling &= ~(static_cast<u8>(Enums::Castling::Black_Queen));
    }

    if (m_castling & static_cast<u8>(Enums::Castling::White_King)) {
        castle += "K";
    }
    if (m_castling & static_cast<u8>(Enums::Castling::White_Queen)) {
        castle += "Q";
    }
    if (m_castling & static_cast<u8>(Enums::Castling::Black_King)) {
        castle += "k";
    }
    if (m_castling & static_cast<u8>(Enums::Castling::Black_Queen)) {
        castle += "q";
    }

    if (castle.length() == 0) {
        castle = "-";
    }

    return castle;
}

std::string Board::RecalculateEnPassant()
{
    std::string enPassant = "-";
    if (m_enPassant != INVALID_ENPASSANT) {
        enPassant = Convert::IndexToMove(m_enPassant);
    }

    return enPassant;
}

u32 Board::RecalculateHalfMoves(bool isCaptureOrPawn)
{
    if (isCaptureOrPawn) {
        return 0;
    }

    std::string_view strHalfMoves = m_fen.substr(m_fen.find(' ') + 1);
    strHalfMoves                  = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    strHalfMoves                  = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    strHalfMoves                  = strHalfMoves.substr(strHalfMoves.find(' ') + 1);

    u32 halfMoves = std::stoul(strHalfMoves.data());
    return (halfMoves + 1);
}

u32 Board::RecalculateFullMoves()
{
    std::string_view strHalfMoves = m_fen.substr(m_fen.find(' ') + 1);
    strHalfMoves                  = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    strHalfMoves                  = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    strHalfMoves                  = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    strHalfMoves                  = strHalfMoves.substr(strHalfMoves.find(' ') + 1);

    u32 fullMoves = std::stoul(strHalfMoves.data());
    if (m_playerColour == Enums::Colour::Black) {
        fullMoves++;
    }

    return fullMoves;
}
