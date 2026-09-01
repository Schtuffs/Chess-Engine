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

static std::atomic<bool> m_isSearching;   // Search algorithm can change this when done.
static std::atomic<bool> m_stopSearching; // Main thread can call this to end engine.
static std::pair<u16, std::array<u64, SEARCH_PARAMS.size()>> m_searchParams;

void Perft(const Position& pos, u64 depth);

// ----- Helpers -----

static std::string Name() { return "SchtuffsEngine"; }

static std::string Author() { return "Schtuffs"; }

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

static void MakeMove(std::string_view move)
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

// ----- UCI -----

void Engine::Quit() { m_stopSearching = true; }

void Engine::Uci() { SyncPrintln("id name {}\nid author {}\nuciok", Name(), Author()); }

void Engine::IsReady() { m_stopSearching = true; }

void Engine::SetOption(std::stringstream ss) { (void)ss; }

void Engine::SetPosition(std::stringstream ss)
{
    // New game
    std::string token;
    ss >> token;
    if (token == "ucinewgame") {
        m_position = Position(Fen::DEFAULT);
        DebugPrintln("Engine::SetState: Valid fen: {}", m_position.Fen());
        return;
    }

    // Get position data
    if (token != "position") {
        WarningPrintln("Engine::SetState: Invalid token: {}", token);
        m_position = Position(Fen::DEFAULT);
        return;
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
        return;
    }

    // Check for moves
    m_position = Position(Fen::DEFAULT);
    ss >> token;
    if (ss.fail()) {
        DebugPrintln("Engine::SetState: Valid fen: {}", m_position.Fen());
        return;
    }
    if (token != "moves") {
        m_position = Position(Fen::DEFAULT);
        return;
    }

    // Play moves
    ss >> token;
    while (!ss.fail()) {
        MakeMove(token);
        ss >> token;
    }
    DebugPrintln("Engine::SetState: Valid fen: {}", m_position.Fen());
}

void Engine::Go(std::stringstream ss)
{
    // Preparation
    std::string keyStr, valueStr;
    ss >> keyStr;
    ss >> keyStr;
    ss >> valueStr;

    while (!ss.fail()) {
        u64 key = GetSearchParamKey(keyStr);
        u64 val = GetSearchParamValue(valueStr);

        // Perft check
        if (keyStr == "perft") {
            if (ValidValue(val)) {
                Perft(m_position, val);
            }
            return;
        }

        if (ValidValue(key) && ValidValue(val)) {
            m_searchParams.first |= (u16)key;
            m_searchParams.second[KeyIndex(key)] = val;
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

void Engine::Stop() { m_stopSearching = true; }

void Engine::PonderHit() {}

// ----- Custom UCI -----

void Engine::Flip() { m_isWhiteTurn = !m_isWhiteTurn; }

void Engine::D() { SyncPrintln("{}", m_position.Str()); }

void Engine::Help() {}

// ----- Other -----

bool Engine::IsSearching() { return !m_stopSearching; }

// ----- Testing -----

u64 Perft(Position& pos, u64 depth)
{
    u64 total = 0;

    if (depth == 0) {
        return 1;
    }

    MoveList list;
    MoveGen::Generate(pos, list);
    list.Legalize(pos);
    if (depth == 1) {
        return list.size;
    }

    for (Move move : list) {
        Position act(pos);
        act.MakeMove(move);

        u64 val = Perft(act, depth - 1);
        total += val;

        act.UnmakeMove(move);
    }

    return total;
}

void Perft(const Position& pos, u64 depth)
{
    auto start = std::chrono::steady_clock::now();

    u64      total = 0;
    MoveList list;
    MoveGen::Generate(pos, list);
    list.Legalize(pos);

    for (Move move : list) {
        Position act(pos);
        act.MakeMove(move);

        u64 val = Perft(act, depth - 1);
        total += val;

        act.UnmakeMove(move);
        SyncPrintln("{}: {}", Convert::MoveToStr(move), val);
    }

    auto end = std::chrono::steady_clock::now();
    SyncPrintln("\nNodes searched: {}", total);
    SyncPrintln("Total time: {}ms",
                std::chrono::milliseconds(std::chrono::nanoseconds(end - start).count() / 1'000'000)
                    .count());
}
