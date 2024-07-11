#pragma once

#include "raylib.h"

namespace celestelike {

namespace debug {

    struct DebugInspect {
        const char* name;
    };

    enum class MenuState {
        Main,
        Inspector,
    };

    struct DebugState {
        bool active;
        MenuState state;

        float menu_width;
        Rectangle rect;

        DebugState();
        void resize();
    };

}
}
