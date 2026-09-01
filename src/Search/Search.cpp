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

void Search::Begin(const Position& pos)
{
    (void)pos;
    SyncPrintln("bestmove {} ponder {}", "e2e4", "e7e5");
    Engine::Stop();
}
