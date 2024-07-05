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
    auto sprites = reg.view<Pos, Size, Render, Sprite>();

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
        Size& size = sprites.get<Size>(entity);
        Sprite& sprite = sprites.get<Sprite>(entity);

        draw_rect(pos, size, sprite);
    }

    EndMode2D();

    EndDrawing();
}
