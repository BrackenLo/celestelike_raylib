#include "player_inner_characters.hpp"

#include "player.hpp"
#include "raymath.h"

//====================================================================

DebugPlayerInner::DebugPlayerInner(Player* outer)
    : PlayerInner(outer)
{
    outer->half_width = 0;
    outer->half_height = 0;

    accel = 1000.0f;
}

void DebugPlayerInner::fixed_update(World* world, float dt)
{
    walk(world, dt);
}

void DebugPlayerInner::walk(World* world, float dt)
{
    Vector2 input_dir = Vector2Normalize(outer->input_dir);
    outer->velocity = Vector2Scale(input_dir, accel);
}

void DebugPlayerInner::render(World* world)
{
    DrawRectangle(outer->pos.x - 16, outer->pos.y - 16, 32, 32, BLUE);
}

//====================================================================

AvianPlayerInner::AvianPlayerInner(Player* outer)
    : PlayerInner(outer)
{
    player_type = PlayerType::Avian;

    outer->half_width = 20;
    outer->half_height = 36;

    player_color_1 = GREEN;

    accel = 2400.0f;
    deaccel = 2000.0f;

    max_velocity_x = 450.0f;
    max_fall_speed = 700.0f;

    jump_height = -60.0f; // Height without holding space
    jump_time_to_peak = 0.3f;
    jump_time_to_descent = 0.2f;
    variable_jump_height = -200.0f;
    total_jumps = 2;

    default_max_fall_speed = max_fall_speed;
    glide_max_fall_speed = 300.0f;

    update_jump_variables();
}

void AvianPlayerInner::update(World* world)
{
    PlayerInner::update(world);

    if (outer->jump_held && !outer->grounded) {
        max_fall_speed = glide_max_fall_speed;
    } else {
        max_fall_speed = default_max_fall_speed;
    }
}

//====================================================================

CelestePlayerInner::CelestePlayerInner(Player* outer)
    : PlayerInner(outer)
{
    player_color_1 = RED;
    player_color_2 = BLUE;

    dash_cooldown = 0.0f;
    dash_cooldown_size = 0.2f;

    dashes = dashes_count = 1;

    dash_power = 800;

    default_max_velocity_x = max_velocity_x;
}

void CelestePlayerInner::do_ability1(World* world, float dt)
{
    if (dash_cooldown > 0.0f || dashes <= 0) {
        return;
    }

    Vector2 input = outer->input_dir;
    if (input.x == 0.0f && input.y == 0.0f)
        input.x = 1;

    Vector2 dash_velocity = Vector2Scale(Vector2Normalize(input), dash_power);

    outer->velocity = dash_velocity;

    dash_cooldown = dash_cooldown_size;
    dashes -= 1;

    player_color_1 = BLUE;

    max_velocity_x = dash_power;
}

void CelestePlayerInner::do_ability2(World* world, float dt)
{
}

void CelestePlayerInner::on_grounded(World* world, float dt)
{
    PlayerInner::on_grounded(world, dt);

    dashes = dashes_count;
    player_color_1 = RED;
}

void CelestePlayerInner::fixed_update(World* world, float dt)
{
    PlayerInner::fixed_update(world, dt);

    if (dash_cooldown > 0.0f) {
        dash_cooldown -= dt;
        if (dash_cooldown <= 0.0f) {
            max_velocity_x = default_max_velocity_x;
        }
    }
}

float CelestePlayerInner::get_gravity(World* world)
{
    if (dash_cooldown > 0.0f)
        return 0.0f;

    return PlayerInner::get_gravity(world);
}

//====================================================================
