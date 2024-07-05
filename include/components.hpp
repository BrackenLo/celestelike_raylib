#pragma once

#include "raylib.h"

namespace celestelike {

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
    int half_width;
    int half_height;
};

struct Solid { };
struct Actor { };
struct Player { };

struct Render {
    bool visible;
};

struct Sprite {
    Color color;
    int half_width;
    int half_height;
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
    operator const Camera2D&() { return camera; }
};

}
