#include "system.hpp"

#include "entt/entity/entity.hpp"
#include "helper.hpp"
#include "raylib.h"

#include "components.hpp"
#include "resources.hpp"

void RenderSystem::init(entt::registry& reg)
{
    reg.ctx().emplace<celestelike::ClearColor>(celestelike::ClearColor { RAYWHITE });
}

void RenderSystem::run(entt::registry& reg)
{
    using namespace celestelike;

    auto cameras = reg.view<GameCamera>();
    auto sprites = reg.view<Pos, Render, Sprite>();

    entt::entity camera_entity = cameras.front();
    Camera2D camera = camera_entity == entt::null ? default_camera() : cameras.get<GameCamera>(camera_entity);

    BeginDrawing();

    Color clear_color = reg.ctx().contains<ClearColor>() ? reg.ctx().get<ClearColor>() : RAYWHITE;
    ClearBackground(clear_color);

    BeginMode2D(camera);

    for (const entt::entity entity : sprites) {

        Render& render = sprites.get<Render>(entity);

        if (!render.visible)
            continue;

        Pos& pos = sprites.get<Pos>(entity);
        Sprite& sprite = sprites.get<Sprite>(entity);

        draw_rect(pos, sprite);
    }

    EndMode2D();

    EndDrawing();
}

void PhysicsSystem::run(entt::registry& reg)
{
    using namespace celestelike;

    float dt = 1.0f / 60.0f;

    auto actors = reg.view<Actor, Pos, CollisionBounds, Velocity>();
    auto solids = reg.view<Solid, Pos, CollisionBounds>();

    for (entt::entity entity : actors) {
        Pos& pos = actors.get<Pos>(entity);
        const CollisionBounds& bounds = actors.get<CollisionBounds>(entity);
        const Velocity& vel = actors.get<Velocity>(entity);

        Pos new_pos = pos;
        new_pos.x += vel.x * dt;
        new_pos.y += vel.y * dt;

        bool hit = false;

        for (entt::entity solid_entity : solids) {
            const Pos& solid_pos = solids.get<Pos>(solid_entity);
            const CollisionBounds& solid_bounds = solids.get<CollisionBounds>(solid_entity);

            if (check_collision(new_pos, bounds, solid_pos, solid_bounds)) {
                hit = true;
                break;
            }
        }

        if (hit)
            continue;

        pos = new_pos;
    }
}
