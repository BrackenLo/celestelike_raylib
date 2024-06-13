#pragma once

#include "camera.h"
#include "world.h"
#include <vector>

class Level {

public:
    Level();
    ~Level();

    virtual void init();
    virtual void update();
    virtual void render();

protected:
    virtual void render_2d_inner();

protected:
    Color background;

    World world;
    GameCamera camera;
};
