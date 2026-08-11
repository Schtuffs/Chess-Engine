#include "Fen.h"

#include <cmath>
#include <sstream>
#include <string>

#include "Fen.h"

#include "Utils.h"

// ----- Hidden -----

/**
 * Validates piece data in given fen string.
 */
static bool ValidatePieces(std::string_view fen)
{
    // Check files and ranks
    i8     files = 0, ranks = 0;
    size_t index = 0;
    while (index < fen.size()) {
        char c = fen[index];

        // Marks end of rank data, validate
        if (c == '/') {
            if (files != 8) {
                WarningPrintln("Fen::ValidatePieces: Invalid number of files in fen: {}", files);
                return false;
            }
            files = 0;
            ranks++;
        }

        // Marks end of all ranks, validate
        else if (c == ' ') {
            if (files != 8 && ranks != 8) {
                WarningPrintln(
                    "Fen::ValidatePieces: Invalid number of files or ranks in fen: {}, {}", files,
                    ranks);
                return false;
            }
            break;
        }

        else if (c == 'b' || c == 'B' || c == 'k' || c == 'K' || c == 'n' || c == 'N' || c == 'p' ||
                 c == 'P' || c == 'q' || c == 'Q' || c == 'r' || c == 'R') {
            // No last rank pawns
            if (c == 'p') {
                if (ranks == 7) {
                    std::println("Bad");
                }
            } else if (c == 'P') {
                if (ranks == 0) {
                    std::println("Bad");
                }
            }
            files++;
        }

        else if (isdigit(c)) {
            files += c - '0';
        }

        else {
            WarningPrintln("Fen::ValidatePieces: Invalid char in fen: {}", c);
            return false;
        }

        index++;
    }

    if (files != 8 && ranks != 8) {
        return false;
    }

    return true;
}

/**
 * Validates the current player to move.
 */
static bool ValidateMove(std::string_view fen)
{
    char c = fen[0];
    if (fen.length() != 1 && c != 'w' && c != 'b') {
        WarningPrintln("Fen::ValidateMove: Invalid player-to-move argument in fen: {}", c);
        return false;
    }

    return true;
}

/**
 * Validates castling data.
 */
static bool ValidateCastling(std::string_view fen)
{
    if (fen.length() > 4) {
        return false;
    }

    if (fen[0] == '-') {
        return true;
    }

    constexpr std::string_view str    = "KQkq";
    u8                         strPos = 0;
    for (char c : fen) {
        while (strPos < str.length() && c != str[strPos]) {
            strPos++;
        }

        if (strPos == str.length()) {
            ErrorPrintln("Fen::ValidateCastling: Invalid castling data in fen: {}", fen);
            return false;
        }
        strPos++;
    }

    return true;
}

/**
 * Validates en passant square.
 */
static bool ValidateEnPassant(std::string_view fen)
{
    if (fen[0] == '-') {
        return true;
    }

    if (fen.length() != 2) {
        return false;
    }

    char c = fen[0];
    if ('a' > c || c > 'h') {
        ErrorPrintln("Fen::ValidateEnPassant: Invalid file in fen: {}", c);
        return false;
    }

    c = fen[1];
    if (c != '3' && c != '6') {
        ErrorPrintln("Fen::ValidateEnPassant: Invalid rank in fen: {}", c);
        return false;
    }

    return true;
}

/**
 * Helper function to ensure next check passes
 */
static bool NextCheck(std::istringstream& ss, bool (*validate)(std::string_view))
{
    std::string str;
    ss >> str;
    if (ss.fail() || !validate(str)) {
        return false;
    }
    return true;
}

bool Fen::IsValidFen(const char* data)
{
    std::istringstream ss(data);
    if (!NextCheck(ss, ValidatePieces)) {
        ErrorPrintln("Fen::IsValidFen: Invalid pieces in fen");
        return false;
    }

    if (!NextCheck(ss, ValidateMove)) {
        ErrorPrintln("Fen::IsValidFen: Invalid player to move in fen");
        return false;
    }

    if (!NextCheck(ss, ValidateCastling)) {
        ErrorPrintln("Fen::IsValidFen: Invalid castling rights in fen");
        return false;
    }

    if (!NextCheck(ss, ValidateEnPassant)) {
        ErrorPrintln("Fen::IsValidFen: Invalid en passant in fen");
        return false;
    }

    return true;
}

static std::string GenPieces(std::span<const Piece, 64> pieces)
{
    std::string fen;
    u8          extra = 0;
    for (u64 rank = 8 - 1; rank < 8; rank--) {
        if (extra > 0) {
            fen += std::to_string(extra);
            extra = 0;
        }

        if (rank != 8 - 1) {
            fen += "/";
        }

        for (u64 file = 0; file < 8; file++) {
            u64 i = rank * 8 + file;

            const Piece& piece = pieces[i];
            char         p     = piece.AsChar();
            if (!p) {
                extra++;
                continue;
            }

            if (extra > 0) {
                fen += std::to_string(extra);
                extra = 0;
            }

            fen += p;
        }
    }

    if (extra > 0) {
        fen += std::to_string(extra);
        extra = 0;
    }

    return fen;
}

static std::string GenPlayer(char player)
{
    std::string fen = " ";

    fen += player;

    return fen;
}

static std::string GenCastling(std::string_view castling)
{
    std::string fen = " ";

    fen += castling;

    return fen;
}

static std::string GenEnPassant(std::string_view enPassant)
{
    std::string fen = " ";

    fen += enPassant;

    return fen;
}

static std::string GenHalfMoves(u32 halfMoves)
{
    std::string fen = " ";

    fen += std::to_string(halfMoves);

    return fen;
}

static std::string GenFullMoves(u32 fullMoves)
{
    std::string fen = " ";

    fen += std::to_string(fullMoves);

    return fen;
}

std::string Fen::GenerateFen(std::span<const Piece, 64> pieces, char player,
                             std::string_view castling, std::string_view enPassant, u32 halfMoves,
                             u32 fullMoves)
{
    std::string fen = "";

    fen += GenPieces(pieces);
    fen += GenPlayer(player);
    fen += GenCastling(castling);
    fen += GenEnPassant(enPassant);
    fen += GenHalfMoves(halfMoves);
    fen += GenFullMoves(fullMoves);

    if (!IsValidFen(fen.c_str())) {
        WarningPrintln("Fen::GenerateFen: Could not generate valid fen.");
        return "";
    }

    return fen;
}
