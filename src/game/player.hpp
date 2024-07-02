#pragma once
#include "../engine/entity.hpp"
#include "../engine/save.hpp"
#include "player_inner.hpp"
#include "raylib.h"
#include <vector>

//====================================================================

class Player : public Actor, public IToRawData {
public:
    friend class PlayerInner;
    friend class AvianPlayerInner;
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
    bool ability_2_pressed = false;
    bool ability_3_pressed = false;
    bool ability_4_pressed = false;

    Vector2 velocity = { 0 };

    bool grounded = false;
    bool on_ceiling = false;
    bool on_wall = false;

    float time_since_grounded = 0.0f;
    float jump_buffer = 0.0f;
    bool jumping = false;
    int remaining_jumps = 1;

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
    std::vector<int> key_ability_4 = { KEY_V };

    float switch_character_cooldown_size = 0.4f;

public:
    // IDebug functionality
    virtual const char* get_name() override;
    virtual void get_properties(std::vector<DebugProperty>* properties) override;

public:
    // IToRawData functionality
    virtual std::unique_ptr<class RawEntity> ToRaw() override;
};

//====================================================================
// Save data stuff

struct RawPlayer : public RawEntity {
public:
    RawPlayer() { }
    RawPlayer(int x, int y, std::vector<PlayerType> player_characters, int player_character_index);

public:
    std::vector<PlayerType> player_characters;
    int player_character_index;

public:
    virtual std::unique_ptr<class Entity> ToEntity() override;

    template <class Archive>
    void serialize(Archive& archive, std::uint32_t const version)
    {
        // Version 1
        archive(
            cereal::base_class<RawEntity>(this),
            cereal::make_nvp("character_types", player_characters),
            cereal::make_nvp("character_index", player_character_index));

        // Version 2
        // ...
    }
};

CEREAL_REGISTER_TYPE(RawPlayer);
CEREAL_CLASS_VERSION(RawPlayer, 1);
