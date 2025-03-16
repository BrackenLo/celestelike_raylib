#pragma once

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <vector>

#include "factory.hpp"
#include "helper.hpp"

namespace celestelike {

namespace save {

    struct SaveTile {
        TileDescriptor desc;
        operator const TileDescriptor&() const { return desc; }

        template <class Archive>
        void serialize(Archive& archive, std::uint32_t const version)
        {
            // Order of save/load
            // V1
            // 1. x
            // 2. y
            // 3. width
            // 4. height

            if (version == 1) {
                archive(
                    cereal::make_nvp("x", desc.x),
                    cereal::make_nvp("y", desc.y),
                    cereal::make_nvp("width", desc.half_width),
                    cereal::make_nvp("height", desc.half_height));
            }

            else {
                tools::warn("Invalid tile save version");
                assert(0);
            }
        }
    };

    struct SavePlayer {
        PlayerDescriptor desc;
        operator const PlayerDescriptor&() const { return desc; }

        template <class Archive>
        void serialize(Archive& archive, std::uint32_t const version)
        {
            // Order of save/load
            // V1
            // 1. x
            // 2. y
            // V2
            // 3. vec of player characters
            // 4. current player character

            if (version == 1) {
                tools::trace("serializing player Version 1");
                archive(
                    cereal::make_nvp("x", desc.x),
                    cereal::make_nvp("y", desc.y));
            }

            else if (version == 2) {
                tools::trace("serializing player Version 2");
                archive(
                    cereal::make_nvp("x", desc.x),
                    cereal::make_nvp("y", desc.y),
                    cereal::make_nvp("characters", desc.characters),
                    cereal::make_nvp("index", desc.character_index));
            }

            else {
                tools::warn("Invalid player save version");
                assert(0);
            }
        }
    };

    //----------------------------------------------

    struct SaveData {
    public:
        SaveData() {};
        SaveData(entt::registry& reg);

    public:
        std::vector<SaveTile> tiles;
        SavePlayer player;

    public:
        template <class Archive>
        void serialize(Archive& archive, std::uint32_t const version)
        {
            archive(
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

CEREAL_CLASS_VERSION(celestelike::save::SaveTile, 1);
CEREAL_CLASS_VERSION(celestelike::save::SavePlayer, 2);
CEREAL_CLASS_VERSION(celestelike::save::SaveData, 1);
