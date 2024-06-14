#include "tools.h"

#include "raylib.h"
#include <cmath>

float step(float val, float target, float step)
{
    if (std::signbit(target - val))
        return std::fmax((val - step), target);
    else
        return std::fmin((val + step), target);
}

std::string int_to_str(int val, int len = 5)
{
    std::string str = std::to_string(val);
    int remain = len - str.length();

    if (remain <= 0)
        return str;

    return std::string(remain, ' ') + str;
}

void Logger::render()
{
    for (int x = 0; x < messages.size(); x++) {
        DrawText(messages[x].c_str(), 20, 20 + 20 * x, 20, BLACK);
    }

    messages.clear();
}
