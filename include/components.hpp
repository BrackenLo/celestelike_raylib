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

struct Jump {
    int impulse;
    bool ended_early;
};

struct Gravity {
    int fall_speed;
    int max_fall_speed;
};

struct CollisionBounds {
    Bounds bounds;

    operator Bounds&() { return bounds; }
    operator const Bounds&() const { return bounds; }
};

struct OnGround {
    bool just_started = false;
};
struct OnCeiling {
    bool just_started = false;
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
    Pos target;
    float speed_x;
    float speed_y;
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
