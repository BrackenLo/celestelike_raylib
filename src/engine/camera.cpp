#include "camera.h"

#include "raylib.h"
#include "raymath.h"
#include "world.h"

GameCamera::GameCamera()
{
    camera.target = { 0 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    speed = 2;
    move_target = { 0 };
    follow_target = nullptr;

    zoom_speed = 4;
    zoom_target = 1.0f;
}

void GameCamera::update(World* world)
{
    float delta = GetFrameTime();
    float move_speed = speed * delta;

    if (follow_target != nullptr)
        pos = Vector2Lerp(pos, follow_target->pos, move_speed);
    else
        pos = Vector2Lerp(pos, move_target, move_speed);

    camera.target = pos;
    camera.zoom = Lerp(camera.zoom, zoom_target, zoom_speed * delta);

    if (IsWindowResized())
        reset();
}

void GameCamera::reset()
{
    camera.offset = {
        GetScreenWidth() / 2.0f,
        GetScreenHeight() / 2.0f
    };
}
