#include "factory.hpp"

#include <imgui_entt_entity_editor.hpp>

#include "components.hpp"
#include "player.hpp"

namespace celestelike {

entt::entity spawn_player(entt::registry& reg, PlayerDescriptor desc)
{
    entt::entity entity = reg.create();

    Bounds bounds = { 25, 32 };

    reg.emplace<MM::Name>(entity, "Player");

    reg.emplace<Actor>(entity);
    reg.emplace<Player>(entity);

    reg.emplace<Pos>(entity, desc.x, desc.y);
    reg.emplace<Velocity>(entity, 0, 0);

    reg.emplace<Render>(entity, true);

    reg.emplace<player::PlayerCharacters>(entity, player::PlayerCharacters(desc.characters, desc.character_index));
    player::set_player_character_type(reg, entity); // Should insert all the main components

    reg.emplace<CameraTarget>(entity);

    return entity;
}

//

entt::entity spawn_camera(entt::registry& reg)
{
    entt::entity entity = reg.create();

    reg.emplace<MM::Name>(entity, "Camera");

    reg.emplace<Pos>(entity, 0, 0);
    reg.emplace<PosLerp>(entity, 0, 0, 4.f, 4.f);
    reg.emplace<GameCamera>(entity);

    return entity;
}

//

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
