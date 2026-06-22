#include "Engine.h"

#include <print>

#include "Constants.h"
#include "Utils.h"



// ----- Creation / Destruction -----

Engine::Engine()
  : m_name("EngineOfRunning"), m_author("Schtuffs"),
    m_fen(DEFAULT_FEN), m_isWhite(true)
{}

Engine::~Engine()
{}



// ----- Read -----

std::string Engine::Name()
{
    return m_name;
}

std::string Engine::Author()
{
    return m_author;
}

// Hidden

/**
 * Validates piece data in given fen string
 */
bool ValidatePieces(std::string_view fen)
{
    // Check files and ranks
    i8 files = 0, ranks = 0;
    size_t index = 0;
    while (index < fen.size()) {
        char c = fen[index];
    
        // Marks end of rank data, validate
        if (c == '/') {
            if (files != 8) {
                WarningPrintln("Invalid number of files in fen: {}", files);
                return false;
            }
            files = 0;
            ranks++;
        }
    
        // Marks end of all ranks, validate
        else if (c == ' ') {
            if (files != 8 && ranks != 8) {
                WarningPrintln("Invalid number of files or ranks in fen: {}, {}", files, ranks);
                return false;
            }
            break;
        }
    
        else if (
            c == 'b' || c == 'B' ||
            c == 'k' || c == 'K' ||
            c == 'n' || c == 'N' ||
            c == 'p' || c == 'P' ||
            c == 'q' || c == 'Q' ||
            c == 'r' || c == 'R'
        ) {
            files++;
        }
    
        else if (isdigit(c)) {
            files += c - '0';
        }

        else {
            WarningPrintln("Invalid char in fen: {}", c);
            return false;
        }
    
        index++;
    }

    return true;
}

/**
 * Validates the current player to move.
 */
bool ValidateMove(std::string_view fen)
{
    size_t index = fen.find(' ');
    if (index != 1) {
        WarningPrintln("Too many player-to-move arguments in fen: {}", index);
        return false;
    }
    
    char c = fen[0];
    if (c != 'w' && c != 'b') {
        WarningPrintln("Invalid player-to-move argument in fen: {}", c);
        return false;
    }

    return true;
}

/**
 * Validates castling data.
 */
bool ValidateCastling(std::string_view fen)
{
    size_t index = fen.find(' ');
    if (index == std::string_view::npos) {
        return false;
    }
    
    if (index == 1 && fen[0] == '-') {
        return true;
    }
    fen = fen.substr(0, index);

    constexpr std::string_view str = "KQkq";
    u8 strPos = 0;
    for (char c : fen) {
        while (strPos < str.length() && c != str[strPos]) {
            strPos++;
        }
        
        if (strPos == str.length()) {
            WarningPrintln("Invalid castling data in fen: {}", fen);
            return false;
        }
        strPos++;
    }
    
    return true;
}

/**
 * Validates en passant square.
 */
bool ValidateEnPassant(std::string_view fen)
{
    size_t index = fen.find(' ');
    if (index == std::string_view::npos) {
        return false;
    }
    
    if (index == 1 && fen[0] == '-') {
        return true;
    }

    char c = fen[0];
    if ('a' > c || c > 'h') {
        WarningPrintln("Invalid file in fen: {}", c);
        return false;
    }
    
    c = fen[1];
    if (c != '3' && c != '6') {
        WarningPrintln("Invalid rank in fen: {}", c);
        return false;
    }

    return true;
}

/**
 * Validates a given fen.
 */
bool ValidFen(std::string_view fen)
{
    if (!ValidatePieces(fen)) {
        return false;
    }

    size_t index = fen.find(' ');
    if (index == std::string_view::npos || index + 1 >= fen.length()) {
        return false;
    }
    fen = fen.substr(index + 1);

    if (!ValidateMove(fen)) {
        return false;
    }
    fen = fen.substr(2);

    index = fen.find(' ');
    if (
        index == std::string_view::npos ||
        index + 1 >= fen.length() ||
        !ValidateCastling(fen)
    ) {
        return false;
    }
    fen = fen.substr(fen.find(' ') + 1);

    if (!ValidateEnPassant(fen)) {
        return false;
    }
    fen = fen.substr(2);
    
    DebugPrintln("Fen: {}", fen);

    return true;
}



// ----- Update -----

void Engine::Flip()
{
    m_isWhite = !m_isWhite;
}

void Engine::Ready()
{
    // Do something
}

bool Engine::SetState(std::string_view data)
{
    size_t index = data.find(' ');
    if (index == std::string_view::npos) {
        ErrorPrintln("Failed to receive postion data: {}", data);
        return false;
    }
    data = data.substr(index + 1);
    
    index = data.find("moves") - 1;
    if (index == std::string_view::npos - 1) {
        DebugPrintln("No moves data in fen: {}", data);
        index = data.length();
    }
    
    std::string_view fen = data.substr(0, index);
    if (fen == "startpos") {
        fen = DEFAULT_FEN;
    }
    DebugPrintln("Fen: {}", fen);

    if (!ValidFen(fen)) {
        ErrorPrintln("Failed to parse fen: {}", fen);
        return false;
    }

    return true;
}

void Engine::Stop()
{
    // Do something
}

