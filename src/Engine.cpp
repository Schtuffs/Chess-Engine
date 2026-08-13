#include "Engine.h"

#include <charconv>
#include <cstdlib>
#include <sstream>
#include <thread>

#include "Search/Search.h"

#include "Convert.h"
#include "Fen.h"
#include "Utils.h"

// ----- Variables -----

static constexpr u64 INVALID_VALUE = 0xff'ff'ff'ff'ff'ff'ff'ff;
// clang-format off
static constexpr std::array<std::pair<std::string_view, u16>, 12> SEARCH_PARAMS = {
    std::pair{"wtime",       0x00'01},
    std::pair{"btime",       0x00'02},
    std::pair{"winc",        0x00'04},
    std::pair{"binc",        0x00'08},
    std::pair{"movestogo",   0x00'10},
    std::pair{"movetime",    0x00'20},

    std::pair{"depth",       0x00'40},
    std::pair{"nodes",       0x00'80},
    std::pair{"mate",        0x01'00},
    std::pair{"infinite",    0x02'00},

    std::pair{"ponder",      0x04'00},
    std::pair{"searchmoves", 0x08'00},
};
// clang-format on

static bool  m_isWhiteTurn = true;
static Board m_board;

static std::atomic<bool> m_isSearching;   // Search algorithm can change this when done
static std::atomic<bool> m_stopSearching; // Main thread can call this to end engine
static std::pair<u16, std::array<u64, 12>> m_searchParams;

// ----- Helpers -----

u64 GetSearchParamKey(std::string_view str)
{
    for (u64 i = 0; i < SEARCH_PARAMS.size(); i++) {
        if (SEARCH_PARAMS[i].first == str) {
            return SEARCH_PARAMS[i].second;
        }
    }

    return INVALID_VALUE;
}

u64 GetSearchParamValue(std::string_view str)
{
    try {
        return std::stoull(str.data());
    } catch (...) {
        return INVALID_VALUE;
    }
}

bool ValidValue(u64 val) { return (val != INVALID_VALUE); }

u64 KeyIndex(u64 key) { return std::log2(key); }

// ----- Read -----

std::string Engine::Name() { return "EngineOfRunning"; }

std::string Engine::Author() { return "Schtuffs"; }

std::string Engine::GetState() { return m_board.ToString(); }

bool Engine::IsSearching() { return !m_stopSearching; }

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
        m_board.MakeMove(token);
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

bool Engine::MakeMove(std::string_view move)
{
    return m_board.MakeMove(move);
}

void Engine::Stop() { m_stopSearching = true; }
