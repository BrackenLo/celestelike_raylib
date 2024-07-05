#include "level_scene.hpp"

#include "factory.hpp"
#include "tools.hpp"

namespace celestelike {

void LevelScene::init()
{
    trace("init level");

    render_system.init(reg);
    spawn_player(reg);
    spawn_camera(reg);
}

void LevelScene::update()
{
    render_system.run(reg);
}

}
