#pragma once

#include <array>
#include <span>
#include <string>
#include <string_view>

#include "Piece.h"

/**
 * @brief Holds the state of the game board.
 * @class Board
 * @date 2026-06-14
 */
class Board {
public:
    // ----- Creation / Destruction -----

    // Create board with given fen.
    Board(std::string_view fen = DEFAULT_FEN);

    // Free memory.
    ~Board();

    // ----- Read -----

    // Get board castling rights
    u8 Castling() const noexcept;

    // Get current gamestate fen.
    std::string_view Fen() const noexcept;

    // Get the board's piece list.
    std::span<const Piece, 64> Pieces() const noexcept;

    // ----- Update -----

    // Try to play given move (long algebraic notation).
    bool MakeMove(std::string_view move);

private:
    std::string           m_fen;
    std::array<Piece, 64> m_pieces;
    u8                    m_castling;
    Index                 m_enPassant;
    Enums::Colour         m_playerColour;

    // ----- Update -----

    bool ValidateMove(std::string_view move);
    bool ValidatePromotion(std::string_view move);
    void MovePiece(std::string_view move);
    void MoveEnPassant(std::string_view move);
    void MoveCastling(std::string_view move);

    // ----- Fen -----

    std::string RecalculateFen();
    std::string RecalculateFen(bool isCaptureOrPawn);
    char        RecalculatePlayer();
    std::string RecalculateCastling();
    std::string RecalculateEnPassant();
    u32         RecalculateHalfMoves(bool isCaptureOrPawn);
    u32         RecalculateFullMoves();
};
