#include "debug.h"

#include "../defs.h"
#include "raylib.h"
#include "world.h"
#include <cmath>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//====================================================================

Debugger::Debugger() { }

//====================================================================
// Access functions

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

//====================================================================
// Updating and rendering

void Debugger::update(World* world)
{
    if (IsKeyPressed(KEY_F1))
        is_log_enabled = !is_log_enabled;

    if (IsKeyPressed(KEY_F2))
        is_level_editor_enabled = !is_level_editor_enabled;

    if (IsKeyPressed(KEY_F3)) {
        resize_debug_menu();
        is_debug_menu = !is_debug_menu;
    }

    if (IsKeyPressed(KEY_F5)) {
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

    if (is_debug_menu)
        render_debug_menu(world);
}

//====================================================================
// Logging

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

//====================================================================
// Debug menu stuff

void Debugger::resize_debug_menu()
{
    const float width = GetScreenWidth();
    const float height = GetScreenHeight();

    menu_rect.width = menu_width;
    menu_rect.height = height - 20;

    menu_rect.x = width - menu_rect.width;
    menu_rect.y = 10;
}

void Debugger::render_debug_menu(World* world)
{
    if (IsWindowResized()) {
        resize_debug_menu();
    }

    switch (current_menu) {
    case LevelSelect:
        render_level_menu(world);
        return;
    case Inspector:
        build_inspector_menu(world);
        render_inspector_menu(world);
        return;
    case Main:
        break;
    }

    GuiPanel(menu_rect, NULL);

    if (GuiButton({ menu_rect.x + 24, menu_rect.y + 24, 240, 48 }, "Level Select")) {
        current_menu = DebugMenu::LevelSelect;
    }

    if (GuiButton({ menu_rect.x + 24, menu_rect.y + 96, 240, 48 }, "Inspector")) {
        current_menu = DebugMenu::Inspector;
    }
}

void Debugger::render_level_menu(World* world)
{
    if (GuiWindowBox(menu_rect, "Level Menu")) {
        current_menu = DebugMenu::Main;
    }

    GuiListView(
        { menu_rect.x + 8, menu_rect.y + 32, 274, 104 },
        "Todo",
        &level_list_scroll_index,
        &level_list_active);

    if (GuiButton({ menu_rect.x + 48, menu_rect.y + 144, 192, 32 }, "Load")) {
    }

    GuiLine({ menu_rect.x + 8, menu_rect.y + 184, 272, 16 }, NULL);
    GuiTextBox({ menu_rect.x + 8, menu_rect.y + 208, 272, 48 }, level_name, 128, true);
    if (GuiButton({ menu_rect.x + 48, menu_rect.y + 264, 182, 32 }, "Save Level")) {
    }
}

void Debugger::render_inspector_menu(World* world)
{
    if (GuiWindowBox(menu_rect, "Inspector")) {
        current_menu = DebugMenu::Main;
    }

    //----------------------------------------------

    GuiLabel({ menu_rect.x + 24, menu_rect.y + 32, 96, 24 }, "Entities");

    int result = 0;

    if (debug_entities.empty()) {
        GuiPanel({ menu_rect.x + 8, menu_rect.y + 64, 128, 128 }, NULL);
    } else {
        std::string entity_list = "";

        for (IDebug* actor : debug_entities) {
            entity_list.append(";");
            entity_list.append(actor->get_name());
        }

        entity_list.erase(0, 1);

        result = GuiListView(
            { menu_rect.x + 8, menu_rect.y + 64, 128, 128 },
            entity_list.c_str(),
            &inspector_list_scroll_index,
            &inspector_list_active);
    }

    if (GuiButton({ menu_rect.x + 152, menu_rect.y + 64, 120, 32 }, "Refresh")) {
        build_inspector_menu(world);
    }

    GuiLabel({ menu_rect.x + 152, menu_rect.y + 112, 120, 24 }, TextFormat("active = %d", inspector_list_active));
    GuiLabel({ menu_rect.x + 152, menu_rect.y + 144, 120, 24 }, TextFormat("enttiy active = %d", entity_list_active));
    GuiLabel({ menu_rect.x + 152, menu_rect.y + 168, 120, 24 }, TextFormat("Result = %d", result));

    const bool entity_changed = inspector_list_active_previous != inspector_list_active;
    inspector_list_active_previous = inspector_list_active;

    //----------------------------------------------

    GuiLine({ menu_rect.x + 8, menu_rect.y + 192, 264, 16 }, NULL);

    float tweak_panel_height = 120;
    float value_panel_height = menu_rect.height - menu_rect.y - 208 - tweak_panel_height - 10;
    float property_y = menu_rect.y + 208 + value_panel_height + 10;

    // If we have valid entity
    if (inspector_list_active >= 0 && inspector_list_active < debug_entities.size()) {

        if (entity_changed) {
            entity_properties = "";

            std::vector<std::string> properties = debug_entities[inspector_list_active]->get_properties();

            for (std::string val : properties) {
                entity_properties += ";" + val;
            }
            entity_properties.erase(0, 1);

            entity_list_scroll_index = 0;
            entity_list_active = -1;
            entity_list_active_previous = -1;

            entity_property_count = properties.size();
        }

        if (entity_properties == "") {
            GuiPanel({ menu_rect.x + 8, menu_rect.y + 208, 272, value_panel_height }, NULL);
            // Empty property panel
            GuiPanel({ menu_rect.x + 8, property_y, 272, tweak_panel_height }, NULL);
            return;
        }

        // Entity Properties
        GuiListView(
            { menu_rect.x + 8, menu_rect.y + 208, 272, value_panel_height },
            entity_properties.c_str(),
            &entity_list_scroll_index,
            &entity_list_active);

        const bool property_changed = entity_list_active != entity_list_active_previous;
        entity_list_active_previous = entity_list_active;

        // Empty property panel
        GuiPanel({ menu_rect.x + 8, property_y, 272, tweak_panel_height }, NULL);

        // Get new property data
        if (entity_list_active >= 0 && entity_list_active < entity_property_count) {
            if (property_changed) {
                debug_entities[inspector_list_active]->get_property_type(
                    entity_list_active,
                    &property_current_type,
                    property_current_pointer);
            }

            switch (property_current_type) {
            case None:
                break;
            case Boolean: {
                bool* toggle = reinterpret_cast<bool*>(property_current_pointer);

                GuiToggle({ menu_rect.x + 16, property_y + 10, 88, 24 }, "Toggle", toggle);
                break;
            }
            case String:
                break;
            case Float:
                break;
            }
        }

    } else {
        GuiPanel({ menu_rect.x + 8, menu_rect.y + 208, 272, value_panel_height }, NULL);
        // Empty property panel
        GuiPanel(
            {
                menu_rect.x + 8,
                property_y,
                272,
                tweak_panel_height //
            },
            NULL);
    }
}

void Debugger::build_inspector_menu(World* world)
{
    debug_entities.clear();

    for (Actor* actor : *world->get_actors()) {
        IDebug* debug_actor = dynamic_cast<IDebug*>(actor);
        if (!debug_actor)
            continue;

        debug_entities.push_back(debug_actor);
    }
}

//====================================================================
// Level editor update and render

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

//====================================================================
