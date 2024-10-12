#pragma once

#include <iostream>

#include "Library.h"
#include "BoardManager.h"
#include "Defines.h"

class EventManager {
private:
    static bool s_eventClick, s_eventPromotion;
    static INDEX s_indexPromotion;
    static POINT s_mousePos;

    BoardManager* m_board;

    // ----- Read -----

    // Manages click events
    void manageClickEvents();

    // Manages promotion events
    void managePromotionEvents();

public:
    // ----- Creation -----

    EventManager();

    // ----- Read -----

    // Manages all triggered events
    void manageEvents();

    // ----- Update -----

    // Adds a board so functions can be called and managed
    void setBoard(BoardManager* board);

    // Sends a click event to be managed
    static void eventClick(POINT mousePos);

    // Sends a pawn promotion to be managed
    static void eventPromotion(INDEX index);

    // ----- Destruction -----

    ~EventManager();    
};

