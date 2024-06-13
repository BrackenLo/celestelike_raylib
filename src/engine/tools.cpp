#include "tools.h"
#include <cmath>

float step(float val, float target, float step)
{
    if (std::signbit(target - val))
        return std::fmax((val - step), target);
    else
        return std::fmin((val + step), target);
}
