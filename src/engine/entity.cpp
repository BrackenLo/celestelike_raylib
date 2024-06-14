#include "entity.h"

//====================================================================

Entity::Entity()
{
    pos = { 0 };
}

Entity::Entity(Vector2 new_pos)
{
    pos = new_pos;
}

//====================================================================

CollisionEntity::CollisionEntity()
{
    pos = { 0 };
    half_width = 16;
    half_height = 16;
}

CollisionEntity::CollisionEntity(Vector2 new_pos, int h_width, int h_height)
{
    pos = new_pos;
    half_width = h_width;
    half_height = h_height;
}

void CollisionEntity::render(World* world)
{
    DrawRectangle(
        pos.x - half_width,
        pos.y - half_height,
        half_width * 2,
        half_height * 2,
        GREEN);
}

//====================================================================

//====================================================================
