#pragma once

#include "raylib.h"
#include <vector>

struct PlayerInput {
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

    bool ability_1_pressed = false;
    bool ability_1_down = false;
    bool ability_2_pressed = false;
    bool ability_2_down = false;

    bool ability_3_pressed = false;
};
