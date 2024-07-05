#pragma once

#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"

namespace celestelike {

struct PlayerDescriptor {
    int x = 0;
    int y = 0;
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
