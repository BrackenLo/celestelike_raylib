#pragma once

#include "raylib.h"

namespace celestelike {

struct Pos {
    int x;
    int y;
};

struct Size {
    int half_width;
    int half_height;
};

struct Velocity {
    int x;
    int y;
};

struct Tile { };
struct Actor { };
struct Player { };

struct Render {
    bool visible;
};

struct Sprite {
    Color color;
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
