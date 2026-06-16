#pragma once

#include <string>
#include <string_view>

/**
 * @brief Holds the state of the game board.
 * @class Board
 * @author Kyle Wagler
 * @date 2026-06-14
 */
class Board {
public:
    // ----- Creation / Destruction -----

    /**
     * @brief Create board with given fen.
     * @author Kyle Wagler
     * @date 2026-06-14
     */
    Board(const std::string& fen);

    /**
     * @brief Frees memory.
     * @author Kyle Wagler
     * @date 2026-06-14
     */
    ~Board();

    // ----- Read -----
    
    /**
     * @brief Get the current fen gamestate.
     * @author Kyle Wagler
     * @date 2026-06-14
     */
    std::string Fen();

    // ----- Update -----
    
    /**
     * @brief Attempt to make a move on the board.
     * @param move A move in long algebraic notation.
     * @return `true` on move successfully made.
     * @author Kyle Wagler
     * @date 2026-06-14
     */
    bool MakeMove(std::string_view move);
    
private:
    std::string m_fen;
};

