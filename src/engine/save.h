#pragma once

#include "cereal/cereal.hpp"
#include <cereal/access.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>
#include <string>
#include <vector>

//====================================================================

struct SaveData {
public:
    std::string version;
    std::vector<std::unique_ptr<class RawEntity>> entities;

public:
    SaveData() { }
    SaveData(class World* world);

public:
    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("version", version),
            cereal::make_nvp("entities", entities));
    }

private:
    void ToRaw(class Entity* entity);
};

//====================================================================

class IToRawData {
public:
    virtual std::unique_ptr<class RawEntity> ToRaw() = 0;
};

//----------------------------------------------

struct RawEntity {
    RawEntity() { }
    RawEntity(int x, int y);
    virtual ~RawEntity() { }

    int x;
    int y;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("x", x),
            cereal::make_nvp("y", y));
    }

    virtual std::unique_ptr<class Entity> ToEntity() = 0;
};

//====================================================================
