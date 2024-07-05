#include "helper.hpp"

#include "raylib.h"

namespace celestelike {

const Camera2D default_camera()
{
    Camera2D cam;
    cam.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    cam.target = { 0, 0 };
    cam.rotation = 0;
    cam.zoom = 1;
    return cam;
}

void draw_rect(const Pos& pos, const Sprite& sprite)
{
    int x = pos.x - sprite.half_width;
    int y = pos.y - sprite.half_height;
    int width = sprite.half_width * 2.0f;
    int height = sprite.half_height * 2.0f;

    DrawRectangle(x, y, width, height, sprite.color);
}

bool check_collision(const Pos pos_1, const CollisionBounds bounds_1, const Pos pos_2, const CollisionBounds bounds_2)
{
    const int e1_x1 = pos_1.x - bounds_1.half_width;
    const int e1_x2 = pos_1.x + bounds_1.half_width;
    const int e1_y1 = pos_1.y - bounds_1.half_height;
    const int e1_y2 = pos_1.y + bounds_1.half_height;

    const int e2_x1 = pos_2.x - bounds_2.half_width;
    const int e2_x2 = pos_2.x + bounds_2.half_width;
    const int e2_y1 = pos_2.y - bounds_2.half_height;
    const int e2_y2 = pos_2.y + bounds_2.half_height;

    return (e1_x1 < e2_x2 && e1_x2 > e2_x1 && e1_y1 < e2_y2 && e1_y2 > e2_y1);
}

}
