#pragma once

#include "components.hpp"

namespace celestelike {

const Camera2D default_camera();

void draw_rect(const Pos& pos, const Sprite& sprite);
bool check_collision(const Pos pos_1, const CollisionBounds bounds_1, const Pos pos_2, const CollisionBounds bounds_2);

}
