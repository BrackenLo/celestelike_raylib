#pragma once

#include "engine/world.h"
#include <memory>

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 450;
static const char* WINDOW_TITLE = "celestelike";

class Game {

public:
    Game();
    int run();

private:
    std::shared_ptr<World> world;
};
