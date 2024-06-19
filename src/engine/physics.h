#pragma once

#include "raylib.h"
#include <optional>

struct PhysicsData {
    float elapsed = 0.0f;
    float timestep = 1.0f / 60.0f;
    float accumulator = 0.0f;
};

struct Collision {
    class CollisionEntity* entity;
    Vector2 pos;
    Vector2 delta;
    Vector2 normal;
    float time;
};

bool overlap_aabb(class CollisionEntity* entity_1, class CollisionEntity* entity_2);

std::optional<Collision> intersect_aabb(class CollisionEntity* solid, class CollisionEntity* actor);
