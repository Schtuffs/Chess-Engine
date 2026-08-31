#include "Search/Search.h"

#include <bit>
#include <chrono>
#include <print>
#include <random>
#include <tuple>
#include <utility>
#include <vector>

#include "Evaluation/Evaluation.h"
#include "MoveGen/MoveGen.h"

#include "Engine.h"
#include "Utils/Convert.h"
#include "Utils/Utils.h"

// Not great, but its temporary
std::random_device rd;
std::mt19937_64    randoms(rd());

u64 Perft(Position& pos, int depth)
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

void PerftMain(const Position& pos, int depth)
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

void Search::Begin(const Position& pos)
{
    PerftMain(pos, 5);
    // SyncPrintln("bestmove {} ponder {}", "e2e4", "e7e5");
    Engine::Stop();
}
