#include "physics.h"

#include "entity.h"
#include <cmath>
#include <cstdlib>

std::optional<Collision> intersect_aabb(CollisionEntity* solid, CollisionEntity* actor)
{
    float dx = actor->pos.x - solid->pos.x;
    float px = (actor->half_width + solid->half_width) - std::abs(dx);

    if (px <= 0.0f)
        return std::nullopt;

    float dy = actor->pos.y - solid->pos.y;
    float py = (actor->half_height + solid->half_height) - std::abs(dy);

    if (py <= 0.0f)
        return std::nullopt;

    Collision collision;

    if (px < py) {
        float sx = std::copysign(1.0f, dx);

        collision.entity = solid;
        collision.pos = { solid->pos.x + solid->half_width * sx, actor->pos.y };
        collision.delta = { px * sx, 0 };
        collision.normal = { sx, 0 };
    } else {
        float sy = std::copysign(1.0f, dy);

        collision.entity = solid;
        collision.pos = { actor->pos.x, solid->pos.y + solid->half_height * sy };
        collision.delta = { 0., py * sy };
        collision.normal = { 0, sy };
    }

    return std::optional(collision);
}
