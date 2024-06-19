#include "world.h"

#include "entity.h"
#include <algorithm>

World::World()
{
    clear_color = RAYWHITE;
}

World::~World()
{
    for (auto entity : actors) {
        delete entity;
    }
    for (auto entity : solids) {
        delete entity;
    }
}

//====================================================================

bool World::destroy_actor(Actor* actor)
{
    auto it = std::find(actors.begin(), actors.end(), actor);
    if (it != actors.end()) {
        actors.erase(it);
        return true;
    }

    return false;
}

bool World::destroy_solid(Solid* solid)
{
    auto it = std::find(solids.begin(), solids.end(), solid);
    if (it != solids.end()) {
        solids.erase(it);
        return true;
    }

    return false;
}

//====================================================================

std::vector<Collision> World::check_collision(CollisionEntity* to_check)
{
    std::vector<Collision> collisions;

    for (Solid* solid : solids) {

        std::optional<Collision> collision = intersect_aabb(solid, to_check);

        if (!collision.has_value())
            continue;

        collisions.push_back(collision.value());
    }

    return collisions;
}

std::vector<CollisionEntity*> World::check_overlap(CollisionEntity* to_check)
{
    std::vector<CollisionEntity*> collisions;

    for (Solid* solid : solids) {
        if (overlap_aabb(solid, to_check)) {
            collisions.push_back(solid);
        }
    }

    return collisions;
}

//====================================================================

void World::init(PhysicsData* new_physics_data)
{
    physics_data = new_physics_data;
    camera.reset();
}

void World::update()
{
    for (Solid* solid : solids)
        solid->update(this);

    for (Actor* actor : actors)
        actor->update(this);

    camera.update(this);

    debug.update(this);
}

void World::fixed_update(float dt)
{
    for (Solid* solid : solids)
        solid->fixed_update(this, dt);

    for (Actor* actor : actors)
        actor->fixed_update(this, dt);
}

void World::render()
{
    BeginDrawing();
    ClearBackground(clear_color);

    BeginMode2D(camera.get_camera());
    render_2d_inner();
    debug.render_2d(this);
    EndMode2D();

    debug.render(this);

    EndDrawing();
}

void World::render_2d_inner()
{
    for (Solid* solid : solids)
        solid->render(this);

    for (Actor* actor : actors)
        actor->render(this);
}
