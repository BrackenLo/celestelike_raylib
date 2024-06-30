#pragma once
#include "../engine/entity.h"
#include "../engine/save.h"
#include "cereal/details/helpers.hpp"
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
    void update_velocity(class World* world, float dt);
    float get_gravity(class World* world);

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

    Player();
    Player(Vector2 pos);
    Player(Vector2 pos, PlayerType inner_type);

    virtual void update(class World* world) override;
    virtual void fixed_update(class World* world, float dt) override;
    virtual void render(class World* world) override;

private:
    void player_input();
    void resolve_collisions(World* world, float dt);

protected:
    std::unique_ptr<class PlayerInner> inner;
    std::vector<PlayerType> player_characters = { PlayerType::Base };

    Vector2 old_pos = { 0 };

    // Active Player Variables
    Vector2 input_dir = { 0 };
    bool jump_held = false;
    bool jump_pressed = false;

    Vector2 velocity = { 0 };

    bool grounded = false;
    bool on_ceiling = false;
    bool on_wall = false;

    float time_since_grounded = 0.0f;
    float jump_buffer = 0.0f;
    bool jumping = false;
    int remaining_jumps = 1;

protected:
    // Config Player Variables
    std::vector<int> key_up = { KEY_I };
    std::vector<int> key_down = { KEY_K };
    std::vector<int> key_left = { KEY_J };
    std::vector<int> key_right = { KEY_L };

    std::vector<int> key_jump = { KEY_SPACE };
    std::vector<int> key_ability_1 = { KEY_Z };
    std::vector<int> key_ability_2 = { KEY_X };
    std::vector<int> key_ability_3 = { KEY_C };
    std::vector<int> key_ability_4 = { KEY_V };

    Color player_color;

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
    RawPlayer() { }
    RawPlayer(int x, int y, PlayerType player_type);

    PlayerType player_type;

    virtual std::unique_ptr<class Entity> ToEntity() override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::base_class<RawEntity>(this),
            cereal::make_nvp("type", player_type));
    }
};

CEREAL_REGISTER_TYPE(RawPlayer);
