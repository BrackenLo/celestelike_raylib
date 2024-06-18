#include "debug.h"

#include "../defs.h"
#include "raygui.h"
#include "raylib.h"
#include "tools.h"
#include "world.h"

void Debugger::update(World* world)
{
    if (IsKeyPressed(KEY_F1))
        is_log_enabled = !is_log_enabled;

    if (IsKeyPressed(KEY_F2))
        is_level_editor_enabled = !is_level_editor_enabled;

    update_level_editor(world);
}

void Debugger::render_2d(World* world)
{
    render_level_editor(world);
}

void Debugger::render(World* world)
{
    render_log(world);
}

void Debugger::render_log(World* world)
{
    if (is_log_enabled) {
        for (int x = 0; x < messages.size(); x++) {
            DrawText(messages[x].c_str(), 20, 20 + 20 * x, 20, BLACK);
        };
    }

    messages.clear();
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
        BLUE);

    // Draw Grid
    Vector2 pos = GetScreenToWorld2D({ 0, 0 }, world->camera.get_camera());

    int origin_x = ((int)pos.x + TILE_WIDTH - 1) & -TILE_WIDTH;
    int origin_y = ((int)pos.y + TILE_HEIGHT - 1) & -TILE_HEIGHT;

    int width = GetScreenWidth();
    int height = GetScreenHeight();

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
