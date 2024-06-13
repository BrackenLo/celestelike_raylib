#include "game.h"

#include "game/simple_level.h"
#include <memory>

Game::Game()
{
    TraceLog(TraceLogLevel::LOG_INFO, "Constructing Game");
    level = std::unique_ptr<Level>(new SimpleLevel());
}

int Game::run()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    // InitAudioDevice();

    SetTargetFPS(60);
    SetWindowState(ConfigFlags::FLAG_WINDOW_RESIZABLE);
    // SetExitKey(0);

    level->init();

    while (!WindowShouldClose()) {
        level->update();
        level->render();
    }

    CloseWindow();

    return 0;
}
