#include "level_scene.hpp"

#include "factory.hpp"
#include "tools.hpp"

namespace celestelike {

void LevelScene::init()
{
    trace("init level");

    render_system.init(reg);
    player_system.init(reg);

    spawn_player(reg);
    spawn_camera(reg);

    spawn_tile(reg, { 100, 0 });
}

void LevelScene::update()
{
    render_system.run(reg);
    player_system.run(reg);
    physics_system.run(reg);
}

}
