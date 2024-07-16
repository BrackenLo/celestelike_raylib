#include "save.hpp"

#include <cereal/archives/json.hpp>
#include <cstring>
#include <fstream>
#include <raylib.h>

#include "components.hpp"
#include "helper.hpp"

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

        auto v_solids = reg.view<Solid, Pos, CollisionBounds>();

        for (entt::entity solid : v_solids) {
            const Pos& pos = v_solids.get<Pos>(solid);
            const Bounds& bounds = v_solids.get<CollisionBounds>(solid);

            data.tiles.push_back({ pos.x, pos.y, bounds.half_width, bounds.half_height });
        }

        auto v_player = reg.view<Player, Pos, player::PlayerCharacters>();

        tools::trace("Checking for players: ");
        for (entt::entity player : v_player) {
            tools::trace("   - Found player");
        }

        if (v_player.begin() == v_player.end()) {
            tools::error("No player found to save");
            return false;
        }
        entt::entity player = *v_player.begin();

        const Pos& player_pos = v_player.get<Pos>(player);
        const player::PlayerCharacters& player_characters = v_player.get<player::PlayerCharacters>(player);
        PlayerDescriptor desc(player_pos.x, player_pos.y);
        desc.characters = player_characters.available_characters;
        desc.character_index = player_characters.current_character_index;

        data.player = SavePlayer { desc };

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
            tools::error(TextFormat("Could not open level '%s'", level_file_name));
            return false;
        }

        SaveData data;

        try {
            cereal::JSONInputArchive archive(file);
            archive(data);
        } catch (cereal::Exception save_exception) {
            tools::error(TextFormat("Could not deserialise level - %s", save_exception.what()));
            return false;
        }

        //----------------------------------------------

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
