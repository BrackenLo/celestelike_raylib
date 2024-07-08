#pragma once

#include "raylib.h"

namespace celestelike {

struct Time {
    float elapsed;
};

struct FixedTimestep {
    float elapsed = 0.0f;
    float timestep = 1.0f / 60.0f;
    float accumulator = 0.0f;
    bool paused = false;
};

struct ClearColor {
    Color color;

    ClearColor() { color = RAYWHITE; }
    operator const Color&() { return color; }
};

}
