#include "Engine.h"

#include <charconv>
#include <cstdlib>
#include <sstream>
#include <thread>

#include "Search/Search.h"

#include "MoveGen/MoveGen.h"
#include "Utils/Convert.h"
#include "Utils/Fen.h"
#include "Utils/Utils.h"

// ----- Variables -----

static constexpr u64 INVALID_VALUE = 0xff'ff'ff'ff'ff'ff'ff'ff;
// clang-format off
static constexpr std::array<std::pair<std::string_view, u16>, 13> SEARCH_PARAMS = {
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
    std::pair{"perft",       0x10'00},
};
// clang-format on

static bool     m_isWhiteTurn = true;
static Position m_position(Fen::DEFAULT);

static std::atomic<bool> m_isSearching;   // Search algorithm can change this when done
static std::atomic<bool> m_stopSearching; // Main thread can call this to end engine
static std::pair<u16, std::array<u64, SEARCH_PARAMS.size()>> m_searchParams;

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

std::string Engine::Name() { return "SchtuffEngine"; }

std::string Engine::Author() { return "Schtuffs"; }

std::string Engine::GetState() { return m_position.Str(); }

bool Engine::IsSearching() { return !m_stopSearching; }

// ----- Update -----

void Engine::Flip() { m_isWhiteTurn = !m_isWhiteTurn; }

void Engine::Ready() { m_stopSearching = true; }

bool Engine::SetState(std::string_view data)
{
    // New game
    if (data == "ucinewgame") {
        m_position = Position(Fen::DEFAULT);
        DebugPrintln("Engine::SetState: Valid fen: {}", m_position.Fen());
        return true;
    }

    // Get position data
    std::stringstream ss(data.data());
    std::string       token;
    ss >> token;
    if (token != "position") {
        WarningPrintln("Engine::SetState: Invalid token: {}", token);
        m_position = Position(Fen::DEFAULT);
        return false;
    }

    // Get fen
    std::string fen;
    ss >> fen;
    if (fen == "startpos") {
        fen = Fen::DEFAULT;
    } else {
        ss >> token;
        while (!ss.fail() && token != "moves") {
            fen += " ";
            fen += token;
            ss >> token;
        }
    }

    // Fen check
    if (!Fen::IsValidFen(fen.data())) {
        m_position = Position(Fen::DEFAULT);
        return false;
    }

    // Check for moves
    m_position = Position(Fen::DEFAULT);
    ss >> token;
    if (ss.fail()) {
        DebugPrintln("Engine::SetState: Valid fen: {}", m_position.Fen());
        return true;
    }
    if (token != "moves") {
        m_position = Position(Fen::DEFAULT);
        return false;
    }

    // Play moves
    ss >> token;
    while (!ss.fail()) {
        Engine::MakeMove(token);
        ss >> token;
    }
    DebugPrintln("Engine::SetState: Valid fen: {}", m_position.Fen());
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

    std::thread(Search::Begin, std::ref(m_position)).detach();
}

void Engine::MakeMove(std::string_view move)
{
    MoveList list;
    list.Add(Convert::StrToMove(move, m_position.Player()));
    list.Legalize(m_position);

    for (Move move : list) {
        if (m_position.Pieces(PAWN) & move.From()) {
            if (std::abs(move.From() - move.To()) == 16) {
                move = Move::MakeEnPassant(move.From(), move.To());
            }
        }
        m_position.MakeMove(move);
    }
}

void Engine::Stop() { m_stopSearching = true; }
