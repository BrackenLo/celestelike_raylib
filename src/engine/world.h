#pragma once

#include "camera.h"
#include "physics.h"
#include <vector>

class World {
public:
    World();
    ~World();

    inline void add_actor(class Actor* actor) { actors.push_back(actor); }
    inline void add_solid(class Solid* solid) { solids.push_back(solid); }

    inline std::vector<class Actor*>* get_actors() { return &actors; }
    inline std::vector<class Solid*>* get_solids() { return &solids; };

    std::vector<Collision> check_collision(class CollisionEntity* to_check);

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
};
