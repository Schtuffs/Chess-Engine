#pragma once

#include <iostream>

#include "Library.h"
#include "BoardManager.h"
#include "Defines.h"

class EventManager {
private:
    static bool s_eventClick, s_eventKey, s_eventPromotion;
    static INDEX s_indexPromotion;
    static POINT s_mousePos;
    static int   s_key;

    BoardManager* m_board;

    // ----- Read -----

    // Manages click events
    void manageClickEvents();

    // Manages key events
    void manageKeyEvents();

    // Manages promotion events
    void managePromotionEvents();

    // Prints all commands to console
    void showHelp();

public:
    // ----- Creation -----

    EventManager();

    // ----- Read -----

    // Manages all triggered events
    void manageEvents();

    // Returns index of click
    static INDEX getClickIndex();

    // ----- Update -----

    // Adds a board so functions can be called and managed
    void setBoard(BoardManager* board);

    // Sends a click event to be managed
    static void eventClick(POINT mousePos);

    static void eventKey(int key);

    // Sends a pawn promotion to be managed
    static void eventPromotion(INDEX index);

    // ----- Destruction -----

    ~EventManager();    
};

