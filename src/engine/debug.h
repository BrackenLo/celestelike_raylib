#pragma once

#include "raylib.h"
#include <string>
#include <vector>

class Debugger {
public:
    inline void add_message(const char* text)
    {
        messages.push_back(std::string(text));
    }

private:
    void update(class World* world);
    void render_2d(class World* world);
    void render(class World* world);
    friend class World;

private:
    void render_log(World* world);

    void destroy_tile(World* world);

    void update_level_editor(class World* world);
    void render_level_editor(class World* world);

private:
    // Toggles
    bool is_log_enabled = false;
    bool is_level_editor_enabled = false;

    // Logging stuff
    std::vector<std::string> messages;

    // Level editor stuff
    Vector2 world_mouse_pos;
    int snapped_mouse_x;
    int snapped_mouse_y;
};
