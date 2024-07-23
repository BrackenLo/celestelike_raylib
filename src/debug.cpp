#include "debug.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <magic_enum.hpp>
#include <raylib.h>

#include "components.hpp"
#include "save.hpp"

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

    ImGui::DragFloat("coyote_time", &val.coyote_time);
    ImGui::DragFloat("jump_buffer", &val.jump_buffer);
    ImGui::DragFloat("fall_multiplier", &val.fall_multiplier);

    ImGui::BeginDisabled(true);
    ImGui::Checkbox("coyote_usable", &val.coyote_usable);
    ImGui::Checkbox("buffered_jump_usable", &val.buffered_jump_usable);
    ImGui::Checkbox("ended_early", &val.ended_early);
    ImGui::EndDisabled();
}

template <>
void ComponentEditorWidget<celestelike::JumpData>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::JumpData>(e);

    ImGui::Text("Jumps used: %d", val.jumps_used);
    ImGui::Text("time_left_ground: %f", val.time_left_ground);
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

    if (reg.any_of<celestelike::CollisionBounds>(e) && ImGui::Button("Copy collision bounds")) {
        const celestelike::Bounds& bounds = reg.get<celestelike::CollisionBounds>(e);

        val.size.half_width = bounds.half_width;
        val.size.half_height = bounds.half_height;
    }
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

template <>
void ComponentEditorWidget<celestelike::player::PlayerCharacters>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::player::PlayerCharacters>(e);

    constexpr auto player_types = magic_enum::enum_names<celestelike::player::PlayerCharacterTypes>();

    auto& current_character_type = val.available_characters[val.current_character_index];
    int current_character_type_index = static_cast<int>(current_character_type);

    ImGui::Text("current character: %s", player_types[current_character_type_index].data());
    ImGui::Text("current_character_index: '%d'", val.current_character_index);

    for (int x = 0; x < val.available_characters.size(); x++) {
        auto& character_types = val.available_characters[x];

        int character_type_index = static_cast<int>(character_types);

        if (ImGui::BeginCombo(TextFormat("Character %d", x), player_types[character_type_index].data())) {

            for (int n = 0; n < player_types.size(); n++) {
                const bool is_selected = (character_type_index == n);

                if (ImGui::Selectable(player_types[n].data(), is_selected)) {
                    character_type_index = n;
                    character_types = static_cast<celestelike::player::PlayerCharacterTypes>(n);
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
    }

    if (val.available_characters.size() < 4) {
        if (ImGui::Button("+")) {
            val.available_characters.push_back(celestelike::player::PlayerCharacterTypes::Base);
        }
        ImGui::SameLine();
    }

    if (val.available_characters.size() > 1) {
        if (ImGui::Button("-")) {
            val.available_characters.pop_back();
        }
    }

    ImGui::Spacing();

    ImGui::Separator();

    ImGui::DragFloat("switch_character_cooldown", &val.switch_character_cooldown);
    ImGui::Text("switch_character_time: '%f'", val.switch_character_time);
}

template <>
void ComponentEditorWidget<celestelike::player::Glide>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& val = reg.get<celestelike::player::Glide>(e);
    ImGui::DragInt("fall_speed", &val.fall_speed);
    ImGui::DragInt("max_fall_speed", &val.max_fall_speed);
}

}

namespace celestelike {

namespace debug {

    DebugState::DebugState()
    {
        menu_active = false;
        width = 400;
        level_editor_active = false;
        level_grid_active = true;
        levels = save::get_levels();
        selected_level = -1;

        comp_list = { entt::type_hash<MM::Name>::value() };
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
        inspector.registerComponent<JumpData>("JumpData");
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

        inspector.registerComponent<player::PlayerCharacters>("Player Characters");
        inspector.registerComponent<player::Ability1>("Ability1");
        inspector.registerComponent<player::Ability2>("Ability2");
        inspector.registerComponent<player::Glide>("Glide");

        inspector.registerComponent<DisableGravity>("Disable Gravity");
        inspector.registerComponent<DisableMovement>("Disable Movement");
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
