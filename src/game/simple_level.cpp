#include "simple_level.h"

#include "../engine/entity.h"
#include "../game/player.h"

void SimpleLevel::init()
{
    Level::init();

    world.add_solid(new Solid({ 600, 100 }, 1000.0f, 25.0f));
    world.add_solid(new Solid({ -1000, 0 }, 25.0f, 100.0f));

    world.add_solid(new Solid({ 600, 157 }, 50.0f, 25.0f));

    Player* player = new Player({ 0, -100.0f });
    world.add_actor(player);
    camera.follow_target = player;
}
