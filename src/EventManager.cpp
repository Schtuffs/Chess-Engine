#include "EventManager.h"

#include "WindowManager.h"
#include "Defines.h"

// ----- Creation -----

bool EventManager::s_eventClick = false;
bool EventManager::s_eventKey = false;
bool EventManager::s_eventPromotion = false;
POINT EventManager::s_mousePos = { 0, 0 };
int EventManager::s_key = 0;
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
    // Deals with key events
    if (s_eventKey) {
        s_eventKey = false;
        this->manageKeyEvents();
    }
    // Deals with pawn promotions
    if (s_eventPromotion) {
        this->s_eventPromotion = false;
        this->managePromotionEvents();
    }
}

INDEX EventManager::getClickIndex() {
    // Figure out what to do about click
    INDEX index = Library::physToIndex(s_mousePos);
    int min = Library::min(WindowManager::winSize());
    
    // Only check if the mouse position is within bounds
    if (s_mousePos.x < min) {
        return index;
    }
    return CODE_INVALID;
}

// ----- Read ----- Hidden -----

void EventManager::manageClickEvents() {
    // Figure out what to do about click
    INDEX index = Library::physToIndex(s_mousePos);
    int min = Library::min(WindowManager::winSize());
    
    // Only check if the mouse position is within bounds
    if (s_mousePos.x < min) {
        this->m_board->ManageInput(index);
    }
}

void EventManager::manageKeyEvents() {
    if (s_key == GLFW_KEY_F) {
        this->m_board->changeFlip();
    }
    else if (s_key == GLFW_KEY_H) {
        this->showHelp();
    }
    else if (s_key == GLFW_KEY_P) {
        this->m_board->changePerspective();
    }
}

void EventManager::managePromotionEvents() {
    this->m_board->setPromotion(s_indexPromotion);
    this->s_indexPromotion = CODE_INVALID;
}

void EventManager::showHelp() {
    std::cout << "H: Show this menu" << std::endl;
    std::cout << "R: Reset board" << std::endl;
    std::cout << "F: Enable/disable board flipping" << std::endl;
    std::cout << "P: Change board perspective" << std::endl;
    std::cout << "`: Show FPS" << std::endl;
    std::cout << "ESC: Close program" << std::endl;
    std::cout << std::endl;
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

void EventManager::eventKey(int key) {
    s_eventKey = true;
    s_key = key;
}

void EventManager::eventPromotion(INDEX index) {
    s_indexPromotion = index;
    s_eventPromotion = true;
}

// ----- Destruction -----

EventManager::~EventManager() {
    // Nothing todo
}

