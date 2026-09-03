#include "Types/Position.h"

#include <bit>
#include <print>

#include "MoveGen/Magic.h"
#include "MoveGen/MoveGen.h"
#include "Utils/Convert.h"
#include "Utils/Fen.h"
#include "Utils/Utils.h"

// ----- Creation ----- Destruction -----

Position::Position(std::string_view fen) noexcept
    : m_state(std::make_shared<StateStore>()), m_castling(0)
{
    m_state->previous = nullptr;
    m_state->fen      = fen;
    m_state->captured = TYPE_NONE;

    if (!Fen::IsValidFen(m_state->fen.data())) {
        m_state->fen = Fen::DEFAULT;
    }
    m_bbColour.fill(0ull);
    m_bbType.fill(0ull);

    // Get pieces
    u8 idx = 0;
    fen    = m_state->fen;
    for (u64 rank = 7; rank < 8; rank--) {
        for (u64 file = 0; file < 8; file++) {
            Square sq = Square(rank * 8 + file);
            char   c  = fen[idx++];

            if (isdigit(c)) {
                file += c - '0' - 1;
                continue;
            }

            if (c == '/') {
                rank++;
                break;
            }

            switch (c) {
            case 'b':
            case 'B':
                m_bbType[BISHOP] |= sq;
                (isupper(c) ? m_bbColour[WHITE] : m_bbColour[BLACK]) |= sq;
                break;
            case 'k':
            case 'K':
                m_bbType[KING] |= sq;
                (isupper(c) ? m_bbColour[WHITE] : m_bbColour[BLACK]) |= sq;
                break;
            case 'n':
            case 'N':
                m_bbType[KNIGHT] |= sq;
                (isupper(c) ? m_bbColour[WHITE] : m_bbColour[BLACK]) |= sq;
                break;
            case 'p':
            case 'P':
                m_bbType[PAWN] |= sq;
                (isupper(c) ? m_bbColour[WHITE] : m_bbColour[BLACK]) |= sq;
                break;
            case 'q':
            case 'Q':
                m_bbType[QUEEN] |= sq;
                (isupper(c) ? m_bbColour[WHITE] : m_bbColour[BLACK]) |= sq;
                break;
            case 'r':
            case 'R':
                m_bbType[ROOK] |= sq;
                (isupper(c) ? m_bbColour[WHITE] : m_bbColour[BLACK]) |= sq;
                break;
            }
        }
    }

    // Get player
    m_player = (m_state->fen[idx + 1] == 'w' ? WHITE : BLACK);
    CalculateAttacks();

    // Get castling
    std::string_view castling = m_state->fen.substr(idx + 3);

    char c;
    idx = 0;
    while ((c = castling[idx++]) != ' ') {
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
    m_state->enPassant         = SQ_BAD;
    std::string_view enPassant = castling.substr(idx);
    if (enPassant[0] != '-') {
        m_state->enPassant = Convert::StrToSquare(enPassant);
    }

    // Get move counts
    std::string_view halfMoves = enPassant.substr(enPassant.find(' ') + 1);
    m_state->ply               = std::stoul(halfMoves.substr(0, halfMoves.find(' ')).data());

    std::string_view fullMoves = halfMoves.substr(halfMoves.find(' ') + 1);
    m_state->totalMoves        = std::stoul(fullMoves.substr(0, fullMoves.find(' ')).data());
}

// ----- Read -----

u8 Position::Castling() const noexcept { return m_castling; }

u8 Position::Checkers() const noexcept { return Checkers(Player()); }
u8 Position::Checkers(Colour player) const noexcept
{
    return m_state->checkers[(u8)player].Count();
}

Square Position::EnPassant() const noexcept { return m_state->enPassant; }

std::string Position::Fen() const noexcept { return m_state->fen; }

bool Position::IsCastleLegal(Square from, Square to) const noexcept
{
    Colour    us  = Player();
    Direction dir = (to > from ? EAST : WEST);
    BitBoard  bb  = from;

    if (!Utils::IsValidSquare(from)) {
        return false;
    }

    if (!Utils::IsValidSquare(to)) {
        return false;
    }

    // Kingside
    if (to > from) {
        bb = bb << 1;
        bb |= bb << 1;
    } else {
        bb = bb >> 1;
        bb |= bb >> 1;
        bb |= bb >> 1;
    }

    // Can't have other pieces there
    if (Pieces() & bb) {
        return false;
    }

    // No attacks allowed
    if (IsAttacked(from, Pieces(), us)) {
        return false;
    }
    if (IsAttacked(from + dir, Pieces(), us)) {
        return false;
    }
    if (IsAttacked(to, Pieces(), us)) {
        return false;
    }

    return true;
}

bool Position::IsLegal(Move move) const noexcept
{
    Colour us   = Player();
    Square from = move.From();
    Square to   = move.To();

    InfoPrintln("Position::IsLegal: Move: {}", move.Str());

    // Valid move
    if (!move.IsValid()) {
        InfoPrintln("Position::IsLegal: Invalid move: {}", move.Str());
        return false;
    }

    // Needs to be from us
    if (!(Pieces(us) & from)) {
        InfoPrintln("Position::IsLegal: Invalid from piece: {}", move.Str());
        return false;
    }

    // Cannot be to us
    if ((Pieces(us) & to)) {
        InfoPrintln("Position::IsLegal: Invalid to piece: {}", move.Str());
        return false;
    }

    // Special castling
    if (move.IsCastle()) {
        InfoPrintln("Position::IsLegal: Castle: {}", move.Str());
        return IsCastleLegal(from, to);
    }

    // En passant
    if (move.IsEnPassant()) {
        InfoPrintln("Position::IsLegal: En passant: {}", move.Str());
        Square mid = Square(from + ((to - from) / 2));
        return ((std::abs(from - to) == 16) && !(Pieces() & mid) && !(Pieces() & to));
    }

    // King
    if (Pieces(KING) & from) {
        InfoPrintln("Position::IsLegal: King: {}", move.Str());
        return !(IsAttacked(to, Pieces(), us));
    }

    // In Check
    if (Checkers(~Player())) {
        InfoPrintln("Position::IsLegal: Check: {}", move.Str());
        // Block/capture
        return (m_state->attackRays[us] & to);
    }

    // Check pawn
    if (Pieces(PAWN) & from) {
        InfoPrintln("Position::IsLegal: Pawn: {}", move.Str());
        if (to == m_state->enPassant) {
            return true;
        }

        Square fromFile = from % 8;
        Square toFile   = to % 8;

        // Check attack gets enemy
        if (fromFile != toFile) {
            if (!(Pieces(~m_player) & to)) {
                return false;
            }
        } else {
            // Move cannot be on another piece
            if ((Pieces() & to)) {
                return false;
            }
        }

        return true;
    }

    return true;
}

BitBoard Position::Pieces() const noexcept { return (m_bbColour[WHITE] | m_bbColour[BLACK]); }
BitBoard Position::Pieces(Colour colour, PieceType type) const noexcept
{
    return (m_bbColour[colour] & m_bbType[type]);
}
BitBoard Position::Pieces(Colour colour) const noexcept { return m_bbColour[colour]; }
BitBoard Position::Pieces(PieceType type) const noexcept { return m_bbType[type]; }
Colour   Position::Player() const noexcept { return m_player; }

std::string Position::Str() const noexcept
{
    const std::string VERT_SPACE = "\n  ┼───┼───┼───┼───┼───┼───┼───┼───┼\n";
    const std::string HORZ_SPACE = " │ ";
    std::string       ret = VERT_SPACE, line = HORZ_SPACE;

    // Pieces
    {
        int rank  = 1;
        u64 index = m_state->fen.find(' ');
        for (u64 i = 0; i < index; i++) {
            char c = m_state->fen[i];
            if (isalpha(c)) {
                line += c + HORZ_SPACE;
            }

            if (isdigit(c)) {
                int count = c - '0';
                while (count--) {
                    line += ' ' + HORZ_SPACE;
                }
            }

            if (c == '/') {
                ret += std::to_string(rank++) + line + VERT_SPACE;
                line = HORZ_SPACE;
            }
        }
        ret += std::to_string(rank++) + line + VERT_SPACE;
        ret += "    a   b   c   d   e   f   g   h\n";
    }

    // Turn, Moves
    {
        ret += "  Player: ";
        ret += (Player() == WHITE ? "White" : "Black");

        ret += ", Ply: ";
        ret += std::format("{:>2}", std::to_string(m_state->ply));

        ret += ", Move: ";
        ret += std::format("{:>3}", std::to_string(m_state->totalMoves));
    }

    // Fen
    {
        ret += "\n  Fen: ";
        ret += Fen();
    }

    return ret;
}

// ----- Read ----- Hidden -----

BitBoard Position::GetAttackRays() const noexcept
{
    Colour   us = Player();
    BitBoard rays;
    BitBoard attackers = m_state->checkers[us];
    Square   ksq       = Pieces(us, KING).PopLSB();

    while (attackers) {
        Square sq = attackers.PopLSB();

        if ((Pieces(~us, BISHOP) | Pieces(~us, QUEEN)) & sq) {
            rays |= Magic::GetKingAttacks(sq, ksq, false);
        }
        if ((Pieces(~us, ROOK) | Pieces(~us, QUEEN)) & sq) {
            rays |= Magic::GetKingAttacks(sq, ksq, true);
        }
        rays |= sq;
    }

    return rays;
}

BitBoard Position::GetCheckers() const noexcept
{
    Colour   us   = Player();
    Square   ksq  = Pieces(us, KING).PopLSB();
    BitBoard occu = Pieces();

    if (ksq == SQ_TOTAL) {
        return 0ull;
    }

    BitBoard bb;

    bb |= (Magic::GetAttacks<ROOK>(ksq, occu, ~us)) & (Pieces(~us, ROOK) | Pieces(~us, QUEEN));
    bb |= (Magic::GetAttacks<BISHOP>(ksq, occu, ~us)) & (Pieces(~us, BISHOP) | Pieces(~us, QUEEN));
    bb |= (Magic::GetAttacks<PAWN>(ksq, occu, us)) & Pieces(~us, PAWN);
    bb |= (Magic::GetAttacks<KNIGHT>(ksq, occu, us)) & Pieces(~us, KNIGHT);
    bb |= (Magic::GetAttacks<KING>(ksq, occu, ~us)) & Pieces(~us, KING);

    return bb;
}

PieceType Position::GetType(Square sq) const noexcept
{
    // clang-format off
    if      (Pieces(BISHOP) & sq) { return    BISHOP; }
    else if (Pieces(KING)   & sq) { return      KING; }
    else if (Pieces(KNIGHT) & sq) { return    KNIGHT; }
    else if (Pieces(PAWN)   & sq) { return      PAWN; }
    else if (Pieces(QUEEN)  & sq) { return     QUEEN; }
    else if (Pieces(ROOK)   & sq) { return      ROOK; }
    else                          { return TYPE_NONE; }
    // clang-format on
}

bool Position::IsAttacked(Square sq, BitBoard occu, Colour us) const noexcept
{
    BitBoard bb, pawns;
    pawns = (Magic::GetAttacks<PAWN>(sq, occu, us) & Pieces(~us, PAWN));
    while (pawns) {
        Square   pawn = pawns.PopLSB();
        BitBoard attk = Magic::GetAttacks<PAWN>(pawn, occu, ~us);
        attk &= ~BitBoard(pawn);
        bb |= attk & sq;
    }

    bb |= (Magic::GetAttacks<ROOK>(sq, occu, ~us) & (Pieces(~us, ROOK) | Pieces(~us, QUEEN)));
    bb |= (Magic::GetAttacks<BISHOP>(sq, occu, ~us) & (Pieces(~us, BISHOP) | Pieces(~us, QUEEN)));
    bb |= (Magic::GetAttacks<KNIGHT>(sq, occu, us) & Pieces(~us, KNIGHT));
    bb |= (Magic::GetAttacks<KING>(sq, occu, ~us) & Pieces(~us, KING));

    return bb;
}

// ----- Update -----

void Position::MakeMove(Move move) noexcept
{
    auto store      = std::make_shared<StateStore>();
    store->previous = m_state;
    m_state         = store;

    m_state->ply        = m_state->previous->ply;
    m_state->totalMoves = m_state->previous->totalMoves;
    m_state->fen        = m_state->previous->fen;
    m_state->enPassant  = m_state->previous->enPassant;

    // Moving
    m_state->captured = MovePiece(move);
    if (move.IsCastle()) {
        ManageCastle(move);
    }

    // En passant management
    if (move.IsEnPassant()) {
        ManageEnPassant(move);
    } else {
        m_state->enPassant = SQ_BAD;
    }

    // Promotion
    if (move.IsPromo()) {
        ManagePromotion(move);
    }

    // Update gettable states - attacks twice for both players
    CalculateAttacks();
    m_player = ~m_player;
    CalculateAttacks();

    UpdateMoves(move);
    UpdateCastling();
    m_state->fen = UpdateFen();
}

void Position::UnmakeMove(Move move) noexcept
{
    m_player    = ~m_player;
    Colour us   = Player();
    Square from = move.To();
    Square to   = move.From();

    // Unpromote
    if (move.IsPromo()) {
        PieceType pt = move.Promotion();
        m_bbType[pt] &= ~BitBoard(from);
        m_bbType[PAWN] |= from;
    }

    MovePiece(Move::Make(from, to));

    PieceType capturedType = m_state->captured;
    if (capturedType != TYPE_NONE) {
        // A piece needs to be uncaptured
        if (from == m_state->previous->enPassant) {
            u8       off = (us == WHITE ? -8 : 8);
            BitBoard bb(Square(from + Square(off)));
            m_bbColour[~us] |= bb;
            m_bbType[capturedType] |= bb;
        } else {
            m_bbColour[~us] |= from;
            m_bbType[capturedType] |= from;
        }
    }

    if (move.IsCastle()) {
        // Move the rook
        if (to > from) {
            // Kingside
            m_bbType[ROOK] &= ~BitBoard(Square(from - 1));
            m_bbType[ROOK] |= BitBoard(Square(from + 1));
            m_bbColour[us] &= ~BitBoard(Square(from - 1));
            m_bbColour[us] |= BitBoard(Square(from + 1));
        } else {
            // Queenside
            m_bbType[ROOK] &= ~BitBoard(Square(from + 1));
            m_bbType[ROOK] |= BitBoard(Square(from - 2));
            m_bbColour[us] &= ~BitBoard(Square(from + 1));
            m_bbColour[us] |= BitBoard(Square(from - 2));
        }
    }

    m_state = m_state->previous;
}

// ----- Update ----- Hidden -----

void Position::ManageCastle(Move move) noexcept
{
    Colour us   = Player();
    Square from = move.From();
    Square to   = move.To();

    // Move the rook
    if (to > from) {
        // Kingside
        m_bbType[ROOK] &= ~BitBoard(Square(to + 1));
        m_bbType[ROOK] |= BitBoard(Square(to - 1));

        m_bbColour[us] &= ~BitBoard(Square(to + 1));
        m_bbColour[us] |= BitBoard(Square(to - 1));
    } else {
        // Queenside
        m_bbType[ROOK] &= ~BitBoard(Square(to - 2));
        m_bbType[ROOK] |= BitBoard(Square(to + 1));

        m_bbColour[us] &= ~BitBoard(Square(to - 2));
        m_bbColour[us] |= BitBoard(Square(to + 1));
    }
}

void Position::ManageEnPassant(Move move) noexcept
{
    i8 off             = move.To() - move.From();
    m_state->enPassant = Square(move.From() + (off / 2));
}

void Position::ManagePromotion(Move move) noexcept
{
    Square sq = move.To();

    m_bbType[PAWN] &= ~BitBoard(sq);

    PieceType pt = move.Promotion();
    m_bbType[pt] |= sq;
}

void Position::CalculateAttacks() noexcept
{
    Colour us               = Player();
    m_state->checkers[us]   = GetCheckers();
    m_state->attackRays[us] = GetAttackRays();
}

PieceType Position::MovePiece(Move move) noexcept
{
    Colour us   = Player();
    Square from = move.From();
    Square to   = move.To();

    // Type based BitBoard updates
    BitBoard fRem = ~BitBoard(from);
    BitBoard tRem = ~BitBoard(to);

    // Get Piece types
    PieceType movedType    = GetType(from);
    PieceType capturedType = GetType(to);
    if (to == m_state->enPassant && (Pieces(PAWN) & from)) {
        // Remove pawn
        capturedType = PAWN;
        Square sq    = Square(us == WHITE ? -8 : 8);
        tRem         = ~BitBoard(Square(to + sq));
    }

    // Remove from us
    m_bbType[movedType] &= fRem;
    m_bbColour[us] &= fRem;

    // Remove from them
    m_bbType[capturedType] &= tRem;
    m_bbColour[~us] &= tRem;

    // Add to us
    m_bbType[movedType] |= to;
    m_bbColour[us] |= to;

    return capturedType;
}

void Position::UpdateCastling() noexcept
{
    if (!(Pieces(KING) & SQ_E1)) {
        m_castling &= ~(Enums::Castling::White_King | Enums::Castling::White_Queen);
    }

    if (!(Pieces(KING) & SQ_E8)) {
        m_castling &= ~(Enums::Castling::Black_King | Enums::Castling::Black_Queen);
    }

    if (!(Pieces(ROOK) & SQ_H1)) {
        m_castling &= ~(Enums::Castling::White_King);
    }

    if (!(Pieces(ROOK) & SQ_A1)) {
        m_castling &= ~(Enums::Castling::White_Queen);
    }

    if (!(Pieces(ROOK) & SQ_H8)) {
        m_castling &= ~(Enums::Castling::Black_King);
    }

    if (!(Pieces(ROOK) & SQ_A8)) {
        m_castling &= ~(Enums::Castling::Black_Queen);
    }
}

std::string Position::UpdateFen() const noexcept
{
    // Pieces
    std::string line, fen;
    for (u8 rank = 8; rank > 0; rank--) {
        u8 offset = 0;
        for (u8 file = 0; file < 8; file++) {
            Square sq = Square((rank - 1) * 8 + file);
            if (!(Pieces(WHITE) & sq) && !(Pieces(BLACK) & sq)) {
                offset++;
                continue;
            }

            if (offset) {
                line += (offset + '0');
                offset = 0;
            }

            char c = '\0';
            if (Pieces(BISHOP) & sq) {
                c = 'b';
            } else if (Pieces(KING) & sq) {
                c = 'k';
            } else if (Pieces(KNIGHT) & sq) {
                c = 'n';
            } else if (Pieces(PAWN) & sq) {
                c = 'p';
            } else if (Pieces(QUEEN) & sq) {
                c = 'q';
            } else if (Pieces(ROOK) & sq) {
                c = 'r';
            }

            if (Pieces(WHITE) & sq) {
                c = toupper(c);
            }

            line += c;
        }

        fen += line;
        line.clear();

        if (offset) {
            fen += (offset + '0');
        }

        if (rank != 1) {
            fen += "/";
        }
    }

    // Player to move
    fen += (m_player == WHITE ? " w " : " b ");

    // Castling
    if (m_castling == 0) {
        fen += '-';
    } else {
        if (m_castling & Enums::Castling::White_King) {
            fen += 'K';
        }
        if (m_castling & Enums::Castling::White_Queen) {
            fen += 'Q';
        }
        if (m_castling & Enums::Castling::Black_King) {
            fen += 'k';
        }
        if (m_castling & Enums::Castling::Black_Queen) {
            fen += 'q';
        }
    }
    fen += ' ';

    // En passant
    if (m_state->enPassant != SQ_BAD) {
        fen += Convert::SquareToStr(m_state->enPassant);
        fen += " ";
    } else {
        fen += "- ";
    }

    // Half moves
    fen += std::to_string(m_state->ply);
    fen += " ";

    // Whole moves
    fen += std::to_string(m_state->totalMoves);

    return fen;
}

void Position::UpdateMoves(Move move) noexcept
{
    if (m_state->captured != TYPE_NONE || (Pieces(PAWN) & move.To())) {
        m_state->ply = 0;
    } else {
        m_state->ply++;
    }

    if (Player() == BLACK && m_state->fen != Fen::DEFAULT) {
        m_state->totalMoves++;
    }
}
