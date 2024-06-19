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
    update_velocity(world);

    resolve_collisions(world);

    if (!grounded)
        time_since_grounded += GetFrameTime();
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

void Player::update_velocity(World* world)
{
    float delta = GetFrameTime();

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

    velocity.x = step(velocity.x, target_velocity, speed * delta);

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
    velocity.y += get_gravity() * delta;
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

void Player::resolve_collisions(World* world)
{
    float delta = GetFrameTime();

    grounded = false;
    on_ceiling = false;
    on_wall = false;

    old_pos = pos;

    //
    //----------------------------------------------

    Vector2 pos_to_move = Vector2Scale(velocity, delta);

    if (pos_to_move.x == 0.0f) {
        world->add_message("NOT HORIZONTAL");
    }
    if (pos_to_move.y == 0.0f) {
        world->add_message("NOT VERTICAL");
    }

    // int steps = Vector2Length(pos_to_move) / 10; // maybe???
    // int steps = 5;

    //
    //----------------------------------------------
    // Check x axis
    // pos.x += pos_to_move.x;

    // int x_nudge_left = 0;
    // int x_nudge_right = 0;

    // // Check all collisions on x axis
    // for (Collision collision : world->check_collision(this)) {

    //     world->add_message(
    //         TextFormat(
    //             "x collision: delta = (%d, %d), normal = (%d, %d)",
    //             (int)collision.delta.x, (int)collision.delta.y,
    //             (int)collision.normal.x, (int)collision.normal.y),
    //         1);

    //     // Has collision left or right
    //     if (collision.normal.x != 0.0f)
    //         on_wall = true;

    //     // Skip resolving movement if not the thing
    //     else
    //         continue;

    //     if (collision.delta.x > 0.) {
    //         world->add_message("Nudge right", 2);
    //         x_nudge_right = std::fmax(x_nudge_right, collision.delta.x);
    //     }

    //     else {
    //         world->add_message("Nudge left", 2);
    //         x_nudge_left = std::fmin(x_nudge_left, collision.delta.x);
    //     }

    //     // pos.x += collision.delta.x;
    // }

    // if (x_nudge_left && x_nudge_right) {
    //     // TODO - squish
    // }

    // world->add_message(
    //     TextFormat(
    //         "x nudge left = %s, nudge right = %s",
    //         int_to_str(x_nudge_left, 3).c_str(),
    //         int_to_str(x_nudge_right, 3).c_str()),
    //     2);

    // pos.x += x_nudge_left + x_nudge_right;

    //----------------------------------------------

    //

    pos.x += pos_to_move.x;

    for (CollisionEntity* value : world->check_overlap(this)) {
        world->add_message("X Collision", 1);

        pos.x = old_pos.x;
    }

    //

    pos.y += pos_to_move.y;

    for (CollisionEntity* value : world->check_overlap(this)) {
        world->add_message("Y Collision", 1);

        pos.y = old_pos.y;
    }

    //----------------------------------------------
    // Check y axis

    // pos.y += pos_to_move.y;

    // int y_nudge_up = 0;
    // int y_nudge_down = 0;

    // for (Collision collision : world->check_collision(this)) {

    //     world->add_message(
    //         TextFormat(
    //             "y collision: delta = (%d, %d), normal = (%d, %d)",
    //             (int)collision.delta.x, (int)collision.delta.y,
    //             (int)collision.normal.x, (int)collision.normal.y),
    //         1);

    //     if (collision.normal.y == 0.0) {
    //         continue;
    //     }

    //     // Has collision Below
    //     if (collision.normal.y < 0.0f)
    //         grounded = true;

    //     // Has Collision Above
    //     if (collision.normal.y > 0.0f)
    //         on_ceiling = true;

    //     // Nudge up
    //     if (collision.delta.y > 0.0) {
    //         world->add_message("Nudged up", 2);
    //         y_nudge_up = std::fmax(y_nudge_up, collision.delta.y);
    //         // y_nudge_up += collision.delta.y;
    //     }

    //     // Nudge down
    //     else {
    //         world->add_message("Nudged down", 2);
    //         y_nudge_down = std::fmin(y_nudge_down, collision.delta.y);
    //         // y_nudge_down += collision.delta.y;
    //     }
    // }

    // if (y_nudge_down && y_nudge_up) {
    //     // TODO - squish
    // }

    // world->add_message(
    //     TextFormat(
    //         "y nudge up = %s, nudge down = %s",
    //         int_to_str(y_nudge_up, 3).c_str(),
    //         int_to_str(y_nudge_down, 3).c_str()),
    //     2);

    // pos.y += y_nudge_up + y_nudge_down;

    //----------------------------------------------

    // for (auto collision : world->check_collision(this)) {

    //     world->add_message(
    //         TextFormat(
    //             "collision: delta = (%d, %d), normal = (%d, %d)",
    //             (int)collision.delta.x, (int)collision.delta.y,
    //             (int)collision.normal.x, (int)collision.normal.y),
    //         1);

    //     // Has collision Below
    //     if (collision.normal.y < 0.0f)
    //         grounded = true;

    //     // Has Collision Above
    //     if (collision.normal.y > 0.0f)
    //         on_ceiling = true;

    //     // Has collision left or right
    //     if (collision.normal.x != 0.0f)
    //         on_wall = true;

    //     // Has both axies of collision
    //     if (collision.normal.x != 0.0f && collision.normal.y != 0.0f) {
    //         // Prioritise using y axis
    //         pos.y += collision.delta.y;
    //         return;
    //     }

    //     pos.x += collision.delta.x;
    //     pos.y += collision.delta.y;
    // }

    if (grounded) {
        velocity.y = fmin(velocity.y, 0.0f);
        remaining_jumps = total_jumps;
        time_since_grounded = 0.0f;
    }

    if (on_ceiling && velocity.y < 0.0f)
        velocity.y = step(velocity.y, 0.0f, fall_gravity * delta);

    if (on_wall)
        velocity.x = step(velocity.x, 0.0f, deaccel * delta * 1.3f);
}

//====================================================================

void Player::render(World* world)
{

    world->add_message(TextFormat(
        "Player Pos = (%s, %s)",
        int_to_str(pos.x, 4).c_str(),
        int_to_str(pos.y, 4).c_str()));

    world->add_message(TextFormat(
        "Player Vel = (%s, %s)",
        int_to_str(velocity.x, 4).c_str(),
        int_to_str(velocity.y, 4).c_str()));

    world->add_message(TextFormat("Grounded %d, On Ceiling %d, On Wall %d", grounded, on_ceiling, on_wall));
    world->add_message(TextFormat("Time since grounded = %.2f", time_since_grounded));
    world->add_message(TextFormat("Total jumps %d, remaining jumps %d", total_jumps, remaining_jumps));

    if (grounded)
        world->add_message("GROUNDED");
    if (on_wall)
        world->add_message("ON WALL");

    DrawRectangle(
        pos.x - half_width,
        pos.y - half_height,
        half_width * 2,
        half_height * 2,
        RED);
}
