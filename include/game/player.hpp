#pragma once
#include "../engine/entity.hpp"
#include "player_inner_base.hpp"
#include "raylib.h"
#include <memory>
#include <vector>

//====================================================================

class Player : public Actor {
public:
    friend class PlayerInner;
    friend class DebugPlayerInner;
    friend class AvianPlayerInner;
    friend class CelestePlayerInner;

    friend class Debugger;

    Player();
    Player(Vector2 pos);
    Player(Vector2 pos, std::vector<PlayerType> characters, int index);

    virtual void update(class World* world) override;
    virtual void fixed_update(class World* world, float dt) override;
    virtual void render(class World* world) override;

private:
    void set_inner(PlayerType inner_type);
    void player_input();
    void resolve_collisions(World* world, float dt);

protected:
    std::unique_ptr<class PlayerInner> inner;
    std::vector<PlayerType> player_characters = { PlayerType::Base, PlayerType::Avian };
    int player_character_index = 0;

    // Managed Player Variables
    Vector2 old_pos = { 0 };

    Vector2 input_dir = { 0 };
    bool jump_held = false;
    bool jump_pressed = false;

    bool ability_1_pressed = false;
    bool ability_1_down = false;
    bool ability_2_pressed = false;
    bool ability_2_down = false;

    bool ability_3_pressed = false;

    Vector2 velocity = { 0 };

    bool grounded = false;
    bool on_ceiling = false;
    bool on_wall = false;

    float time_since_grounded = 0.0f;

    float jump_buffer = 0.0f;
    int remaining_jumps = 1;

    bool jumping = false;
    float wall_jump_control_timer = 0.0f;

    float switch_character_cooldown = 0.0f;

protected:
    // Config Player Variables
    std::vector<int> key_up = { KEY_I };
    std::vector<int> key_down = { KEY_K };
    std::vector<int> key_left = { KEY_J };
    std::vector<int> key_right = { KEY_L };

    std::vector<int> key_jump = { KEY_SPACE };
    std::vector<int> key_ability_1 = { KEY_C };
    std::vector<int> key_ability_2 = { KEY_X };
    std::vector<int> key_ability_3 = { KEY_Z };

    float switch_character_cooldown_size = 0.4f;
};
