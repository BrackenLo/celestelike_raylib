#pragma once

#include "scene.hpp"

namespace celestelike {

class LevelScene : public Scene {

public:
    virtual void init() override;
    virtual void update() override;
};

}
