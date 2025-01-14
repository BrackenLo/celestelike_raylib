#include "camera.hpp"

#include "raylib.h"
#include "raymath.h"
#include "world.hpp"

GameCamera::GameCamera()
{
    camera.target = { 0 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    speed = { 4.0f, 4.0f }; // TODO - Split into x and y components
    move_target = { 0 };
    follow_target = nullptr;

    zoom_speed = 4;
    zoom_target = 1.0f;
}

void GameCamera::update(World* world)
{
    float delta = GetFrameTime();
    Vector2 move_speed = Vector2Scale(speed, delta);

    Vector2 target = follow_target != nullptr ? follow_target->pos : move_target;

    pos.x = Lerp(pos.x, target.x, move_speed.x);
    pos.y = Lerp(pos.y, target.y, move_speed.y);

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

void GameCamera::set_follow_target(Entity* target, bool snap)
{
    if (!target) {
        TraceLog(TraceLogLevel::LOG_WARNING, "Camera set follow target - invalid entity provided");
        return;
    }

    follow_target = target;
    if (snap) {
        pos = follow_target->pos;
    }
}

void GameCamera::get_properties(std::vector<DebugProperty>* properties)
{
    properties->push_back({ "position", &pos, false });

    properties->push_back({ "target", &camera.target, false });
    properties->push_back({ "rotation", &camera.rotation, true, 0, 360 });
    properties->push_back({ "speed", &speed, true, 1, 20 });

    properties->push_back({ "zoom", &camera.zoom, false });
    properties->push_back({ "zoom_speed", &zoom_speed, true, 0.1, 20, 0.05 });
    properties->push_back({ "zoom_target", &zoom_target, true, 0.1, 20, 0.05 });
}
