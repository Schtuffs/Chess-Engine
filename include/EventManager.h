#pragma once

#include <iostream>

#include "Library.h"
#include "BoardManager.h"

class EventManager {
private:
    static bool s_eventClick;
    static POINT s_mousePos;

    BoardManager* m_board;

    // ----- Read -----

    // Manages click events
    void manageClickEvents();

public:
    // ----- Creation -----

    EventManager();

    // ----- Read -----

    // Manages all triggered events
    void manageEvents();

    // ----- Update -----

    // Adds a board so functions can be called and handled
    void setBoard(BoardManager* board);

    // Sends a click event to be handled
    static void eventClick(POINT mousePos);

    // ----- Destruction -----

    ~EventManager();    
};

