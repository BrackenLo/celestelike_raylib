#pragma once
#include "../engine/entity.h"
#include "raylib.h"

class Player : public Actor {
public:
    Player();
    Player(Vector2 pos);

    virtual void update(class World* world) override;
    virtual void render() override;

protected:
    virtual void update_velocity();

private:
    void player_input();

    void update_x_vel(float delta);
    void update_y_vel(float delta);
    float get_gravity();

    void resolve_collisions(World* world);

protected:
    Vector2 input_dir = { 0 };
    bool jump_held = false;
    bool jump_pressed = false;

    Vector2 velocity = { 0 };

    float accel = 2200.0f;
    float deaccel = 2600.0f;

    bool grounded = false;
    bool on_ceiling = false;
    bool on_wall = false;

    float max_speed = 400.0f;
    float max_fall_speed = 800.0f;

    bool jumping = false;
    float jump_buffer = 0.0f;
    float jump_buffer_size = 0.1f;

    // https://www.youtube.com/watch?v=KbtcEVCM7bw
    float jump_height = -60.0f;
    float jump_time_to_peak = 0.25f;
    float jump_time_to_descent = 0.2f;
    float variable_jump_height = -150.0f;

    float jump_impulse;
    float jump_gravity;
    float fall_gravity;
    float variable_jump_gravity;
};
