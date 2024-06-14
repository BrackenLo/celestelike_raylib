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
    void update_x_vel(Vector2 input_dir);
    void update_y_vel(Vector2 input_dir);
    void resolve_collisions(World* world);

protected:
    Vector2 velocity = { 0 };

    float accel = 2200.0f;
    float deaccel = 2600.0f;

    bool grounded = false;
    bool on_ceiling = false;
    bool on_wall = false;

    float gravity = 1800.0f;
    float max_speed = 400.0f;
    float max_fall_speed = 800.0f;

    float up_gravity = 1800.0f;
    float fall_gravity = 2500.0f;
    float jump_gravity = 500.0f;

    float jump_impulse = 400.0f;
    bool jumping = false;

    bool jump_held = false;
    bool jump_pressed = false;
    float jump_buffer = 0.0f;
    float jump_buffer_size = 0.1f;
};
