#pragma once
#include "../engine/entity.h"
#include "../engine/save.h"
#include "raylib.h"
#include <vector>

enum class PlayerType {
    Base,
    Avian,
};

//====================================================================

class PlayerInner : public IDebug {
public:
    friend class Player;

    PlayerInner(class Player* outer);

protected:
    virtual void update(class World* world);
    virtual void fixed_update(class World* world, float dt);
    virtual void render(class World* world);

    virtual bool do_jump(class World* world, float dt);
    virtual bool do_wall_jump(class World* world, float dt);
    virtual void on_grounded(class World* world, float dt);
    virtual void on_ceiling(class World* world, float dt);
    virtual void on_wall(class World* world, float dt);

    virtual void do_ability1(class World* world, float dt);
    virtual void do_ability2(class World* world, float dt);

protected:
    virtual void update_jump_variables();
    virtual float get_gravity(class World* world);

    inline PlayerType get_player_type() { return player_type; }

protected:
    PlayerType player_type;

    // Config Player Variables
    float accel;
    float deaccel;

    float max_velocity_x;
    float max_fall_speed;

    float jump_buffer_size;
    float coyote_time;

    float jump_height;
    float jump_time_to_peak;
    float jump_time_to_descent;
    float variable_jump_height;
    int total_jumps;

    float wall_slide_gravity;
    float wall_jump_impulse_x; // TODO - figure out a better algorithm (horizontal and vertical)

    Color player_color;

protected:
    // Managed player variables
    class Player* outer;

    float jump_impulse;
    float jump_gravity;
    float fall_gravity;
    float variable_jump_gravity;

public:
    // IDebug functionality
    virtual const char* get_name() override { return "player_inner"; }
    virtual void get_properties(std::vector<DebugProperty>* properties) override;
};

//====================================================================

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
