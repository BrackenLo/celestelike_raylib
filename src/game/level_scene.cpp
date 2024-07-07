#include "level_scene.hpp"

#include "factory.hpp"
#include "player_resources.hpp"
#include "resources.hpp"
#include "systems.hpp"
#include "tools.hpp"

namespace celestelike {

void LevelScene::init()
{
    trace("init level");

    reg.ctx().emplace<ClearColor>(ClearColor { RAYWHITE });
    reg.ctx().emplace<PlayerInput>();

    spawn_player(reg);
    spawn_camera(reg);

    spawn_tile(reg, { 100, 0 });
}

void LevelScene::update()
{
    float dt = 1.0f / 60.0f;

    render::render(reg, dt);

    player::update_input(reg, dt);
    player::update_velocity(reg, dt);

    camera::camera_follow(reg, dt);
    camera::camera_update(reg, dt);

    update::pos_lerp(reg, dt);

    physics::apply_velocity_collision(reg, dt);
}

}
