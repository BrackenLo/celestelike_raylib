#pragma once

#include <raylib.h>
#include <vector>

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

struct PlayerInput {
    struct AbilityInput {
        bool pressed = false;
        bool down = false;
        bool released = false;
    };

    // Config Player Variables
    std::vector<int> key_up = { KEY_I };
    std::vector<int> key_down = { KEY_K };
    std::vector<int> key_left = { KEY_J };
    std::vector<int> key_right = { KEY_L };

    std::vector<int> key_jump = { KEY_SPACE };
    std::vector<int> key_ability_1 = { KEY_C };
    std::vector<int> key_ability_2 = { KEY_X };
    std::vector<int> key_ability_3 = { KEY_Z };

    // Managed Player Variables
    int x_dir = 0;
    int y_dir = 0;
    bool jump_held = false;
    bool jump_pressed = false;
    float time_jump_pressed = 0.0f;

    AbilityInput ability_1;
    AbilityInput ability_2;

    bool ability_3_pressed = false;
};

}
