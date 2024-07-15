#include "helper.hpp"

#include <cmath>
#include <cstdlib>
#include <raylib.h>

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

namespace render {
    void draw_rect(const Pos& pos, const Sprite& sprite)
    {
        int x = pos.x - sprite.size.half_width;
        int y = pos.y - sprite.size.half_height;
        int width = sprite.size.half_width * 2.0f;
        int height = sprite.size.half_height * 2.0f;

        DrawRectangle(x, y, width, height, sprite.color);
    }
}

namespace physics {
    bool check_collision_auto(entt::registry& reg, const entt::entity entity)
    {
        auto v_to_check = reg.view<Actor, CollisionBounds, Pos>(entt::exclude<Solid>);
        auto v_solid = reg.view<Solid, CollisionBounds, Pos>();

        if (!v_to_check.contains(entity))
            return false;

        const CollisionBounds& bounds = v_to_check.get<CollisionBounds>(entity);
        const Pos& pos = v_to_check.get<Pos>(entity);

        for (entt::entity solid : v_solid) {
            const Pos& solid_pos = v_solid.get<Pos>(solid);
            const CollisionBounds& solid_bounds = v_solid.get<CollisionBounds>(solid);

            if (check_collision(pos, bounds, solid_pos, solid_bounds)) {
                return true;
            }
        }

        return true;
    }

    bool check_collision(const Pos pos1, const Bounds bounds1, const Pos pos2, const Bounds bounds2)
    {
        const int e1_x1 = pos1.x - bounds1.half_width;
        const int e1_x2 = pos1.x + bounds1.half_width;
        const int e1_y1 = pos1.y - bounds1.half_height;
        const int e1_y2 = pos1.y + bounds1.half_height;

        const int e2_x1 = pos2.x - bounds2.half_width;
        const int e2_x2 = pos2.x + bounds2.half_width;
        const int e2_y1 = pos2.y - bounds2.half_height;
        const int e2_y2 = pos2.y + bounds2.half_height;

        return (e1_x1 < e2_x2 && e1_x2 > e2_x1 && e1_y1 < e2_y2 && e1_y2 > e2_y1);
    }

    int get_collision_depth(const int pos1, const int size1, const int pos2, const int size2)
    {
        int d = pos1 - pos2;
        int p = (size1 + size2) - std::abs(d);

        if (p <= 0)
            return 0;

        int s = std::copysign(1, d);
        int depth = p * s;
        return depth;
    }

    int get_collision_depth_x(const Pos& pos1, const Bounds& bounds1, const Pos& pos2, const Bounds& bounds2)
    {
        return get_collision_depth(pos1.x, bounds1.half_width, pos2.x, bounds2.half_width);
    }

    int get_collision_depth_y(const Pos& pos1, const Bounds& bounds1, const Pos& pos2, const Bounds& bounds2)
    {
        return get_collision_depth(pos1.y, bounds1.half_height, pos2.y, bounds2.half_height);
    }
}

namespace tools {

    void trace(const char* msg)
    {
        TraceLog(TraceLogLevel::LOG_INFO, msg);
    }

    void warn(const char* msg)
    {
        TraceLog(TraceLogLevel::LOG_WARNING, msg);
    }

    bool are_keys_down(std::vector<int> keys)
    {
        for (int key : keys)
            if (IsKeyDown(key))
                return true;
        return false;
    }

    bool are_keys_pressed(std::vector<int> keys)
    {
        for (int key : keys)
            if (IsKeyPressed(key))
                return true;
        return false;
    }

    int round_to(int num, int multiple)
    {
        // TODO - find more optimised solution? this one rounds up by default
        int is_positive = (int)(num >= 0);
        return ((num + is_positive * (multiple - 1)) / multiple) * multiple - multiple;

        // for use with multiples of two
        // return (((int)num + multiple - 1) & -multiple) - multiple;
    }
}

}
