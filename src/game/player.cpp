#include "player.hpp"

#include "../engine/tools.hpp"
#include "../engine/world.hpp"
#include "player_inner.hpp"
#include "raymath.h"
#include <cmath>
#include <memory>
#include <raylib.h>

//====================================================================

Player::Player()
{
    TraceLog(TraceLogLevel::LOG_INFO, "Creating player");

    player_characters = { PlayerType::Base, PlayerType::Avian };
    player_character_index = 0;

    set_inner(player_characters[player_character_index]);
}

Player::Player(Vector2 new_pos)
    : Player()
{
    pos = new_pos;
}

// Doesn't call default constructor
Player::Player(Vector2 new_pos, std::vector<PlayerType> characters, int index)
    : player_characters(characters)
    , player_character_index(index)
{
    TraceLog(TraceLogLevel::LOG_INFO, "Creating player");
    pos = new_pos;
    set_inner(characters[index]);
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

    // Change character
    // Check if button pressed, not on cooldown and we have stuff to change to
    if (ability_3_pressed && switch_character_cooldown <= 0.0f && player_characters.size() > 1) {
        // TraceLog(TraceLogLevel::LOG_DEBUG, "Ability 3 fired - Change Character");

        if (player_character_index == player_characters.size() - 1)
            player_character_index = 0;
        else
            player_character_index += 1;

        set_inner(player_characters[player_character_index]);

        switch_character_cooldown = switch_character_cooldown_size;
    }

    if (ability_1_pressed) {
        inner->do_ability1(world, dt);
    }
    if (ability_2_pressed) {
        inner->do_ability2(world, dt);
    }

    inner->fixed_update(world, dt);

    resolve_collisions(world, dt);

    if (!grounded)
        time_since_grounded += dt;

    ability_1_pressed = false;
    ability_2_pressed = false;
    ability_3_pressed = false;
    ability_4_pressed = false;

    if (switch_character_cooldown > 0) {
        switch_character_cooldown -= dt;
    }
}

void Player::set_inner(PlayerType inner_type)
{
    switch (inner_type) {
    case PlayerType::Base:
        inner = std::unique_ptr<PlayerInner>(new PlayerInner(this));
        break;
    case PlayerType::Avian:
        inner = std::unique_ptr<PlayerInner>(new AvianPlayerInner(this));
        break;
    case PlayerType::Celeste:
        inner = std::unique_ptr<PlayerInner>(new CelestePlayerInner(this));
        break;
    };
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

    if (are_keys_pressed(key_ability_1))
        ability_1_pressed = true;

    if (are_keys_pressed(key_ability_2))
        ability_2_pressed = true;

    if (are_keys_pressed(key_ability_3))
        ability_3_pressed = true;

    if (are_keys_pressed(key_ability_4))
        ability_4_pressed = true;
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
            if (hit_left && hit_right) {
                world->log("X SQUISH", 1);
            } else {

                world->log("X Collision", 1);

                on_wall = true;

                pos.x += left_nudge + right_nudge;
            }
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

            if (hit_top && hit_bottom) {
                world->log("Y SQUISH", 1);

            } else {
                if (hit_top)
                    grounded = true;
                else
                    on_ceiling = true;

                pos.y += up_nudge + down_nudge;
            }
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

// Player -> Raw
std::unique_ptr<class RawEntity> Player::ToRaw()
{
    RawPlayer* raw = new RawPlayer(pos.x, pos.y, player_characters, player_character_index);
    return std::unique_ptr<RawEntity>(raw);
}

//----------------------------------------------

// Raw constructor
RawPlayer::RawPlayer(int x, int y, std::vector<PlayerType> player_characters, int player_character_index)
    : RawEntity(x, y)
    , player_characters(player_characters)
    , player_character_index(player_character_index)
{
}

// Raw -> Player
std::unique_ptr<class Entity> RawPlayer::ToEntity()
{
    Vector2 pos = {
        static_cast<float>(x),
        static_cast<float>(y),
    };

    std::unique_ptr<Player> player(new Player(pos, player_characters, player_character_index));
    return player;
}
