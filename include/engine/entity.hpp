#pragma once

#include "debug_x.hpp"
#include "raylib.h"

//====================================================================
// Base entity class

class Entity {
public:
    Entity();
    Entity(Vector2 pos);

    ~Entity() { }
    virtual void update(class World* world) {};
    virtual void fixed_update(class World* world, float dt) {};
    virtual void render(class World* world) {};

    Vector2 pos;
};

//====================================================================
// Base Actor/Solid class

class CollisionEntity : public Entity {

public:
    CollisionEntity();
    CollisionEntity(Vector2 pos, int h_width, int h_height);

    virtual void render(class World* world) override;

    int half_width;
    int half_height;
};

//====================================================================
// Actor class

class Actor : public CollisionEntity, public IDebug {
    using CollisionEntity::CollisionEntity;

public:
    Rectangle get_rect();

public:
    virtual inline const char* get_name() override { return "actor"; }
    virtual void get_properties(std::vector<DebugProperty>* properties) override;
};

//====================================================================
// Solid class

class Solid : public CollisionEntity {
    using CollisionEntity::CollisionEntity;
};
