#pragma once

#include "player_systems.hpp"
#include "scene.hpp"
#include "system.hpp"

namespace celestelike {

class LevelScene : public Scene {

public:
    virtual void init() override;
    virtual void update() override;

private:
    RenderSystem render_system;
    PlayerSystem player_system;
    PhysicsSystem physics_system;
};

}
