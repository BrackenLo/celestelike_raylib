#include "world.h"

#include "entity.h"

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

void World::init()
{
    camera.reset();
}

void World::update()
{
    for (Solid* solid : solids)
        solid->update(this);

    for (Actor* actor : actors)
        actor->update(this);

    camera.update(this);
}

void World::render()
{
    BeginDrawing();
    ClearBackground(clear_color);

    BeginMode2D(camera.get_camera());
    render_2d_inner();
    EndMode2D();

    EndDrawing();
}

void World::render_2d_inner()
{
    for (Solid* solid : solids)
        solid->render();

    for (Actor* actor : actors)
        actor->render();
}
