#include "player.hpp"

#include "components.hpp"

namespace celestelike {

player::BasePlayerStats::BasePlayerStats()
{
    walk_speed.accel = 2200;
    walk_speed.deaccel = 2600;
    walk_speed.max_walk_speed = 400;

    // jump has default
    jump.impulse = -800;
    jump.fall_multiplier = 3.0f;

    gravity.fall_speed = 2000;
    gravity.max_fall_speed = 800;

    Bounds bounds = { 25, 32 };

    collision_bounds.bounds = bounds;
    sprite.size = bounds;
    sprite.color = RED;
}

void player::next_character_type(entt::registry& reg, entt::entity player)
{
    if (!reg.any_of<PlayerCharacters>(player))
        return;

    PlayerCharacters& player_character = reg.get<PlayerCharacters>(player);

    // Remove the old character
    remove_player_character_type(reg, player);

    // Set the next character index
    player_character.current_character_index += 1;
    player_character.current_character_index %= player_character.available_characters.size();

    // Set the new character
    set_player_character_type(reg, player);
}

void player::set_player_character_type(entt::registry& reg, entt::entity player)
{
    if (!reg.any_of<PlayerCharacters>(player))
        return;

    const PlayerCharacters& player_character = reg.get<PlayerCharacters>(player);

    int index = player_character.current_character_index % player_character.available_characters.size();
    PlayerCharacterTypes current_type = player_character.available_characters[index];

    switch (current_type) {
    case PlayerCharacterTypes::Base:
        setup_base(reg, player);
        break;
    case PlayerCharacterTypes::Debug:
        setup_debug(reg, player);
        break;
    case PlayerCharacterTypes::Avian:
        setup_avian(reg, player);
        break;
    case PlayerCharacterTypes::Celeste:
        setup_celeste(reg, player);
        break;
    }
}

void player::remove_player_character_type(entt::registry& reg, entt::entity player)
{
    if (!reg.any_of<PlayerCharacters>(player))
        return;

    const PlayerCharacters& player_character = reg.get<PlayerCharacters>(player);

    int index = player_character.current_character_index % player_character.available_characters.size();
    PlayerCharacterTypes current_type = player_character.available_characters[index];

    switch (current_type) {
    case PlayerCharacterTypes::Base:
        reset_base(reg, player);
        break;
    case PlayerCharacterTypes::Debug:
        reset_debug(reg, player);
        break;
    case PlayerCharacterTypes::Avian:
        reset_avian(reg, player);
        break;
    case PlayerCharacterTypes::Celeste:
        reset_celeste(reg, player);
        break;
    }
}

void player::reset_player(entt::registry& reg, entt::entity player, BasePlayerStats stats)
{
    // Don't remove
    // Pos, Velocity, Walk speed, Jump, Gravity, Collision Bounds,
    // OnGround, OnCeiling, Actor, Player, Render, Sprite, CameraTarget
    // PlayerCharacters

    reg.emplace_or_replace<WalkSpeed>(player, stats.walk_speed);
    reg.emplace_or_replace<Jump>(player, stats.jump);
    reg.emplace_or_replace<Gravity>(player, stats.gravity);
    reg.emplace_or_replace<CollisionBounds>(player, stats.collision_bounds);
    reg.emplace_or_replace<Sprite>(player, stats.sprite);

    reg.remove<Ability1>(player);
    reg.remove<Ability2>(player);
}

void player::setup_base(entt::registry& reg, entt::entity player)
{
    reset_player(reg, player, BasePlayerStats());
}
void player::reset_base(entt::registry& reg, entt::entity player)
{
}

void player::setup_debug(entt::registry& reg, entt::entity player)
{
    BasePlayerStats stats;
    reset_player(reg, player, stats);
    // reg.remove<CollisionBounds>(player);
    // reg.emplace<DebugMovement>(player);
}
void player::reset_debug(entt::registry& reg, entt::entity player)
{
}

void player::setup_avian(entt::registry& reg, entt::entity player)
{
    BasePlayerStats stats;

    Bounds bounds = { 20, 36 };
    stats.collision_bounds.bounds = bounds;

    stats.sprite.size = bounds;
    stats.sprite.color = GREEN;

    stats.walk_speed.accel = 2400;
    stats.walk_speed.deaccel = 2000;

    stats.jump.impulse = -1000;

    stats.gravity.max_fall_speed = 700;

    reset_player(reg, player, stats);

    // add double jump
    // add glide
}
void player::reset_avian(entt::registry& reg, entt::entity player)
{
    // remove double jump
    // remove glide
}

void player::setup_celeste(entt::registry& reg, entt::entity player)
{
    // add dash
}
void player::reset_celeste(entt::registry& reg, entt::entity player)
{
    // remove dash
}

}
