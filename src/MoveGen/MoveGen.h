#pragma once

#include <array>
#include <span>

#include "Board.h"
#include "Constants.h"
#include "Piece.h"

class MoveGen {
public:
    // Useful for determining if generated moves are invalid.
    static constexpr BitBoard INVALID = 0x00'00'00'00'00'00'00'00;

    // ----- Creation / Destruction -----

    MoveGen();
    ~MoveGen() = default;

    // ----- Read -----

    BitBoard GetMoves(Index index) const noexcept;
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
    BitBoard                 m_friendly, m_enemies, m_occupied;
    BitBoard                 m_bishops, m_kings, m_knights, m_pawns, m_queens, m_rooks, m_enPassant;
    BitBoard                 m_attacks, m_kingAttacks;
    std::array<BitBoard, 64> m_pins, m_pseudoLegal;

    // Output items
    bool                     m_hasGenerated;
    std::array<BitBoard, 64> m_legal;
    BitBoard                 m_totalLegal;

    void Reset();
    void SetupPieceBoards();

    BitBoard GenMoves(const Piece& piece) const noexcept;
    BitBoard GenBishop(const Piece& piece) const noexcept;
    BitBoard GenCastling(const Piece& piece) const noexcept;
    BitBoard GenKing(const Piece& piece) const noexcept;
    BitBoard GenKnight(const Piece& piece) const noexcept;
    BitBoard GenPawn(const Piece& piece) const noexcept;
    BitBoard GenQueen(const Piece& piece) const noexcept;
    BitBoard GenRook(const Piece& piece) const noexcept;

    void GenAttacks();
    void AddAttacks(const Piece& piece, Index king, BitBoard moves);
    void AddCheck();

    void GenPseudoLegal();

    void GenLegal();
};
