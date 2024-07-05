#pragma once

#include "scene.hpp"
#include "system.hpp"

namespace celestelike {

class LevelScene : public Scene {

public:
    virtual void init() override;
    virtual void update() override;

private:
    RenderSystem render_system;
};

}
