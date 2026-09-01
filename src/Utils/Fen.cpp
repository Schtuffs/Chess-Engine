#include "Utils/Fen.h"

#include <cmath>
#include <sstream>
#include <string>

#include "Utils/Utils.h"

// ----- Hidden -----

/**
 * Validates piece data in given fen string.
 */
static bool ValidatePieces(std::string_view fen)
{
    // Check files and ranks
    i8     files = 0, ranks = 0;
    size_t sq = 0;
    while (sq < fen.size()) {
        char c = fen[sq];

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
                    return false;
                }
            } else if (c == 'P') {
                if (ranks == 0) {
                    return false;
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

        sq++;
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
        WarningPrintln("Fen::ValidateCastling: Too much castling: {}", fen);
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
        WarningPrintln("Fen::ValidateEnPassant: Invalid en passant: {}", fen);
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
        ErrorPrintln("Fen::NextCheck: Failed to get next token.");
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
