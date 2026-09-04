#include "Search/Search.h"

#include "Evaluation/Evaluation.h"
#include "MoveGen/MoveGen.h"

#include "MoveGen/MoveGen.h"
#include "Utils/Convert.h"
#include "Utils/Utils.h"

#define DEBUG

#ifdef DEBUG
#include <atomic>
#include <chrono>
static std::atomic<u64> totalPositions = 0;
#endif

static bool      m_isSearching = false;
constexpr double EXIT_VAL      = 12789503572893456.;

double SearchMoves(Position& pos, u64 depth)
{
#ifdef DEBUG
    totalPositions++;
#endif
    double posEval = Evaluation::Eval(pos);
    if (depth == 0) {
        return posEval;
    }

    MoveList list;
    MoveGen::Generate(pos, list);
    for (Move move : list) {
        if (!m_isSearching) {
            return EXIT_VAL;
        }

        if (!pos.IsLegal(move)) {
            continue;
        }

        pos.MakeMove(move);
        if (pos.Checkers(~pos.Player()) == 0) {
            double deeper = -SearchMoves(pos, depth - 1);
            if (posEval < deeper) {
                posEval = deeper;
            }
        }
        pos.UnmakeMove(move);
    }

    return posEval;
}

void Search::Begin(const Position& position, Engine::SearchParams params)
{
#ifdef DEBUG
    totalPositions = 0;
    auto start     = std::chrono::steady_clock::now();
#endif
    u64 depth = params.depth;
    if (depth == 0) {
        depth = 1;
    }

    m_isSearching = true;

    // Depth always searches opposite players move
    if (depth % 2 == 1) {
        depth++;
    }

    MoveList list;
    Position pos(position);
    MoveGen::Generate(pos, list);
    double mult     = (pos.Player() == WHITE ? -1. : 1.);
    double bestEval = -EXIT_VAL;
    std::println("Begin val: {}", bestEval);
    Move bestMove;

    for (Move move : list) {
        if (!m_isSearching) {
            break;
        }

        if (!pos.IsLegal(move)) {
            continue;
        }

        pos.MakeMove(move);
        if (pos.Checkers(~pos.Player()) == 0) {
            double searched = SearchMoves(pos, depth - 1) * mult;
            std::println("Searched: {:4}, Move: {}", searched, move.Str());
            if (bestEval < searched && (std::abs(searched) != EXIT_VAL)) {
                bestEval = searched;
                bestMove = move;
            }
        }
        pos.UnmakeMove(move);
    }
    SyncPrintln("bestmove {} ponder {}", Convert::MoveToStr(bestMove), "none");
#ifdef DEBUG
    auto end = std::chrono::steady_clock::now();
    SyncPrintln("Total positions: {}", totalPositions.load());
    SyncPrintln("Total time:{:6}ms",
                std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
#endif
    Engine::Stop();
}

void Search::Stop() { m_isSearching = false; }
