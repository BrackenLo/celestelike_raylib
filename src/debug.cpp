#include "debug.hpp"

namespace celestelike {

namespace debug {

    DebugState::DebugState()
    {
        active = false;
        menu_width = 320;
    }

    void DebugState::resize()
    {
        const float width = GetScreenWidth();
        const float height = GetScreenHeight();

        rect.width = menu_width;
        rect.height = height;

        rect.x = width - rect.width;
        rect.y = 0;
    }
}

}
