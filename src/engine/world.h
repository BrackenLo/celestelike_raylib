#pragma once

#include "camera.h"
#include "physics.h"
#include <vector>

class World {
public:
    World();
    ~World();

    void add_actor(class Actor* actor);
    void add_solid(class Solid* solid);

    std::vector<class Actor*>* get_actors();
    std::vector<class Solid*>* get_solids();

    std::vector<Collision> check_collision(class Actor* actor);

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
