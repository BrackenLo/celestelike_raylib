#include "player.h"

#include "../engine/tools.h"
#include "../engine/world.h"
#include "raymath.h"
#include <cmath>
#include <memory>
#include <raylib.h>

//====================================================================

Player::Player()
{
    TraceLog(TraceLogLevel::LOG_INFO, "Creating player");

    half_width = 25;
    half_height = 32;

    update_jump_variables();
}

Player::Player(Vector2 new_pos)
    : Player()
{
    pos = new_pos;
}

void Player::update_jump_variables()
{
    jump_impulse = (2.0f * jump_height) / jump_time_to_peak;
    jump_gravity = (-2.0f * jump_height) / (jump_time_to_peak * jump_time_to_peak);
    fall_gravity = (-2.0f * jump_height) / (jump_time_to_descent * jump_time_to_descent);
    variable_jump_gravity = ((jump_impulse * jump_impulse) / (2.0f * variable_jump_height)) * -1.0f;
}

//====================================================================

void Player::update(World* world)
{
    player_input();
}

void Player::fixed_update(World* world, float dt)
{
    update_velocity(world, dt);

    resolve_collisions(world, dt);

    if (!grounded)
        time_since_grounded += dt;
}

void Player::player_input()
{
    float delta = GetFrameTime();

    input_dir = { 0 };

    // Get movement directions
    // Left and right
    if (are_keys_down(key_left))
        input_dir.x -= 1.0f;
    if (are_keys_down(key_right))
        input_dir.x += 1.0f;

    // Up and down
    if (are_keys_down(key_up))
        input_dir.y -= 1.0f;
    if (are_keys_down(key_down))
        input_dir.y += 1.0f;

    jump_held = are_keys_down(key_jump);

    // Tick jump buffer
    if (jump_pressed) {
        jump_buffer -= delta;
        if (jump_buffer <= 0.0f) {
            jump_pressed = false;
            jump_buffer = 0.0f;
        }
    }

    // Start jump buffer
    if (are_keys_pressed(key_jump)) {
        jump_pressed = true;
        jump_buffer = jump_buffer_size;
    }
}

void Player::update_velocity(World* world, float dt)
{
    //----------------------------------------------
    // Update X Velocity

    float target_velocity = input_dir.x * max_velocity_x;
    float speed;

    // Do deacceleration if no player input
    if (input_dir.x == 0.0f)
        speed = deaccel;

    // Do Acceleration
    else {
        float target_sign = std::copysign(1.0f, target_velocity);
        float current_sign = std::copysign(1.0f, velocity.x);

        // TODO - use accel speed when velocity == 0
        speed = (target_sign == current_sign) ? accel : deaccel;
    }

    velocity.x = step(velocity.x, target_velocity, speed * dt);

    //----------------------------------------------
    // Wall jump

    bool is_wall_jump = false;

    if (!grounded && jump_pressed) {
        CollisionEntity to_check = CollisionEntity(pos, half_width + 4, half_height + 4);
        std::vector<Collision> collisions = world->check_collision(&to_check);

        bool left_collision = false;
        bool right_collision = false;

        for (Collision collision : collisions) {
            if (collision.normal.x == 1.0f)
                left_collision = true;
            if (collision.normal.x == -1.0f)
                right_collision = true;
        }

        if (left_collision || right_collision) {
            is_wall_jump = true;
            jump_buffer = 0.0f;

            float x_mul;

            if (left_collision && right_collision)
                x_mul = 0.;
            else
                x_mul = left_collision ? 1.0f : -1.0f;

            velocity.x = wall_jump_impulse_x * x_mul;
            velocity.y = jump_impulse;
            jumping = true;
        }
    }

    //----------------------------------------------
    // Update Y Velocity

    // Start jumping
    if (jump_pressed && !is_wall_jump) {

        bool first_jump = remaining_jumps == total_jumps;
        bool has_ground = grounded || time_since_grounded < coyote_time;
        bool can_jump = remaining_jumps > 0 && ((first_jump && has_ground) || !first_jump);

        if (can_jump) {
            jump_pressed = false;
            jump_buffer = 0.0f;
            remaining_jumps -= 1;

            jumping = true;
            velocity.y = jump_impulse;
        }
    }

    // If jumping, check end jump conditions
    if (jumping && (!jump_held || velocity.y > 0.1))
        jumping = false;

    // Apply and clamp gravity
    velocity.y += get_gravity() * dt;
    velocity.y = std::fmin(velocity.y, max_fall_speed);
}

float Player::get_gravity()
{
    if (on_wall && velocity.y > 0.0f)
        return wall_slide_gravity;

    // Variable Jump Gravity
    if (jumping)
        return variable_jump_gravity;

    // Jump Gravity
    if (velocity.y < 0.0f)
        return jump_gravity;

    // Falling Gravity
    return fall_gravity;
}

void Player::resolve_collisions(World* world, float dt)
{
    grounded = false;
    on_ceiling = false;
    on_wall = false;

    old_pos = pos;

    //
    //----------------------------------------------

    Vector2 pos_to_move = Vector2Scale(velocity, dt);
    pos_to_move.x = round(pos_to_move.x);
    pos_to_move.y = round(pos_to_move.y);

    // int steps = Vector2Length(pos_to_move) / 10; // maybe???
    int sub_steps = 4;

    Vector2 pos_to_move_step = Vector2Scale(pos_to_move, 1.0f / sub_steps);

    world->log(TextFormat("step = (%f, %f)", pos_to_move.x, pos_to_move.y), 1);
    world->log(TextFormat("sub-step = (%f, %f)", pos_to_move_step.x, pos_to_move_step.y), 1);

    //----------------------------------------------

    for (int step = 0; step < sub_steps; step++) {

        pos.x += pos_to_move_step.x;

        float left_nudge = 0.0f;
        float right_nudge = 0.0f;

        for (CollisionEntity* solid : world->check_overlap(this)) {
            // TODO - optimise and reduce this
            float dx = pos.x - solid->pos.x;
            float px = (half_width + solid->half_width) - std::abs(dx);
            float sx = std::copysign(1.0f, dx);
            float depth = px * sx;

            // Colliding from the right
            if (depth > 0) {
                right_nudge = fmax(right_nudge, depth);
            }
            // Colliding from the left
            else {
                left_nudge = fmin(left_nudge, depth);
            }
        }

        bool hit_left = left_nudge != 0.0f;
        bool hit_right = right_nudge != 0.0f;

        if (hit_left || hit_right) {
            // TODO - check squish

            world->log("X Collision", 1);

            on_wall = true;

            pos.x += left_nudge + right_nudge;
        }

        //----------------------------------------------

        pos.y += pos_to_move_step.y;

        float up_nudge = 0.0f;
        float down_nudge = 0.0f;

        for (CollisionEntity* solid : world->check_overlap(this)) {
            // TODO - optimise and reduce this
            float dy = pos.y - solid->pos.y;
            float py = (half_height + solid->half_height) - std::abs(dy);
            float sy = std::copysign(1.0f, dy);
            float depth = py * sy;

            // Colliding from below
            if (depth > 0) {
                down_nudge = fmax(down_nudge, depth);
            }
            // Colliding from the top
            else {
                up_nudge = fmin(up_nudge, depth);
            }
        }

        bool hit_top = up_nudge != 0.0f;
        bool hit_bottom = down_nudge != 0.0f;

        if (hit_top || hit_bottom) {
            // TODO - check squish

            if (hit_top)
                grounded = true;
            else
                on_ceiling = true;

            pos.y += up_nudge + down_nudge;
        }
    }

    if (grounded) {
        velocity.y = fmin(velocity.y, 0.0f);
        remaining_jumps = total_jumps;
        time_since_grounded = 0.0f;
    }

    if (on_ceiling && velocity.y < 0.0f)
        velocity.y = step(velocity.y, 0.0f, fall_gravity * dt * 0.5);

    if (on_wall)
        velocity.x = step(velocity.x, 0.0f, deaccel * dt);
}

//====================================================================

void Player::render(World* world)
{
    world->log(TextFormat(
        "Player Pos = (%s, %s)",
        int_to_str(pos.x, 4).c_str(),
        int_to_str(pos.y, 4).c_str()));

    world->log(TextFormat(
        "Player Vel = (%s, %s)",
        int_to_str(velocity.x, 4).c_str(),
        int_to_str(velocity.y, 4).c_str()));

    world->log(TextFormat("Grounded %d, On Ceiling %d, On Wall %d", grounded, on_ceiling, on_wall));
    world->log(TextFormat("Time since grounded = %.2f", time_since_grounded));
    world->log(TextFormat("Total jumps %d, remaining jumps %d", total_jumps, remaining_jumps));

    if (grounded)
        world->log("GROUNDED");
    if (on_wall)
        world->log("ON WALL");

    DrawRectangle(
        pos.x - half_width,
        pos.y - half_height,
        half_width * 2,
        half_height * 2,
        RED);
}

//====================================================================

const char* Player::get_name() { return "player"; }

void Player::get_properties(std::vector<DebugProperty>* properties)
{
    Actor::get_properties(properties);

    properties->push_back({ "input_dir", &input_dir, false });
    properties->push_back({ "jump_held", &jump_held, false });
    // properties->push_back({ "jump_pressed", &jump_pressed, false });

    properties->push_back({ "velocity", &velocity, false });
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

std::unique_ptr<class RawEntity> Player::ToRaw()
{
    RawPlayer* raw = new RawPlayer(pos.x, pos.y);
    return std::unique_ptr<RawEntity>(raw);
}

std::unique_ptr<class Entity> RawPlayer::ToEntity()
{
    Vector2 pos = {
        static_cast<float>(x),
        static_cast<float>(y),
    };

    std::unique_ptr<Player> player(new Player(pos));
    return player;
}
