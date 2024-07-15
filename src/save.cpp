#include "save.hpp"

#include "cereal/archives/json.hpp"
#include "helper.hpp"
#include "raylib.h"
#include <cstring>
#include <fstream>

namespace celestelike {

namespace save {

    std::vector<const char*> get_levels()
    {
        tools::trace("Retrieving levels: ");

        std::vector<const char*> levels;

        FilePathList files = LoadDirectoryFiles(GetWorkingDirectory());
        for (int i = 0; i < files.count; i++) {
            const char* file_path = files.paths[i];
            const char* file_name = GetFileName(file_path);

            const char* ptr = strstr(file_name, "level-");
            if (file_name == ptr) {
                levels.push_back(file_name);
                tools::trace(TextFormat("   %s", file_name));
            }
        }

        return levels;
    }

    bool save_level(const char* level_name, entt::registry& reg)
    {
        // JSON archive
        std::string file_name;
        file_name
            .append("level-")
            .append(level_name)
            .append(".json");

        std::ofstream file(file_name);
        if (!file.is_open())
            return false;
        cereal::JSONOutputArchive archive(file);

        // // Binary archive
        // std::string file_name;
        // file_name
        //     .append("level-")
        //     .append(level_name)
        //     .append(".bin");
        // std::ofstream file(file_name, std::ios::binary);
        // if (!file.is_open())
        //     return false;
        // cereal::PortableBinaryOutputArchive archive(file);

        // // XML archive
        // std::string file_name;
        // file_name
        //     .append("level-")
        //     .append(level_name)
        //     .append(".xml");
        // std::ofstream file(file_name);
        // if (!file.is_open())
        //     return false;
        // cereal::XMLOutputArchive archive(file);

        tools::trace(TextFormat("Saving file: %s", file_name.c_str()));

        //----------------------------------------------

        save::SaveData data;
        data.version = "0.01";

        auto v_solids = reg.view<Solid, Pos, CollisionBounds>();

        for (entt::entity solid : v_solids) {
            const Pos& pos = v_solids.get<Pos>(solid);
            const Bounds& bounds = v_solids.get<CollisionBounds>(solid);

            data.tiles.push_back({ pos.x, pos.y, bounds.half_width, bounds.half_height });
        }

        auto v_player = reg.view<Player, Pos>();
        entt::entity player = v_player.begin() != v_player.end() ? *v_player.begin() : throw("no player to save");

        const Pos& player_pos = v_player.get<Pos>(player);
        data.player = SavePlayer { player_pos.x, player_pos.y };

        //----------------------------------------------

        tools::trace("Prepped save data.");

        archive(data);
        return true;
    }

    bool load_level(const char* level_file_name, entt::registry& reg)
    {
        tools::trace(TextFormat("Loading level file: %s", level_file_name));

        // JSON archive
        std::string file_name(level_file_name);
        // file_name
        //     .append("level-")
        //     .append(level_name)
        //     .append(".json");
        std::ifstream file(file_name);
        if (!file.is_open()) {
            tools::warn(TextFormat("Could not open level '%s'", level_file_name));
            return false;
        }

        cereal::JSONInputArchive archive(file);

        //----------------------------------------------

        SaveData data;

        try {
            archive(data);
        } catch (cereal::Exception val) {
            tools::trace(TextFormat("Could not deserialise level '%s' - %s", level_file_name, val.what()));
            return false;
        }

        //----------------------------------------------

        reg.clear();

        int loaded_solids = 0;

        for (save::SaveTile tile : data.tiles) {
            spawn_tile(reg, tile);
            loaded_solids += 1;
        }

        tools::trace(TextFormat("Loaded %d solids", loaded_solids));

        spawn_player(reg, data.player);
        spawn_camera(reg);

        //----------------------------------------------

        return true;
    }

}

}
