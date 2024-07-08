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

    bool coyote_usable;
    float coyote_time;
    bool buffered_jump_usable;
    float jump_buffer;
    bool ended_early;
    float fall_multiplier;

    float time_left_ground;

    Jump()
        : Jump(-800, 0.2f, 0.2f, 3.0f) {};

    Jump(int impulse, float coyote_time, float jump_buffer, float fall_multiplier)
        : impulse(impulse)
        , coyote_time(coyote_time)
        , jump_buffer(jump_buffer)
        , fall_multiplier(fall_multiplier)
    {
        coyote_usable = false;
        buffered_jump_usable = false;
        ended_early = false;
        time_left_ground = 0.0f;
    }
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
    float zoom_target;
    float zoom_speed;

    GameCamera()
    {
        camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
        camera.target = { 0, 0 };
        camera.rotation = 0;
        camera.zoom = zoom_target = 1;

        zoom_speed = 1;
    }
    operator Camera2D&() { return camera; }
    operator const Camera2D&() const { return camera; }
};

struct CameraTarget { };

}
