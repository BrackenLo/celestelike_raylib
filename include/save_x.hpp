#pragma once

#include "cereal/cereal.hpp"
#include "entt/entity/fwd.hpp"
#include <string>
#include <vector>

#include "factory.hpp"

namespace celestelike {

namespace save {

    struct Tile {
        TileDescriptor desc;
        operator const TileDescriptor&() const { return desc; }
    };

    struct Player {
        PlayerDescriptor desc;
        operator const PlayerDescriptor&() const { return desc; }
    };

    //----------------------------------------------

    struct SaveData {
    public:
        SaveData() {};
        SaveData(entt::registry& reg);

    public:
        std::string version;
        std::vector<Tile> tiles;
        Player player;

    public:
        template <class Archive>
        void
        serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("version", version),
                cereal::make_nvp("tiles", tiles),
                cereal::make_nvp("player", player)
                //
            );
        }
    };
}

}
