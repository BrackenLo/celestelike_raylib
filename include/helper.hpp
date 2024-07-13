#pragma once

#include "components.hpp"
#include <cmath>
#include <vector>

namespace celestelike {

const Camera2D default_camera();

namespace render {
    void draw_rect(const Pos& pos, const Sprite& sprite);
}

namespace physics {
    bool check_collision(const Pos pos1, const Bounds bounds1, const Pos pos2, const Bounds bounds2);
    int get_collision_depth(const int pos1, const int size1, const int pos2, const int size2);
    int get_collision_depth_x(const Pos& pos1, const Bounds& bounds1, const Pos& pos2, const Bounds& bounds2);
    int get_collision_depth_y(const Pos& pos1, const Bounds& bounds1, const Pos& pos2, const Bounds& bounds2);
}

namespace tools {
    void trace(const char* msg);

    bool are_keys_down(std::vector<int> keys);
    bool are_keys_pressed(std::vector<int> keys);

    template <typename T>
    T step(T val, T target, T step)
    {
        if (std::signbit(target - val))
            return std::max((val - step), target);
        else
            return std::min((val + step), target);
    }

    int round_to(int num, int multiple);
}

}
