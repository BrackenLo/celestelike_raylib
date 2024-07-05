#include "factory.hpp"

#include "components.hpp"

namespace celestelike {

entt::entity spawn_player(entt::registry& reg)
{
    PlayerDescriptor desc;

    return spawn_player(reg, desc);
}

entt::entity spawn_player(entt::registry& reg, PlayerDescriptor desc)
{
    entt::entity entity = reg.create();

    reg.emplace<Pos>(entity, desc.x, desc.y);
    reg.emplace<Size>(entity, 32, 64);
    reg.emplace<Velocity>(entity, 0, 0);
    reg.emplace<Actor>(entity);
    reg.emplace<Player>(entity);
    reg.emplace<Render>(entity, true);
    reg.emplace<Sprite>(entity, RED);

    return entity;
}

entt::entity spawn_camera(entt::registry& reg)
{
    entt::entity entity = reg.create();

    reg.emplace<Pos>(entity, 0, 0);
    reg.emplace<GameCamera>(entity);

    return entity;
}

}
