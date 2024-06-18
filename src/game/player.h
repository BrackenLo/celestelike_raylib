#pragma once
#include "../engine/entity.h"
#include "raylib.h"

class Player : public Actor {
public:
    Player();
    Player(Vector2 pos);

    virtual void update(class World* world) override;
    virtual void render(class World* world) override;

private:
    void update_jump_variables();

    void player_input();

    void update_velocity(World* world);
    float get_gravity();

    void resolve_collisions(World* world);

protected:
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

    float jump_impulse;
    float jump_gravity;
    float fall_gravity;
    float variable_jump_gravity;

protected:
    // Config Player Variables
    float accel = 2200.0f;
    float deaccel = 2600.0f;

    float max_velocity_x = 400.0f;
    float max_fall_speed = 800.0f;

    float jump_buffer_size = 0.1f;
    float coyote_time = 0.09f;

    float jump_height = -60.0f;
    float jump_time_to_peak = 0.25f;
    float jump_time_to_descent = 0.2f;
    float variable_jump_height = -150.0f;
    int total_jumps = 1;

    float wall_slide_gravity = 300.0f; // TODO
    // Vector2 wall_jump_impulse = { 400.0f, -500.0f };
    float wall_jump_impulse_x = 500.0f;
};
