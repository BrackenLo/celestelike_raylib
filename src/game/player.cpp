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
    // Change character
    // Check if button pressed, not on cooldown and we have stuff to change to
    if (
        ability_3_pressed
        && switch_character_cooldown <= 0.0f
        && player_characters.size() > 1 //
    ) {
        player_character_index += 1;
        player_character_index %= player_characters.size();

        set_inner(player_characters[player_character_index]);
        switch_character_cooldown = switch_character_cooldown_size;
    }

    // Update inner
    inner->fixed_update(world, dt);
    resolve_collisions(world, dt);

    //----------------------------------------------
    // Reset ability button pressed

    ability_1_pressed = false;
    ability_2_pressed = false;
    ability_3_pressed = false;

    //----------------------------------------------
    // Update timers

    if (switch_character_cooldown > 0.0f) {
        switch_character_cooldown -= dt;
    }

    if (!grounded)
        time_since_grounded += dt;

    // Tick jump buffer
    if (jump_pressed) {
        jump_buffer -= dt;
        if (jump_buffer <= 0.0f) {
            jump_pressed = false;
            jump_buffer = 0.0f;
        }
    }

    if (wall_jump_control_timer > 0.0f) {
        wall_jump_control_timer -= dt;
    }
}

void Player::set_inner(PlayerType inner_type)
{
    switch (inner_type) {
    case PlayerType::Base:
        inner = std::unique_ptr<PlayerInner>(new PlayerInner(this));
        break;
    case PlayerType::Debug:
        inner = std::unique_ptr<PlayerInner>(new DebugPlayerInner(this));
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

    // Start jump buffer
    if (are_keys_pressed(key_jump)) {
        jump_pressed = true;
        jump_buffer = inner->jump_buffer_size;
    }

    jump_held = are_keys_down(key_jump);

    if (are_keys_pressed(key_ability_1))
        ability_1_pressed = true;

    if (are_keys_pressed(key_ability_2))
        ability_2_pressed = true;

    if (are_keys_pressed(key_ability_3))
        ability_3_pressed = true;

    ability_1_down = are_keys_down(key_ability_1);
    ability_2_down = are_keys_down(key_ability_2);
}

void Player::resolve_collisions(World* world, float dt)
{
    grounded = false;
    on_ceiling = false;
    on_wall = false;

    old_pos = pos;

    //----------------------------------------------
    // TODO - Revise this?

    Vector2 pos_to_move = Vector2Scale(velocity, dt);
    pos_to_move.x = round(pos_to_move.x);
    pos_to_move.y = round(pos_to_move.y);

    // int steps = Vector2Length(pos_to_move) / 10; // maybe???
    int sub_steps = 4;

    Vector2 pos_to_move_step = Vector2Scale(pos_to_move, 1.0f / sub_steps);

    //----------------------------------------------

    for (int step = 0; step < sub_steps; step++) {

        pos.x += pos_to_move_step.x;

        float left_nudge = 0.0f;
        float right_nudge = 0.0f;

        for (CollisionEntity* solid : world->check_overlap(this)) {
            float dx = pos.x - solid->pos.x;
            float px = (half_width + solid->half_width) - std::abs(dx);
            float sx = std::copysign(1.0f, dx);
            float depth = px * sx;

            // Colliding from the right
            if (depth > 0)
                right_nudge = fmax(right_nudge, depth);
            // Colliding from the left
            else
                left_nudge = fmin(left_nudge, depth);
        }

        bool hit_left = left_nudge != 0.0f;
        bool hit_right = right_nudge != 0.0f;

        if (hit_left != hit_right) {
            on_wall = true;
            pos.x += left_nudge + right_nudge;
        } else {
            // TODO - check squish
        }

        //----------------------------------------------

        pos.y += pos_to_move_step.y;

        float up_nudge = 0.0f;
        float down_nudge = 0.0f;

        for (CollisionEntity* solid : world->check_overlap(this)) {
            float dy = pos.y - solid->pos.y;
            float py = (half_height + solid->half_height) - std::abs(dy);
            float sy = std::copysign(1.0f, dy);
            float depth = py * sy;

            // Colliding from below
            if (depth > 0)
                down_nudge = fmax(down_nudge, depth);

            // Colliding from the top
            else
                up_nudge = fmin(up_nudge, depth);
        }

        bool hit_top = up_nudge != 0.0f;
        bool hit_bottom = down_nudge != 0.0f;

        if (hit_top != hit_bottom) {
            if (hit_top)
                grounded = true;
            else
                on_ceiling = true;

            pos.y += up_nudge + down_nudge;
        } else {
            // TODO - check squish
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
