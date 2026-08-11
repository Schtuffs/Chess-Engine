#include "Engine.h"

#include <charconv>
#include <cstdlib>
#include <format>
#include <print>
#include <sstream>

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
    std::string state = Convert::FenToString(m_board.Fen());

    if (m_isWhiteTurn) {
        state += " Player: White";
    } else {
        state += " Player: Black";
    }

    state += ", Full: " + std::format("{:2d}", m_halfMoves);
    state += ", Half: " + std::format("{:2d}", m_halfMoves);

    state += '\n';
    return state;
}

// ----- Update -----

void Engine::Flip() { m_isWhiteTurn = !m_isWhiteTurn; }

void Engine::Ready() {}

bool Engine::SetState(std::string_view data)
{
    if (data == "ucinewgame") {
        m_board = Board();
        DebugPrintln("Engine::SetState: Valid fen: {}", m_board.Fen());
        return true;
    }

    std::stringstream ss(data.data());
    std::string       token;
    ss >> token;
    if (token != "position") {
        m_board = Board();
        return false;
    }

    ss >> token;
    if (token == "startpos") {
        std::println("Start");
        token = DEFAULT_FEN;
    }

    if (!Fen::IsValidFen(token.data())) {
        m_board = Board();
        return false;
    }

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

    ss >> token;
    while (!ss.fail()) {
        if (!m_board.MakeMove(token)) {
            m_board = Board();
            return false;
        }
        m_halfMoves++;
        ss >> token;
    }
    DebugPrintln("Engine::SetState: Valid fen: {}", m_board.Fen());
    return true;
}

void Engine::Stop() {}
