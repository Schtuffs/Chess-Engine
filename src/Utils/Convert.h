#pragma once

#include <cmath>
#include <string>
#include <string_view>

#include "Types/BitBoard.h"
#include "Types/Types.h"
#include "Utils/Constants.h"

/**
 * @brief Conversion utility
 * @namespace Convert
 * @date 2026-07-03
 */
namespace Convert {

// Converts a letter to a colour
inline constexpr Colour CharToColour(char c) { return (isupper(c) ? WHITE : BLACK); }

// Converts a letter to a piece type.
inline constexpr PieceType CharToType(char c)
{
    c = tolower(c);
    switch (c) {
    case 'b':
        return BISHOP;
    case 'k':
        return KING;
    case 'n':
        return KNIGHT;
    case 'p':
        return PAWN;
    case 'q':
        return QUEEN;
    case 'r':
        return ROOK;
    default:
        return TYPE_NONE;
    }
}

inline constexpr char TypeToChar(PieceType type)
{
    switch (type) {
    case BISHOP:
        return 'b';
    case KING:
        return 'k';
    case KNIGHT:
        return 'n';
    case PAWN:
        return 'p';
    case QUEEN:
        return 'q';
    case ROOK:
        return 'r';
    default:
        return '\0';
    }
}

inline constexpr Square StrToSquare(std::string_view str)
{
    return (Square)(((str[1] - '1') * 8) + (str[0] - 'a'));
}

inline std::string SquareToStr(Square sq)
{
    std::string str;
    str += (char((sq % 8) + (i32)'a'));
    str += (char((sq / 8) + (i32)'1'));
    return str;
}

inline constexpr Move StrToMove(std::string_view str, Colour player)
{
    if (str == "O-O") {
        if (player == WHITE) {
            return Move::MakeCastle(SQ_E1, SQ_G1);
        }
        else {
            return Move::MakeCastle(SQ_E8, SQ_G8);
        }
    }
    else if (str == "O-O-O") {
        if (player == WHITE) {
            return Move::MakeCastle(SQ_E1, SQ_C1);
        }
        else {
            return Move::MakeCastle(SQ_E8, SQ_C8);
        }
    }
    Square from  = Convert::StrToSquare(str);
    Square to    = Convert::StrToSquare(str.substr(2));
    char   promo = (str.length() == 5 ? str[4] : '\0');

    if (promo) {
        return Move::MakePromo(from, to, Convert::CharToType(promo));
    } else {
        return Move::Make(from, to);
    }
}

inline constexpr std::string MoveToStr(Move move)
{
    constexpr const char LETTERS[] = "abcdefgh";
    constexpr const char NUMBERS[] = "12345678";

    if (move.IsCastle()) {
        // TODO
        return "O-O";
    }

    std::string str;
    str += LETTERS[move.From() % 8];
    str += NUMBERS[move.From() / 8];
    str += LETTERS[move.To() % 8];
    str += NUMBERS[move.To() / 8];
    if (move.IsPromo()) {
        str += Convert::TypeToChar(move.Promotion());
    }
    return str;
}
} // namespace Convert
