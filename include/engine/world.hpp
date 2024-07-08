#pragma once

#include "debug.hpp"
#include "physics.hpp"
#include "scene.hpp"

#include <vector>

class World {
public:
    World();
    ~World();
    int run();

public:
    inline void add_actor(class Actor* actor) { actors.push_back(actor); }
    inline void add_solid(class Solid* solid) { solids.push_back(solid); }

    bool destroy_actor(class Actor* actor);
    bool destroy_solid(class Solid* solid);
    void clear_all();
    void clear_level();

    inline std::vector<class Actor*>* get_actors() { return &actors; }
    inline std::vector<class Solid*>* get_solids() { return &solids; }
    inline class Player* get_player() { return player_character; }

    std::vector<Collision> check_collision(class CollisionEntity* to_check);
    std::vector<CollisionEntity*> check_overlap(class CollisionEntity* to_check);

    inline void log(const char* text, int log = 0) { debug.add_message(text, log); }

    inline PhysicsData* get_physics_data() { return &physics_data; }

public:
    std::vector<const char*> get_levels();
    bool save_level(const char* level_name);
    bool load_level(const char* level_file_name);

private:
    void init();
    void update();
    void fixed_update(float dt);
    void render();
    void render_2d_inner();

    friend class Game;

private:
    std::vector<class Actor*> actors;
    std::vector<class Solid*> solids;

    class Player* player_character;

private:
    PhysicsData physics_data;
    Debugger debug;

private:
    // ECS Overhaul
    std::unique_ptr<Scene> scene;
};
