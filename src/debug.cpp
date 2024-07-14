#include "debug.hpp"

#include "components.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "raylib.h"

namespace MM {

template <>
void ComponentEditorWidget<celestelike::Pos>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::Pos>(e);
    ImGui::InputInt2("pos", (int*)&val);
    ImGui::DragInt("x", &val.x);
    ImGui::DragInt("y", &val.y);
}

template <>
void ComponentEditorWidget<celestelike::Velocity>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::Velocity>(e);
    ImGui::DragInt("x", &val.x);
    ImGui::DragInt("y", &val.y);
}

template <>
void ComponentEditorWidget<celestelike::WalkSpeed>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::WalkSpeed>(e);
    ImGui::DragInt("accel", &val.accel);
    ImGui::DragInt("deaccel", &val.deaccel);
    ImGui::DragInt("max_walk_speed", &val.max_walk_speed);
}

template <>
void ComponentEditorWidget<celestelike::Jump>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::Jump>(e);
    ImGui::DragInt("impulse", &val.impulse);
}

template <>
void ComponentEditorWidget<celestelike::Gravity>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::Gravity>(e);
    ImGui::DragInt("fall_speed", &val.fall_speed);
    ImGui::DragInt("max_fall_speed", &val.max_fall_speed);
}

template <>
void ComponentEditorWidget<celestelike::CollisionBounds>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::CollisionBounds>(e);
    ImGui::DragInt("half_width", &val.bounds.half_width);
    ImGui::DragInt("half_height", &val.bounds.half_height);
}

template <>
void ComponentEditorWidget<celestelike::Render>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::Render>(e);
    ImGui::Checkbox("visible", &val.visible);
}

template <>
void ComponentEditorWidget<celestelike::Sprite>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::Sprite>(e);

    float col[4] = {
        static_cast<float>(val.color.r) / 255,
        static_cast<float>(val.color.g) / 255,
        static_cast<float>(val.color.b) / 255,
        static_cast<float>(val.color.a) / 255
    };

    if (ImGui::ColorEdit4("color", col)) {
        val.color.r = static_cast<unsigned char>(col[0] * 255);
        val.color.g = static_cast<unsigned char>(col[1] * 255);
        val.color.b = static_cast<unsigned char>(col[2] * 255);
        val.color.a = static_cast<unsigned char>(col[3] * 255);
    }

    ImGui::DragInt("half_width", &val.size.half_width);
    ImGui::DragInt("half_height", &val.size.half_height);
}

template <>
void ComponentEditorWidget<celestelike::PosLerp>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::PosLerp>(e);
    ImGui::DragInt2("target", (int*)&val.target);
    ImGui::DragFloat("speed_x", &val.speed_x);
    ImGui::DragFloat("speed_y", &val.speed_y);
}

template <>
void ComponentEditorWidget<celestelike::GameCamera>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::GameCamera>(e);
    // ImGui::DragInt("val", &val.val);

    ImGui::DragFloat2("Camera Offset", (float*)&val.camera.offset);
    ImGui::DragFloat2("Camera Target", (float*)&val.camera.target);

    ImGui::DragFloat("Camera Rotation", &val.camera.rotation);
    ImGui::DragFloat("Camera Zoom", &val.camera.zoom);

    ImGui::Separator();
    ImGui::DragFloat("zoom_target", &val.zoom_target);
    ImGui::DragFloat("zoom_speed", &val.zoom_speed);
}

template <>
void ComponentEditorWidget<Name>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<Name>(e);
    ImGui::InputText("Name", &val.name);
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
        inspector.registerComponent<Actor>("Actor");
        inspector.registerComponent<Player>("Player");
        inspector.registerComponent<Solid>("Solid");

        inspector.registerComponent<Pos>("Pos");
        inspector.registerComponent<Velocity>("Velocity");
        inspector.registerComponent<WalkSpeed>("Walk Speed");
        inspector.registerComponent<Jump>("Jump");
        inspector.registerComponent<Gravity>("Gravity");
        inspector.registerComponent<CollisionBounds>("Collision Bounds");

        inspector.registerComponent<OnGround>("OnGround");
        inspector.registerComponent<OnCeiling>("OnCeiling");

        inspector.registerComponent<Render>("Render");
        inspector.registerComponent<Sprite>("Sprite");

        inspector.registerComponent<PosLerp>("PosLerp");
        inspector.registerComponent<GameCamera>("Game Camera");
        inspector.registerComponent<CameraTarget>("Camera Target");

        inspector.registerComponent<MM::Name>("Name");
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
