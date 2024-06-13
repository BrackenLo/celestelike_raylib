#pragma once

#include "raylib.h"
#include <optional>

struct Collision {
    class CollisionEntity* entity;
    Vector2 pos;
    Vector2 delta;
    Vector2 normal;
    float time;
};

std::optional<Collision> intersect_aabb(class Solid* solid, class Actor* actor);
