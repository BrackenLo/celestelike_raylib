#pragma once

#include "save_x.hpp"
#include "scene.hpp"

namespace celestelike {

class LevelScene : public Scene {

public:
    virtual void init() override;
    virtual void update() override;

    void fixed_update();

    void load(save::SaveData data);
    save::SaveData save();
};

}
