#include "UCI.h"

#include <charconv>
#include <iostream>
#include <print>
#include <sstream>
#include <string>

#include "Utils.h"

UCI::UCI(int argc, char** argv)
{
    (void)argc;
    (void)argv;
}

UCI::~UCI()
{
    // Nothing todo
}



// ----- Read -----



// ----- Update -----

void UCI::Loop()
{
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
        
        if (token == "quit") {
            m_engine.Stop();
        }

        else if (token == "uci") {
            SyncPrintln("id name {}\nid author {}\nuciok", m_engine.Name(), m_engine.Author());
        }

        else if (token == "ucinewgame") {
            m_engine.SetState(std::string(" ") + DEFAULT_FEN.data());
        }

        else if (token == "isready") {
            m_engine.Ready();
            SyncPrintln("readyok");
        }

        else if (token == "setoption") {
        }

        else if (token == "position") {
            m_engine.SetState(ss.str());
        }

        else if (token == "go") {
        }

        else if (token == "stop") {
            m_engine.Stop();
        }

        else if (token == "ponderhit") {
        }

        // Custom commands

        else if (token == "flip" || token == "0000") {
            m_engine.Flip();
        }

        else if (token == "loglevel") {
            ss >> token;
            size_t i = 0, max = sizeof(Enums::ToString::LogLevel) / sizeof(Enums::ToString::LogLevel[0]);
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

