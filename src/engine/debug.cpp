#include "debug.hpp"

#include "../defs.hpp"
#include "../game/player.hpp"
#include "raylib.h"
#include "tools.hpp"
#include "ui.hpp"
#include "world.hpp"
#include <algorithm>
#include <cmath>
#include <magic_enum.hpp>

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
    case DebugMenu::LevelSelect:
        render_level_menu(world);
        return;
    case DebugMenu::Inspector:
        render_inspector_menu(world);
        return;
    case DebugMenu::Physics:
        render_physics_menu(world);
        return;
    case DebugMenu::Player:
        render_player_menu(world);
        return;
    case DebugMenu::Main:
        break;
    }

    GuiPanel(menu_rect, NULL);

    if (GuiButton({ menu_rect.x + 24, menu_rect.y + 24, 240, 48 }, "Level Select")) {
        build_level_menu(world);
        current_menu = DebugMenu::LevelSelect;
    }

    if (GuiButton({ menu_rect.x + 24, menu_rect.y + 96, 240, 48 }, "Inspector")) {
        build_inspector_menu(world);
        current_menu = DebugMenu::Inspector;
    }

    if (GuiButton({ menu_rect.x + 24, menu_rect.y + 168, 240, 48 }, "Physics")) {
        current_menu = DebugMenu::Physics;
    }

    if (GuiButton({ menu_rect.x + 24, menu_rect.y + 240, 240, 48 }, "Player")) {
        build_player_menu(world);
        current_menu = DebugMenu::Player;
    }
}

void Debugger::render_level_menu(World* world)
{
    if (GuiWindowBox(menu_rect, "Level Menu")) {
        current_menu = DebugMenu::Main;
    }

    GuiListView(
        { menu_rect.x + 8, menu_rect.y + 32, 274, 104 },
        levels_name_list.c_str(),
        &level_list_scroll_index,
        &level_list_active);

    if (GuiButton({ menu_rect.x + 48, menu_rect.y + 144, 192, 32 }, "Load")) {
        if (level_list_active >= 0 && level_list_active < levels.size()) {
            world->load_level(levels[level_list_active]);
        }
    }

    if (GuiButton({ menu_rect.x + 48, menu_rect.y + 192, 192, 32 }, "Refresh")) {
        build_level_menu(world);
    }

    GuiLine({ menu_rect.x + 8, menu_rect.y + 232, 272, 16 }, NULL);

    Rectangle text_box_rect = { menu_rect.x + 8, menu_rect.y + 256, 272, 48 };
    bool editing = CheckCollisionPointRec(GetMousePosition(), text_box_rect);

    GuiTextBox(text_box_rect, level_menu_name, 128, editing);
    if (GuiButton({ menu_rect.x + 48, menu_rect.y + 312, 192, 32 }, "Save Level")) {
        std::string name(level_menu_name);

        name.erase(
            std::remove_if(name.begin(), name.end(), ::isspace),
            name.end());

        if (!name.empty()) {
            world->save_level(name.c_str());
            build_level_menu(world);
        }
    }

    if (GuiButton({ menu_rect.x + 48, menu_rect.y + 360, 192, 32 }, "Clear Level")) {
        world->clear_level();
    }
}

void Debugger::build_level_menu(World* world)
{
    levels = world->get_levels();

    levels_name_list.clear();
    for (const char* level : levels) {
        levels_name_list.append(";").append(level);
    }

    // Remove leading semi-colon
    levels_name_list.erase(0, 1);
}

void Debugger::render_inspector_menu(World* world)
{
    // Window
    if (GuiWindowBox(menu_rect, "Inspector")) {
        current_menu = DebugMenu::Main;
    }

    //----------------------------------------------
    // Top, entity selection area

    GuiLabel({ menu_rect.x + 24, menu_rect.y + 32, 96, 24 }, "Entities");

    int inspector_list_active_previous = inspector_list_active;

    if (debug_entities.empty()) {
        GuiPanel({ menu_rect.x + 8, menu_rect.y + 64, 128, 128 }, NULL);
    } else {
        std::string entity_list = "";

        for (IDebug* actor : debug_entities) {
            entity_list.append(";");
            entity_list.append(actor->get_name());
        }

        entity_list.erase(0, 1);

        GuiListView(
            { menu_rect.x + 8, menu_rect.y + 64, 128, 128 },
            entity_list.c_str(),
            &inspector_list_scroll_index,
            &inspector_list_active);
    }

    if (GuiButton({ menu_rect.x + 152, menu_rect.y + 64, 120, 32 }, "Refresh")) {
        build_inspector_menu(world);
    }

    GuiLabel({ menu_rect.x + 152, menu_rect.y + 112, 120, 24 }, TextFormat("active = %d", inspector_list_active));

    // Check if a new entity has been selected
    const bool entity_changed = inspector_list_active_previous != inspector_list_active;

    //----------------------------------------------
    // Bottom property selection + access area

    GuiLine({ menu_rect.x + 8, menu_rect.y + 192, 264, 16 }, NULL);

    float value_panel_height = menu_rect.height - menu_rect.y - 208;
    float property_y = menu_rect.y + 208 + value_panel_height + 10;

    Rectangle property_list_rect = { menu_rect.x + 8, menu_rect.y + 208, 272, value_panel_height };

    // We don't have a valid entity
    if (inspector_list_active < 0 || inspector_list_active >= debug_entities.size()) {
        // Draw empty panel
        GuiPanel(property_list_rect, NULL);
    }

    // We have valid entity
    else {

        // We have a new entity
        // Reset values and get it's properties
        if (entity_changed) {
            entity_properties.clear();
            debug_entities[inspector_list_active]->get_properties(&entity_properties);

            entity_list_scroll_index = 0;
        }

        // no properties
        if (entity_properties.size() == 0) {
            // Draw empty panel
            GuiPanel(property_list_rect, NULL);
            return;
        }

        GuiPropertyListView(property_list_rect, entity_properties, &entity_list_scroll_index);
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

    {
        IDebug* debug_actor = dynamic_cast<IDebug*>(&world->camera);
        if (debug_actor)
            debug_entities.push_back(debug_actor);
    }
}

void Debugger::render_physics_menu(World* world)
{
    if (GuiWindowBox(menu_rect, "Physics Menu")) {
        current_menu = DebugMenu::Main;
    }

    PhysicsData* data = world->get_physics_data();

    GuiLabel({ menu_rect.x + 24, menu_rect.y + 48, 120, 24 }, "Elapsed");
    GuiLabel({ menu_rect.x + 24, menu_rect.y + 72, 120, 24 }, "Accumulator");
    GuiLabel({ menu_rect.x + 24, menu_rect.y + 96, 120, 24 }, "Timestep");

    GuiStatusBar({ menu_rect.x + 144, menu_rect.y + 48, 120, 24 }, std::to_string(data->elapsed).c_str());
    GuiStatusBar({ menu_rect.x + 144, menu_rect.y + 72, 120, 24 }, std::to_string(data->accumulator).c_str());
    GuiStatusBar({ menu_rect.x + 144, menu_rect.y + 96, 120, 24 }, std::to_string(data->timestep).c_str());

    GuiLabel({ menu_rect.x + 24, menu_rect.y + 144, 120, 24 }, "Frames per second");
    GuiLabel({ menu_rect.x + 24, menu_rect.y + 168, 120, 24 }, "Fixed updates per second");
    GuiLabel({ menu_rect.x + 24, menu_rect.y + 192, 120, 24 }, "Freeze fixed updates");

    Vector2 mouse_pos = GetMousePosition();
    Rectangle edit_rect = { menu_rect.x + 144, menu_rect.y + 144, 120, 24 };
    bool editing = false;

    // FPS
    int* fps = &data->fps;
    int old_fps = *fps;
    editing = CheckCollisionPointRec(mouse_pos, edit_rect);

    GuiSpinner(edit_rect, NULL, fps, 5, 255, editing);
    if (*fps != old_fps) {
        SetTargetFPS(*fps);
    }

    // Fixed FPS
    edit_rect.y = menu_rect.y + 168;
    int fixed_update = std::round(1.0f / data->timestep);
    int old_fixed_update = fixed_update;
    editing = CheckCollisionPointRec(mouse_pos, edit_rect);

    GuiSpinner(edit_rect, NULL, &fixed_update, 0, 255, editing);
    if (fixed_update != old_fixed_update)
        data->timestep = 1.0f / fixed_update;

    // Freeze
    edit_rect.y = menu_rect.y + 192;
    GuiCheckBox({ menu_rect.x + 144, menu_rect.y + 192, 24, 24 }, NULL, &data->freeze_fixed_update);
}

void Debugger::render_player_menu(World* world)
{
    if (GuiWindowBox(menu_rect, "Player Menu")) {
        current_menu = DebugMenu::Main;
    }

    if (GuiButton({ menu_rect.x + 24, menu_rect.y + 48, 120, 24 }, "Sync")) {
        build_player_menu(world);
    }

    if (GuiButton({ menu_rect.x + 24, menu_rect.y + 96, 120, 24 }, "Apply")) {
        // player_slot1.
        Player* player = world->get_player();
        // world->get_player()->player_character_index = 0;
        player->player_character_index = 0;

        player->player_characters.clear();

        for (auto val : player_slots) {
            PlayerType value = static_cast<PlayerType>(val);
            player->player_characters.push_back(value);
        }

        player->set_inner(player->player_characters[0]);
    }

    if (player_slots.size() > 1 && GuiButton({ menu_rect.x + 192, menu_rect.y + 144, 24, 24 }, "-")) {
        player_slots.pop_back();
    }
    if (player_slots.size() < 4 && GuiButton({ menu_rect.x + 240, menu_rect.y + 144, 24, 24 }, "+")) {
        player_slots.push_back(0);
    }

    Rectangle combo_rect = { menu_rect.x + 24, menu_rect.y + 144, 144, 24 };

    for (int& player_type : player_slots) {
        GuiComboBox(combo_rect, player_options.c_str(), &player_type);
        combo_rect.y += 48;
    }
}

void Debugger::build_player_menu(World* world)
{
    TraceLog(TraceLogLevel::LOG_INFO, "Rebuilding player debug");

    constexpr auto player_types = magic_enum::enum_names<PlayerType>();

    player_options.clear();
    for (auto type : player_types) {
        player_options.append(";").append(type);
    }
    player_options.erase(0, 1);

    TraceLog(TraceLogLevel::LOG_INFO, TextFormat("options: '%s'", player_options.c_str()));

    player_slots.clear();

    for (PlayerType val : world->get_player()->player_characters) {
        int value = static_cast<int>(val);
        player_slots.push_back(value);
    }
}

//====================================================================
// Level editor update and render

void Debugger::update_level_editor(World* world)
{
    if (!is_level_editor_enabled)
        return;

    world_mouse_pos = GetScreenToWorld2D(GetMousePosition(), world->camera.get_camera());

    snapped_mouse_x = round_to(world_mouse_pos.x, TILE_WIDTH);
    snapped_mouse_y = round_to(world_mouse_pos.y, TILE_HEIGHT);

    // Skip if mouse in menu
    if (is_debug_menu && CheckCollisionPointRec(GetMousePosition(), menu_rect))
        return;

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

    int origin_x = round_to(pos.x, TILE_WIDTH);
    int origin_y = round_to(pos.y, TILE_HEIGHT);

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

    DrawLineEx({ -999, 0 }, { 999, 0 }, 3, RED);
    DrawLineEx({ 0, -999 }, { 0, 999 }, 3, RED);
}

//====================================================================
