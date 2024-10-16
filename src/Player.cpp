#include "Player.h"

Player::Player(FLAG colour, FLAG type) {
    this->m_playerData = colour | type;
}

FLAG Player::Type() {
    return (this->m_playerData & PLAYER_TYPE);
}

FLAG Player::Colour() {
    return (this->m_playerData & PLAYER_COLOUR);
}

Player::~Player() {
    // Nothing todo
}

