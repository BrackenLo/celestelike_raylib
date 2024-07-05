#pragma once

#include "system.hpp"

class PlayerSystem final : public System {

public:
    void init(entt::registry& reg) override;
    void run(entt::registry& reg) override;
};

void update_player_input(entt::registry& reg);
void update_player_velocity(entt::registry& reg, float dt);
