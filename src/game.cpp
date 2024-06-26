#include "game.h"

#include "engine/physics.h"
#include "game/simple_world.h"
#include "raylib.h"

#include "raygui.h"

Game::Game()
{
    TraceLog(TraceLogLevel::LOG_INFO, "Constructing Game");
    world = std::unique_ptr<World>(new SimpleWorld());
}

int Game::run()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    // InitAudioDevice();

    SetWindowState(ConfigFlags::FLAG_WINDOW_RESIZABLE);
    // SetExitKey(0);

    SetTraceLogLevel(TraceLogLevel::LOG_ALL);
    GuiLoadStyle("style_candy.rgs");

    PhysicsData data;
    world->init(&data);

    SetTargetFPS(data.fps);

    while (!WindowShouldClose()) {
        world->update();

        data.accumulator += GetFrameTime() * !data.freeze_fixed_update;

        if (data.accumulator >= data.timestep) {
            world->fixed_update(data.timestep);
            data.accumulator -= data.timestep;
            data.elapsed += data.timestep;
        }

        world->render();
    }

    CloseWindow();

    return 0;
}
