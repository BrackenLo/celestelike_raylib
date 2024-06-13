#include "world.h"

#include "entity.h"

World::World() { }

World::~World()
{
    for (auto entity : actors) {
        delete entity;
    }
    for (auto entity : solids) {
        delete entity;
    }
}

void World::add_solid(Solid* solid)
{
    solids.push_back(solid);
}

void World::add_actor(Actor* actor)
{
    actors.push_back(actor);
}

std::vector<Actor*>* World::get_actors()
{
    return &actors;
}

std::vector<Solid*>* World::get_solids()
{
    return &solids;
}

std::vector<Collision> World::check_collision(Actor* actor)
{
    std::vector<Collision> collisions;

    for (Solid* solid : solids) {

        auto collision = intersect_aabb(solid, actor);

        if (!collision.has_value())
            continue;

        collisions.push_back(collision.value());
    }

    return collisions;
}
