#include "level_scene.hpp"

#include "components.hpp"
#include "factory.hpp"
#include "helper.hpp"
#include "player_resources.hpp"
#include "resources.hpp"
#include "systems.hpp"

namespace celestelike {

void LevelScene::init()
{
    tools::trace("init level");

    reg.ctx().emplace<Time>();
    reg.ctx().emplace<FixedTimestep>();
    reg.ctx().emplace<ClearColor>();
    reg.ctx().emplace<PlayerInput>();

    player::init_systems(reg);

    spawn_player(reg);
    spawn_camera(reg);

    spawn_tile(reg, { 100, 0 });
    spawn_tile(reg, { 0, 100, 1000, 25 });
}

void LevelScene::update()
{
    float dt = GetFrameTime();

    Time& time = reg.ctx().get<Time>();
    time.elapsed += dt;

    //----------------------------------------------

    player::update_input(reg, dt);

    camera::camera_follow(reg, dt);
    camera::camera_update(reg, dt);

    //----------------------------------------------

    fixed_update();

    //----------------------------------------------

    render::render(reg, dt);
}

void LevelScene::fixed_update()
{
    FixedTimestep& fixed = reg.ctx().get<FixedTimestep>();

    if (fixed.paused)
        return;

    fixed.accumulator += GetFrameTime();

    if (fixed.accumulator < fixed.timestep)
        return;

    fixed.accumulator -= fixed.timestep;
    fixed.elapsed += fixed.timestep;

    float dt = fixed.timestep;

    physics::check_on_walls(reg, dt);

    player::examine_collisions(reg, dt);
    player::handle_walk(reg, dt);
    player::handle_jump(reg, dt);
    player::handle_gravity(reg, dt);

    player::reset_values(reg, dt);

    update::pos_lerp(reg, dt);

    physics::apply_velocity_collision(reg, dt);
}

void LevelScene::load(save::SaveData data)
{
    int loaded_solids = 0;

    for (save::Tile tile : data.tiles) {
        spawn_tile(reg, tile);
        loaded_solids += 1;
    }

    tools::trace_fmt("Loaded %d solids", loaded_solids);

    spawn_player(reg, data.player);
    spawn_camera(reg);
}

save::SaveData LevelScene::save()
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

}
