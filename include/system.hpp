#pragma once

#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"

class System {
public:
    virtual void init(entt::registry& reg) {};
    virtual void run(entt::registry& reg) = 0;
};

class RenderSystem final : public System {

public:
    void init(entt::registry& reg) override;
    void run(entt::registry& reg) override;
};

class PhysicsSystem final : public System {
public:
    void run(entt::registry& reg) override;
};
