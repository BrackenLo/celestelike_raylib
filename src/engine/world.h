#pragma once

#include "camera.h"
#include "physics.h"
#include "tools.h"
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

    inline void add_message(const char* text) { logger.add_message(text); }

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

    Logger logger;
};
