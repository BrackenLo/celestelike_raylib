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

#ifdef DEBUG
    debug.render();
#endif

    EndDrawing();
}

void World::render_2d_inner()
{
    for (Solid* solid : solids)
        solid->render(this);

    for (Actor* actor : actors)
        actor->render(this);
}
