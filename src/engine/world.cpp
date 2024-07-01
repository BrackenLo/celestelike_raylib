#include "world.h"

#include "cereal/details/helpers.hpp"
#include "raylib.h"
#include <algorithm>
#include <cstring>
#include <string>

#include "../game/player.h"
#include "entity.h"

#include "save.h"
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <fstream>

#include "raygui.h"

World::World()
{
    clear_color = RAYWHITE;
    // clear_color = Color(48, 41, 40);
}

World::~World()
{
    clear_all();
}

int World::run()
{
    InitWindow(800, 450, "celestelike");

    SetTraceLogLevel(TraceLogLevel::LOG_ALL);
    GuiLoadStyle("style_candy.rgs");

    SetTargetFPS(physics_data.fps);

    init();

    while (!WindowShouldClose()) {
        update();

        physics_data.accumulator += GetFrameTime() * !physics_data.freeze_fixed_update;

        if (physics_data.accumulator >= physics_data.timestep) {
            fixed_update(physics_data.timestep);
            physics_data.accumulator -= physics_data.timestep;
            physics_data.elapsed += physics_data.timestep;
        }

        render();
    }

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

    // Binary archive
    // std::string file_name;
    // file_name
    //     .append("level-")
    //     .append(level_name)
    //     .append(".bin")
    // std::ofstream file(file_name, std::ios::binary);
    // cereal::PortableBinaryOutputArchive archive(file);

    // XML archive
    // std::string file_name;
    // file_name
    //     .append("level-")
    //     .append(level_name)
    //     .append(".xml");
    // std::ofstream file(file_name);
    // cereal::XMLOutputArchive archive(file);

    TraceLog(TraceLogLevel::LOG_INFO, TextFormat("Saving file: %s", file_name.c_str()));

    SaveData data(this);
    archive(data);
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

    SaveData data;

    try {
        archive(data);
    } catch (cereal::Exception val) {
        TraceLog(TraceLogLevel::LOG_WARNING, TextFormat("Could not deserialise level '%s' - %s", level_file_name, val.what()));
        return false;
    }

    int loaded_actors = 0;
    int loaded_solids = 0;
    int loaded_other = 0;

    for (std::unique_ptr<RawEntity>& raw : data.entities) {
        Entity* entity = raw->ToEntity().release();

        Actor* actor = dynamic_cast<Actor*>(entity);
        if (actor) {
            // TraceLog(TraceLogLevel::LOG_INFO, TextFormat("Spawning Actor"));
            loaded_actors += 1;
            actors.push_back(actor);
            continue;
        }

        Solid* solid = dynamic_cast<Solid*>(entity);
        if (solid) {
            // TraceLog(TraceLogLevel::LOG_INFO, TextFormat("Spawning Solid"));
            loaded_solids += 1;
            solids.push_back(solid);
            continue;
        }

        TraceLog(TraceLogLevel::LOG_WARNING, "World load level - Entity was neither actor or solid");
        loaded_other += 1;
        delete entity;
    }

    for (Actor* actor : actors) {
        Player* player = dynamic_cast<Player*>(actor);
        if (player) {
            player_character = player;
            camera.set_follow_target(player_character, true);
            break;
        }
    }

    TraceLog(TraceLogLevel::LOG_INFO, "--------World load level - Done!--------");
    TraceLog(
        TraceLogLevel::LOG_INFO,
        "    Loaded %d actors, %d solids and found %d other",
        loaded_actors, loaded_solids, loaded_other);
    return true;
}

//====================================================================

void World::init()
{
    TraceLog(TraceLogLevel::LOG_INFO, "Initializing world!");

    camera.reset();

    // Try load default level or spawn default
    if (!load_level("level-default.json")) {
        add_solid(new Solid({ 0, 100 }, 1000.0f, 25.0f)); // Floor

        Player* player = new Player({ 0, -100.0f });
        add_actor(player);
        camera.follow_target = player;
    }
}

void World::update()
{
    for (Solid* solid : solids)
        solid->update(this);

    for (Actor* actor : actors)
        actor->update(this);

    camera.update(this);

    debug.update(this);
}

void World::fixed_update(float dt)
{
    for (Solid* solid : solids)
        solid->fixed_update(this, dt);

    for (Actor* actor : actors)
        actor->fixed_update(this, dt);
}

void World::render()
{
    BeginDrawing();
    ClearBackground(clear_color);

    BeginMode2D(camera.get_camera());
    render_2d_inner();
    debug.render_2d(this);
    EndMode2D();

    debug.render(this);

    EndDrawing();
}

void World::render_2d_inner()
{
    for (Solid* solid : solids)
        solid->render(this);

    for (Actor* actor : actors)
        actor->render(this);
}
