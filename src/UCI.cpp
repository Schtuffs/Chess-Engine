#include "UCI.h"

#include <chrono>
#include <thread>

#include <charconv>
#include <iostream>
#include <print>
#include <sstream>
#include <string>

#include "Engine.h"
#include "Utils/Fen.h"
#include "Utils/Utils.h"

// ----- Update -----

void UCI::Loop(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    std::string cmd, token;

    do {
        if (!getline(std::cin, cmd)) {
            cmd = "quit";
        }

        std::istringstream ss(cmd);

        token.clear();
        ss >> token;
        DebugPrintln("Token: {}", token);

        // UCI commands

        if (token == "quit" || token == "q") {
            Engine::Stop();
            token = "quit";
        }

        else if (token == "uci") {
            Engine::Uci();
        }

        else if (token == "ucinewgame") {
            Engine::SetPosition(std::stringstream(ss.str()));
        }

        else if (token == "isready") {
            Engine::IsReady();
            SyncPrintln("readyok");
        }

        else if (token == "setoption") {
            Engine::SetOption(std::stringstream(ss.str()));
        }

        else if (token == "position") {
            Engine::SetPosition(std::stringstream(ss.str()));
        }

        else if (token == "go") {
            Engine::Go(std::stringstream(ss.str()));
        }

        else if (token == "stop") {
            Engine::Stop();
        }

        else if (token == "ponderhit") {
            Engine::PonderHit();
        }

        // Custom commands

        else if (token == "d") {
            Engine::D();
        }

        else if (token == "flip" || token == "0000") {
            Engine::Flip();
        }

        else if (token == "loglevel") {
            ss >> token;
            size_t i   = 0,
                   max = sizeof(Enums::ToString::LogLevel) / sizeof(Enums::ToString::LogLevel[0]);
            while (i < max) {
                if (token == Enums::ToString::LogLevel[i]) {
                    Utils::SetLogLevel((static_cast<Utils::LogLevel>(i)));
                    DebugPrintln("Setting log level to: {}", token);
                    break;
                }
                i++;
            }

            if (i == max) {
                ErrorPrintln("Failed to receive valid log level: {}", token);
            }
        }

        else {
            ErrorPrintln("Unknown command: {}", token);
        }

    } while (token != "quit");
}
