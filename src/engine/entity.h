#pragma once

#include "debug.h"
#include "raylib.h"

//====================================================================

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

class CollisionEntity : public Entity {

public:
    CollisionEntity();
    CollisionEntity(Vector2 pos, int h_width, int h_height);

    virtual void render(class World* world) override;

    int half_width;
    int half_height;
};

//====================================================================

class Actor : public CollisionEntity, public IDebug {
    using CollisionEntity::CollisionEntity;

public:
    virtual inline const char* get_name() override { return "actor"; }
    virtual void get_properties(std::vector<DebugProperty>* properties) override;
};

//====================================================================

class Solid : public CollisionEntity {
    using CollisionEntity::CollisionEntity;
};
