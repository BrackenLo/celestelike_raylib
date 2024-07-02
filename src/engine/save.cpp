#include "save.hpp"

#include "entity.hpp"
#include "world.hpp"

void SaveData::ToRaw(Entity* entity)
{
    IToRawData* to_raw = dynamic_cast<IToRawData*>(entity);
    if (!to_raw)
        return;

    entities.push_back(to_raw->ToRaw());
}

SaveData::SaveData(World* world)
{
    version = "0.01";

    entities.clear();

    for (Entity* entity : *world->get_solids())
        ToRaw(entity);
    for (Entity* entity : *world->get_actors())
        ToRaw(entity);
}

RawEntity::RawEntity(int x, int y)
    : x(x)
    , y(y)
{
}
