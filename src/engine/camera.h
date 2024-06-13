#pragma once

#include "entity.h"
#include "raylib.h"

class GameCamera : public Entity {

public:
    GameCamera();
    virtual void update(class World* world) override;
    virtual void reset();

    Camera2D get_camera();

public:
    float speed;
    Vector2 move_target;
    Entity* follow_target;

private:
    Camera2D camera;
};
