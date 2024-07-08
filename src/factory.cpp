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

    Bounds bounds = { 25, 32 };

    reg.emplace<Pos>(entity, desc.x, desc.y);
    reg.emplace<CollisionBounds>(entity, bounds);

    reg.emplace<Velocity>(entity, 0, 0);
    reg.emplace<WalkSpeed>(entity, 2200, 2600, 400);
    reg.emplace<Jump>(entity);
    reg.emplace<Gravity>(entity, 2000, 800);

    reg.emplace<Actor>(entity);
    reg.emplace<Player>(entity);

    reg.emplace<Render>(entity, true);
    reg.emplace<Sprite>(entity, RED, bounds);

    reg.emplace<CameraTarget>(entity);

    return entity;
}

//

entt::entity spawn_camera(entt::registry& reg)
{
    entt::entity entity = reg.create();

    reg.emplace<Pos>(entity, 0, 0);
    reg.emplace<PosLerp>(entity, 0, 0, 4.f, 4.f);
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
