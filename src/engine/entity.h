#pragma once

#include "raylib.h"

//====================================================================

class Entity {
public:
    Entity();
    Entity(Vector2 pos);

    ~Entity() { }
    virtual void update(class World* world) {};
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

class Actor : public CollisionEntity {
    using CollisionEntity::CollisionEntity;
};

//====================================================================

class Solid : public CollisionEntity {
    using CollisionEntity::CollisionEntity;
};
