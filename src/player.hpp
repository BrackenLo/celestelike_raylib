#pragma once

#include "components.hpp"
#include <entt/entt.hpp>
#include <raylib.h>

namespace celestelike {

namespace player {

    struct BasePlayerStats {
        WalkSpeed walk_speed;
        Jump jump;
        Gravity gravity;
        CollisionBounds collision_bounds;
        Sprite sprite;

        BasePlayerStats();
    };

    void next_character_type(entt::registry& reg, entt::entity player);
    void set_player_character_type(entt::registry& reg, entt::entity player);
    void remove_player_character_type(entt::registry& reg, entt::entity player);

    void reset_player(entt::registry& reg, entt::entity player, BasePlayerStats stats);

    void setup_base(entt::registry& reg, entt::entity player);
    void reset_base(entt::registry& reg, entt::entity player);

    void setup_debug(entt::registry& reg, entt::entity player);
    void reset_debug(entt::registry& reg, entt::entity player);

    void setup_avian(entt::registry& reg, entt::entity player);
    void reset_avian(entt::registry& reg, entt::entity player);

    void setup_celeste(entt::registry& reg, entt::entity player);
    void reset_celeste(entt::registry& reg, entt::entity player);
}

}
