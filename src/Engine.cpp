#include "Engine.h"

#include <charconv>
#include <cstdlib>
#include <sstream>
#include <thread>

#include "Search/Search.h"

#include "Convert.h"
#include "Fen.h"
#include "Utils.h"

// ----- Creation / Destruction -----

Engine::Engine()
    : m_name("EngineOfRunning"), m_author("Schtuffs"), m_isWhiteTurn(true), m_halfMoves(0)
{
}

Engine::~Engine() {}

// ----- Read -----

std::string Engine::Name() const noexcept { return m_name; }

std::string Engine::Author() const noexcept { return m_author; }

std::string Engine::GetState() const noexcept
{
    return m_board.ToString();
}

// ----- Read ----- Hidden -----

u64 Engine::GetSearchParamKey(std::string_view str) const noexcept
{
    for (u64 i = 0; i < SEARCH_PARAMS.size(); i++) {
        if (SEARCH_PARAMS[i].first == str) {
            return SEARCH_PARAMS[i].second;
        }
    }

    return INVALID_VALUE;
}

u64 Engine::GetSearchParamValue(std::string_view str) const noexcept
{
    try {
        return std::stoull(str.data());
    } catch (...) {
        return INVALID_VALUE;
    }
}

bool Engine::ValidValue(u64 val) const noexcept { return (val != INVALID_VALUE); }

u64 Engine::KeyIndex(u64 key) const noexcept { return std::log2(key); }

// ----- Update -----

void Engine::Flip() { m_isWhiteTurn = !m_isWhiteTurn; }

void Engine::Ready() { m_stopSearching = true; }

bool Engine::SetState(std::string_view data)
{
    // New game
    if (data == "ucinewgame") {
        m_board = Board();
        DebugPrintln("Engine::SetState: Valid fen: {}", m_board.Fen());
        return true;
    }

    // Get position data
    std::stringstream ss(data.data());
    std::string       token;
    ss >> token;
    if (token != "position") {
        m_board = Board();
        return false;
    }

    // Get fen
    ss >> token;
    if (token == "startpos") {
        std::println("Start");
        token = DEFAULT_FEN;
    }

    // Fen check
    if (!Fen::IsValidFen(token.data())) {
        m_board = Board();
        return false;
    }

    // Check for moves
    m_board = Board(token);
    ss >> token;
    if (ss.fail()) {
        DebugPrintln("Engine::SetState: Valid fen: {}", m_board.Fen());
        return true;
    }
    if (token != "moves") {
        m_board = Board();
        return false;
    }

    // Play moves
    ss >> token;
    while (!ss.fail()) {
        if (m_board.MakeMove(token)) {
            m_halfMoves++;
        }
        ss >> token;
    }
    DebugPrintln("Engine::SetState: Valid fen: {}", m_board.Fen());
    return true;
}

void Engine::Search(std::string_view data)
{
    // Preparation
    std::stringstream ss(data.data());
    std::string       keyStr, valueStr;
    ss >> keyStr;
    ss >> keyStr;
    ss >> valueStr;

    while (!ss.fail()) {
        u64 key    = GetSearchParamKey(keyStr);
        u64 valStr = GetSearchParamValue(valueStr);

        if (ValidValue(key) && ValidValue(valStr)) {
            m_searchParams.first |= (u16)key;
            m_searchParams.second[KeyIndex(key)] = valStr;
        } else {
            ErrorPrintln("Invalid arg: {} = {}", keyStr, valueStr);
            return;
        }

        ss >> keyStr;
        ss >> valueStr;
    }

#ifndef NDEBUG
    for (u64 i = 0; i < SEARCH_PARAMS.size(); i++) {
        DebugPrintln("{:12} {}", SEARCH_PARAMS[i].first, m_searchParams.second[i]);
    }
#endif

    std::thread(Search::Begin, std::ref(m_board)).detach();
}

void Engine::Stop() { m_stopSearching = true; }
