#pragma once

#include "components.hpp"

namespace celestelike {

const Camera2D default_camera();

namespace render {
    void draw_rect(const Pos& pos, const Sprite& sprite);
}

namespace physics {
    bool check_collision(const Pos pos1, const CollisionBounds bounds1, const Pos pos2, const CollisionBounds bounds2);
    int get_collision_depth(const int pos1, const int size1, const int pos2, const int size2);
    int get_collision_depth_x(const Pos& pos1, const Bounds& bounds1, const Pos& pos2, const Bounds& bounds2);
    int get_collision_depth_y(const Pos& pos1, const Bounds& bounds1, const Pos& pos2, const Bounds& bounds2);
}

}
