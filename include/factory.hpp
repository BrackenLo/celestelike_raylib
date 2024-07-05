#pragma once

#include "entt/entt.hpp"

namespace celestelike {

struct PlayerDescriptor {
    int x = 0;
    int y = 0;
};

entt::entity spawn_player(entt::registry& reg);
entt::entity spawn_player(entt::registry& reg, PlayerDescriptor desc);

entt::entity spawn_camera(entt::registry& reg);

}
