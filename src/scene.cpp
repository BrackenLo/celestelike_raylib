#include "scene.hpp"

#include <raylib.h>

#include "debug.hpp"
#include "factory.hpp"
#include "helper.hpp"
#include "resources.hpp"
#include "save.hpp"
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

void Scene::init_level_register()
{
    tools::trace("Initializing level register");

    //----------------------------------------------
    // Add basic resources
    reg.ctx().emplace<Time>();
    reg.ctx().emplace<FixedTimestep>();
    reg.ctx().emplace<ClearColor>();
    reg.ctx().emplace<PlayerInput>();
    reg.ctx().emplace<debug::DebugState>().init();

    // Init systems
    player::init_systems(reg);

    //----------------------------------------------
    // Update stuff
    add_update({ &player::update_input, Order::First });

    add_update({ &camera::camera_follow, Order::First });
    add_update({ &camera::camera_update, Order::First + 1 });

    // Fixed update stuff
    add_fixed({ &physics::check_on_walls, Order::First });

    add_fixed({ &player::examine_collisions, Order::Third });
    add_fixed({ &player::handle_walk, Order::Third + 1 });
    add_fixed({ &player::handle_ability_1, Order::Third + 2 });
    add_fixed({ &player::handle_ability_2, Order::Third + 3 });
    add_fixed({ &player::handle_character_change, Order::Third + 4 });
    add_fixed({ &player::handle_jump, Order::Third + 5 });
    add_fixed({ &player::handle_gravity, Order::Third + 6 });
    add_fixed({ &player::handle_ability_glide, Order::Third + 7 });
    add_fixed({ &player::handle_ability_multi_jump, Order::Third + 6 });

    add_fixed({ &player::reset_values, Order::Fith });

    add_fixed({ &update::pos_lerp, Order::Third });

    add_fixed({ &physics::apply_velocity_collision, Order::Fith });

    // Render stuff
    add_render({ &render::start_render, Order::First });

    add_render({ &render::render, Order::Third });
    add_render({ &debug::run_debug_systems, Order::Fith - 1 });

    add_render({ &render::finish_render, Order::Fith });
}

void Scene::init_level_scene()
{
    tools::trace("Initializing level scene");

    // Try loading default level or spawn default entities
    if (!save::load_level("level-default.json", reg)) {
        spawn_player(reg, PlayerDescriptor(0, -32));
        spawn_camera(reg);

        spawn_tile(reg, { 16, 16, 16, 16 });
        spawn_tile(reg, { -16, 16, 16, 16 });
    }
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
