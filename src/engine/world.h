#pragma once

#include "camera.h"
#include "debug.h"
#include "physics.h"
#include <vector>

class World {
public:
    World();
    ~World();

    inline void add_actor(class Actor* actor) { actors.push_back(actor); }
    inline void add_solid(class Solid* solid) { solids.push_back(solid); }

    bool destroy_actor(class Actor* actor);
    bool destroy_solid(class Solid* solid);

    inline std::vector<class Actor*>* get_actors() { return &actors; }
    inline std::vector<class Solid*>* get_solids() { return &solids; };

    std::vector<Collision> check_collision(class CollisionEntity* to_check);
    std::vector<CollisionEntity*> check_overlap(class CollisionEntity* to_check);
    // float check_horizontal_collision(class CollisionEntity* to_check, float horizontal_movement);
    // float check_vertical_collision(class CollisionEntity* to_check, float vertical_movement);

public:
    Color clear_color;
    GameCamera camera;

protected:
    virtual void init();
    virtual void update();
    virtual void render();
    virtual void render_2d_inner();

    friend class Game;

private:
    std::vector<class Actor*> actors;
    std::vector<class Solid*> solids;

public:
    inline void add_message(const char* text, int log = 0) { debug.add_message(text, log); }

private:
    Debugger debug;
};
