#pragma once

#include "raylib.h"

namespace celestelike {

//====================================================================

struct Bounds {
    int half_width;
    int half_height;
};

//====================================================================

struct Pos {
    int x;
    int y;
};

struct Velocity {
    int x;
    int y;
};

struct WalkSpeed {
    int accel;
    int deaccel;

    int max_walk_speed;
};

struct CollisionBounds {
    Bounds bounds;

    operator Bounds&() { return bounds; }
    operator const Bounds&() const { return bounds; }
};

struct Solid { };
struct Actor { };
struct Player { };

struct Render {
    bool visible;
};

struct Sprite {
    Color color;
    Bounds size;
};

struct PosLerp {
    Pos target = { 0, 0 };
    float speed_x = 4.0f;
    float speed_y = 4.0f;
};

struct GameCamera {
    Camera2D camera;

    GameCamera()
    {
        camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
        camera.target = { 0, 0 };
        camera.rotation = 0;
        camera.zoom = 1;
    }
    operator Camera2D&() { return camera; }
    operator const Camera2D&() const { return camera; }
};

struct CameraTarget { };

}
