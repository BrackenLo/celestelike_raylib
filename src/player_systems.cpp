#include "player_systems.hpp"

#include "components.hpp"
#include "player_resources.hpp"
#include "tools.hpp"

void PlayerSystem::init(entt::registry& reg)
{
    reg.ctx().emplace<PlayerInput>();
}

void PlayerSystem::run(entt::registry& reg)
{
    float dt = 1.0f / 60.0f;

    update_player_input(reg);
    update_player_velocity(reg, dt);
}

void update_player_input(entt::registry& reg)
{
    PlayerInput& input = reg.ctx().get<PlayerInput>();

    bool right_down = are_keys_down(input.key_right);
    bool left_down = are_keys_down(input.key_left);
    input.x_dir = right_down - left_down;

    bool up_down = are_keys_down(input.key_up);
    bool down_down = are_keys_down(input.key_down);
    input.y_dir = up_down - down_down;

    input.jump_held = are_keys_down(input.key_jump);
    input.jump_pressed = input.jump_pressed || are_keys_pressed(input.key_jump);

    input.ability_1_pressed = input.ability_1_pressed || are_keys_pressed(input.key_ability_1);
    input.ability_2_pressed = input.ability_2_pressed || are_keys_pressed(input.key_ability_2);
    input.ability_3_pressed = input.ability_3_pressed || are_keys_pressed(input.key_ability_3);

    input.ability_1_down = are_keys_down(input.key_ability_1);
    input.ability_2_down = are_keys_down(input.key_ability_2);
}

void update_player_velocity(entt::registry& reg, float dt)
{
    using namespace celestelike;

    const PlayerInput& input = reg.ctx().get<PlayerInput>();
    auto players = reg.view<Player, Pos, Velocity, WalkSpeed>();

    for (const entt::entity entity : players) {
        Pos& pos = players.get<Pos>(entity);
        Velocity& velocity = players.get<Velocity>(entity);
        WalkSpeed& walk_speed = players.get<WalkSpeed>(entity);

        int target_velocity = input.x_dir * walk_speed.max_walk_speed;
        int speed;

        if (input.x_dir == 0)
            speed = walk_speed.deaccel;
        else {
            int target_sign = std::copysign(1, target_velocity);
            int current_sign = std::copysign(1, velocity.x);

            speed = target_sign == current_sign ? walk_speed.accel : walk_speed.deaccel;
        }

        velocity.x = step(velocity.x, target_velocity, speed * dt);
    }
}
