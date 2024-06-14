#include "player.h"

#include "../engine/tools.h"
#include "../engine/world.h"
#include "raymath.h"
#include <cmath>
#include <raylib.h>

Player::Player()
{
    TraceLog(TraceLogLevel::LOG_INFO, "Creating player");

    half_width = 25;
    half_height = 32;

    jump_impulse = (2.0f * jump_height) / jump_time_to_peak;
    jump_gravity = (-2.0f * jump_height) / (jump_time_to_peak * jump_time_to_peak);
    fall_gravity = (-2.0f * jump_height) / (jump_time_to_descent * jump_time_to_descent);
    variable_jump_gravity = ((jump_impulse * jump_impulse) / (2.0f * variable_jump_height)) * -1.0f;
}

Player::Player(Vector2 new_pos)
    : Player()
{
    pos = new_pos;
}

void Player::update(World* world)
{
    player_input();
    update_velocity(world);

    resolve_collisions(world);
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
    // Update Y Velocity

    // Start jumping
    if (jump_pressed && grounded) {
        jump_pressed = false;
        jump_buffer = 0.0f;

        jumping = true;
        velocity.y = jump_impulse;
    }

    // If jumping, check end jump conditions
    if (jumping && (!jump_held || velocity.y > 0.1))
        jumping = false;

    // Apply and clamp gravity
    velocity.y += get_gravity() * delta;
    velocity.y = std::fmin(velocity.y, max_fall_speed);

    //----------------------------------------------
    // Wall jump

    if (!grounded && jump_pressed) {
        CollisionEntity to_check = CollisionEntity(pos, half_width + 4, half_height + 4);
        std::vector<Collision> collisions = world->check_collision(&to_check);

        // TODO - go through collisions, check if left or right and add velocity accordingly

        bool left_collision = false;
        bool right_collision = false;

        for (Collision collision : collisions) {
            if (collision.normal.x == 1.0f)
                left_collision = true;
            if (collision.normal.x == -1.0f)
                right_collision = true;
        }

        if (left_collision || right_collision) {
            jumping = false;
            jump_buffer = 0.0f;

            float x_mul;

            if (left_collision && right_collision)
                x_mul = 0.;
            else
                x_mul = left_collision ? 1.0f : -1.0f;

            velocity.x
                = wall_jump_impulse.x * x_mul;
            velocity.y = wall_jump_impulse.y;
        }
    }

    //----------------------------------------------
    // Update Velocity

    pos = Vector2Add(
        pos,
        Vector2Scale(velocity, delta));
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

    for (auto collision : world->check_collision(this)) {

        if (collision.delta.y < 0.0f)
            grounded = true;
        if (collision.delta.y > 0.0f)
            on_ceiling = true;

        if (collision.delta.x != 0.0f)
            on_wall = true;

        if (collision.delta.x != 0.0f && collision.delta.y != 0.0f) {
            pos.y += collision.delta.y;
            return;
        }

        pos.x += collision.delta.x;
        pos.y += collision.delta.y;
    }

    if (grounded)
        velocity.y = 0.0f;

    if (on_ceiling && velocity.y < 0.0f)
        velocity.y = step(velocity.y, 0.0f, fall_gravity * delta);

    if (on_wall)
        velocity.x = step(velocity.x, 0.0f, deaccel * delta * 1.3f);
}

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

    DrawRectangle(
        pos.x - half_width,
        pos.y - half_height,
        half_width * 2,
        half_height * 2,
        RED);
}
