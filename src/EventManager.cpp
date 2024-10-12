#include "EventManager.h"

#include "WindowManager.h"
#include "Defines.h"

// ----- Creation -----

bool EventManager::s_eventClick = false;
bool EventManager::s_eventPromotion = false;
POINT EventManager::s_mousePos = { 0, 0 };
INDEX EventManager::s_indexPromotion = CODE_INVALID;


EventManager::EventManager() {
    // Nothing todo
}

// ----- Read -----

void EventManager::manageEvents() {
    // Deals with click events
    if (s_eventClick) {
        s_eventClick = false;
        this->manageClickEvents();
    }
    // Deals with pawn promotions
    if (s_eventPromotion) {
        this->s_eventPromotion = false;
        this->managePromotionEvents();
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

void EventManager::managePromotionEvents() {
    this->m_board->setPromotion(s_indexPromotion);
    this->s_indexPromotion = CODE_INVALID;
}

// ----- Update -----

void EventManager::setBoard(BoardManager* board) {
    this->m_board = board;
}

void EventManager::eventClick(POINT mousePos) {
    s_eventClick = true;
    s_mousePos = mousePos;

    // Determines if height of click must be adjusted
    POINT winSize = WindowManager::winSize();
    if (winSize.y > winSize.x) {
        s_mousePos.y -= (winSize.y - winSize.x);
    }
}

void EventManager::eventPromotion(INDEX index) {
    s_indexPromotion = index;
    s_eventPromotion = true;
}

// ----- Destruction -----

EventManager::~EventManager() {
    // Nothing todo
}

