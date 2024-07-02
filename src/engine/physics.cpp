#include "physics.hpp"

#include "entity.hpp"
#include <cmath>
#include <cstdlib>

bool overlap_aabb(class CollisionEntity* entity_1, class CollisionEntity* entity_2)
{
    const int e1_x1 = entity_1->pos.x - entity_1->half_width;
    const int e1_x2 = entity_1->pos.x + entity_1->half_width;
    const int e1_y1 = entity_1->pos.y - entity_1->half_height;
    const int e1_y2 = entity_1->pos.y + entity_1->half_height;

    const int e2_x1 = entity_2->pos.x - entity_2->half_width;
    const int e2_x2 = entity_2->pos.x + entity_2->half_width;
    const int e2_y1 = entity_2->pos.y - entity_2->half_height;
    const int e2_y2 = entity_2->pos.y + entity_2->half_height;

    return (e1_x1 < e2_x2 && e1_x2 > e2_x1 && e1_y1 < e2_y2 && e1_y2 > e2_y1);
}

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
