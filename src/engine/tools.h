#pragma once

#include "raylib.h"
#include <string>
#include <vector>

float step(float val, float target, float step);

std::string int_to_str(int val, int len);

inline void trace(const char* msg)
{
    TraceLog(TraceLogLevel::LOG_INFO, msg);
}
