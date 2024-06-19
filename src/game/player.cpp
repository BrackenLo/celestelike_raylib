#include "player.h"

#include "../engine/tools.h"
#include "../engine/world.h"
#include "raymath.h"
#include <cmath>
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
    if (IsKeyDown(KEY_A))
        input_dir.x -= 1.0f;
    if (IsKeyDown(KEY_D))
        input_dir.x += 1.0f;

    if (IsKeyDown(KEY_W))
        input_dir.y -= 1.0f;
    if (IsKeyDown(KEY_S))
        input_dir.y += 1.0f;

    jump_held = IsKeyDown(KEY_SPACE);

    // Tick jump buffer
    if (jump_pressed) {
        jump_buffer -= delta;
        if (jump_buffer <= 0.0f) {
            jump_pressed = false;
            jump_buffer = 0.0f;
        }
    }

    // Start jump buffer
    if (IsKeyPressed(KEY_SPACE)) {
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

    Vector2 valid_pos = old_pos;

    for (int step = 0; step < sub_steps; step++) {
        pos.x += pos_to_move_step.x;

        for (CollisionEntity* value : world->check_overlap(this)) {
            world->log("X Collision", 1);

            pos.x = valid_pos.x;
            on_wall = true; // temp
        }

        pos.y += pos_to_move_step.y;

        for (CollisionEntity* value : world->check_overlap(this)) {
            world->log("Y Collision", 1);

            pos.y = valid_pos.y;
            grounded = true; // temp
        }

        valid_pos = pos;
    }

    if (grounded) {
        velocity.y = fmin(velocity.y, 0.0f);
        remaining_jumps = total_jumps;
        time_since_grounded = 0.0f;
    }

    if (on_ceiling && velocity.y < 0.0f)
        velocity.y = step(velocity.y, 0.0f, fall_gravity * dt);

    if (on_wall)
        velocity.x = step(velocity.x, 0.0f, deaccel * dt * 1.3f);
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
