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

Rectangle Actor::get_rect()
{
    return Rectangle {
        pos.x - half_width,
        pos.y - half_height,
        half_width * 2.0f,
        half_height * 2.0f,
    };
}

void Actor::get_properties(std::vector<DebugProperty>* properties)
{
    properties->push_back({ "position", &pos, true, -99999999.0f, 99999999.0f });
    properties->push_back({ "half_width", &half_width, true, -99999999.0f, 99999999.0f });
    properties->push_back({ "half_height", &half_height, true, -99999999.0f, 99999999.0f });
}

//====================================================================

RawSolid::RawSolid(int x, int y, int half_width, int half_height)
    : RawEntity(x, y)
    , half_width(half_width)
    , half_height(half_height)
{
}

std::unique_ptr<RawEntity> Solid::ToRaw()
{
    RawSolid* raw = new RawSolid(pos.x, pos.y, half_width, half_height);

    return std::unique_ptr<RawEntity>(raw);
}

std::unique_ptr<Entity> RawSolid::ToEntity()
{
    Vector2 pos = {
        static_cast<float>(x),
        static_cast<float>(y),
    };

    std::unique_ptr<Solid> solid(new Solid(pos, half_width, half_height));
    return solid;
}
