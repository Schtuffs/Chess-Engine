#pragma once

#include "Defines.h"

class Player {
private:
    FLAG m_playerData;

public:
    Player(FLAG colour, FLAG type = PLAYER_TYPE_HUMAN);

    FLAG Type();
    FLAG Colour();

    ~Player();
};

