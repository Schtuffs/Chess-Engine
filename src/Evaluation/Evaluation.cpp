#include "Evaluation/Evaluation.h"

constexpr double PAWN_VALUE   = 100.;
constexpr double KNIGHT_VALUE = 300.;
constexpr double BISHOP_VALUE = 350.;
constexpr double ROOK_VALUE   = 500.;
constexpr double QUEEN_VALUE  = 900.;

double Evaluation::Eval(const Position& pos)
{
    Colour us  = pos.Player();
    double val = 0;

    // clang-format off
    val += pos.Pieces(us,    PAWN).Count() * PAWN_VALUE;
    val += pos.Pieces(us,  KNIGHT).Count() * KNIGHT_VALUE;
    val += pos.Pieces(us,  BISHOP).Count() * BISHOP_VALUE;
    val += pos.Pieces(us,    ROOK).Count() * ROOK_VALUE;
    val += pos.Pieces(us,   QUEEN).Count() * QUEEN_VALUE;

    val -= pos.Pieces(~us,   PAWN).Count() * PAWN_VALUE;
    val -= pos.Pieces(~us, KNIGHT).Count() * KNIGHT_VALUE;
    val -= pos.Pieces(~us, BISHOP).Count() * BISHOP_VALUE;
    val -= pos.Pieces(~us,   ROOK).Count() * ROOK_VALUE;
    val -= pos.Pieces(~us,  QUEEN).Count() * QUEEN_VALUE;
    // clang-format on

    if (us == BLACK) {
        return -val;
    }

    return val;
}
