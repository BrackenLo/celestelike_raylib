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
    reg.emplace<CollisionBounds>(entity, 32, 64);

    reg.emplace<Velocity>(entity, 0, 0);
    reg.emplace<WalkSpeed>(entity, 2200, 2600, 400);

    reg.emplace<Actor>(entity);
    reg.emplace<Player>(entity);

    reg.emplace<Render>(entity, true);
    reg.emplace<Sprite>(entity, RED, 32, 64);

    return entity;
}

//

entt::entity spawn_camera(entt::registry& reg)
{
    entt::entity entity = reg.create();

    reg.emplace<Pos>(entity, 0, 0);
    reg.emplace<GameCamera>(entity);

    return entity;
}

//

entt::entity spawn_tile(entt::registry& reg)
{
    return spawn_tile(reg, TileDescriptor());
}

entt::entity spawn_tile(entt::registry& reg, TileDescriptor desc)
{
    entt::entity entity = reg.create();

    reg.emplace<Pos>(entity, desc.x, desc.y);
    reg.emplace<CollisionBounds>(entity, desc.half_width, desc.half_height);

    reg.emplace<Solid>(entity);

    reg.emplace<Render>(entity, true);
    reg.emplace<Sprite>(entity, GREEN, desc.half_width, desc.half_height);

    return entity;
}

}
