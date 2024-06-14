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
}

void GameCamera::update(World* world)
{
    float delta = GetFrameTime();
    float move_speed = speed * delta;

    if (follow_target != nullptr)
        pos = Vector2Lerp(pos, follow_target->pos, speed * delta);
    else
        pos = Vector2Lerp(pos, move_target, speed * delta);

    camera.target = pos;

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
