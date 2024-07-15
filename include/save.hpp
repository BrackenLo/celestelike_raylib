#pragma once

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

#include "entt/entity/fwd.hpp"
#include <string>
#include <vector>

#include "factory.hpp"

namespace celestelike {

namespace save {

    struct SaveTile {
        TileDescriptor desc;
        operator const TileDescriptor&() const { return desc; }

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("x", desc.x),
                cereal::make_nvp("y", desc.y),
                cereal::make_nvp("width", desc.half_width),
                cereal::make_nvp("height", desc.half_height));
        }
    };

    struct SavePlayer {
        PlayerDescriptor desc;
        operator const PlayerDescriptor&() const { return desc; }

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("x", desc.x),
                cereal::make_nvp("y", desc.y));
        }
    };

    //----------------------------------------------

    struct SaveData {
    public:
        SaveData() {};
        SaveData(entt::registry& reg);

    public:
        std::string version;
        std::vector<SaveTile> tiles;
        SavePlayer player;

    public:
        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("version", version),
                cereal::make_nvp("tiles", tiles),
                cereal::make_nvp("player", player)
                //
            );
        }
    };

    std::vector<const char*> get_levels();
    bool save_level(const char* level_name, entt::registry& reg);
    bool load_level(const char* level_file_name, entt::registry& reg);

}

}
