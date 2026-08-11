#include "Convert.h"

#include <print>

std::string_view Convert::CastleToMove(std::string_view move, Enums::Colour player)
{
    // Short castle
    if (move == "O-O") {
        if (player == Enums::Colour::White) {
            return "e1g1";
        } else if (player == Enums::Colour::Black) {
            return "e8g8";
        }
    }
    // Long castle
    else if (move == "O-O-O") {
        if (player == Enums::Colour::White) {
            return "e1c1";
        } else if (player == Enums::Colour::Black) {
            return "e8c8";
        }
    }

    return move;
}

std::string Convert::IndexToMove(Index index)
{
    return ((char)((index % (Index)8) + 'a') + std::to_string((index / (Index)8) + 1));
}

Index Convert::MoveToIndex(std::string_view move)
{
    return (Index)((move[1] - '1') * 8) + (move[0] - 'a');
}

std::string Convert::FenToString(std::string_view fen)
{
    const std::string VERT_SPACE = "\n +---+---+---+---+---+---+---+---+\n";
    const std::string HORZ_SPACE = " | ";
    std::string       ret = VERT_SPACE, line = HORZ_SPACE;

    u64 index = fen.find(' ');
    for (u64 i = 0; i < index; i++) {
        char c = fen[i];
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
            ret += line + VERT_SPACE;
            line = HORZ_SPACE;
        }
    }

    ret += line + VERT_SPACE;
    return ret;
}

std::string Convert::BitboardToString(Bitboard val, char on, char off)
{
    const std::string VERT_SPACE = "\n +---+---+---+---+---+---+---+---+\n";
    const std::string HORZ_SPACE = " | ";
    std::string       ret = "\n", line = HORZ_SPACE;

    for (int i = 0; i < 64; i++) {
        if ((val & ((Bitboard)1 << i)) > 0) {
            line += on + HORZ_SPACE;
        } else {
            line += off + HORZ_SPACE;
        }

        if (i != 0 && i % 8 == 7) {
            ret  = line + VERT_SPACE + ret;
            line = HORZ_SPACE;
        }
    }

    ret = VERT_SPACE + ret;
    return ret;
}

std::string Convert::PiecesToString(const Piece* pieces)
{
    const std::string VERT_SPACE = "\n +---+---+---+---+---+---+---+---+\n";
    const std::string HORZ_SPACE = " | ";
    std::string       ret = "\n", line = HORZ_SPACE;

    for (int i = 0; i < 64; i++) {
        const Piece& piece = pieces[i];
        if (!piece.IsValid()) {
            line += ' ' + HORZ_SPACE;
        } else {
            line += piece.AsChar() + HORZ_SPACE;
        }

        if (i != 0 && i % 8 == 7) {
            ret  = line + VERT_SPACE + ret;
            line = HORZ_SPACE;
        }
    }

    ret = VERT_SPACE + ret;
    return ret;
}
