#pragma once

#include "player.h"

class AvianPlayerInner : public PlayerInner {
public:
    AvianPlayerInner(Player* outer);
};
