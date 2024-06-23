#pragma once

#include "raylib.h"
#include <string>
#include <vector>

enum DebugMenu {
    Main,
    LevelSelect,
    Inspector,
};

enum PropertyType {
    None,
    Boolean,
    String,
    Float,
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
    void render_inspector_menu(World* world);
    void build_inspector_menu(World* world);

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
    char level_name[128] = "Level Name";

    // Debug Menu - Inspector
    int inspector_list_scroll_index = 0;
    int inspector_list_active = -1;
    int inspector_list_active_previous = -1;

    std::vector<class IDebug*> debug_entities;
    std::string entity_properties = "";
    int entity_property_count = 0;
    int entity_list_scroll_index = 0;
    int entity_list_active = -1;
    int entity_list_active_previous = -1;

    PropertyType property_current_type = PropertyType::None;
    void* property_current_pointer;

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
    virtual std::vector<std::string> get_properties() = 0;
    virtual void get_property_type(const int index, PropertyType* value_type, void* value) = 0;
};
