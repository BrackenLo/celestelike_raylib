#pragma once

#include "raylib.h"

namespace celestelike {

struct ClearColor {
    Color color;

    operator const Color&() { return color; }
};

}
