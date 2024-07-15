#include "app.hpp"

#include <cstring>
#include <raylib.h>
#include <rlImGui.h>

int celestelike::App::run()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE); // TODO - Look into flags
    InitWindow(1280, 800, "celestelike");
    rlImGuiSetup(true);

    SetTraceLogLevel(TraceLogLevel::LOG_ALL);

    SetTargetFPS(60);

    TraceLog(TraceLogLevel::LOG_INFO, "Initializing world!");

    scene.init_level_register();
    scene.init_level_scene();
    scene.init();

    while (!WindowShouldClose()) {
        scene.update();
    }

    rlImGuiShutdown();

    TraceLog(TraceLogLevel::LOG_INFO, "Closing program");
    CloseWindow();
    return 0;
}
