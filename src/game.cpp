#include "game.h"

#include "game/simple_world.h"
#include <memory>

Game::Game()
{
    TraceLog(TraceLogLevel::LOG_INFO, "Constructing Game");
    world = std::unique_ptr<World>(new SimpleWorld());
}

int Game::run()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    // InitAudioDevice();

    SetTargetFPS(60);
    SetWindowState(ConfigFlags::FLAG_WINDOW_RESIZABLE);
    // SetExitKey(0);

    world->init();

    while (!WindowShouldClose()) {
        world->update();
        world->render();
    }

    CloseWindow();

    return 0;
}
