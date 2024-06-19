#pragma once

#include "../engine/world.h"

class SimpleWorld : public World {
protected:
    void init(PhysicsData* physics_data) override;
};
