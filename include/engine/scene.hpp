#pragma once

#include "entt/entt.hpp"
#include <vector>

namespace celestelike {

enum Order {
    First = 10,
    Second = 20,
    Third = 30,
    Fourth = 40,
    Fith = 50
};

struct System {

    std::function<void(entt::registry&, float)> system;
    int priority = Order::Third;
};

class Scene {
public:
    Scene();
    ~Scene() {};

    void init();
    void update();

    void init_level_register();
    void init_level_scene();

protected:
    entt::registry reg;
    std::vector<System> update_systems;
    std::vector<System> fixed_systems;
    std::vector<System> render_systems;

private:
    void add_update(System system);
    void add_fixed(System system);
    void add_render(System system);
};

}
