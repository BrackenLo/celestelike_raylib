#include "debug.hpp"

#include "components.hpp"
#include "imgui.h"
#include "raylib.h"

namespace MM {

template <>
void ComponentEditorWidget<celestelike::Pos>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::Pos>(e);
    ImGui::DragInt("x", &val.x);
    ImGui::DragInt("y", &val.y);
}

}

namespace celestelike {

namespace debug {

    DebugState::DebugState()
    {
        active = false;
        width = 320;
    }

    void DebugState::init()
    {
        inspector.registerComponent<Pos>("Pos");
    }

    void DebugState::resize()
    {
        const float screen_width = GetScreenWidth();
        const float screen_height = GetScreenHeight();

        height = screen_height;

        x = screen_width - width;
        y = 0;
    }
}

}
