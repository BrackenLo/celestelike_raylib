#pragma once

#include "entt/entt.hpp"

class Scene {
public:
    Scene() {};
    ~Scene() {};

    virtual void init() {};
    virtual void update() = 0;

protected:
    entt::registry reg;
};
