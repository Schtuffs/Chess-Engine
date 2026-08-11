#pragma once

#include <array>
#include <span>

#include "Board.h"
#include "Constants.h"
#include "Piece.h"

class MoveGen {
public:
    // Useful for determining if generated moves are invalid.
    static constexpr Bitboard INVALID = 0x00'00'00'00'00'00'00'00;

    // ----- Creation / Destruction -----

    MoveGen();
    ~MoveGen() = default;

    // ----- Read -----

    Bitboard GetMoves(Index index) const noexcept;
    bool     IsCheckmate() const noexcept;
    bool     IsStalemate() const noexcept;

    // ----- Update -----

    void Generate(const Board& board, Enums::Colour colour);

private:
    // Passed parameters
    const Board*  m_board;
    Enums::Colour m_genColour;

    // Calculated items
    bool                     m_generatingAttacks;
    bool                     m_inCheck, m_inDoubleCheck;
    Bitboard                 m_friendly, m_enemies, m_occupied;
    Bitboard                 m_bishops, m_kings, m_knights, m_pawns, m_queens, m_rooks, m_enPassant;
    Bitboard                 m_attacks, m_kingAttacks;
    std::array<Bitboard, 64> m_pins, m_pseudoLegal;

    // Output items
    bool                     m_hasGenerated;
    std::array<Bitboard, 64> m_legal;
    Bitboard                 m_totalLegal;

    void Reset();
    void SetupPieceBoards();

    Bitboard GenMoves(const Piece& piece) const noexcept;
    Bitboard GenBishop(const Piece& piece) const noexcept;
    Bitboard GenCastling(const Piece& piece) const noexcept;
    Bitboard GenKing(const Piece& piece) const noexcept;
    Bitboard GenKnight(const Piece& piece) const noexcept;
    Bitboard GenPawn(const Piece& piece) const noexcept;
    Bitboard GenQueen(const Piece& piece) const noexcept;
    Bitboard GenRook(const Piece& piece) const noexcept;

    void GenAttacks();
    void AddAttacks(const Piece& piece, Index king, Bitboard moves);
    void AddCheck();

    void GenPseudoLegal();

    void GenLegal();
};
