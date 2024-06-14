#include "simple_world.h"

#include "../engine/entity.h"
#include "../game/player.h"

void SimpleWorld::init()
{
    World::init();

    add_solid(new Solid({ 0, 100 }, 1000.0f, 25.0f)); // Floor
    add_solid(new Solid({ -1000, 0 }, 25.0f, 100.0f)); // Left Wall
    add_solid(new Solid({ -600, -157 }, 50.0f, 25.0f)); // Left platform
    add_solid(new Solid({ -400, -80 }, 50.0f, 25.0f)); // Left platform

    Player* player = new Player({ 0, -100.0f });
    add_actor(player);
    camera.follow_target = player;
}
