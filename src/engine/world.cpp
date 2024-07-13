#include "world.hpp"

#include "raylib.h"
#include <algorithm>
#include <cstring>
#include <string>

#include "entity.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <fstream>

#include "raygui.h"
#include "rlImGui.h"

World::World() { }

World::~World()
{
    clear_all();
}

int World::run()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE); // TODO - Look into flags
    InitWindow(1280, 800, "celestelike");

    rlImGuiSetup(true);

    SetTraceLogLevel(TraceLogLevel::LOG_ALL);
    GuiLoadStyle("style_candy.rgs");

    // SetTargetFPS(physics_data.fps);
    SetTargetFPS(60);

    init();

    while (!WindowShouldClose()) {
        scene.update();
    }

    rlImGuiShutdown();

    TraceLog(TraceLogLevel::LOG_INFO, "Closing program");
    CloseWindow();
    return 0;
}

//====================================================================

bool World::destroy_actor(Actor* actor)
{
    auto it = std::find(actors.begin(), actors.end(), actor);
    if (it != actors.end()) {
        actors.erase(it);
        return true;
    }

    return false;
}

bool World::destroy_solid(Solid* solid)
{
    auto it = std::find(solids.begin(), solids.end(), solid);
    if (it != solids.end()) {
        solids.erase(it);
        return true;
    }

    return false;
}

void World::clear_all()
{
    TraceLog(TraceLogLevel::LOG_INFO, "Clearing World");
    for (auto entity : actors) {
        delete entity;
    }
    actors.clear();

    for (auto entity : solids) {
        delete entity;
    }
    solids.clear();

    player_character = nullptr;
}

void World::clear_level()
{
    for (auto entity : solids) {
        delete entity;
    }
    solids.clear();
}

//====================================================================

// Get collision information about all solids overlapping with provided entity
std::vector<Collision> World::check_collision(CollisionEntity* to_check)
{
    std::vector<Collision> collisions;

    for (Solid* solid : solids) {

        std::optional<Collision> collision = intersect_aabb(solid, to_check);

        if (!collision.has_value())
            continue;

        collisions.push_back(collision.value());
    }

    return collisions;
}

// Get all solids overlapping with provided entity
std::vector<CollisionEntity*> World::check_overlap(CollisionEntity* to_check)
{
    std::vector<CollisionEntity*> collisions;

    for (Solid* solid : solids) {
        if (overlap_aabb(solid, to_check)) {
            collisions.push_back(solid);
        }
    }

    return collisions;
}

//====================================================================

std::vector<const char*> World::get_levels()
{
    std::vector<const char*> levels;

    TraceLog(TraceLogLevel::LOG_INFO, "Retrieving levels:");

    FilePathList files = LoadDirectoryFiles(GetWorkingDirectory());
    for (int i = 0; i < files.count; i++) {
        const char* file_path = files.paths[i];
        const char* file_name = GetFileName(file_path);
        // TraceLog(TraceLogLevel::LOG_INFO, file_path);
        // TraceLog(TraceLogLevel::LOG_INFO, GetFileName(file_path));

        const char* ptr = strstr(file_name, "level-");
        if (file_name == ptr) {
            levels.push_back(file_name);
            TraceLog(TraceLogLevel::LOG_INFO, TextFormat("   %s", file_name));
        }
    }

    return levels;
}

bool World::save_level(const char* level_name)
{
    // JSON archive
    std::string file_name;
    file_name
        .append("level-")
        .append(level_name)
        .append(".json");
    std::ofstream file(file_name);
    if (!file.is_open())
        return false;
    cereal::JSONOutputArchive archive(file);

    // // Binary archive
    // std::string file_name;
    // file_name
    //     .append("level-")
    //     .append(level_name)
    //     .append(".bin");
    // std::ofstream file(file_name, std::ios::binary);
    // cereal::PortableBinaryOutputArchive archive(file);

    // // XML archive
    // std::string file_name;
    // file_name
    //     .append("level-")
    //     .append(level_name)
    //     .append(".xml");
    // std::ofstream file(file_name);
    // cereal::XMLOutputArchive archive(file);

    TraceLog(TraceLogLevel::LOG_INFO, TextFormat("Saving file: %s", file_name.c_str()));

    // SaveData data(this);
    // archive(data);
    return true;
}

bool World::load_level(const char* level_file_name)
{
    TraceLog(TraceLogLevel::LOG_INFO, TextFormat("Loading level file: %s", level_file_name));

    clear_all();

    // JSON archive
    std::string file_name(level_file_name);
    // file_name
    //     .append("level-")
    //     .append(level_name)
    //     .append(".json");
    std::ifstream file(file_name);
    if (!file.is_open()) {
        TraceLog(TraceLogLevel::LOG_WARNING, TextFormat("Could not open level '%s'", level_file_name));
        return false;
    }

    cereal::JSONInputArchive archive(file);

    // SaveData data;

    // try {
    //     archive(data);
    // } catch (cereal::Exception val) {
    //     TraceLog(TraceLogLevel::LOG_WARNING, TextFormat("Could not deserialise level '%s' - %s", level_file_name, val.what()));
    //     return false;
    // }

    // TraceLog(TraceLogLevel::LOG_INFO, "--------World load level - Done!--------");
    return true;
}

//====================================================================

void World::init()
{
    TraceLog(TraceLogLevel::LOG_INFO, "Initializing world!");

    scene.init_level_scene();
    scene.init();
}

void World::update()
{
    scene.update();
}
