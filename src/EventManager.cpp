#include "EventManager.h"

#include "WindowManager.h"
#include "Defines.h"

// ----- Creation -----

bool EventManager::s_eventClick = false;
POINT EventManager::s_mousePos = { 0, 0 };

EventManager::EventManager() {
    // Nothing todo
}

// ----- Read -----

void EventManager::eventClick(POINT mousePos) {
    s_eventClick = true;
    s_mousePos = mousePos;

    // Determines if height of click must be adjusted
    POINT winSize = WindowManager::winSize();
    if (winSize.y > winSize.x) {
        s_mousePos.y -= (winSize.y - winSize.x);
    }
}

void EventManager::manageEvents() {
    // Deals with click events
    if (s_eventClick) {
        s_eventClick = false;
        this->manageClickEvents();
    }
}

// ----- Read ----- Hidden -----

void EventManager::manageClickEvents() {
    // Figure out what to do about click
    INDEX index = Library::physToIndex(s_mousePos);
    int min = Library::min(WindowManager::winSize());
    
    // Only check if the mouse position is within bounds
    if (s_mousePos.x < min) {
        this->m_board->check(index);
    }
}

// ----- Update -----

void EventManager::setBoard(BoardManager* board) {
    this->m_board = board;
}

// ----- Destruction -----

EventManager::~EventManager() {
    // Nothing todo
}

