#include "scene.hpp"

#include "debug.hpp"
#include "helper.hpp"
#include "player_resources.hpp"
#include "raylib.h"
#include "resources.hpp"
#include "systems.hpp"

namespace celestelike {

Scene::Scene() { }

void Scene::init()
{
    tools::trace("Initializing scene");

    auto system_sort = [](System const& a, System const& b) {
        return a.priority < b.priority;
    };

    std::sort(update_systems.begin(), update_systems.end(), system_sort);
    std::sort(fixed_systems.begin(), fixed_systems.end(), system_sort);
    std::sort(render_systems.begin(), render_systems.end(), system_sort);
}

void Scene::update()
{
    float dt = GetFrameTime();

    Time& time = reg.ctx().get<Time>();
    time.elapsed += dt;

    // Do main update stuff
    for (auto system : update_systems)
        system.system(reg, dt);

    // Do fixed timestep stuff
    FixedTimestep& fixed = reg.ctx().get<FixedTimestep>();
    if (!fixed.paused)
        fixed.accumulator += dt;

    while (fixed.accumulator >= fixed.timestep) {
        // if (fixed.accumulator >= fixed.timestep) {
        fixed.accumulator -= fixed.timestep;
        fixed.elapsed += fixed.timestep;

        float fixed_dt = fixed.timestep;

        for (System& system : fixed_systems)
            system.system(reg, fixed_dt);
    }

    // Do render stuff
    for (const System& system : render_systems)
        system.system(reg, dt);
}

void Scene::init_level_scene()
{
    tools::trace("Initializing level scene");

    // Add basic resources
    reg.ctx().emplace<Time>();
    reg.ctx().emplace<FixedTimestep>();
    reg.ctx().emplace<ClearColor>();
    reg.ctx().emplace<PlayerInput>();
    reg.ctx().emplace<debug::DebugState>();

    // Init systems
    player::init_systems(reg);

    // Spawn level stuff
    spawn_player(reg);
    spawn_camera(reg);

    spawn_tile(reg, { 100, 0 });
    spawn_tile(reg, { 0, 100, 1000, 25 });

    // Update stuff
    add_update({ &player::update_input, Order::First });

    add_update({ &camera::camera_follow, Order::First });
    add_update({ &camera::camera_update, Order::First + 1 });

    // Fixed update stuff
    add_fixed({ &physics::check_on_walls, Order::First });

    add_fixed({ &player::examine_collisions, Order::Third });
    add_fixed({ &player::handle_walk, Order::Third });
    add_fixed({ &player::handle_jump, Order::Third });
    add_fixed({ &player::handle_gravity, Order::Third + 1 });

    add_fixed({ &player::reset_values, Order::Fith });

    add_fixed({ &update::pos_lerp, Order::Third });

    add_fixed({ &physics::apply_velocity_collision, Order::Fith });

    // Render stuff

    add_render({ &render::start_render, Order::First });

    add_render({ &render::render, Order::Third });
    add_render({ &debug::run_debug_systems, Order::Fith - 1 });

    add_render({ &render::finish_render, Order::Fith });
}

void Scene::load_level(save::SaveData data)
{
    int loaded_solids = 0;

    for (save::Tile tile : data.tiles) {
        spawn_tile(reg, tile);
        loaded_solids += 1;
    }

    tools::trace(TextFormat("Loaded %d solids", loaded_solids));

    spawn_player(reg, data.player);
    spawn_camera(reg);
}

save::SaveData Scene::save_level()
{
    save::SaveData data;
    data.version = "0.01";

    auto v_tiles = reg.view<Solid, Pos, CollisionBounds>();

    for (entt::entity tile : v_tiles) {
        const Pos& pos = v_tiles.get<Pos>(tile);
        const Bounds& bounds = v_tiles.get<CollisionBounds>(tile);

        data.tiles.push_back({ pos.x, pos.y, bounds.half_width, bounds.half_height });
    }

    auto v_player = reg.view<Player, Pos>();
    entt::entity player = v_player.begin() != v_player.end() ? *v_player.begin() : throw("no player to save");

    auto val = v_player.begin();

    const Pos& player_pos = v_player.get<Pos>(player);
    data.tiles.push_back({ player_pos.x, player_pos.y });

    return data;
}

void Scene::add_update(System system)
{
    update_systems.push_back(system);
}

void Scene::add_fixed(System system)
{
    fixed_systems.push_back(system);
}

void Scene::add_render(System system)
{
    render_systems.push_back(system);
}

}
