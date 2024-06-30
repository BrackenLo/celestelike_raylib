#include "player.h"

#include "../engine/tools.h"
#include "../engine/world.h"
#include "player_avian.h"
#include "raymath.h"
#include <cmath>
#include <memory>
#include <raylib.h>

//====================================================================

PlayerInner::PlayerInner(Player* outer)
{
    this->outer = outer;

    player_type = PlayerType::Base;

    outer->half_width = 25;
    outer->half_height = 32;

    outer->player_color = RED;

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

    update_jump_variables();
}

void PlayerInner::update(World* world)
{
}

void PlayerInner::fixed_update(World* world, float dt)
{
    update_velocity(world, dt);
}

void PlayerInner::render(World* world)
{
}

bool PlayerInner::do_jump(World* world, float dt)
{
    bool first_jump = outer->remaining_jumps == total_jumps;
    bool has_ground = outer->grounded || outer->time_since_grounded < coyote_time;
    bool can_jump = outer->remaining_jumps > 0 && ((first_jump && has_ground) || !first_jump);

    if (can_jump) {
        outer->jump_pressed = false;
        outer->jump_buffer = 0.0f;
        outer->remaining_jumps -= 1;

        outer->jumping = true;
        outer->velocity.y = jump_impulse;
        return true;
    }
    return false;
}

bool PlayerInner::do_wall_jump(World* world, float dt)
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

    if (left_collision || right_collision) {
        outer->jump_buffer = 0.0f;

        float x_mul;

        if (left_collision && right_collision)
            x_mul = 0.;
        else
            x_mul = left_collision ? 1.0f : -1.0f;

        outer->velocity.x = wall_jump_impulse_x * x_mul;
        outer->velocity.y = jump_impulse;
        outer->jumping = true;
        return true;
    }
    return false;
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

void PlayerInner::do_ability1(World* world, float dt) { }
void PlayerInner::do_ability2(World* world, float dt) { }

void PlayerInner::update_jump_variables()
{
    jump_impulse = (2.0f * jump_height) / jump_time_to_peak;
    jump_gravity = (-2.0f * jump_height) / (jump_time_to_peak * jump_time_to_peak);
    fall_gravity = (-2.0f * jump_height) / (jump_time_to_descent * jump_time_to_descent);
    variable_jump_gravity = ((jump_impulse * jump_impulse) / (2.0f * variable_jump_height)) * -1.0f;
}

void PlayerInner::update_velocity(World* world, float dt)
{
    //----------------------------------------------
    // Update X Velocity

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

    outer->velocity.x = step(outer->velocity.x, target_velocity, speed * dt); // TEST

    //----------------------------------------------
    // Wall jump

    bool is_wall_jump = false;

    if (!outer->grounded && outer->jump_pressed) { // TEST
    }

    //----------------------------------------------
    // Update Y Velocity

    // Start jumping
    if (outer->jump_pressed && !is_wall_jump) {
        do_jump(world, dt);
    }

    // If jumping, check end jump conditions
    if (outer->jumping && (!outer->jump_held || outer->velocity.y > 0.1))
        outer->jumping = false;

    // Apply and clamp gravity
    outer->velocity.y += get_gravity(world) * dt;
    outer->velocity.y = std::fmin(outer->velocity.y, max_fall_speed);
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

Player::Player()
{
    TraceLog(TraceLogLevel::LOG_INFO, "Creating player");
    inner = std::unique_ptr<PlayerInner>(new PlayerInner(this));
}

Player::Player(Vector2 new_pos)
    : Player()
{
    pos = new_pos;
}

Player::Player(Vector2 new_pos, PlayerType inner_type)
    : Player(new_pos)
{
    switch (inner_type) {
    case PlayerType::Avian:
        inner = std::unique_ptr<PlayerInner>(new AvianPlayerInner(this));
        break;
    case PlayerType::Base:
        break;
    };
}

//====================================================================

void Player::update(World* world)
{
    player_input();
    inner->update(world);
}

void Player::fixed_update(World* world, float dt)
{
    // Do player character and ability stuff here
    // Check if key pressed since last fixed update (or buffer it like jump)

    inner->fixed_update(world, dt);

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
        jump_buffer = inner->jump_buffer_size; // TEST
    }
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
        velocity.y = fmin(velocity.y, 0.0f); // TODO - move this functionality into inner
        remaining_jumps = inner->total_jumps;
        time_since_grounded = 0.0f;
        inner->on_grounded(world, dt);
    }

    if (on_ceiling && velocity.y < 0.0f) {
        inner->on_ceiling(world, dt);
    }

    if (on_wall) {
        velocity.x = step(velocity.x, 0.0f, inner->deaccel * dt);
        inner->on_wall(world, dt);
    }
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
    world->log(TextFormat("Total jumps %d, remaining jumps %d", inner->total_jumps, remaining_jumps));

    if (grounded)
        world->log("GROUNDED");
    if (on_wall)
        world->log("ON WALL");

    DrawRectangle(
        pos.x - half_width,
        pos.y - half_height,
        half_width * 2,
        half_height * 2,
        player_color);

    inner->render(world);
}

//====================================================================

const char* Player::get_name() { return "player"; }

void Player::get_properties(std::vector<DebugProperty>* properties)
{
    Actor::get_properties(properties);

    properties->push_back({ "input_dir", &input_dir, false });
    properties->push_back({ "jump_held", &jump_held, false });

    properties->push_back({ "velocity", &velocity, false });

    inner->get_properties(properties);
}

//====================================================================

RawPlayer::RawPlayer(int x, int y, PlayerType player_type)
    : RawEntity(x, y)
    , player_type(player_type)
{
}

std::unique_ptr<class RawEntity> Player::ToRaw()
{
    RawPlayer* raw = new RawPlayer(pos.x, pos.y, inner->get_player_type());
    return std::unique_ptr<RawEntity>(raw);
}

std::unique_ptr<class Entity> RawPlayer::ToEntity()
{
    Vector2 pos = {
        static_cast<float>(x),
        static_cast<float>(y),
    };

    std::unique_ptr<Player> player(new Player(pos, player_type));
    return player;
}
