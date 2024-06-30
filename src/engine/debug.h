#pragma once

#include "raylib.h"
#include <string>
#include <variant>
#include <vector>

enum class DebugMenu {
    Main,
    LevelSelect,
    Inspector,
    Physics,
};

// TODO - add spacing decorative variant + other variants
using PropertyType = std::variant<int*, bool*, float*, Vector2*>;

struct DebugProperty {
    const char* name;
    PropertyType property_type;
    bool can_edit = true;
    int min_val = -999;
    int max_val = 999;
};

class Debugger {
public:
    Debugger();
    void add_message(const char* text, int log);

private:
    void update(class World* world);
    void render_2d(class World* world);
    void render(class World* world);
    friend class World;

private:
    void render_log(std::vector<std::string>* messages, int index);

    void resize_debug_menu();
    void render_debug_menu(World* world);
    void render_level_menu(World* world);
    void build_level_menu(World* world);
    void render_inspector_menu(World* world);
    void build_inspector_menu(World* world);
    void render_physics_menu(World* world);

    void destroy_tile(World* world);

    void update_level_editor(class World* world);
    void render_level_editor(class World* world);

private:
    // Toggles
    bool is_log_enabled = false;
    bool is_level_editor_enabled = false;
    bool is_slow_mode = false;
    bool is_debug_menu = false;

    //----------------------------------------------
    // Debug Menu
    float menu_width = 288;
    Rectangle menu_rect = { 0, 0, 288, 0 };
    DebugMenu current_menu = DebugMenu::Main;

    // Debug Menu - Level Select
    int level_list_scroll_index = 0;
    int level_list_active = -1;
    std::vector<const char*> levels;
    std::string levels_name_list;
    char level_menu_name[128] = "Level Name";

    // Debug Menu - Inspector
    int inspector_list_scroll_index = 0;
    int inspector_list_active = -1;

    std::vector<class IDebug*> debug_entities;
    int entity_list_scroll_index = 0;

    std::vector<DebugProperty> entity_properties;

    //----------------------------------------------
    // Logging stuff
    std::vector<std::string> messages_0;
    std::vector<std::string> messages_1;
    std::vector<std::string> messages_2;

    //----------------------------------------------
    // Level editor stuff
    Vector2 world_mouse_pos;
    int snapped_mouse_x;
    int snapped_mouse_y;
};

class IDebug {
public:
    virtual const char* get_name() = 0;
    virtual void get_properties(std::vector<DebugProperty>* properties) = 0;
};
