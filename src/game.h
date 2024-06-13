#pragma once

#include "raylib.h"
#include <memory>
#include <vector>

#include "engine/world.h"

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
