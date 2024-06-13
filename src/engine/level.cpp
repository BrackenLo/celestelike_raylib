#include "level.h"

#include "raylib.h"

Level::Level()
{
    background = RAYWHITE;
}

Level::~Level() { }

void Level::init()
{
    camera.reset();
}

void Level::update()
{
    for (Solid* solid : *world.get_solids()) {
        solid->update(&world);
    }

    for (Actor* actor : *world.get_actors()) {
        actor->update(&world);
    }

    camera.update(&world);
}

void Level::render()
{
    BeginDrawing();
    ClearBackground(background);

    BeginMode2D(camera.get_camera());
    render_2d_inner();
    EndMode2D();

    EndDrawing();
}

void Level::render_2d_inner()
{
    for (Solid* solid : *world.get_solids()) {
        solid->render();
    }

    for (Actor* actor : *world.get_actors()) {
        actor->render();
    }
}
