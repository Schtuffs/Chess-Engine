#include "Search/Search.h"

#include <bit>
#include <print>
#include <random>
#include <utility>
#include <vector>

#include "MoveGen/MoveGen.h"

#include "Convert.h"
#include "Engine.h"
#include "Utils.h"

// Not great, but its temporary
std::random_device rd;
std::mt19937_64    randoms(rd());

void Search::Begin(const Board& board)
{
    MoveGen gen;
    gen.Generate(board, board.Player());
    if (gen.IsCheckmate()) {
        SyncPrintln("{} detected Checkmate!", Engine::Name());
    }
    if (gen.IsStalemate()) {
        SyncPrintln("{} detected Stalemate!", Engine::Name());
    }

    std::vector<std::pair<u8, BitBoard>> moves;
    moves.reserve(16);
    for (u64 i = 0; i < 64; i++) {
        BitBoard bb = gen.GetMoves(i);
        if (bb) {
            moves.push_back(std::pair{i, bb});
        }
    }

    // "random"
    std::uniform_int_distribution<u8> distrib(0, moves.size() - 1);
    
    std::string move;
    do {
        u8 i = distrib(randoms);
        u8 index = moves[i].first;
        BitBoard moveBB = moves[i].second;

        std::uniform_int_distribution<u8> moveChooser(0, 63);
        while (true) {
            u8 shift = moveChooser(randoms);
            if (moveBB & (1ull << shift)) {
                moveBB &= (1ull << shift);
                break;
            }
        }

        move = Convert::IndexToMove(index) + Convert::IndexToMove(Convert::BitBoardToIndex(moveBB));
    } while (!Engine::MakeMove(move));

    SyncPrintln("bestmove {} ponder {}", move, move);
}
