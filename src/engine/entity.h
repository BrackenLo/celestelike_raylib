#pragma once

#include "cereal/cereal.hpp"
#include "debug.h"
#include "raylib.h"
#include "save.h"

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
    virtual inline const char* get_name() override { return "actor"; }
    virtual void get_properties(std::vector<DebugProperty>* properties) override;
};

//====================================================================
// Solid class

class Solid : public CollisionEntity, public IToRawData {
    using CollisionEntity::CollisionEntity;

public:
    virtual std::unique_ptr<class RawEntity> ToRaw() override;
};

//----------------------------------------------
// Base 'Raw Solid' save info

struct RawSolid : public RawEntity {
    RawSolid() { }
    RawSolid(int x, int y, int half_width, int half_height);

    int half_width;
    int half_height;

    virtual std::unique_ptr<class Entity> ToEntity() override;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::base_class<RawEntity>(this),
            cereal::make_nvp("half_width", half_width),
            cereal::make_nvp("half_height", half_height));
    }
};

CEREAL_REGISTER_TYPE(RawSolid);
