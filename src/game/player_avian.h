#pragma once

#include "player.h"

class AvianPlayerInner : public PlayerInner {
public:
    AvianPlayerInner(Player* outer);

protected:
    virtual float get_gravity(class World* world) override;

protected:
    float glide_gravity;
};
