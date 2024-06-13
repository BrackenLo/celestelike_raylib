#pragma once

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

private:
    std::vector<class Actor*> actors;
    std::vector<class Solid*> solids;
};
