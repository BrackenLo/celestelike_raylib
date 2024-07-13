#include "debug.hpp"

#include "raylib.h"

namespace celestelike {

namespace debug {

    DebugState::DebugState()
    {
        active = false;
        width = 320;
    }

    void DebugState::resize()
    {
        const float screen_width = GetScreenWidth();
        const float screen_height = GetScreenHeight();

        height = screen_height;

        x = screen_width - width;
        y = 0;
    }
}

}
