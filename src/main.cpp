/*------------------------------

===== TODO =====

UCI:
- quit
-- Exit program ASAP
- uci
-- Use UCI
- ucinewgame
-- Next position is a new game
- isready
-- Synchronize engine
- setoption
-- Change engine parameters
- position
-- Setup game position
- go
-- Start calculating
- stop
-- Stop calculation ASAP, guarenteed after 5 seconds
- ponderhit
-- Player played expected move

Extra:
- flip
-- Flips the side to move

================

------------------------------*/

#include <print>
#include <sstream>
#include <string>

#include "Constants.h"
#include "UCI.h"
#include "Utils.h"

void PrintEngineVersion()
{
    constexpr std::string_view months("Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec");

    std::string       month, day, year;
    std::stringstream date(__DATE__);

    date >> month >> day >> year;
    SyncPrintln("Chess Engine {}.{} - {:04}-{:02}-{:02}", VERSION_MAJOR, VERSION_MINOR,
                std::stoi(year), (months.find(month) / 4 + 1), std::stoi(day));
}

int main(int argc, char** argv)
{
    PrintEngineVersion();

    UCI::Loop(argc, argv);

    return 0;
}
