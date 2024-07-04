#include "player_inner_base.hpp"

#include "../engine/physics.hpp"
#include "../engine/tools.hpp"
#include "../engine/world.hpp"
#include "player.hpp"
#include "raymath.h"

#include <algorithm>
#include <cmath>

//====================================================================

PlayerInner::PlayerInner(Player* outer)
{
    this->outer = outer;

    player_type = PlayerType::Base;

    outer->half_width = 25;
    outer->half_height = 32;

    player_color_1 = RED;
    player_color_2 = ORANGE;

    accel = 2200.0f;
    deaccel = 2600.0f;

    max_velocity_x = 400.0f;
    max_fall_speed = 800.0f;

    jump_buffer_size = 0.1;
    coyote_time = 0.09f;

    jump_height = -60.0f;
    jump_time_to_peak = 0.25f;
    jump_time_to_descent = 0.2f;
    variable_jump_height = -150.0f;
    total_jumps = 1;

    wall_slide_gravity = 300.0f;
    wall_jump_impulse_x = 500.0f;
    wall_jump_control_timer_size = 0.6f;

    update_jump_variables();
}

void PlayerInner::update(World* world)
{
}

void PlayerInner::walk(World* world, float dt)
{
    float target_velocity = outer->input_dir.x * max_velocity_x; // TEST
    float speed;

    // Do deacceleration if no player input
    if (outer->input_dir.x == 0.0f) // TEST
        speed = deaccel;

    // Do Acceleration
    else {
        float target_sign = std::copysign(1.0f, target_velocity);
        float current_sign = std::copysign(1.0f, outer->velocity.x); // TEST

        // TODO - use accel speed when velocity == 0
        speed = (target_sign == current_sign) ? accel : deaccel;
    }

    if (outer->wall_jump_control_timer > 0.0f) {
        float progress = 1.0f - outer->wall_jump_control_timer / wall_jump_control_timer_size;
        speed *= progress;
    }

    // TODO - only clamp like this on floor?
    outer->velocity.x = step(outer->velocity.x, target_velocity, speed * dt);
}

void PlayerInner::fixed_update(World* world, float dt)
{
    walk(world, dt);

    if (outer->jump_pressed) {
        auto val = check_wall_jump(world, dt);

        if (!outer->grounded && val.has_value())
            do_wall_jump(world, dt, val.value());

        else if (check_can_jump(world, dt))
            do_jump(world, dt);
    }

    // If jumping, check end jump conditions
    if (outer->jumping && (!outer->jump_held || outer->velocity.y > 0.1))
        outer->jumping = false;

    // Apply and step gravity towards max_fall_speed
    outer->velocity.y = step(
        outer->velocity.y,
        max_fall_speed,
        get_gravity(world) * dt);

    if (outer->ability_1_pressed) {
        do_ability1(world, dt);
    }
}

void PlayerInner::render(World* world)
{
    DrawRectangleGradientEx(
        outer->get_rect(),
        player_color_1, player_color_2, player_color_2, player_color_1);
}

bool PlayerInner::check_can_jump(World* world, float dt)
{
    // Check if has all jumps
    bool first_jump = outer->remaining_jumps == total_jumps;
    // Check on ground or coyote time
    bool has_ground = outer->grounded || outer->time_since_grounded < coyote_time;
    // If falling and didn't jump, remove first jump
    if (first_jump && !has_ground) {
        outer->remaining_jumps -= 1;
        first_jump = false;
    }
    // Check if has jumps and is on ground with first jump or it's not the first jump
    bool can_jump = outer->remaining_jumps > 0 && ((first_jump && has_ground) || !first_jump);

    return can_jump;
}

void PlayerInner::do_jump(World* world, float dt)
{
    outer->jump_pressed = false;
    outer->jump_buffer = 0.0f;
    outer->remaining_jumps -= 1;

    outer->jumping = true;
    outer->velocity.y = jump_impulse;
}

std::optional<int> PlayerInner::check_wall_jump(World* world, float dt)
{
    CollisionEntity to_check = CollisionEntity(outer->pos, outer->half_width + 4, outer->half_height + 4); // TEST
    std::vector<Collision> collisions = world->check_collision(&to_check);

    bool left_collision = false;
    bool right_collision = false;

    for (Collision collision : collisions) {
        if (collision.normal.x == 1.0f)
            left_collision = true;
        if (collision.normal.x == -1.0f)
            right_collision = true;
    }

    if (left_collision || right_collision)
        return std::optional(right_collision - left_collision);

    return std::nullopt;
}

void PlayerInner::do_wall_jump(World* world, float dt, int direction)
{
    direction = std::clamp(direction, -1, 1);
    float x_mul = static_cast<float>(direction);

    Vector2 jump_direction = { 0.5f * x_mul, 0.5 };

    outer->velocity = Vector2Scale(jump_direction, jump_impulse * 1.8);

    outer->jump_buffer = 0.0f;
    outer->jumping = true;
    outer->wall_jump_control_timer = wall_jump_control_timer_size;
}

void PlayerInner::on_grounded(World* world, float dt)
{
    // velocity.y = fmin(velocity.y, 0.0f); // TODO - Remove this functionaltiy from outer
    // remaining_jumps = total_jumps;
    // time_since_grounded = 0.0f;
}

void PlayerInner::on_ceiling(World* world, float dt)
{
    outer->velocity.y = step(outer->velocity.y, 0.0f, fall_gravity * dt * 0.5);
}

void PlayerInner::on_wall(World* world, float dt)
{
    outer->velocity.x = step(outer->velocity.x, 0.0f, deaccel * dt);
}

void PlayerInner::do_ability1(World* world, float dt)
{
    TraceLog(TraceLogLevel::LOG_DEBUG, "Empty ability 1 fired");
}
void PlayerInner::do_ability2(World* world, float dt)
{
    TraceLog(TraceLogLevel::LOG_DEBUG, "Empty ability 2 fired");
}

void PlayerInner::update_jump_variables()
{
    jump_impulse = (2.0f * jump_height) / jump_time_to_peak;
    jump_gravity = (-2.0f * jump_height) / (jump_time_to_peak * jump_time_to_peak);
    fall_gravity = (-2.0f * jump_height) / (jump_time_to_descent * jump_time_to_descent);

    variable_jump_gravity = ((jump_impulse * jump_impulse) / (2.0f * variable_jump_height)) * -1.0f;
}

float PlayerInner::get_gravity(World* world)
{
    if (outer->on_wall && outer->velocity.y > 0.0f)
        return wall_slide_gravity;

    // Variable Jump Gravity
    if (outer->jumping)
        return variable_jump_gravity;

    // Jump Gravity
    if (outer->velocity.y < 0.0f)
        return jump_gravity;

    // Falling Gravity
    return fall_gravity;
}

void PlayerInner::get_properties(std::vector<DebugProperty>* properties)
{
    properties->push_back({ "accel", &accel, true, 0, 99999 });
    properties->push_back({ "deaccel", &deaccel, true, 0, 99999 });
    properties->push_back({ "max_velocity_x", &max_velocity_x, true, 0, 99999 });
    properties->push_back({ "max_fall_speed", &max_fall_speed, true, 0, 99999 });

    properties->push_back({ "jump_impulse", &jump_impulse, true, -9999, 0 });
    properties->push_back({ "jump_gravity", &jump_gravity, false });
    properties->push_back({ "fall_gravity", &fall_gravity, false });
    properties->push_back({ "total_jumps", &total_jumps, true });
}

//====================================================================
