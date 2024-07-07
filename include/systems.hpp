#pragma once

#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"

typedef void (*GameSystem)(entt::registry& reg, float dt);

namespace celestelike {

namespace render {
    void render(entt::registry& reg, float dt);
}

namespace physics {
    void apply_velocity_collision(entt::registry& reg, float dt);
}

namespace update {
    void pos_lerp(entt::registry& reg, float dt);
}

namespace player {
    void update_input(entt::registry& reg, float dt);
    void check_collisions(entt::registry& reg, float dt);

    void handle_walk(entt::registry& reg, float dt);
    void handle_jump(entt::registry& reg, float dt);
    void handle_gravity(entt::registry& reg, float dt);

    void reset_values(entt::registry& reg, float dt);
}

namespace camera {
    void camera_follow(entt::registry& reg, float dt);
    void camera_update(entt::registry& reg, float dt);
}

}
