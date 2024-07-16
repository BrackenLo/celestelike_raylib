#pragma once

#include "components.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>

namespace celestelike {

struct PlayerDescriptor {
    int x;
    int y;
    std::vector<player::PlayerCharacterTypes> characters;
    int character_index;

    PlayerDescriptor()
    {
        x = 0;
        y = 0;
        characters = { player::PlayerCharacterTypes::Base };
        character_index = 0;
    }
    PlayerDescriptor(int x, int y)
        : PlayerDescriptor()
    {
        this->x = x;
        this->y = y;
    }
};

entt::entity spawn_player(entt::registry& reg);
entt::entity spawn_player(entt::registry& reg, PlayerDescriptor desc);

entt::entity spawn_camera(entt::registry& reg);

struct TileDescriptor {
    int x = 0;
    int y = 0;
    int half_width = 16;
    int half_height = 16;
};

entt::entity spawn_tile(entt::registry& reg);
entt::entity spawn_tile(entt::registry& reg, TileDescriptor desc);

}
