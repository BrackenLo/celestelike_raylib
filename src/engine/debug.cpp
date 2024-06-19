#include "debug.h"

#include "../defs.h"
#include "raygui.h"
#include "raylib.h"
#include "world.h"
#include <cmath>

void Debugger::add_message(const char* text, int log)
{
    switch (log) {
    case 0:
        messages_0.push_back(std::string(text));
        break;
    case 1:
        messages_1.push_back(std::string(text));
        break;
    default:
        messages_2.push_back(std::string(text));
    }
}

void Debugger::update(World* world)
{
    if (IsKeyPressed(KEY_F1))
        is_log_enabled = !is_log_enabled;

    if (IsKeyPressed(KEY_F2))
        is_level_editor_enabled = !is_level_editor_enabled;

    if (IsKeyPressed(KEY_F4)) {
        is_slow_mode = !is_slow_mode;
        world->get_physics_data()->timestep = //
            is_slow_mode ? 1.0f / 30.0f : 1.0f / 60.0f;
    }

    // Zoom in
    if (IsKeyDown(KEY_COMMA))
        world->camera.zoom_target = fmin(world->camera.zoom_target + 0.2, 20.0);
    // Zoom out
    if (IsKeyDown(KEY_PERIOD))
        world->camera.zoom_target = fmax(world->camera.zoom_target - 0.2, 0.2);

    update_level_editor(world);
}

void Debugger::render_2d(World* world)
{
    render_level_editor(world);
}

void Debugger::render(World* world)
{
    if (is_log_enabled) {

        auto data = world->get_physics_data();
        const char* elapsed = TextFormat("elapsed: %f", data->elapsed);
        const char* fps = TextFormat("FPS: %d", GetFPS());
        const char* timestep = TextFormat("Timestep: %f", data->timestep);
        const char* accumulator = TextFormat("Accumulator: %f", data->accumulator);

        std::vector<std::string> temp {
            std::string(elapsed),
            std::string(fps),
            std::string(timestep),
            std::string(accumulator)
        };

        messages_0.insert(messages_0.begin(), temp.begin(), temp.end());

        render_log(&messages_0, 0);
        render_log(&messages_1, 1);
        render_log(&messages_2, 2);
    }
}

void Debugger::render_log(std::vector<std::string>* messages, int index)
{
    if (messages->empty())
        return;

    const int spacing = 20;
    const int box_width = 500;
    int box_height = spacing * messages->size() + 20;

    int start_x = 20 + (box_width + spacing) * index;
    const int start_y = 20;

    int box_start_x = start_x - 10;
    const int box_start_y = start_y - 10;

    Color color;
    switch (index) {
    case 0:
        color = SKYBLUE;
        break;
    case 1:
        color = RED;
        break;
    default:
        color = GREEN;
    }

    DrawRectangle(box_start_x, box_start_y, box_width, box_height, Fade(color, 0.6f));
    DrawRectangleLines(box_start_x, box_start_y, box_width, box_height, color);

    for (int x = 0; x < messages->size(); x++) {
        DrawText(
            (*messages)[x].c_str(),
            start_x,
            start_y + spacing * x,
            20,
            BLACK);
    };

    messages->clear();
}

void Debugger::update_level_editor(World* world)
{
    if (!is_level_editor_enabled)
        return;

    world_mouse_pos = GetScreenToWorld2D(GetMousePosition(), world->camera.get_camera());

    snapped_mouse_x = (((int)world_mouse_pos.x + TILE_WIDTH - 1) & -TILE_WIDTH) - TILE_WIDTH;
    snapped_mouse_y = (((int)world_mouse_pos.y + TILE_HEIGHT - 1) & -TILE_HEIGHT) - TILE_HEIGHT;

    bool mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    bool mouse_right = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

    if (mouse_left || mouse_right) {

        int half_width = TILE_WIDTH / 2;
        int half_height = TILE_HEIGHT / 2;

        Solid* new_tile = new Solid(
            { (float)snapped_mouse_x + half_width, (float)snapped_mouse_y + half_height },
            half_width,
            half_height);

        for (Collision collision : world->check_collision(new_tile)) {
            auto* derived = dynamic_cast<Solid*>(collision.entity);
            if (derived)
                world->destroy_solid(derived);
        }

        // add tile if addition
        if (mouse_left)
            world->add_solid(new_tile);

        // delete temp tile if removal
        else
            delete new_tile;
    }
}

void Debugger::render_level_editor(World* world)
{
    if (!is_level_editor_enabled)
        return;

    // Draw mouse tile
    DrawRectangle(
        snapped_mouse_x,
        snapped_mouse_y,
        TILE_WIDTH,
        TILE_HEIGHT,
        Fade(BLUE, 0.4));

    // Draw Grid
    Vector2 pos = GetScreenToWorld2D({ 0, 0 }, world->camera.get_camera());

    int origin_x = ((int)pos.x + TILE_WIDTH - 1) & -TILE_WIDTH - TILE_WIDTH;
    int origin_y = ((int)pos.y + TILE_HEIGHT - 1) & -TILE_HEIGHT - TILE_HEIGHT;

    int width = GetScreenWidth() + 200;
    int height = GetScreenHeight() + 200;

    int end_x = origin_x + width;
    int end_y = origin_y + height;

    int x_amount = width / TILE_WIDTH;
    int y_amount = height / TILE_HEIGHT;

    for (int x = 0; x < x_amount; x++) {
        int start_x = origin_x + TILE_WIDTH * x;
        DrawLine(start_x, origin_y, start_x, end_y, BLACK);
    }

    for (int y = 0; y < y_amount; y++) {
        int start_y = origin_y + TILE_HEIGHT * y;
        DrawLine(origin_x, start_y, end_x, start_y, BLACK);
    }
}
