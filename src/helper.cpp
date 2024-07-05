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

const void draw_rect(const Pos& pos, const Size& size, const Sprite& sprite)
{
    int x = pos.x - size.half_width;
    int y = pos.y - size.half_height;
    int width = size.half_width * 2.0f;
    int height = size.half_height * 2.0f;

    DrawRectangle(x, y, width, height, sprite.color);
}

}
