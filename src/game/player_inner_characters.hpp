#pragma once

#include "player_inner_base.hpp"

//====================================================================

class DebugPlayerInner : public PlayerInner {
public:
    DebugPlayerInner(class Player* outer);

protected:
    void fixed_update(World* world, float dt) override;
    void walk(World* world, float dt) override;
    void render(World* world) override;
};

//====================================================================

class AvianPlayerInner : public PlayerInner {
public:
    AvianPlayerInner(Player* outer);

protected:
    void update(World* world) override;

protected:
    float default_max_fall_speed;
    float glide_max_fall_speed;
};

//====================================================================

class CelestePlayerInner : public PlayerInner {
public:
    CelestePlayerInner(Player* outer);

protected:
    void do_ability1(class World* world, float dt) override;
    void do_ability2(class World* world, float dt) override;

    void on_grounded(class World* world, float dt) override;
    void fixed_update(class World* world, float dt) override;
    float get_gravity(class World* world) override;

protected:
    float dash_cooldown;
    float dash_cooldown_size;
    int dashes;
    int dashes_count;
    int dash_power;

    float default_max_velocity_x;
};

//====================================================================
