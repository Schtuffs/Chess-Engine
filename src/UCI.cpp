#include "UCI.h"

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

        // UCI commands
        
        if (token == "quit") {
            m_engine.Stop();
        }

        else if (token == "uci") {
            SyncPrintln("id name {}\nid author {}\nuciok", m_engine.Name(), m_engine.Author());
        }

        else if (token == "ucinewgame") {
            m_engine.SetState(DEFAULT_FEN);
        }

        else if (token == "isready") {
            m_engine.Ready();
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

    } while (token != "quit");
}

