#pragma once

#include "../engine/debug.hpp"

//====================================================================

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

class AvianPlayerInner : public PlayerInner {
public:
    AvianPlayerInner(Player* outer);

protected:
    virtual void update(World* world) override;

protected:
    float default_max_fall_speed;
    float glide_max_fall_speed;
};

//====================================================================
