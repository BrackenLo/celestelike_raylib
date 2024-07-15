#pragma once

#include "imgui_entt_entity_editor.hpp"

namespace celestelike {

namespace debug {

    struct DebugInspect {
        const char* name;
    };

    struct DebugState {
        bool menu_active;

        float x;
        float y;
        float width;
        float height;

        bool level_editor_active;
        bool level_grid_active;

        MM::EntityEditor<entt::entity> inspector;
        std::set<entt::id_type> comp_list;
        entt::entity highlight_entity;

        DebugState();
        void init();
        void resize();
    };

}
}
