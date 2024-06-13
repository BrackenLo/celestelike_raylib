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
}

Player::Player(Vector2 new_pos)
    : Player()
{
    pos = new_pos;
}

void Player::update(World* world)
{
    float delta = GetFrameTime();

    update_velocity();

    pos = Vector2Add(
        pos,
        Vector2Scale(velocity, delta));

    resolve_collisions(world);
}

void Player::update_velocity()
{
    Vector2 input_dir = { 0 };

    if (IsKeyDown(KEY_A))
        input_dir.x -= 1.0f;
    if (IsKeyDown(KEY_D))
        input_dir.x += 1.0f;

    if (IsKeyDown(KEY_W))
        input_dir.y -= 1.0f;
    if (IsKeyDown(KEY_S))
        input_dir.y += 1.0f;

    update_x_vel(input_dir);
    update_y_vel(input_dir);
}

void Player::update_x_vel(Vector2 input_dir)
{
    float delta = GetFrameTime();

    // No input pressed
    if (input_dir.x == 0.0f) {
        velocity.x = step(velocity.x, 0., deaccel * delta);
        return;
    }

    float input_sign = std::copysign(1.0f, input_dir.x);
    float vel_sign = std::copysign(1.0f, velocity.x);

    float speed;

    if ((input_sign == 1.0f && vel_sign == -1.0f)
        || (input_sign == -1.0f && vel_sign == 1.0f)) {
        speed = deaccel;
    } else
        speed = accel;

    velocity.x = step(velocity.x, max_speed * input_sign, speed * delta);
}

void Player::update_y_vel(Vector2 input_dir)
{
    float delta = GetFrameTime();

    if (IsKeyPressed(KEY_SPACE) && grounded) {
        velocity.y = -jump_impulse;
    }

    velocity.y += gravity * delta;
    velocity.y = std::fmin(velocity.y, max_fall_speed);
}

void Player::resolve_collisions(World* world)
{
    grounded = false;
    on_wall = false;

    for (auto collision : world->check_collision(this)) {

        if (collision.delta.y < 0.0f)
            grounded = true;

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

    if (on_wall)
        velocity.x = 0.0f;
}

void Player::render()
{
    DrawText(TextFormat("Player Pos = (%d, %d)", (int)pos.x, (int)pos.y), -200, -200, 20, BLACK);
    DrawText(TextFormat("Player vel = (%d, %d)", (int)velocity.x, (int)velocity.y), -200, -180, 20, BLACK);

    DrawRectangle(
        pos.x - half_width,
        pos.y - half_height,
        half_width * 2,
        half_height * 2,
        RED);
}
