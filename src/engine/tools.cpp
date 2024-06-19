#include "tools.h"

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

bool are_keys_down(std::vector<int> keys)
{
    for (int key : keys)
        if (IsKeyDown(key))
            return true;

    return false;
}

bool are_keys_pressed(std::vector<int> keys)
{
    for (int key : keys)
        if (IsKeyPressed(key))
            return true;

    return false;
}
