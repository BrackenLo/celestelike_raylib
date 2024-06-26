#pragma once

#include "entity.h"
#include "raylib.h"

class GameCamera : public Entity, public IDebug {

public:
    GameCamera();
    virtual void update(class World* world) override;
    virtual void reset();

    inline Camera2D get_camera() { return camera; }

public:
    float speed;
    Vector2 move_target;
    Entity* follow_target;

    float zoom_speed;
    float zoom_target;

private:
    Camera2D camera;

public:
    virtual inline const char* get_name() override { return "camera"; }
    virtual void get_properties(std::vector<DebugProperty>* properties) override;
};
