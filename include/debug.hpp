#pragma once

namespace celestelike {

namespace debug {

    struct DebugInspect {
        const char* name;
    };

    struct DebugState {
        bool active;

        float x;
        float y;
        float width;
        float height;

        DebugState();
        void resize();
    };

}
}
